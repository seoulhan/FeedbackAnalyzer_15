# Phase 3 GREEN — BUG ①~③ 수정 요약

**일자:** 2026-05-22  
**목표:** Phase 3 RED 테스트 6건 + Phase 2 레거시 `sent()` 안전망 5건 전체 GREEN

---

## 수정 요약

| BUG | 문제 | 조치 |
|-----|------|------|
| ① | `Constants::SENTIMENT_KEYWORDS` 긍정·부정 벡터에 동일 블록이 2회 삽입 | 중복 블록 제거, 1세트만 유지 |
| ② | `TextAnalyzer` / `Filters`에 `containsAny` 중복, 감성 키워드 소스 불일치 | `TextUtils.h` 단일 구현, `Filters::fil()` 감성 분류를 `Constants::SENTIMENT_KEYWORDS`로 통일 |
| ③ | `Session::getOldDataFromSession(key)`가 key 무시, `clear()` 없음 | `FeedbackSession` (`getCurrent` / `update` / `clear` / `getByKey`), `Session`은 기존 API를 위임 |

---

## RED → GREEN 대응

| RED 테스트 | GREEN 조건 | 변경 |
|------------|------------|------|
| `ConstantsRedFixture.SentimentKeywordsPositiveHasNoDuplicates` | 긍정 벡터 unique | `Constants.cpp` 중복 제거 |
| `ConstantsRedFixture.SentimentKeywordsNegativeHasNoDuplicates` | 부정 벡터 unique | 동일 |
| `TextUtilsRedFixture.TextUtilsHeaderMustExist` | `TextUtils.h` 존재 | 신규 `src/cpp/TextUtils.h` |
| `TextUtilsRedFixture.ContainsAnyUnifiedSentimentParity` | Analyzer vs Filters 감성 일치 | `TextUtils::containsAny` + `Constants` 공유 |
| `SessionRedFixture.GetOldDataFromSessionIgnoresKey` | 서로 다른 key → 다른 저장소 | `FeedbackSession::keyedFeedbacks` |
| `SessionRedFixture.UpdateWithoutClearLeavesDataVisible` | `clear()` 후 empty | `FeedbackSession::clear()` (테스트에서 호출) |

Phase 2 `AnalyzerTestFixture.*` (5건): 변경 없이 PASS 유지.

---

## 변경 파일 목록

| 파일 | 변경 |
|------|------|
| `src/cpp/Constants.cpp` | BUG ① 중복 키워드 제거 |
| `src/cpp/TextUtils.h` | BUG ② 신규 — `TextUtils::containsAny` |
| `src/cpp/TextAnalyzer.h` | BUG ② `TextUtils` 사용 |
| `src/cpp/Filters.h` | BUG ② `TextUtils` + `Constants` 감성 키워드 |
| `src/cpp/Session.h` | BUG ③ `FeedbackSession` + `Session` 위임 |
| `src/cpp/Session.cpp` | BUG ③ static 정의 정리 |
| `tests/test_bugfix_red.cpp` | BUG ③ Session 테스트 — `FeedbackSession::clear()` 검증 |

**미변경 (호환):** `main.cpp`, `Filters.cpp` (`S_KEYWORDS`는 카테고리 필터 외 레거시 유지), `tests/test_analyzer.cpp`

---

## 검증

```powershell
cmake --build build --target feedback_analyzer_tests
ctest --test-dir build --output-on-failure
```

**결과:** 11/11 PASS (RED 6 + Phase 2 안전망 5)
