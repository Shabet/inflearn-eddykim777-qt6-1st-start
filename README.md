# Qt 6 프로그래밍 1편 실습

[[입문] Qt 6 프로그래밍 1편](https://inf.run/i7SX2) 강의를 따라가며 실습한 저장소입니다.

강의에서 다루는 Qt 6 기초를 CMake 멀티 프로젝트로 구성합니다. 섹션별로 디렉터리를 나누고, 루트 `CMakeLists.txt`에서 하위 프로젝트를 포함합니다.

## 프로젝트 구조

```
.
├── CMakeLists.txt
├── README.md
├── section02/
├── section03/
└── ...
```

각 `sectionXX` 디렉터리 아래에 해당 강의 섹션의 서브프로젝트가 위치합니다.

## 강의 섹션

| 섹션 | 주제 |
| --- | --- |
| Section 0 | Qt 6 소개와 특징 |
| Section 1 | Qt 6 프레임워크 설치 |
| Section 2 | Qt 프로그래밍 시작하기 |
| Section 3 | 시그널과 슬롯 |
| Section 4 | GUI 프로그래밍 - 위젯 |
| Section 5 | GUI 프로그래밍 - 레이아웃 |
| Section 6 | GUI 프로그래밍 - QMainWindow |
| Section 7 | 기본 자료형과 유용한 타입 |
| Section 8 | 컨테이너 클래스 |
| Section 9 | Model / View |
| Section 10 | STREAM |
| Section 11 | 파일 입출력 |

## 실행 방법

1. [Qt Creator](https://www.qt.io/product/development-tools)에서 루트 `CMakeLists.txt`를 엽니다.
2. Kit을 선택한 뒤 Configure합니다.
3. 실행할 서브 프로젝트(예: `P01_Console_Application`)를 선택하고 빌드합니다.

Qt 6.5 이상이 필요합니다.
