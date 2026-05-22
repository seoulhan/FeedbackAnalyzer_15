# Phase 7 신규 기능 명세 — 물리 CSV 저장 및 가중치 감성 스코어링

**일자:** 2026-05-22  
**범위:** `FileHandler`, `TextAnalyzer::classifyWeighted`, `FeedbackService` 연동

---

## 1. 개요

Phase 7에서는 레거시 stub이던 파일 저장과 첫 매칭(first-match) 감성 분류를 다음 두 축으로 교체했습니다.

| 기능 | Before | After |
|------|--------|-------|
| 결과 저장 | `saveResult()` → `std::cout`만 출력 | `FileHandler::saveToCsv()` → 디스크 CSV |
| 감성 분류 | 긍정 키워드 우선 `containsAny` | 키워드 **개수** 비교 `classifyWeighted` |

---

## 2. `FileHandler::saveToCsv`

### API

```cpp
static bool saveToCsv(const std::vector<Feedback>& data, const std::string& path);
```

### 동작

1. `path`가 비어 있으면 `false`.
2. `std::ofstream`을 `binary | trunc`로 열고, 열기 실패 시 `false`.
3. UTF-8 BOM(`\xEF\xBB\xBF`) 기록.
4. 헤더 한 줄: `text`
5. 각 `Feedback::getText()`를 한 줄씩 기록 (필드 내 `,`, `"`, 줄바꿈은 RFC 스타일 이스케이프).
6. 스트림 상태가 정상이면 `true`, 중간 실패 시 `false`.

### 연동

`FeedbackService::runFilter`가 필터 성공 시 프로젝트 작업 디렉터리에 `filtered_feedback.csv`를 저장합니다. HTTP `/download`는 기존처럼 메모리의 `FilterResult`를 응답으로 직렬화합니다.

### 제거

- `saveResult()`, `save()` stub 및 `std::cout` 기반 가짜 저장 로직 전부 삭제.

---

## 3. `TextAnalyzer::classifyWeighted`

### 알고리즘

```
positiveCount = TextUtils::countMatchingKeywords(text, SENTIMENT_KEYWORDS["긍정"])
negativeCount = TextUtils::countMatchingKeywords(text, SENTIMENT_KEYWORDS["부정"])

if positiveCount > negativeCount → "긍정"
else if negativeCount > positiveCount → "부정"
else → "중립"   // 동점 또는 키워드 0개
```

`countMatchingKeywords`는 리스트의 각 키워드가 텍스트에 **한 번이라도** 포함되면 1점을 더합니다(부분 문자열 `find`).

### 적용 경로

- `TextAnalyzer::classifySentimentForText` → `classifyWeighted` 위임.
- `Filters::inferSentimentLabel` → `TextAnalyzer::classifyWeighted` 호출로 분석·필터 감성 라벨 일치 유지.

### 혼합 문장 예

| 텍스트 | 긍정 키워드 | 부정 키워드 | 결과 |
|--------|------------|------------|------|
| `품질은 최고인데 … 실망했어요.` | 최고 (1) | 실망 (1) | **중립** (레거시는 긍정 우선 → 긍정) |
| `최고라고 하지만 실망스럽고 불만… 최악` | 1 | 3 | **부정** |
| `정말 최고입니다. 만족스럽고 감사합니다.` | 다수 | 0 | **긍정** |

---

## 4. 테스트 (`tests/test_phase7_feature.cpp`)

| 테스트 | 검증 |
|--------|------|
| `ClassifyWeightedMixedTieIsNeutral` | 혼합·동점 → 중립 |
| `ClassifyWeightedNegativeDominates` | 부정 키워드 우세 |
| `ClassifyWeightedPositiveDominates` | 긍정 키워드 우세 |
| `ClassifyWeightedNoKeywordsIsNeutral` | 키워드 없음 → 중립 |
| `SaveToCsvWritesBomHeaderAndRows` | BOM·헤더·본문 |
| `SaveToCsvInvalidPathReturnsFalse` | 빈 경로 실패 |
| `SaveToCsvEmptyDataWritesHeaderOnly` | 데이터 없어도 헤더만 생성 |

`tests/test_analyzer.cpp`의 `MixedSentiment`는 가중치 스펙에 맞게 **중립** 기대값으로 갱신했습니다.

---

## 5. 빌드

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

`FileHandler.cpp`는 `feedback_analyzer`와 `feedback_analyzer_tests` 모두에 링크됩니다.
