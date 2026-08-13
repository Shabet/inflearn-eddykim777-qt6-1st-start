# Qt signal slot 개념

Qt 6 시그널/슬롯을 처음 접하는 사람이, 문법부터 내부 구현·디자인 패턴까지 한 흐름으로 이해하도록 정리한 문서입니다.

실습 코드는 `section03/P01_SignalSlot` 입니다. 슬라이더를 움직이면 숫자가 `leText`에 찍히고, 같은 문자열이 `leTextCopy`에도 복사됩니다.

```cpp
connect(ui->hSlider, &QSlider::valueChanged, this, &Widget::slot_valueChanged);
connect(this, &Widget::sig_textChanged, this, &Widget::slot_textChanged);
```

---

## 목차

1. [이 문서에서 다루는 것](#1-이-문서에서-다루는-것)
2. [실습 예제 전체 소스](#2-실습-예제-전체-소스)
3. [프로그램이 하는 일](#3-프로그램이-하는-일)
4. [connect 네 인자와 this](#4-connect-네-인자와-this)
5. [SIGNAL() / SLOT() 없이 연결하기](#5-signal--slot-없이-연결하기)
6. [private 과 slots: 의 의미](#6-private-과-slots-의-의미)
7. [생성자 초기화 목록](#7-생성자-초기화-목록)
8. [소스에 없는 valueChanged는 언제 호출되나](#8-소스에-없는-valuechanged는-언제-호출되나)
9. [시그널과 슬롯의 인자 규칙](#9-시그널과-슬롯의-인자-규칙)
10. [인자가 전달되는 메커니즘](#10-인자가-전달되는-메커니즘)
11. [moc](#11-moc)
12. [컴파일러와 moc, 목적 파일과 실행 파일](#12-컴파일러와-moc-목적-파일과-실행-파일)
13. [콜백과 비교, 적용 범위](#13-콜백과-비교-적용-범위)
14. [Qt가 시그널/슬롯을 구현한 방식](#14-qt가-시그널슬롯을-구현한-방식)
15. [사용된 디자인 패턴](#15-사용된-디자인-패턴)
16. [한눈에 보는 전체 흐름](#16-한눈에-보는-전체-흐름)

---

## 1. 이 문서에서 다루는 것

시그널/슬롯은 “어떤 일이 생기면, 미리 연결해 둔 함수를 호출한다”는 개념입니다. 버튼 클릭, 슬라이더 값 변경, 소켓 데이터 도착처럼 **언제 일어날지 모르는 사건**에 잘 맞습니다.

이 문서는 아래 순서로 깊이를 올립니다.

| 단계 | 내용 |
| --- | --- |
| 사용 | `connect`, `emit`, 생성자, UI 폼 |
| 규칙 | 인자 호환, `private` / `slots:`, `this` |
| 빌드 | moc, `.o`, 실행 파일, “기계어” 용어 |
| 비교 | 일반 콜백, 스레드, 원격 |
| 내부 | `QMetaObject`, `Connection`, `activate` |
| 패턴 | Observer, Command, Adapter, Strategy |

Qt 버전은 6.5 이상을 기준으로 합니다. 함수 포인터 `connect`는 Qt 5에서 도입되었고 Qt 6의 기본 권장 문법입니다.

---

## 2. 실습 예제 전체 소스

### 2.1 `widget.h`

`Q_OBJECT`가 있어야 moc가 이 클래스를 처리합니다. `signals:`에 직접 만든 시그널, `private slots:`에 슬롯이 있습니다.

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;

private:
    Ui::Widget *ui;

signals:
    void sig_textChanged(QString str);

private slots:
    void slot_valueChanged(int val);
    void slot_textChanged(QString str);
};
#endif // WIDGET_H
```

`Ui::Widget`은 지금 작성 중인 `Widget`이 아닙니다. `widget.ui`에서 생성되는 **폼 클래스**입니다.

### 2.2 `widget.cpp`

```cpp
#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    connect(ui->hSlider, &QSlider::valueChanged, this, &Widget::slot_valueChanged);
    connect(this, &Widget::sig_textChanged, this, &Widget::slot_textChanged);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::slot_valueChanged(int val)
{
    QString str = QString("%1").arg(val);
    ui->leText->setText(str);

    emit sig_textChanged(str);
}

void Widget::slot_textChanged(QString str)
{
    ui->leTextCopy->setText(str);
}
```

### 2.3 `widget.ui` (핵심만)

Designer가 만든 화면입니다. `hSlider`, `leText`, `leTextCopy` 세 위젯이 있습니다. `<connections/>`가 비어 있으므로, 시그널/슬롯은 C++의 `connect`로만 연결합니다.

```xml
<widget class="QSlider" name="hSlider">
  <property name="orientation">
    <enum>Qt::Orientation::Horizontal</enum>
  </property>
</widget>
<widget class="QLineEdit" name="leText" ... />
<widget class="QLineEdit" name="leTextCopy" ... />
```

### 2.4 `main.cpp`

```cpp
#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return QApplication::exec();
}
```

`QApplication::exec()`가 이벤트 루프입니다. 마우스 이동, 페인트, 시그널 Queued 전달이 모두 이 루프를 탑니다.

### 2.5 `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.19)
project(P01_SignalSlot LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(P01_SignalSlot
    WIN32 MACOSX_BUNDLE
    main.cpp
    widget.cpp
    widget.h
    widget.ui
)

target_link_libraries(P01_SignalSlot
    PRIVATE
        Qt::Core
        Qt::Widgets
)
```

`qt_standard_project_setup()`과 `qt_add_executable()`이 moc(`widget.h`)와 uic(`widget.ui`)를 자동으로 돌립니다.

---

## 3. 프로그램이 하는 일

```text
사용자가 가로 슬라이더를 움직임
  → QSlider가 내부에서 emit valueChanged(새 값)
  → Widget::slot_valueChanged(int val)
      leText에 숫자 문자열 표시
      emit sig_textChanged(str)
  → Widget::slot_textChanged(QString str)
      leTextCopy에 같은 문자열 표시
```

시그널이 두 단입니다.

1. Qt가 제공하는 `QSlider::valueChanged(int)`
2. 직접 선언한 `Widget::sig_textChanged(QString)`

첫 단은 라이브러리 시그널이라 `emit`를 직접 쓰지 않습니다. 둘째 단은 슬롯 안에서 `emit sig_textChanged(str)`로 직접 보냅니다.

`emit`는 빈 매크로입니다. 컴파일러에게는 `sig_textChanged(str);`과 같습니다. 읽는 사람에게 “이 호출은 시그널이다”라고 알려 줄 뿐입니다.

---

## 4. connect 네 인자와 this

```cpp
connect(ui->hSlider, &QSlider::valueChanged, this, &Widget::slot_valueChanged);
```

| 인자 | 값 | 의미 |
| --- | --- | --- |
| 1 | `ui->hSlider` | 시그널을 **보내는** 객체 |
| 2 | `&QSlider::valueChanged` | 그 객체의 **어떤 시그널** (클래스 멤버 함수 주소) |
| 3 | `this` | 슬롯을 **받을** 객체 |
| 4 | `&Widget::slot_valueChanged` | 그 객체의 **어떤 멤버 함수** |

생성자 `Widget::Widget` 안의 `this`는 **지금 만들고 있는 Widget 인스턴스**입니다. 타입이 `Widget*`입니다.

`Widget`은 클래스 이름이고, `this`는 그 클래스의 **현재 객체**입니다. 창이 두 개면 `this`가 서로 다릅니다.

```text
Widget A  →  A의 슬라이더 → A.slot_valueChanged()
Widget B  →  B의 슬라이더 → B.slot_valueChanged()
```

`&Widget::slot_valueChanged`만으로는 “함수 종류”만 알고, **어느 인스턴스에서 실행할지**는 `this`가 정합니다.

`this->slot_valueChanged`처럼 쓰면 멤버 함수 포인터가 아닙니다. 비정적 멤버 함수 주소는 반드시 `&클래스::함수` 형식입니다.

---

## 5. SIGNAL() / SLOT() 없이 연결하기

예전 매크로 방식:

```cpp
connect(ui->hSlider, SIGNAL(valueChanged(int)), this, SLOT(slot_valueChanged(int)));
```

권장하는 함수 포인터 방식:

```cpp
connect(ui->hSlider, &QSlider::valueChanged, this, &Widget::slot_valueChanged);
```

| 매크로 방식 | 함수 포인터 방식 |
| --- | --- |
| `SIGNAL(valueChanged(int))` | `&QSlider::valueChanged` |
| `SLOT(slot_valueChanged(int))` | `&Widget::slot_valueChanged` |

인자 타입 `(int)`는 쓰지 않습니다. 시그널/슬롯 **함수의 주소**만 넘깁니다.

함수 포인터 방식이 나은 이유:

- 이름이나 타입이 틀리면 **컴파일 에러**가 납니다. `SIGNAL()`/`SLOT()`은 문자열이라 실행 전까지 모를 수 있습니다.
- 오버로드가 하나뿐이면 위처럼 바로 쓰면 됩니다. `QSlider::valueChanged`는 `int` 하나라 문제 없습니다.

매크로는 대략 이렇게 풀립니다. `2`는 시그널, `1`은 슬롯 표시입니다.

```cpp
#define SIGNAL(a)  "2"#a     // "2valueChanged(int)"
#define SLOT(a)    "1"#a     // "1slot_valueChanged(int)"
```

---

## 6. private 과 slots: 의 의미

```cpp
private slots:
    void slot_valueChanged(int val);
```

여기서 역할이 둘로 나뉩니다.

| 키워드 | 하는 일 |
| --- | --- |
| `private` | C++ 접근 제한. 밖에서는 호출/주소 획득 불가 |
| `slots:` | moc가 메타오브젝트에 등록. 예전 `SLOT()` 문자열 조회용 |

`private`이 의미 없는 것이 아닙니다. C++ 접근 제한은 그대로입니다.

생성자 안에서는 `this`의 private 멤버에 접근할 수 있으므로 연결됩니다.

```cpp
connect(ui->hSlider, &QSlider::valueChanged, this, &Widget::slot_valueChanged);
```

클래스 밖에서 이렇게 쓰면 **컴파일 에러**입니다.

```cpp
connect(slider, &QSlider::valueChanged, widget, &Widget::slot_valueChanged);
```

함수 포인터 `connect`는 C++ 함수 주소를 쓰므로 moc 등록이 필수는 아닙니다. 아래처럼 `slots:` 없이 일반 멤버 함수여도 연결됩니다.

```cpp
private:
    void slot_valueChanged(int val);
```

예전 `SIGNAL()`/`SLOT()`은 문자열로 슬롯을 찾습니다. moc 메타오브젝트를 타기 때문에 C++ `private`을 우회해 **밖에서도** private 슬롯에 연결할 수 있었습니다. `private slots:`의 원래 의도는 “C++에서는 직접 호출하지 못하게 하고, Qt `connect`로는 연결할 수 있게 한다”는 것이었습니다.

새 문법에서는 그 우회가 없습니다. `private`이면 **접근 권한이 있는 곳**(보통 클래스 안)에서만 연결할 수 있습니다. 밖에서 연결되게 하려면 함수를 `public`으로 두면 됩니다.

---

## 7. 생성자 초기화 목록

```cpp
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    connect(...);
}
```

콜론(`:`) 뒤는 **멤버 초기화 목록**입니다. 생성자 본문 `{ }`보다 **먼저** 실행됩니다.

### `: QWidget(parent)`

`Widget`은 `QWidget`을 상속합니다. 기반 클래스 생성자는 초기화 목록에서만 호출할 수 있습니다. 창 크기, 이벤트, 부모-자식 관계 같은 Qt 기본 동작이 여기서 준비됩니다.

`parent`는 Qt 위젯 부모입니다. `main`에서 `Widget w;`처럼 최상위 창으로 만들면 `nullptr`입니다. Qt는 부모가 파괴될 때 자식 위젯도 정리합니다.

### `, ui(new Ui::Widget)`

멤버 `Ui::Widget *ui`를 초기화합니다. `widget.ui`를 uic가 C++로 바꾼 폼 객체를 `new`로 만들고, `ui->hSlider`처럼 쓰게 합니다. 소멸자 `delete ui;`와 짝입니다.

### 실행 순서

```text
1. QWidget(parent)     Qt 위젯으로서의 기본 준비
2. ui(new Ui::Widget)  UI 폼 객체 생성
3. setupUi(this)       실제 컨트롤을 이 창에 배치
4. connect(...)        시그널/슬롯 예약 (아직 호출하지 않음)
```

`setupUi(this)`의 `this`도 지금 생성 중인 `Widget`입니다.

---

## 8. 소스에 없는 valueChanged는 언제 호출되나

`&QSlider::valueChanged`는 프로젝트 소스에 없습니다. Qt 라이브러리 `QSlider`가 미리 만들어 둔 **시그널**입니다.

`connect`는 지금 호출하지 않습니다. “슬라이더가 `valueChanged`를 보내면 `slot_valueChanged`를 실행해라”는 **예약**만 합니다.

값이 바뀌는 순간(마우스 드래그, 키보드, `setValue()`) Qt 내부에서 대략 다음이 일어납니다.

```cpp
emit valueChanged(새로운값);   // QSlider 내부, 직접 작성하지 않음
```

직접 만든 `sig_textChanged`는 직접 `emit` 해야 합니다.

```cpp
emit sig_textChanged(str);
```

| 시그널 | 누가 emit 하나 |
| --- | --- |
| `QSlider::valueChanged` | Qt (`QSlider` 구현) |
| `Widget::sig_textChanged` | 우리 코드 (`slot_valueChanged` 안) |

---

## 9. 시그널과 슬롯의 인자 규칙

이 예제의 연결은 인자 개수와 타입이 같습니다.

| 시그널 | 슬롯 |
| --- | --- |
| `QSlider::valueChanged(int)` | `slot_valueChanged(int)` |
| `sig_textChanged(QString)` | `slot_textChanged(QString)` |

필수는 “완전히 동일”이 아니라 **호환**입니다.

1. **슬롯 인자가 더 적어도 됩니다.** 시그널이 보낸 뒤쪽 값은 버려집니다.

```cpp
void foo();  // valueChanged(int)와 연결 가능. int를 안 받아도 됨
```

2. **슬롯 인자가 더 많으면 안 됩니다.** 시그널이 채워 줄 값이 없습니다.

```cpp
void bar(int a, int b);  // valueChanged(int)와 연결 불가
```

3. **앞쪽부터 차례로 맞춥니다.** 1번째끼리, 2번째끼리입니다. 순서를 바꿔 연결해 주지 않습니다.

4. **타입이 변환 가능하면 됩니다.** 시그널 `int` → 슬롯 `double`은 됩니다. `QString` → `int`처럼 안 맞으면 함수 포인터 `connect`는 컴파일 에러입니다.

시그널은 사실상 `void`입니다. 슬롯의 `return` 값을 보낸 쪽이 쓰기 어렵고, 슬롯이 여러 개면 더 애매합니다. “계산해서 값을 돌려줘”에는 일반 함수가 낫습니다.

---

## 10. 인자가 전달되는 메커니즘

기본 연결은 `Qt::AutoConnection`입니다. 슬라이더와 `Widget`이 같은 GUI 스레드에 있으므로 **Direct**로 동작합니다. 값은 이벤트 큐를 거치지 않고, 일반 함수 호출처럼 넘어갑니다.

### Direct (이 예제)

moc가 만든 시그널 함수는 인자 **값**이 아니라 **주소**를 `void*` 배열에 넣습니다.

```cpp
void QSlider::valueChanged(int _t1)
{
    void *_a[] = {
        nullptr,  // [0] 반환값 (void라 비움)
        const_cast<void*>(reinterpret_cast<const void*>(&_t1))  // [1] 1번 인자
    };
    QMetaObject::activate(this, &staticMetaObject, /*시그널 번호*/, _a);
}
```

`activate`가 연결 목록을 찾아, Direct면 바로:

```cpp
(receiver->*slot)(*reinterpret_cast<int*>(argv[1]));
// 즉
this->slot_valueChanged(42);
```

`valueChanged`가 **리턴하기 전에** 슬롯이 끝납니다. 스택의 `int` 주소를 써도 안전합니다.

```text
슬라이더 드래그
  → valueChanged(42)          스택에 42
  → argv[1] = &42
  → slot_valueChanged(42)     같은 스택에서 즉시
      emit sig_textChanged(str)
        → slot_textChanged(str)
  → valueChanged 리턴
```

### Queued (다른 스레드)

받는 객체가 다른 스레드면 스택 주소를 쓸 수 없습니다. 시그널 함수가 끝나면 `_t1`이 사라집니다.

Qt는 인자를 `QMetaType`으로 **복사**해 `QMetaCallEvent`에 넣고, 수신 스레드 이벤트 루프에 게시합니다. 나중에 루프가 꺼내 슬롯을 호출합니다.

| | Direct | Queued |
| --- | --- | --- |
| 시점 | `emit` 도중에 슬롯 실행 | 이벤트 루프가 나중에 실행 |
| 인자 | 주소로 즉시 전달 | 복사본 |
| 조건 | 같은 스레드 | 수신 쪽에 이벤트 루프 필요 |
| 이 예제 | 사용 중 | 사용하지 않음 |

`Qt::BlockingQueuedConnection`은 상대 스레드 슬롯이 끝날 때까지 기다립니다. UI 스레드와 섞으면 데드락이 나기 쉽습니다.

---

## 11. moc

**moc**는 Meta-Object Compiler입니다. Qt가 소스 빌드 전에 돌리는 **코드 생성기**입니다. `.o`를 만들지 않고, 컴파일러가 이해할 C++를 만듭니다.

일반 C++ 컴파일러는 `signals:`, `slots:`, `Q_OBJECT`를 모릅니다. 헤더에 `Q_OBJECT`가 있으면 moc가 그 클래스를 읽어 대략 다음을 생성합니다.

- 클래스 이름 문자열
- 시그널/슬롯 목록과 인자 타입
- 시그널 함수 본문 (`activate` 호출)
- `qt_static_metacall` (인덱스로 메서드 호출)

결과 파일 이름은 보통 `moc_widget.cpp`입니다. CMake가 자동으로 컴파일에 넣습니다.

`signals`와 `slots`는 매크로입니다.

```cpp
#define signals public
#define slots           /* 빈 매크로 */
#define emit            /* 빈 매크로 */
```

컴파일러에게 시그널은 **public 멤버 함수**입니다. moc만 `signals:` 블록을 보고 시그널로 등록합니다.

Designer의 `widget.ui`를 C++로 바꾸는 도구는 **uic**(UI Compiler)이고 moc와 다릅니다.

```text
widget.h   -- moc --> moc_widget.cpp
widget.ui  -- uic --> ui_widget.h
```

---

## 12. 컴파일러와 moc, 목적 파일과 실행 파일

### 12.1 moc의 입력은 보통 `.h`

일반 컴파일러:

```text
widget.h + widget.cpp  →  widget.o
```

moc:

```text
widget.h (Q_OBJECT가 있는 선언)  →  moc_widget.cpp
```

moc는 보통 `widget.cpp`를 읽지 않습니다. 시그널/슬롯/`Q_OBJECT`가 헤더의 클래스 선언에 있기 때문입니다. 예외로 `Q_OBJECT`를 `.cpp`에 두면 moc가 `.cpp`를 읽고 `#include "xxx.moc"`로 넣는 방식도 있습니다.

그 다음 진짜 컴파일:

```text
widget.h ── moc ──→ moc_widget.cpp ── 컴파일러 ──→ moc_widget.o ─┐
     │                                                          ├─ 링크 → 실행 파일
widget.cpp ────────────────────── 컴파일러 ──→ widget.o ────────┘
widget.ui ── uic ──→ ui_widget.h (widget.cpp가 include)
```

### 12.2 “기계어”가 들어간 말

혼란의 핵심은 **기계어 = 실행 파일**이 아니라는 점입니다. 기계어는 CPU가 읽는 명령 형식이고, `.o`와 실행 파일은 그 명령을 담는 상자가 다릅니다.

| 말 | 실제 의미 |
| --- | --- |
| 기계어 | CPU가 이해하는 명령 비트열 |
| 어셈블리어 | 그 기계어를 사람이 읽게 적은 텍스트 (`mov rax, 1`) |
| 바이너리 | 텍스트가 아닌 바이트 데이터. `.o`, 실행 파일, 이미지 모두 해당될 수 있음 |
| 네이티브 코드 | 그 컴퓨터 CPU용 기계어. Qt/C++ 결과물 |
| 바이트코드 | JVM 등 가상머신용 중간 코드. CPU 기계어 아님 |
| 목적 파일 (`.o`) | 한 `.cpp`를 번역한 **미완성 조각**. 기계어 + 심볼 + 재배치 정보 |
| 실행 파일 | `.o`들과 라이브러리를 이은 **완성본** |
| 오브젝트 코드 | 목적 파일 안의 기계어를 가리키는 말 |

같은 기계어가 세 곳에 나옵니다.

```text
소스 코드     widget.cpp          사람이 쓴 텍스트
     ↓ 컴파일
어셈블리      (안 볼 수도 있음)    기계어의 텍스트 표기
     ↓
목적 파일     widget.o            한 파일분의 기계어 + 아직 구멍 있는 심볼
     ↓ 링크
실행 파일     app/MacOS/...       합쳐진 기계어 + 라이브러리 연결 정보
     ↓ 실행
CPU                               그 기계어 명령을 수행
```

`.o`에도 기계어가 있고 실행 파일에도 있습니다. 차이점은 **완성도**입니다.

### 12.3 `.o` 안에 무엇이 있나

`widget.cpp`를 컴파일한 `widget.o`는 함수 몸체를 CPU 명령으로 바꿉니다. 다만 `QString::arg`처럼 Qt 라이브러리 함수 주소는 아직 모릅니다.

```text
widget.o (macOS: Mach-O relocatable object)
├─ 헤더          CPU 종류, 섹션 목록
├─ __TEXT        기계어 (함수 코드)
├─ __DATA        초기값 있는 전역 변수
├─ 심볼 테이블    제공: Widget::slot_valueChanged / 필요: QString::arg, ...
└─ 재배치 정보    “이 위치의 주소는 나중에 고쳐 써라”
```

Linux는 ELF `.o`, Windows는 COFF `.obj`입니다. 개념은 같습니다.

혼자 실행이 안 되는 이유: `main`이 이 파일에 없을 수 있고, 외부 주소가 비어 있으며, OS가 프로그램으로 인정하는 형태가 아닙니다.

### 12.4 링커와 실행 파일

```text
widget.o + moc_widget.o + main.o + Qt 라이브러리
     ↓ 링커
실행 파일
```

링커는 코드를 한 주소 공간에 놓고, 구멍을 메우며, Qt처럼 공유 라이브러리는 “실행 시 찾아라”는 기록만 남깁니다. 시작 주소(`main`)를 헤더에 씁니다.

macOS Qt 위젯 앱은 보통 앱 번들입니다.

```text
P01_SignalSlot.app/
└─ Contents/
   ├─ MacOS/P01_SignalSlot     ← 실제 실행 파일 (Mach-O)
   ├─ Frameworks/              ← Qt 등 .dylib
   └─ Info.plist
```

실행 파일 구조는 `.o`와 비슷하지만 OS가 로드할 정보가 더 있습니다. `__TEXT`가 CPU가 읽는 기계어입니다. `.o`의 `__TEXT`도 기계어지만 주소가 아직 임시입니다.

실행 순간 OS가 파일을 메모리에 올리고, Qt dylib를 올리며, 동적 링커가 `QString::arg` 주소를 채운 뒤 `main()`부터 실행합니다.

---

## 13. 콜백과 비교, 적용 범위

시그널/슬롯은 콜백의 특수 형태입니다. “함수 주소를 저장해 두고 나중에 호출한다”는 점은 같고, Qt는 연결 목록·수명·스레드·타입 정보를 붙입니다.

### 13.1 무엇이 다른가

일반 콜백은 호출하는 쪽이 함수를 들고 있습니다.

```cpp
button->onClick = []{ ... };
```

시그널/슬롯은 발행/구독입니다. 보내는 쪽은 누가 받는지 몰라도 됩니다. `QSlider`는 `Widget::slot_valueChanged`를 모릅니다.

| | 일반 콜백 | Qt 시그널/슬롯 |
| --- | --- | --- |
| 결합 | 호출자가 대상을 앎 | 보낸 쪽은 받는 쪽을 몰라도 됨 |
| 개수 | 1개이거나 리스트를 직접 관리 | `connect`로 N:M |
| 끊기 | 직접 제거, 수명 실수 잦음 | `QObject` 파괴 시 연결도 정리 |
| 스레드 | 직접 동기화/큐잉 | `QueuedConnection`이 값을 복사해 전달 |
| 타입 검사 | 함수 포인터면 컴파일 타임 | 새 `connect`도 컴파일 타임 |
| 필요 조건 | 함수만 있으면 됨 | 시그널을 내려면 `QObject` + moc |

### 13.2 장점

- **느슨한 결합.** 슬라이더는 UI 클래스를 몰라도 됩니다.
- **한 시그널에 여러 슬롯.** 로그, 화면, 전송을 동시에 연결할 수 있습니다.
- **수명 관리.** `Widget`이 사라지면 그 객체로 가던 연결이 끊깁니다.
- **스레드.** 같은 프로세스 안에서 인자를 복사해 상대 스레드 이벤트 루프로 보냅니다.
- **UI.** 클릭, 슬라이더, `readyRead`처럼 “나중에 일어남”에 잘 맞습니다.

### 13.3 단점

- Direct여도 연결 목록을 순회하므로 일반 함수 호출보다 느립니다.
- `emit` 다음이 슬롯으로 바로 안 보이고 moc/`activate`를 거칩니다.
- 시그널을 쓰려면 moc, 이벤트 루프, 스레드 친화도가 필요합니다.
- 반환값이 약합니다.
- Direct면 `emit` 도중에 슬롯이 다시 시그널을 낼 수 있습니다 (이 예제가 그 경우).
- 원격은 기본 기능이 아닙니다.

콜백이 나은 경우: 짧은 수명, 동기 결과, 성능이 중요한 내부 루프, `QObject`가 필요 없는 순수 C++.

### 13.4 스레드

같은 프로세스의 다른 스레드까지는 Qt가 지원합니다.

```cpp
connect(sender, &A::foo, receiver, &B::bar);
// AutoConnection:
//   같은 스레드 → Direct
//   다른 스레드 → Queued
```

조건:

- 받는 쪽이 `QObject`이고, 그 스레드에서 이벤트 루프가 돌아야 합니다.
- Queued면 인자를 복사할 수 있어야 합니다. `int`, `QString`은 됩니다. 커스텀 타입은 `qRegisterMetaType`이 필요할 수 있습니다.
- 다른 스레드 객체를 가리키는 포인터를 인자로 넘기는 것은 위험합니다. 복사된 주소일 뿐 객체는 공유입니다.

한계는 **프로세스 안, 이벤트 루프가 있는 `QObject` 사이**입니다. 인터럽트 핸들러에서 `emit` 하는 용도가 아닙니다.

### 13.5 원격

`connect` 한 줄이 소켓 너머로 전달되지는 않습니다. 같은 프로세스 메모리 전제입니다.

| 범위 | Qt가 해 주는가 |
| --- | --- |
| 같은 객체 | 됨 |
| 같은 스레드, 다른 객체 | 됨 (`connect`) |
| 다른 스레드, 같은 프로세스 | 됨 (Queued) |
| 다른 프로세스, 같은 머신 | 기본 `connect`로는 안 됨. D-Bus, Qt Remote Objects, 로컬 소켓 |
| 네트워크 너머 | 기본 안 됨. QtRO, gRPC, 직접 프로토콜 |

원격으로 쓰려면 직렬화 + 전송 + 반대편에서 다시 `emit`이 필요합니다. Qt Remote Objects는 시그널/슬롯/프로퍼티를 프로세스·네트워크로 복제합니다. 직접 구현하면 `QTcpSocket`으로 보내고 받는 앱에서 다시 시그널을 냅니다.

원격에는 지연, 유실, 상대 프로세스 종료처럼 로컬 시그널에 없던 문제가 생깁니다.

### 13.6 개념을 어디까지 쓸 수 있는가

컴퓨터 과학에서 이 패턴은 Observer / Pub-Sub입니다. 적용 범위는 구현이 어디까지 “전달”을 해 주느냐로 갈립니다.

```text
1. 함수 포인터/람다      같은 스레드, 직접 호출
2. Qt Direct            같은 스레드, 연결 목록 + 수명
3. Qt Queued            스레드 경계, 값 복사 + 이벤트 루프
4. 프로세스 버스         D-Bus, QtRO, IPC
5. 네트워크 Pub-Sub      MQTT, gRPC stream 등
```

1–3은 Qt `connect`가 담당합니다. 4–5는 같은 생각을 다른 전송에 올린 것입니다.

잘 맞는 곳: UI 입력, 비동기 I/O 완료, “상태가 바뀌었다”는 알림.  
안 맞는 곳: 매 프레임 핫패스, 반환값이 필요한 API, 인터럽트 핸들러, 성공/실패를 바로 받아야 하는 원격 호출(그건 RPC).

---

## 14. Qt가 시그널/슬롯을 구현한 방식

시그널/슬롯은 언어 기능이 아닙니다. **연결 목록 + moc가 만든 호출 코드 + `QMetaObject::activate`**로 구현된 옵저버입니다.

```text
[선언]  Q_OBJECT, signals, slots     ← 매크로 + moc가 읽는 표시
[생성]  moc_widget.cpp               ← 메타 테이블, 시그널 함수, qt_static_metacall
[연결]  QObject::connect             ← Connection 노드를 두 리스트에 삽입
[방출]  emit → 시그널 함수 → activate ← 리스트를 돌며 슬롯 호출
[전달]  Direct / Queued / Blocking   ← 같은 스레드면 즉시, 다르면 이벤트
[정리]  QObject 소멸                 ← 양쪽 리스트에서 Connection 제거
```

| 구성 | 역할 |
| --- | --- |
| `QMetaObject` | 클래스별 시그널/슬롯 목록 (읽기 전용 테이블) |
| `QObjectPrivate::Connection` | “이 시그널 → 저 슬롯” 한 줄 |
| `QSlotObjectBase` | 새 문법용 호출 래퍼 (멤버 포인터/람다) |

### 14.1 moc가 만드는 메타 테이블

클래스당 `staticMetaObject`가 하나 있습니다. 인스턴스가 아니라 타입이 시그널/슬롯 목록을 가집니다.

```text
Widget::staticMetaObject
  superdata     → QWidget::staticMetaObject → … → QObject
  stringdata    → "Widget", "sig_textChanged", "slot_valueChanged", ...
  data          → 메서드 개수, 시그널 번호, 인자 타입
  static_metacall → Widget::qt_static_metacall
```

시그널이 앞에, 슬롯이 뒤에 붙습니다. 연결 벡터는 시그널만 세는 인덱스를 씁니다.

시그널 함수 본문 예:

```cpp
void Widget::sig_textChanged(QString _t1)
{
    void *_a[] = {
        nullptr,
        const_cast<void*>(reinterpret_cast<const void*>(&_t1))
    };
    QMetaObject::activate(this, &staticMetaObject, /*local signal index*/ 0, _a);
}
```

인덱스로 실제 함수를 부르는 `qt_static_metacall` 예:

```cpp
void Widget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Widget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->sig_textChanged(*reinterpret_cast<QString*>(_a[1])); break;
        case 1: _t->slot_valueChanged(*reinterpret_cast<int*>(_a[1])); break;
        case 2: _t->slot_textChanged(*reinterpret_cast<QString*>(_a[1])); break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        // &Widget::sig_textChanged 와 비교해서 시그널 번호 반환
    }
}
```

새 문법 `connect(..., &QSlider::valueChanged, ...)`은 문자열이 아니라 멤버 함수 포인터를 `IndexOfMethod`로 넘겨 시그널 번호를 얻습니다.

### 14.2 Connection 노드

`connect`가 만드는 구조는 대략 이렇습니다.

```text
Connection
├─ sender / receiver
├─ signal_index
├─ connectionType     Auto / Direct / Queued / BlockingQueued
├─ isSlotObject       새 문법인가
├─ slotObj 또는 callFunction + method_relative
├─ argumentTypes      Queued일 때 복사할 QMetaType
├─ nextConnectionList 같은 시그널의 다음 슬롯
└─ next / prev        이 수신자가 받는 연결들 (소멸 시 정리)
```

각 `QObject`에 리스트가 두 방향입니다.

```text
보내는 쪽 (hSlider)
  connections.signalVector[valueChanged]
      → Connection → Connection → ...     (방출할 때 순회)

받는 쪽 (Widget)
  connections.senders
      ↔ Connection ↔ Connection           (죽을 때 역방향 해제)
```

양방향이라 어느 쪽이 먼저 파괴돼도 남은 쪽이 죽은 슬롯을 호출하지 않습니다. 연결 추가는 뮤텍스로 보호합니다.

### 14.3 새 문법 connect가 하는 일

```cpp
connect(ui->hSlider, &QSlider::valueChanged, this, &Widget::slot_valueChanged);
```

템플릿이 컴파일 타임에 검사합니다.

- 보내는 클래스에 `Q_OBJECT`가 있는가
- 슬롯 인자 수 ≤ 시그널 인자 수
- 인자 타입이 변환 가능한가

통과하면 시그널 인덱스를 찾고, `QSlotObject`에 `&Widget::slot_valueChanged`를 저장한 뒤, `Connection`을 양쪽 리스트에 넣습니다.

`QSlotObject`는 통일된 호출 인터페이스입니다.

```cpp
slotObj->call(receiver, argv);
// 내부에서:
(static_cast<Widget*>(receiver)->*pmf)(*reinterpret_cast<int*>(argv[1]));
```

람다 `connect`도 같은 베이스입니다. 그래서 슬롯이 `slots:`에 없어도 됩니다.

### 14.4 activate

연결이 하나도 없으면 비트마스크만 보고 바로 return 합니다. 연결 없는 `emit`은 거의 공짜입니다.

연결이 있으면 리스트를 순회합니다. 방출 **중에** `connect`된 새 연결은 이번 회차에 넣지 않습니다.

각 Connection:

```text
Auto + 다른 스레드  → queued_activate (복사 후 postEvent)
Queued              → queued_activate
BlockingQueued      → 이벤트 넣고 세마포어로 대기
Direct              → 뮤텍스 풀고 슬롯 호출, 다시 잠금
```

Direct일 때 `QObject::sender()`가 동작하도록, 수신자에 “지금 보낸 쪽 = hSlider”를 잠깐 기록합니다.

이 앱은 GUI 스레드 하나라 전부 Direct입니다. `activate`가 재귀로 한 번 더 들어갑니다.

```text
valueChanged(42)
  → slot_valueChanged(42)
      emit sig_textChanged(str)
        → slot_textChanged(str)
```

### 14.5 Queued와 파괴

Queued는 `QMetaCallEvent`(`QEvent::MetaCall`)입니다. 네트워크 소켓, 타이머와 같은 이벤트 큐입니다.

`~Widget()`은 `senders`를 순회해 슬라이더 쪽 리스트에서도 Connection을 뺍니다. 슬라이더가 먼저 죽으면 반대 방향으로 같은 일을 합니다.

### 14.6 예제 메모리 그림

```text
ui->hSlider
  signalVector[valueChanged]
      Connection {
        receiver = Widget this
        slotObj  = QSlotObject{ &Widget::slot_valueChanged }
        type     = Auto → Direct
      }

Widget this
  signalVector[sig_textChanged]
      Connection {
        receiver = this
        slotObj  = QSlotObject{ &Widget::slot_textChanged }
      }
  senders ↔ 위의 두 Connection
```

Qt 소스에서 본체가 되는 파일:

- `qtbase/src/corelib/kernel/qobject.cpp` — `connect`, `activate`, `queued_activate`
- `qtbase/src/corelib/kernel/qobject_p.h` — `Connection`, `QMetaCallEvent`
- 빌드 디렉터리 `moc_widget.cpp` — 이 클래스에 moc가 만든 코드

---

## 15. 사용된 디자인 패턴

본체는 **Observer(관찰자)** 입니다. 클래스가 `Observer`라는 이름을 쓰지는 않습니다.

### 15.1 Observer / Publish-Subscribe

| GoF Observer | Qt |
| --- | --- |
| Subject | 시그널을 가진 `QObject` (`hSlider`) |
| `notify()` | `emit` → `activate()` |
| Observer | 슬롯 / 람다 |
| `attach` / `detach` | `connect` / `disconnect` |
| 구독 목록 | `signalVector`의 `Connection` 리스트 |

일반 Observer와 다른 점:

- `update()` 같은 전용 인터페이스가 없습니다. 아무 멤버 함수나 람다가 슬롯이 됩니다.
- 객체마다 시그널이 여러 개입니다.
- 알림에 타입이 있는 인자가 따라갑니다.
- 구독자가 Subject를 몰라도 됩니다.

구현은 Observer이고, 쓰는 느낌은 Pub-Sub에 가깝습니다. 토픽이 시그널 이름/번호입니다.

양방향 등록(시그널 리스트 + 수신자 `senders`)은 Observer의 수명 문제를 풀기 위한 구현입니다.

### 15.2 보조 패턴

**Type Object (메타클래스) / Flyweight** — `QMetaObject`  
클래스당 테이블이 하나이고, 같은 클래스 인스턴스가 공유합니다. moc가 이 타입 객체를 생성합니다.

**Command** — `QSlotObject`, `QMetaCallEvent`  
“나중에 이 객체의 이 함수를, 이 인자로 호출하라”를 객체로 만든 것입니다. Queued는 Command를 복사해 다른 스레드 큐에 넣습니다. `slotObj->call(...)`이 `Command::execute()`에 해당합니다.

**Adapter** — `QSlotObjectBase`  
멤버 포인터, 람다, 옛 moc 인덱스를 `call(receiver, argv)` 하나로 맞춥니다. `activate()`는 대상 종류를 모릅니다.

**Strategy** — 연결 타입  
리스트 순회 알고리즘은 같고, 한 연결을 Direct / Queued / BlockingQueued로 실행하는 방법만 바꿉니다. Auto는 스레드를 보고 전략을 고릅니다.

**Reactor** — Queued 경로의 이벤트 루프  
`postEvent` → `QObject::event()` → `placeMetaCall()`. 시그널 시스템이 아니라 Qt 이벤트 디스패처에 올라갑니다.

**Pimpl** — `QObject` / `QObjectPrivate`  
연결 벡터와 `Connection`은 `d_ptr` 뒤에 있습니다.

**Code Generation** — moc  
C++에 리플렉션이 없어서 메타데이터와 시그널 함수를 생성합니다.

### 15.3 한 흐름에 모이는 모습

```text
connect(...)
    Adapter: QSlotObject 생성
    Observer: Connection을 signalVector에 attach

emit valueChanged(42)
    Type Object: 시그널 번호
    Observer: activate()가 리스트 순회
    Strategy: Auto → Direct
    Command: slotObj->call(this, {&42})
    → Widget::slot_valueChanged(42)

~Widget
    Observer detach: senders로 역방향 구독 해제
```

### 15.4 다른 패턴과 구분

| 패턴 | 시그널/슬롯인가 |
| --- | --- |
| Mediator | 아님. 발신 객체 안에 구독 목록이 있음. `QApplication`이 시그널을 중개하지 않음 |
| Chain of Responsibility | 아님. 슬롯을 순서대로 호출하지만 하나가 처리했다고 나머지를 막지 않음 |
| Callback | 패턴이라기보다 기법. 시그널/슬롯은 타입 있는 다중 콜백 + 수명 + 스레드 |
| MVC | Observer를 UI에 쓴 사례일 수 있음. 구현 자체는 MVC가 아님 |

한 문장으로: Qt는 Observer를 moc 메타데이터와 Command 큐로 구현한 시스템입니다.

---

## 16. 한눈에 보는 전체 흐름

```text
widget.h (Q_OBJECT, signals, slots)
    → moc → moc_widget.cpp (시그널 함수, 메타 테이블)
widget.ui
    → uic → ui_widget.h (hSlider, leText, leTextCopy)

컴파일 → .o 여러 개 → 링크 → 실행 파일
실행 → QApplication::exec() 이벤트 루프

Widget 생성
    QWidget(parent) → new Ui::Widget → setupUi(this)
    connect 두 줄 (예약만)

사용자 드래그
    QSlider::valueChanged(int)          Qt가 emit
      argv로 인자 주소 포장
      activate → Direct → slot_valueChanged
        leText 갱신
        emit sig_textChanged            우리가 emit
          activate → slot_textChanged
            leTextCopy 갱신
```

처음 쓸 때 기억할 것:

1. 보내는 객체, 시그널 주소, 받는 객체(`this`), 슬롯 주소를 `connect`한다.
2. `connect`는 호출이 아니라 예약이다.
3. 라이브러리 시그널은 Qt가 `emit`하고, 직접 만든 시그널은 직접 `emit`한다.
4. 인자는 같아도 되고, 슬롯이 더 적어도 된다. 더 많으면 안 된다.
5. 같은 스레드면 즉시 호출, 다른 스레드면 복사 후 이벤트 루프.
6. moc는 `.h`에서 `.cpp`를 만드는 코드 생성기다.
7. `private`은 여전히 접근 제한이고, `slots:`는 옛 매크로용 표시에 가깝다.

관련 예제 경로: `section03/P01_SignalSlot/`
