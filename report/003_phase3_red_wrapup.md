# Phase 3-RED — CMake GTest 연동 및 RED 테스트 — 작업 완료 보고

**일시:** 2026-05-22  
**워크스페이스:** FeedbackAnalyzer_15  
**지시:** CMake `feedback_analyzer_tests` 연동, BUG ①~③ RED 테스트 추가 (`src/cpp/` 프로덕션 비즈니스 로직 변경 없음)

---

## 수행 내용

1. 루트 `CMakeLists.txt`에 `enable_testing()`, GTest 연동, `feedback_analyzer_tests` 타겟, `gtest_discover_tests` 추가
2. `tests/test_bugfix_red.cpp` — BUG ① Constants 중복, BUG ② TextUtils/매칭 일치, BUG ③ Session 결함 RED 테스트
3. `tests/test_analyzer.cpp` — dangling reference 수정, `UnknownKeywords` 샘플 문장 보정
4. 빌드·`ctest` 검증 및 본 보고서·prompt Export 작성

---

## 산출물

| 경로 | 설명 |
|------|------|
| `CMakeLists.txt` | `feedback_analyzer_tests`, GTest(FetchContent 폴백), WINVER, ctest 등록 |
| `tests/test_bugfix_red.cpp` | RED 6케이스 (BUG ①②③) |
| `tests/test_analyzer.cpp` | Phase 2 안전망 (실행 안정·중립 샘플 보정) |
| `report/003_phase3_red_wrapup.md` | 본 파일 |
| `prompt/003_phase3_red_wrapup_raw.md` | 입력 프롬프트 원문 |
| `prompt/003_phase3_red_wrapup.md` | 대화 transcript Export |

---

## CMake 요약

- `enable_testing()` + `find_package(GTest QUIET)` — 미설치 시 **FetchContent**로 googletest 1.14.0 자동 포함
- `feedback_analyzer_tests`: `test_analyzer.cpp`, `test_bugfix_red.cpp`, `Constants.cpp`, `Filters.cpp`, `Session.cpp`, `TextAnalyzer.cpp`
- `target_link_libraries(... GTest::gtest)` — `test_analyzer.cpp`의 `main()` 유지
- Windows/MinGW: `WINVER=0x0A00`, `_WIN32_WINNT=0x0A00` (메인 타겟과 동일)
- `include(GoogleTest)` + `gtest_discover_tests(feedback_analyzer_tests)`

---

## RED 테스트 설계

| 테스트 | BUG | RED 근거 |
|--------|-----|----------|
| `SentimentKeywordsPositiveHasNoDuplicates` | ① | `긍정` 벡터 size=33, unique=20 |
| `SentimentKeywordsNegativeHasNoDuplicates` | ① | `부정` 벡터 size=35, unique=21 |
| `TextUtilsHeaderMustExist` | ② | `TextUtils.h` 없음 → `FAIL()` (GTEST_SKIP 없음) |
| `ContainsAnyUnifiedSentimentParity` | ② | Analyzer `중립` vs Filters `긍정` (예: "친절한 응대…") |
| `GetOldDataFromSessionIgnoresKey` | ③ | 서로 다른 key가 동일 `static` 벡터 참조 |
| `UpdateWithoutClearLeavesDataVisible` | ③ | `clear()` 없이 `update()` 후에도 데이터 잔존 |

**Fixture:** `Constants::init()`, `Filters::initFilterKeywords()` — BUG ②·분류 비교용

---

## 검증 결과 (`ctest --test-dir build --output-on-failure`)

| 구분 | 결과 |
|------|------|
| Phase 2 (`AnalyzerTestFixture.*`) | **5/5 PASS** |
| RED (`ConstantsRed*`, `TextUtilsRed*`, `SessionRed.*`) | **6/6 FAIL** (의도된 RED) |
| 전체 | 11 tests, exit code 8 (실패 포함 — RED 단계 정상) |

```text
cmake --build build --target feedback_analyzer_tests   # OK
ctest --test-dir build --output-on-failure             # 5 PASS, 6 FAIL
```

---

## 제약 준수

- `src/cpp/` 프로덕션 비즈니스 로직 **일절 변경 없음**
- `build/`, `gen_build.bat`, `refs/` 커밋 제외

---

## 테스트 파일 부가 수정 (프로덕션 외)

- `test_analyzer.cpp`: `kPositive()` 등 `static const std::string` 반환 — `-Wreturn-local-addr`·세그폴트 방지
- `UnknownKeywords`: `좋았습니다` 포함 문장 → 키워드 없는 `무난했습니다` 문장으로 변경 (`sent()` 중립 고정)

---

## 후속 (C_15/green)

1. `Constants.cpp` 중복 키워드 제거 → BUG ① GREEN
2. `TextUtils.h` + `containsAny` 통합, Analyzer/Filters 키워드 소스 정렬 → BUG ② GREEN
3. `FeedbackSession::getCurrent()` / `update()` / `clear()` → BUG ③ GREEN
