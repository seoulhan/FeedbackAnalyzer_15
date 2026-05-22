# [Phase 3-GREEN] BUG ① ~ ③ 수정 — RED 테스트를 GREEN으로
당신은 C++ 디버깅 및 아키텍트 전문가입니다.
phase 3 `red`에서 추가한 RED 테스트를 **모두 통과**시키는 최소 수정만 수행하세요.
Phase 2 `tests/test_analyzer.cpp`의 레거시 `sent()` 동작도 깨지 않게 유지하세요.

[디버깅 및 수정 사양]
1. BUG ① — `Constants.cpp` 중복 키워드 제거:
   - `SENTIMENT_KEYWORDS[u8"긍정"]`, `u8"부정"]` 맵 내 중복 push-back 제거, 1세트만 유지

2. BUG ② — `containsAny()` 통합:
   - `TextAnalyzer.h`, `Filters.h`의 중복 `containsAny` 제거
   - `TextUtils.h`에 `namespace TextUtils { inline bool containsAny(...) }` 단일 구현
   - 두 클래스에서 `#include "TextUtils.h"` 후 공통 호출

3. BUG ③ — Session API 개선:
   - key 무시·전역 의존 구조 개선
   - `FeedbackSession` 클래스: `getCurrent()`, `update(const vector<Feedback>&)`, `clear()` static API
   - 단위 테스트 후 `clear()`로 전역 상태 격리 가능하게 설계

[검증 루프]
- 수정마다: `cmake --build build --target feedback_analyzer_tests`
- `ctest --test-dir build --output-on-failure` → **전체 GREEN**

[산출물]
- `docs/bug_fix.md` : 수정 요약, RED→GREEN 대응 표, 변경 파일 목록

# wrap-up 작업
실행이 완료되면 다음과 같은 마무리 작업을 실행하세요.
 - 프로젝트 루트 아래 report\ 디렉토리에 프롬프트 처리 결과에 대한 보고서 파일을 markdown 포맷의 텍스트로 생성하세요. 파일명의 경우 순서대로 번호 접두사를 붙여 생성하세요.
 - 프로젝트 루트 아래 prompt\ 디렉토리에 입력 프롬프트 원문 markdown과 대화 전체를 Export Transcript 하세요. 파일명의 경우 보고서의 제목에서 report 를 제외하고, 순서대로 번호 접두사를 붙여 생성하세요.
 - 프로젝트 변경사항을 git에 checkout 하고 commit하세요.
