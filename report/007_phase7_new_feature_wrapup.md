# Phase 7 Wrap-up — 물리 CSV 저장 및 가중치 감성 스코어링

**일자:** 2026-05-22  
**입력:** Phase 7 신규 피처 프롬프트 (`refs/Guidebook_FeedbackAnalyzer.md`)  
**관련:** `docs/new_feature.md`, `report/006_phase5_6_srp_controller_service_wrapup.md`

---

## 1. 목표 및 결과

| 항목 | 결과 |
|------|------|
| `FileHandler::saveToCsv` 물리 디스크 저장 | **완료** |
| `saveResult()` stub 제거 | **완료** |
| `TextAnalyzer::classifyWeighted` | **완료** |
| `Filters` / `analyzeSentiment` 가중치 적용 | **완료** |
| `docs/new_feature.md` | **완료** |
| `ctest` (18건) | **PASS** |
| `feedback_analyzer` 빌드 | **OK** |

---

## 2. 변경 파일

| 경로 | 변경 |
|------|------|
| `src/cpp/FileHandler.h` | stub 제거, `saveToCsv` 선언 |
| `src/cpp/FileHandler.cpp` | **신규** — BOM·헤더·본문·에러 처리 |
| `src/cpp/TextUtils.h` | `countMatchingKeywords` |
| `src/cpp/TextAnalyzer.h/.cpp` | `classifyWeighted`, 분류 경로 위임 |
| `src/cpp/Filters.cpp` | `inferSentimentLabel` → 가중치 |
| `src/cpp/FeedbackService.cpp` | 필터 성공 시 `filtered_feedback.csv` 저장 |
| `tests/test_phase7_feature.cpp` | **신규** — 7건 |
| `tests/test_analyzer.cpp` | `MixedSentiment` → 중립 기대 |
| `CMakeLists.txt` | `FileHandler.cpp` 링크 |
| `docs/new_feature.md` | **신규** 명세 |

---

## 3. 기능 요약

### CSV 저장

- UTF-8 BOM + `text` 헤더 + 피드백 본문 줄 단위 기록.
- 빈 경로·스트림 오류 시 `false`.
- `FeedbackService::runFilter` 성공 시 작업 디렉터리 `filtered_feedback.csv` 생성.

### 가중치 감성

- 긍정/부정 키워드 **매칭 개수** 비교; 동점·미매칭 → 중립.
- 혼합 예: `최고` + `실망` 동점 → 레거시(긍정 우선) 대비 **중립**으로 개선.

---

## 4. 검증

```text
cmake --build build
ctest --test-dir build --output-on-failure   # 18/18 Passed
```

---

## 5. 후속 (Phase 8)

- `report.md` 전후 비교·정량 지표
- `/download`와 디스크 CSV 경로 통합 여부 검토
