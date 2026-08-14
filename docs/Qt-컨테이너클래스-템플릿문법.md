# Qt 컨테이너 클래스와 `<>` 문법

Qt에서 `QList<tEmployee>`처럼 보이는 `<>`를 처음 만났을 때, Java의 `ArrayList<Employee>`와 같아 보일 수 있습니다. 겉모습은 같고, 역할도 “어떤 타입을 담을지 정한다”로 같습니다. 다만 C++은 **템플릿**, Java는 **제네릭**이라 컴파일 이후의 동작이 다릅니다.

이 문서는 컨테이너를 처음 쓰는 사람을 기준으로, Qt가 제공하는 컨테이너부터 사용법, 그리고 `<>`의 의미를 Java와 비교해 정리합니다.

관련 실습: `section08/P01_QList`

```cpp
QList<tEmployee> m_employeeList;
```

---

## 목차

1. [컨테이너가 하는 일](#1-컨테이너가-하는-일)
2. [Qt가 지원하는 컨테이너](#2-qt가-지원하는-컨테이너)
3. [사용법](#3-사용법)
4. [`<>` 문법 — Java 제네릭과 비교](#4--문법--java-제네릭과-비교)
5. [실습 코드에서 다시 보기](#5-실습-코드에서-다시-보기)
6. [한눈에 정리](#6-한눈에-정리)

---

## 1. 컨테이너가 하는 일

컨테이너는 **같은 종류의 데이터를 여러 개 담는 상자**입니다.

사원 한 명은 구조체(또는 클래스) 하나로 표현할 수 있습니다. 사원이 10명이면 변수 10개를 만드는 대신, 리스트 하나에 넣어 두고 꺼내 쓰면 됩니다.

Java를 쓴다면 이미 같은 일을 하고 있습니다.

```java
ArrayList<Employee> list = new ArrayList<>();
list.add(employee);
Employee e = list.get(0);
```

Qt / C++에서는 이렇게 씁니다.

```cpp
QList<tEmployee> list;
list.append(employee);
tEmployee e = list.at(0);
```

`ArrayList` 자리에 `QList`, `add` 자리에 `append`, `get` 자리에 `at`이 온다고 보면 시작하기에 충분합니다.

Java의 `List`, `Map`, `Set`이 `java.util` 패키지에 있듯이, Qt의 컨테이너는 Qt Core에 있습니다. GUI 위젯과 별개로, 데이터를 담기 위한 자료구조입니다.

---

## 2. Qt가 지원하는 컨테이너

Qt 6에서 자주 쓰는 컨테이너와 Java 대응을 먼저 보면, 나중에 코드를 읽을 때 선택이 쉬워집니다.

### 2.1 순서가 있는 컨테이너 (리스트 계열)

Java의 `List` / `ArrayList` / `Queue` / `Stack`에 해당합니다. 원소를 **넣은 순서**로 다루고, 인덱스로 접근합니다.

| Qt | Java에 가깝게 보면 | 한 줄 설명 |
| --- | --- | --- |
| `QList<T>` | `ArrayList<T>` | 가장 많이 쓰는 동적 배열. 실습의 `m_employeeList`가 이것 |
| `QStringList` | `ArrayList<String>` | `QList<QString>`에 문자열 전용 기능이 더해진 것 |
| `QStack<T>` | `Stack<T>` / `Deque` | 마지막에 넣은 것을 먼저 꺼냄 (LIFO) |
| `QQueue<T>` | `Queue<T>` / `ArrayDeque` | 먼저 넣은 것을 먼저 꺼냄 (FIFO) |

Qt 6에서는 `QList`와 `QVector`가 같은 구현입니다. 예전에 `QVector`가 연속 메모리 배열, `QList`가 다른 구조였지만, 지금은 `QList<T>`만 알아도 됩니다.

### 2.2 키로 찾는 컨테이너 (맵 / 해시)

Java의 `Map`에 해당합니다. “사원번호 1001번의 이름”처럼 **키 → 값**으로 찾습니다.

| Qt | Java에 가깝게 보면 | 한 줄 설명 |
| --- | --- | --- |
| `QMap<K, V>` | `TreeMap<K, V>` | 키 순서가 정렬됨. 탐색은 비교적 느리고, 순서 보장이 필요할 때 |
| `QHash<K, V>` | `HashMap<K, V>` | 해시 테이블. 보통 `QMap`보다 빠르고, 키 순서는 없음 |
| `QMultiMap<K, V>` | 키 하나에 값 여러 개 (`TreeMap` + `List`, 또는 Guava `Multimap`) | 같은 키를 여러 번 넣을 수 있음 |
| `QMultiHash<K, V>` | `HashMap` + `List`, 또는 Guava `HashMultimap` | `QHash`이면서 키 중복 허용 |

초심자 선택 기준은 단순합니다.

- 키 순서가 필요하면 `QMap`
- 속도가 중요하고 순서가 필요 없으면 `QHash`

### 2.3 중복 없는 집합

Java의 `Set`에 해당합니다.

| Qt | Java에 가깝게 보면 | 한 줄 설명 |
| --- | --- | --- |
| `QSet<T>` | `HashSet<T>` | 같은 값이 두 번 들어가지 않음. 포함 여부 확인에 유리 |

### 2.4 아직 쓰지 않아도 되는 것

`QLinkedList`는 Qt 6에서 제거되었습니다. `QVarLengthArray`는 작은 배열을 스택에 올려 성능을 내는 특수 컨테이너입니다. 처음에는 `QList`, `QStringList`, `QMap` / `QHash`, `QSet`만 알아도 강의와 실습을 따라갈 수 있습니다.

---

## 3. 사용법

공통 패턴은 Java와 같습니다.

1. `<>` 안에 **담을 타입**을 적는다.
2. 변수를 선언한다.
3. 넣고(`append` / `insert`), 꺼내고(`at` / `value`), 개수를 본다(`size`).

헤더는 컨테이너 이름과 같습니다. `QList`를 쓰려면 `#include <QList>`입니다. 다만 `QWidget` 등을 이미 포함했다면 간접적으로 들어오는 경우도 많습니다. 실습 `widget.h`는 `QList`를 직접 include하지 않았지만, Qt 헤더를 통해 사용할 수 있습니다. 다른 파일에서만 쓸 때는 include를 명시하는 편이 안전합니다.

### 3.1 `QList` — Java `ArrayList`

실습에서 쓰는 형태입니다.

```cpp
#include <QList>
#include <QString>

struct tEmployee {
    int num;
    QString name;
    QString part;
};

QList<tEmployee> list;           // ArrayList<Employee> list = new ArrayList<>();

tEmployee employee;
employee.num = 1001;
employee.name = "Kim";
employee.part = "개발";

list.append(employee);           // list.add(employee);
list.prepend(employee);          // 맨 앞에 삽입
list.insert(1, employee);        // list.add(1, employee);

int n = list.size();             // list.size()
bool empty = list.isEmpty();     // list.isEmpty()

tEmployee e1 = list.at(0);       // list.get(0)  — 범위 검사 있음
tEmployee e2 = list[0];          // list.get(0)  — 연산자, 범위 검사 없음
tEmployee first = list.first();
tEmployee last = list.last();

list.replace(0, employee);       // list.set(0, employee);
list.removeAt(0);                // list.remove(0);
list.removeFirst();
list.removeLast();
list.clear();                    // list.clear();

bool has = list.contains(employee);
int idx = list.indexOf(employee); // list.indexOf(employee)
```

Java `add`가 Qt에서는 주로 `append`입니다. `push_back`도 같은 뜻입니다. STL(`std::vector`)을 쓰는 코드와 섞일 때 `push_back`을 보는 경우가 있습니다.

인덱스는 0부터 시작합니다. 개수가 3이면 유효 인덱스는 `0, 1, 2`입니다. 실습의 출력 루프가 이 방식입니다.

```cpp
for (qsizetype i = 0; i < m_employeeList.size(); i++) {
    tEmployee e = m_employeeList.at(i);
}
```

`qsizetype`은 Qt가 컨테이너 크기에 쓰는 정수 타입입니다. Java의 `int`로 `size()`를 받던 습관 대신, Qt 6에서는 `qsizetype`을 쓰는 편이 맞습니다.

범위 기반 for도 Java의 enhanced for와 같습니다.

```cpp
for (const tEmployee &e : m_employeeList) {
    // e.name, e.part, ...
}
```

```java
for (Employee e : employeeList) {
    // e.name, e.part, ...
}
```

C++에서 `const tEmployee &e`는 “복사하지 말고 읽기만 하겠다”는 뜻입니다. Java는 원래 참조를 돌리므로 `&`에 해당하는 문법이 없습니다.

### 3.2 `QStringList` — 문자열 전용 리스트

`QList<QString>`을 써도 되지만, 문자열을 모을 때는 `QStringList`가 편합니다. Java로 치면 `ArrayList<String>`에 `join`, `filter` 비슷한 메서드가 붙어 있는 형태입니다.

```cpp
#include <QStringList>

QStringList names;
names << "Kim" << "Lee" << "Park";   // add를 여러 번 한 것과 비슷

QString joined = names.join(", ");   // "Kim, Lee, Park"  String.join(", ", names)

QStringList parts = QString("a,b,c").split(",");  // "a,b,c".split(",")

bool ok = names.contains("Kim");
names.sort();
```

`<<`는 Qt에서 리스트에 원소를 이어 붙일 때 자주 보는 연산자입니다. `names.append("Kim")`과 같은 일을 더 짧게 쓴 것입니다.

### 3.3 `QStack` / `QQueue`

```cpp
#include <QStack>
#include <QQueue>

QStack<int> stack;
stack.push(10);          // stack.push(10)
stack.push(20);
int top = stack.pop();   // 20  — stack.pop()
int peek = stack.top();  // 남은 꼭대기

QQueue<QString> queue;
queue.enqueue("첫번째");  // queue.offer("첫번째")
queue.enqueue("두번째");
QString first = queue.dequeue();  // "첫번째"  — queue.poll()
```

실행 취소(undo)처럼 “방금 한 일”을 되돌릴 때는 스택, 대기열처럼 “먼저 온 사람”을 처리할 때는 큐를 씁니다.

### 3.4 `QMap` / `QHash` — Java `TreeMap` / `HashMap`

사원번호로 사원을 찾고 싶다면 리스트를 처음부터 끝까지 도는 대신 맵을 씁니다.

```cpp
#include <QMap>
#include <QHash>

QMap<int, QString> map;          // TreeMap<Integer, String>
map.insert(1001, "Kim");         // map.put(1001, "Kim")
map[1002] = "Lee";               // map.put(1002, "Lee")

QString name = map.value(1001);  // map.get(1001)  — 없으면 빈 QString
QString name2 = map.value(9999, "없음");  // getOrDefault(9999, "없음")

bool exists = map.contains(1001); // map.containsKey(1001)
map.remove(1001);                 // map.remove(1001)

QList<int> keys = map.keys();     // map.keySet()
QList<QString> values = map.values();
```

`QHash` 사용법은 `QMap`과 거의 같습니다. 클래스 이름만 바꾸면 됩니다.

```cpp
QHash<int, QString> hash;
hash.insert(1001, "Kim");
```

차이만 기억하면 됩니다.

- `QMap`: 키를 꺼내면 정렬되어 있음 (1, 2, 1001 순서)
- `QHash`: 키 순서는 보장하지 않음. 보통 더 빠름

Java에서 `TreeMap`과 `HashMap`을 고르는 기준과 같습니다.

키와 값을 같이 순회하는 방법은 대략 다음과 같습니다.

```cpp
for (auto it = map.begin(); it != map.end(); ++it) {
    int num = it.key();
    QString name = it.value();
}
```

Java의 `entrySet()` 루프에 해당합니다.

```java
for (Map.Entry<Integer, String> e : map.entrySet()) {
    int num = e.getKey();
    String name = e.getValue();
}
```

Qt 5.10 이후에는 이런 형태도 됩니다.

```cpp
for (auto [num, name] : map.asKeyValueRange()) {
    // num, name
}
```

### 3.5 `QSet` — Java `HashSet`

```cpp
#include <QSet>

QSet<QString> parts;
parts.insert("개발");      // set.add("개발")
parts.insert("개발");      // 두 번째 삽입은 무시됨
parts.insert("기획");

bool ok = parts.contains("개발");  // set.contains("개발")
parts.remove("기획");              // set.remove("기획")
```

부서 목록처럼 “같은 값이 한 번만 있으면 되는” 데이터에 맞습니다.

### 3.6 `QMultiMap` — 키 하나에 값 여러 개

한 부서에 사원이 여러 명일 때, Java라면 `Map<String, List<Employee>>`를 직접 만들기도 합니다. Qt는 그 패턴을 `QMultiMap` / `QMultiHash`로 제공합니다.

```cpp
#include <QMultiMap>

QMultiMap<QString, QString> byPart;
byPart.insert("개발", "Kim");
byPart.insert("개발", "Lee");
byPart.insert("기획", "Park");

QList<QString> devs = byPart.values("개발");  // "Kim", "Lee"
```

### 3.7 메서드 이름 빠른 대조

같은 일을 하는 이름이 Qt와 Java에서 조금씩 다릅니다.

| 하고 싶은 일 | Qt (`QList`) | Java (`ArrayList`) |
| --- | --- | --- |
| 맨 뒤에 넣기 | `append` / `push_back` | `add` |
| 인덱스에 넣기 | `insert` | `add(index, e)` |
| 꺼내기 | `at(i)` / `operator[]` | `get(i)` |
| 바꾸기 | `replace` | `set` |
| 지우기 | `removeAt` | `remove` |
| 개수 | `size` / `count` | `size` |
| 비었는지 | `isEmpty` | `isEmpty` |
| 비우기 | `clear` | `clear` |
| 포함 여부 | `contains` | `contains` |
| 위치 찾기 | `indexOf` | `indexOf` |

맵은 `insert` ↔ `put`, `value` ↔ `get`, `contains` ↔ `containsKey`입니다.

---

## 4. `<>` 문법 — Java 제네릭과 비교

실습 한 줄이 이 장의 출발점입니다.

```cpp
QList<tEmployee> m_employeeList;
```

`QList`는 “리스트”라는 **틀**이고, `tEmployee`는 그 틀에 넣을 **타입**입니다. `<>`가 그 둘을 연결합니다.

Java도 같은 그림을 씁니다.

```java
ArrayList<Employee> employeeList = new ArrayList<>();
```

`ArrayList`가 틀, `Employee`가 타입입니다. 그래서 `<>`를 보면 “제네릭이구나”라고 생각하기 쉽습니다. **문법의 역할은 같고, 컴파일러가 뒤에서 하는 일은 다릅니다.**

### 4.1 같은 점 — 담을 타입을 컴파일할 때 정한다

둘 다 잘못된 타입을 넣으면 컴파일 단계에서 막습니다.

```cpp
QList<tEmployee> list;
list.append(123);        // 컴파일 에러. int는 tEmployee가 아님
```

```java
ArrayList<Employee> list = new ArrayList<>();
list.add(123);           // 컴파일 에러. int는 Employee가 아님
```

이것이 `<>`를 쓰는 첫 번째 이유입니다. `QList`만 쓰면 “아무거나 담는 상자”가 되고, `QList<tEmployee>`는 “사원만 담는 상자”가 됩니다.

꺼낼 때도 타입이 이미 정해져 있습니다.

```cpp
tEmployee e = m_employeeList.at(0);   // 캐스트 불필요
```

```java
Employee e = employeeList.get(0);     // 캐스트 불필요
```

제네릭이 없던 시절의 Java `ArrayList`는 `Object`를 담았고, 꺼낼 때 `(Employee)` 캐스트가 필요했습니다. `<>`는 그 불편과 실수를 없애기 위한 문법입니다. C++ 템플릿도 같은 목적을 다른 방식으로 달성합니다.

### 4.2 다른 점 1 — 템플릿은 타입마다 코드를 만든다

C++ 템플릿은 컴파일러가 `QList<tEmployee>`를 보는 순간, **`tEmployee` 전용 QList 코드**를 만들어 냅니다. `QList<int>`를 쓰면 `int` 전용 코드를 또 만듭니다.

그래서 다음 두 변수는 **완전히 다른 타입**입니다.

```cpp
QList<int> a;
QList<tEmployee> b;
// a = b;   // 컴파일 에러
```

Java 제네릭은 컴파일 때 `Employee`를 검사한 뒤, 런타임에는 그 정보를 지웁니다. 이를 **타입 소거(type erasure)** 라고 합니다. 바이트코드에서 `ArrayList<Integer>`와 `ArrayList<Employee>`는 둘 다 그냥 `ArrayList`입니다.

```java
ArrayList<Integer> a = new ArrayList<>();
ArrayList<Employee> b = new ArrayList<>();
// 런타임 클래스는 둘 다 ArrayList
```

그래서 Java에는 raw type이라는 구멍이 있습니다.

```java
ArrayList list = employeeList;   // <> 를 떼면 Object 리스트처럼 취급
list.add("문자열");              // 컴파일될 수 있음
Employee e = employeeList.get(0); // 런타임 ClassCastException 가능
```

C++ `QList<tEmployee>`에는 이런 raw type이 없습니다. `tEmployee`가 아니면 처음부터 들어가지 않습니다.

한 줄로 말하면 다음과 같습니다.

- Java: 컴파일 때만 검사하고, 실행 파일에는 `ArrayList` 하나
- C++: 타입마다 리스트 구현을 실제로 만들어 냄

### 4.3 다른 점 2 — `int` 같은 기본 타입

C++ 템플릿은 `int`를 그대로 담습니다.

```cpp
QList<int> nums;         // OK
nums.append(10);
```

Java 제네릭은 참조 타입만 받습니다. `int`는 안 되고 `Integer`로 감싸야 합니다.

```java
ArrayList<int> nums;         // 컴파일 에러
ArrayList<Integer> nums;     // OK  (박싱)
nums.add(10);
```

Qt 실습에서 `QList<int>`를 쓰는 것은 자연스러운 일이고, Java 감각으로 “박싱이 필요하겠지”라고 생각할 필요는 없습니다.

### 4.4 다른 점 3 — 값 vs 참조

이 차이가 실무에서 가장 자주 헷갈립니다.

`QList<tEmployee>`는 사원 **복사본**을 저장합니다. `append(employee)`는 리스트 안에 별도의 `tEmployee`를 만듭니다.

```cpp
tEmployee e;
e.name = "Kim";
m_employeeList.append(e);
e.name = "Lee";
// 리스트 안의 원소 이름은 여전히 "Kim"
```

Java `ArrayList<Employee>`는 **참조(주소)** 를 저장합니다. 리스트에 넣은 뒤 원본을 바꾸면 리스트 안 객체도 바뀝니다.

```java
Employee e = new Employee();
e.name = "Kim";
list.add(e);
e.name = "Lee";
// list.get(0).name 도 "Lee"
```

Java에서 값처럼 복사되게 하려면 새 객체를 만들어 넣어야 합니다. C++에서 Java처럼 공유하려면 포인터 리스트를 씁니다.

```cpp
QList<tEmployee *> pointerList;   // 주소만 담음. Java ArrayList에 더 가까움
```

초심자 실습(`P01_QList`)은 값 복사 방식입니다. `append` 이후 입력란을 `clear()`해도, 이미 리스트에 들어간 사원 정보는 그대로입니다.

### 4.5 다른 점 4 — 상속이 `<>` 밖으로 퍼지지 않는다

Java는 와일드카드로 “Employee의 하위 타입 리스트”를 표현할 수 있습니다.

```java
List<? extends Employee> list = new ArrayList<Manager>();  // OK
```

C++ 템플릿은 기본적으로 **불변(invariant)** 입니다. `Manager`가 `Employee`를 상속해도 `QList<Manager>`를 `QList<Employee>`에 넣지 못합니다. 상속 관계는 `<>` 안의 타입에 자동으로 전파되지 않습니다.

처음에는 “`QList<A>`와 `QList<B>`는 A와 B가 부모-자식이어도 다른 타입”이라고만 기억하면 됩니다.

### 4.6 `<>` 안에 들어가는 것

`<>` 안에는 담을 원소의 타입이 들어갑니다.

```cpp
QList<int>             nums;
QList<QString>         names;
QList<tEmployee>       employees;
QMap<int, QString>     idToName;     // 키가 int, 값이 QString
QHash<QString, int>    nameToAge;
QSet<QString>          parts;
```

맵은 타입이 두 개입니다. Java `HashMap<String, Integer>`의 `<>`와 같은 자리입니다.

`tEmployee`처럼 직접 만든 구조체도 넣을 수 있습니다. 실습 `widget.h`가 그 예입니다.

```cpp
typedef struct _tEmployee {
    int num;
    QString name;
    QString part;
} tEmployee;

QList<tEmployee> m_employeeList;
```

Java로 옮기면 대략 다음입니다.

```java
class Employee {
    int num;
    String name;
    String part;
}

ArrayList<Employee> employeeList = new ArrayList<>();
```

`typedef struct ... tEmployee`는 C 스타일로 구조체에 별명을 붙인 것입니다. “사원 한 명의 모양을 `tEmployee`라고 부르자”는 뜻이고, `<>` 안에 그 이름을 넣은 것입니다.

---

## 5. 실습 코드에서 다시 보기

`section08/P01_QList`의 흐름은 컨테이너 사용의 전형적인 예입니다.

1. 멤버로 `QList<tEmployee>`를 선언한다.
2. 저장 버튼에서 입력값을 `tEmployee`에 담고 `append`한다.
3. 출력 버튼에서 `size()`와 `at(i)`로 전체를 읽어 `textEdit`에 보여 준다.

선언:

```cpp
QList<tEmployee> m_employeeList;
```

저장 (`slot_pbtSave`):

```cpp
tEmployee employee;
employee.num = num;
employee.name = name;
employee.part = part;

m_employeeList.append(employee);   // Java: list.add(employee)
```

출력 (`slot_pbtPrint`):

```cpp
for (qsizetype i = 0; i < m_employeeList.size(); i++) {
    int num = m_employeeList.at(i).num;
    QString name = m_employeeList.at(i).name;
    QString part = m_employeeList.at(i).part;
    // ...
}
```

Java로는 같은 프로그램이 이렇게 대응됩니다.

```java
private ArrayList<Employee> employeeList = new ArrayList<>();

employeeList.add(employee);

for (int i = 0; i < employeeList.size(); i++) {
    Employee e = employeeList.get(i);
}
```

이름을 맞춰 보면 `QList` ↔ `ArrayList`, `append` ↔ `add`, `at` ↔ `get`, `<>` 안의 `tEmployee` ↔ `Employee`입니다.

---

## 6. 한눈에 정리

| 질문 | 답 |
| --- | --- |
| 컨테이너가 뭔가 | 같은 타입의 데이터를 여러 개 담는 상자. Java Collection과 같은 역할 |
| 실습에서 쓰는 것 | `QList<tEmployee>` — Java `ArrayList<Employee>` |
| 리스트 / 맵 / 집합 | `QList` / `QMap`·`QHash` / `QSet` |
| `QMap` vs `QHash` | 정렬이 필요하면 Map, 속도면 Hash. `TreeMap` vs `HashMap`과 같음 |
| `<>`가 뭔가 | 담을 타입을 지정하는 문법. Java 제네릭과 **겉모습·목적은 같음** |
| C++과 Java의 차이 | C++은 타입마다 코드를 생성(템플릿). Java는 컴파일 후 타입을 지움(제네릭) |
| `int`를 담을 수 있나 | C++ `QList<int>`는 가능. Java는 `ArrayList<Integer>` |
| 복사인가 참조인가 | `QList<tEmployee>`는 값 복사. Java `ArrayList`는 참조 |

처음 읽을 때 이것만 기억해도 됩니다.

`QList<tEmployee>`는 “사원만 담는 리스트”이다. `<>`는 Java `ArrayList<Employee>`와 같은 자리의 문법이다. 다만 C++은 타입마다 진짜로 다른 리스트를 만들고, Java는 실행 시점에 그 차이를 지운다.
