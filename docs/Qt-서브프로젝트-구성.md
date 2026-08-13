# Qt 서브프로젝트 구성 절차서

이 저장소는 **루트 하나 + 섹션별 서브프로젝트** CMake 멀티 프로젝트입니다.  
Qt Creator에서 새 예제를 만들면 보통 루트에 폴더가 생깁니다. 그 폴더를 `sectionXX` 아래로 옮기고 CMake에 등록하는 절차를 정리합니다.

대상 환경:

- macOS
- Qt Creator
- 빌드 시스템: **CMake** (qmake / `.pro` 사용하지 않음)
- Qt 6.5 이상

---

## 1. 이 저장소가 돌아가는 방식

빌드는 항상 **저장소 루트의 `CMakeLists.txt`** 가 시작점입니다. Qt Creator에서도 이 파일을 엽니다.

```
저장소 루트
├── CMakeLists.txt          ← 여기만 연다
├── section02/
│   ├── CMakeLists.txt      ← 섹션 안의 예제들을 등록
│   ├── P01_Console_Application/
│   ├── P02_GUI_Example/
│   └── P03_GUI_app/
└── section03/
    ├── CMakeLists.txt
    └── P01_SignalSlot/
```

포함 관계는 한 줄입니다.

1. 루트 `CMakeLists.txt` → `add_subdirectory(sectionXX)`
2. `sectionXX/CMakeLists.txt` → `add_subdirectory(P0N_예제이름)`
3. `P0N_예제이름/CMakeLists.txt` → 실제 실행 파일 정의

루트에 예제 폴더를 직접 `add_subdirectory` 하지 않습니다.

---

## 2. 이름 규칙

작업을 시작하기 전에 이름을 정합니다.

| 대상 | 규칙 | 예 |
| --- | --- | --- |
| 섹션 디렉터리 | `section` + 두 자리 번호 | `section02`, `section03`, `section04` |
| 예제 디렉터리 / 타깃 이름 | `P` + 두 자리 번호 + `_` + 영문 이름 | `P01_SignalSlot`, `P02_GUI_Example` |
| 섹션 안 번호 | 그 섹션 안에서만 1부터 | section03의 첫 예제는 `P01_...` |

같은 이름(`P01_...`)이 섹션마다 있어도 됩니다. CMake 타깃 이름은 저장소 전체에서 겹치지 않게 하세요. 지금은 섹션마다 예제 이름이 다르므로 문제 없습니다.

---

## 3. 작업 전에 확인할 것

1. Qt Creator에서 **루트 `CMakeLists.txt`** 가 열려 있는지 확인합니다.
2. 새 예제가 들어갈 섹션 번호를 정합니다. (강의 Section 4면 `section04`)
3. 그 섹션에 이미 예제가 몇 개 있는지 보고, 다음 번호를 정합니다.
4. 빌드 시스템은 반드시 **CMake** 입니다. New Project 마법사에서 qmake를 고르지 마세요.

---

## 4. 전체 흐름 (한눈에)

지금 쓰는 방식은 아래와 같습니다.

```
1. Qt Creator에서 새 프로젝트를 저장소 루트에 만든다
2. 루트에 P0N_이름 폴더가 생긴다
3. sectionXX 가 없으면 만든다
4. P0N_이름 폴더를 sectionXX 안으로 통째로 옮긴다
5. CMakeLists.txt 두 곳(루트, 섹션)을 고친다
6. Qt Creator에서 CMake를 다시 돌린다
7. 실행 타깃을 새 예제로 바꿔 빌드한다
```

`sectionXX` 가 이미 있으면 3번은 건너뜁니다.

---

## 5. 절차 A — Qt Creator에서 예제 프로젝트를 루트에 만들기

예제는 **먼저 루트에 만든 뒤** 옮깁니다. Qt Creator 마법사가 위치를 루트로 잡기 쉽기 때문입니다.

### 5.1 새 프로젝트 시작

1. Qt Creator 메뉴 **File → New Project...** (`⌘N`)
2. 왼쪽에서 **Application** 을 고릅니다.
3. 강의 내용에 맞는 템플릿을 고릅니다.

| 만들고 싶은 것 | 템플릿 |
| --- | --- |
| 콘솔만 | Qt Console Application |
| 위젯 창, `.ui` 없음 | Qt Widget Application |
| 위젯 창 + Designer 폼 | Qt Widget Application, **Generate form** 체크 |

4. **Choose...**

### 5.2 이름과 위치

1. **Name**: `P01_SignalSlot` 처럼 디렉터리/타깃 이름을 그대로 넣습니다. 공백 금지.
2. **Create in**: 저장소 **루트** 경로를 지정합니다.  
   예: `/Users/나/work/inflearn-eddykim777-qt6-1st-start`
3. 하위에 `P01_SignalSlot` 폴더가 새로 생기는지 미리보기에서 확인합니다.
4. **Next**

루트가 아니라 `section03` 안을 Create in 으로 잡아도 됩니다. 그렇게 하면 이후 이동은 생략할 수 있습니다. 다만 실수로 다른 경로에 만들 때가 많아서, 이 문서의 기본은 **루트에 만들고 옮기기** 입니다.

### 5.3 빌드 시스템

1. **Build system** 에서 **CMake** 를 고릅니다.
2. qmake / Qbs 는 고르지 않습니다.
3. **Next**

### 5.4 클래스 / 폼 (위젯 프로젝트인 경우)

1. Class name 은 기본 `Widget` 을 그대로 써도 됩니다.
2. 폼이 필요하면 `.ui` 생성을 켭니다.
3. **Next**

### 5.5 Kit

1. Qt 6.5 이상 Kit 을 선택합니다. (예: Qt 6.11.1 for macOS)
2. **Next → Finish**

### 5.6 생성된 폴더 확인

저장소 루트에 아래가 생겨야 합니다.

```
P01_SignalSlot/
├── CMakeLists.txt
├── main.cpp
├── widget.h          (위젯 프로젝트)
├── widget.cpp        (위젯 프로젝트)
└── widget.ui         (폼을 켠 경우)
```

이 시점에는 아직 루트에 있어도 됩니다. 바로 실행해 동작을 확인한 뒤 옮겨도 됩니다.

Qt Creator가 루트 `CMakeLists.txt` 에 `add_subdirectory(P01_SignalSlot)` 를 넣어 주는 경우가 있습니다. **나중에 반드시 지웁니다.** 루트에는 섹션만 둡니다.

---

## 6. 절차 B — 섹션 디렉터리가 없을 때 만들기

새 강의 섹션의 첫 예제라면 `sectionXX` 가 없습니다.

터미널에서 저장소 루트로 이동합니다.

```bash
cd /경로/inflearn-eddykim777-qt6-1st-start
```

섹션 폴더를 만듭니다. Section 4 라면:

```bash
mkdir section04
```

### 6.1 `section04/CMakeLists.txt`

섹션 CMake는 예제 폴더만 등록합니다. `project()` 나 `find_package()` 는 넣지 않습니다.

```cmake
add_subdirectory(P01_예제이름)
```

예제를 더 만들면 같은 파일에 줄을 추가합니다.

```cmake
add_subdirectory(P01_예제이름)
add_subdirectory(P02_다음예제)
```

### 6.2 `section04/README.md` (권장)

섹션이 무엇을 다루는지, 예제 폴더가 어떤 템플릿인지 적어 둡니다.

```markdown
# Section 4

GUI 프로그래밍 - 위젯

| 디렉터리 | Qt Creator 템플릿 |
| --- | --- |
| `P01_예제이름` | Qt Widget Application |
```

### 6.3 루트 `CMakeLists.txt` 에 섹션 등록

루트 파일은 대략 이렇게 되어 있습니다.

```cmake
cmake_minimum_required(VERSION 3.19)
project(qt6-1st-start LANGUAGES CXX)

add_subdirectory(section02)
add_subdirectory(section03)
```

맨 아래에 한 줄 추가합니다.

```cmake
add_subdirectory(section04)
```

순서는 강의 섹션 번호 순으로 맞춥니다.

**하지 말 것:** 루트에 `add_subdirectory(P01_예제이름)` 를 넣는 것. 예제는 항상 섹션을 거쳐서 들어갑니다.

---

## 7. 절차 C — 예제 폴더를 섹션 안으로 통째로 이동

소스만 복사하지 않습니다. `CMakeLists.txt` 와 `.ui` 를 포함해 **디렉터리 전체를 이동** 합니다.

저장소 루트에서:

```bash
mv P01_예제이름 section04/
```

Finder로 옮겨도 됩니다. 결과는 같아야 합니다.

```
section04/
├── CMakeLists.txt
├── README.md
└── P01_예제이름/
    ├── CMakeLists.txt
    ├── main.cpp
    └── ...
```

루트에 `P01_예제이름/` 이 남아 있으면 안 됩니다.

이미 git 에 추적 중인 폴더라면 `mv` 대신:

```bash
git mv P01_예제이름 section04/
```

아직 untracked 이면 일반 `mv` 로 충분합니다.

### 7.1 예제 `CMakeLists.txt` 는 수정하지 않아도 됩니다

예제 안의 CMake는 상대 경로만 쓰므로, 폴더를 옮겨도 보통 그대로 동작합니다.

확인만 합니다.

- `project(P01_예제이름 ...)`
- `qt_add_executable(P01_예제이름 ...)`
- `main.cpp`, `widget.cpp` 등 파일 이름이 실제 파일과 일치하는지

콘솔 예제는 `Core` 만, 위젯 예제는 `Core Widgets` 가 `find_package` / `target_link_libraries` 에 있어야 합니다.

---

## 8. 절차 D — 이미 있는 섹션에 예제만 추가할 때

`section03` 처럼 폴더가 이미 있으면 섹션을 새로 만들지 않습니다.

1. Qt Creator로 루트에 `P02_다음예제` 를 만든다. (절차 A)
2. 폴더를 옮긴다.

   ```bash
   mv P02_다음예제 section03/
   ```

3. `section03/CMakeLists.txt` 에 한 줄 추가한다.

   ```cmake
   add_subdirectory(P01_SignalSlot)
   add_subdirectory(P02_다음예제)
   ```

4. 루트 `CMakeLists.txt` 는 건드리지 않는다. (`section03` 은 이미 등록되어 있다.)
5. 루트에 `add_subdirectory(P02_다음예제)` 가 생겼다면 그 줄만 삭제한다.
6. 섹션 `README.md` 표에 새 예제를 한 줄 넣는다.

---

## 9. 절차 E — CMake 정리 체크리스트

이동이 끝나면 아래를 순서대로 확인합니다.

### 루트 `CMakeLists.txt`

- [ ] `add_subdirectory(section02)` 처럼 **섹션만** 있다
- [ ] 새 섹션이면 `add_subdirectory(sectionXX)` 가 있다
- [ ] `add_subdirectory(P01_...)` 같은 **예제 직접 등록이 없다**

### `sectionXX/CMakeLists.txt`

- [ ] 파일이 있다
- [ ] `add_subdirectory(P0N_예제이름)` 이 실제 폴더 이름과 같다
- [ ] `project()` / `find_package()` 가 없다

### `sectionXX/P0N_예제이름/`

- [ ] 루트가 아니라 섹션 아래에 있다
- [ ] `CMakeLists.txt`, 소스, `.ui`(있다면) 가 같이 있다
- [ ] 루트에 같은 이름 폴더가 남아 있지 않다

---

## 10. 절차 F — Qt Creator에서 다시 Configure

경로가 바뀌었으므로 이전 CMake 캐시는 낡은 경로를 가리킬 수 있습니다.

### 10.1 CMake 다시 실행

1. 왼쪽 **Projects** 모드 (또는 `⌘5`)
2. 현재 Kit 의 **Build** 설정
3. **Run CMake** 버튼을 누른다

또는 메뉴 **Build → Run CMake**.

오류 없이 끝나면 프로젝트 트리에 `sectionXX / P0N_예제이름` 이 보여야 합니다.

### 10.2 그래도 실패하면 빌드 디렉터리 초기화

1. Qt Creator에서 프로젝트를 닫는다.
2. 저장소의 `build/` 폴더를 지운다. (`build/` 는 gitignore 대상이다.)
3. 루트 `CMakeLists.txt` 를 다시 연다.
4. Kit 을 고르고 Configure 한다.

### 10.3 실행 타깃 바꾸기

여러 실행 파일이 있으므로 기본 타깃이 이전 예제일 수 있습니다.

1. 왼쪽 아래 (또는 상단) **Run** 구성에서 새 타깃 `P0N_예제이름` 을 고른다
2. 빌드 (`⌘B`) 후 실행 (`⌘R`)

---

## 11. 자주 나는 문제

### CMake: `add_subdirectory` given source ... which is not an existing directory

예제 폴더를 옮겼는데 CMake에는 예전 경로가 남아 있습니다.

- 루트 `CMakeLists.txt` 의 `add_subdirectory(P01_...)` 를 지웠는지 확인
- 섹션 CMake의 이름이 실제 폴더명과 같은지 확인 (`P01_SignalSlot` vs `P01_signal_slot`)

### 타깃이 두 개 생기거나 이름이 충돌한다

같은 예제를 루트와 섹션에서 둘 다 `add_subdirectory` 한 상태입니다. 루트 쪽 줄을 지웁니다.

### Qt Creator 트리는 새 위치인데 빌드는 옛 경로를 찾는다

CMake를 다시 돌리거나 `build/` 를 지우고 Configure 합니다.

### 위젯 창이 안 뜨고 콘솔만 된다 / 링크 에러

`find_package(Qt6 ... COMPONENTS Core Widgets)` 와 `target_link_libraries(... Qt::Widgets)` 가 있는지 봅니다. 콘솔 템플릿으로 시작했다면 Widgets 가 없습니다.

### `.ui` 를 썼는데 빌드가 파일을 모른다

예제 `CMakeLists.txt` 의 `qt_add_executable(...)` 목록에 `widget.ui` 가 있어야 합니다.

### qmake 프로젝트(`.pro`)가 생겼다

마법사에서 CMake를 고르지 않은 것입니다. 그 폴더는 쓰지 말고, CMake로 다시 만듭니다.

---

## 12. 작업 예시 — `P01_SignalSlot` 을 `section03` 으로

실제로 한 일과 같은 순서입니다.

1. Qt Creator에서 루트에 `P01_SignalSlot` (Widget + form, CMake) 생성
2. `mkdir section03`
3. `section03/CMakeLists.txt` 작성

   ```cmake
   add_subdirectory(P01_SignalSlot)
   ```

4. `mv P01_SignalSlot section03/`
5. 루트 `CMakeLists.txt` 를 아래처럼 맞춤

   ```cmake
   cmake_minimum_required(VERSION 3.19)
   project(qt6-1st-start LANGUAGES CXX)

   add_subdirectory(section02)
   add_subdirectory(section03)
   ```

6. Qt Creator에서 Run CMake
7. 실행 타깃을 `P01_SignalSlot` 로 바꾸고 빌드

끝난 뒤 트리:

```
section03/
├── CMakeLists.txt
├── README.md
└── P01_SignalSlot/
    ├── CMakeLists.txt
    ├── main.cpp
    ├── widget.cpp
    ├── widget.h
    └── widget.ui
```

---

## 13. 다음 섹션을 추가할 때 복붙용

Section 5 첫 예제 `P01_Layout` 을 넣는다고 가정합니다.

```bash
# 1) Qt Creator로 루트에 P01_Layout 생성 (CMake)

# 2) 섹션이 없으면
mkdir section05

# 3) 이동
mv P01_Layout section05/
```

`section05/CMakeLists.txt`:

```cmake
add_subdirectory(P01_Layout)
```

루트 `CMakeLists.txt` 맨 아래:

```cmake
add_subdirectory(section05)
```

그다음 Qt Creator에서 **Run CMake** → 타깃 `P01_Layout` 선택.
