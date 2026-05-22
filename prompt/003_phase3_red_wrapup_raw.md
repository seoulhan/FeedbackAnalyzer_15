# [Phase 3-RED] CMake GTest 연동 및 RED 테스트 추가

**User — 입력 프롬프트 원문**

---

# [Phase 3-RED] CMake GTest 연동 및 RED 테스트 추가
당신은 C++ QA 엔지니어입니다.
⚠️ `src/cpp/` 프로덕션 비즈니스 로직은 변경하지 마십시오.

[작업 1 — CMake]
- `CMakeLists.txt`에 `enable_testing()`, `find_package(GTest REQUIRED)`, `feedback_analyzer_tests` executable 추가
- `tests/test_analyzer.cpp` 및 분석에 필요한 최소 소스(`Constants.cpp`, `TextAnalyzer.cpp` 등) 링크
- `gtest_discover_tests` 또는 `add_test`로 ctest 등록
- Windows/MinGW 시 기존 `feedback_analyzer`와 동일한 WINVER 정의 적용

[작업 2 — RED 테스트 파일]
- `tests/test_bugfix_red.cpp` (또는 `tests/test_constants.cpp`, `tests/test_session.cpp` 등 역할별 분리) 생성
- `TEST_F` + `SetUp()`에서 `Constants::init()` 및 세션/필터 초기화가 필요하면 Fixture에서 수행

[BUG ① RED — Constants 중복 키워드]
- `Constants::init()` 후 `SENTIMENT_KEYWORDS[u8"긍정"]`, `u8"부정"]` 각 벡터에 **동일 문자열이 2회 이상 없음**을 검증
- 예: 벡터 크기 == `std::set`으로 중복 제거한 크기

[BUG ② RED — containsAny 단일 구현]
- `TextUtils.h`가 **아직 없거나** 중복 정의가 남아 있는 상태를 전제로, 향후 통합 스펙을 문서화하는 테스트 설계
- 예: `TextAnalyzer`와 `Filters`가 동일 입력에 대해 동일한 매칭 결과를 내야 함 (통합 후 GREEN)
- RED 단계에서는 `TextUtils` 미존재 시 **컴파일 실패 또는 명시적 `GTEST_SKIP` 없이 FAIL** 하도록 `#include "TextUtils.h"` 의존 테스트 추가 가능

[BUG ③ RED — Session 테스트 가능 API]
- `FeedbackSession::getCurrent()`, `update()`, `clear()` 인터페이스가 **아직 없으면** 테스트가 컴파일되지 않게 하거나, 기존 `Session`의 결함을 고정하는 테스트 작성
- 예: `update` 후 `clear` 하면 `getCurrent()`가 빈 벡터, 테스트 간 격리

[검증]
- `cmake --build build --target feedback_analyzer_tests`
- `ctest --test-dir build --output-on-failure`
- **기대:** Phase 2 (`test_analyzer.cpp`) PASS, BUG ①~③ RED 테스트 FAIL

[wrap-up 작업]
실행이 완료되면 다음과 같은 마무리 작업을 실행하세요.
 - 프로젝트 루트 아래 report\ 디렉토리에 프롬프트 처리 결과에 대한 보고서 파일을 markdown 포맷의 텍스트로 생성하세요. 파일명의 경우 순서대로 번호 접두사를 붙여 생성하세요.
 - 프로젝트 루트 아래 prompt\ 디렉토리에 입력 프롬프트 원문 markdown과 대화 전체를 Export Transcript 하세요. 파일명의 경우 보고서의 제목에서 report 를 제외하고, 순서대로 번호 접두사를 붙여 생성하세요.
