# Phase 4 Refactoring Wrap-up — 네이밍·상수화·비대 함수 분리

**일자:** 2026-05-22  
**입력:** Phase 4 리팩토링 프롬프트 (`docs/analysis.md` 기반)  
**관련:** `report/004_phase3_green_wrapup.md`, `docs/analysis.md`

---

## 1. 목표 및 결과

| 항목 | 결과 |
|------|------|
| 도메인 메서드/변수 네이밍 | **완료** |
| 매직 넘버·문자열 상수화 (`DomainConstants`) | **완료** |
| 20줄+ 메서드 SRP 분리 (TextAnalyzer, Filters) | **완료** |
| `ctest` (11건) | **PASS** |
| `feedback_analyzer` 빌드 | **OK** |

---

## 2. 네이밍 변경

| 이전 | 이후 |
|------|------|
| `TextAnalyzer::sent()` | `analyzeSentiment()` |
| `TextAnalyzer::kw()` | `analyzeKeywords()` |
| `Filters::fil()` | `filter()` |
| `main.cpp` `fil_data` | `FilterResult filterResult` (`FilterResult.h`) |
| `Session::getOldDataFromSession()` | 제거 → `getCurrentFeedbacks()` (GET `/`), `getFeedbacksByKey()` (키별 저장) |

구현 본문은 헤더 인라인에서 `TextAnalyzer.cpp` / `Filters.cpp`로 이동.

---

## 3. 상수화 (`DomainConstants` in `Constants.h`)

- 감성 라벨: `SENTIMENT_POSITIVE`, `SENTIMENT_NEUTRAL`, `SENTIMENT_NEGATIVE`
- 필터: `FILTER_ALL`, `CATEGORY_SUBKEY_MAIN`
- 수치: `COUNT_INITIAL`, `HTTP_SERVER_PORT`, `URL_ENCODED_HEX_*`, `CSV_FIRST_FIELD_INDEX`

---

## 4. SRP 헬퍼 분리

### TextAnalyzer (private)

- `createEmptySentimentCounts`, `classifySentimentForText`, `incrementSentimentCount`
- `createEmptyKeywordCounts`, `countKeywordsForText`

### Filters (private)

- `inferSentimentLabel`, `applySentimentFilter`, `applyKeywordFilter`, `logFilteredItems`

---

## 5. 검증 로그

```text
cmake --build build --target feedback_analyzer_tests  # OK
ctest --test-dir build --output-on-failure           # 11/11 Passed
cmake --build build --target feedback_analyzer       # OK
```

---

## 6. 산출물

| 경로 | 설명 |
|------|------|
| `src/cpp/FilterResult.h` | 필터 결과 캐시 구조체 |
| `src/cpp/TextAnalyzer.cpp`, `Filters.cpp` | 분리된 구현 |
| `report/005_phase4_refactoring_wrapup.md` | 본 보고서 |
| `prompt/005_phase4_refactoring_wrapup_raw.md` | 입력 프롬프트 원문 |
| `prompt/005_phase4_refactoring_wrapup.md` | 대화 Export Transcript |

---

## 7. 테스트 갱신

- `tests/test_analyzer.cpp`: `analyzeSentiment()` 호출
- `tests/test_bugfix_red.cpp`: `filter()`, `analyzeSentiment()`, `getFeedbacksByKey()` 반영

동작 스펙(긍정 우선 → 부정 → 중립, 키별 세션 격리)은 유지.
