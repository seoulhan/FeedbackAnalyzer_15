# Phase 5 & 6 Wrap-up — SRP Controller / Service 분리

**일자:** 2026-05-22  
**입력:** Phase 5 & 6 리팩토링 프롬프트 (`docs/analysis.md` 기반)  
**관련:** `docs/refactoring.md`, `report/005_phase4_refactoring_wrapup.md`

---

## 1. 목표 및 결과

| 항목 | 결과 |
|------|------|
| `FeedbackController` Boundary 분리 | **완료** |
| `FeedbackService` Application 분리 | **완료** |
| `docs/refactoring.md` (Before/After·의존 명세) | **완료** |
| `main.cpp` 슬림화 (~383 → ~25줄) | **완료** |
| `ctest` (11건) | **PASS** |
| `feedback_analyzer` 빌드 | **OK** |

---

## 2. 신규 파일

| 경로 | 역할 |
|------|------|
| `src/cpp/FeedbackService.h/.cpp` | 세션·CSV import·분석·필터·`FilterResult` 캡슐화 |
| `src/cpp/FeedbackController.h/.cpp` | httplib 라우트, 폼 파싱, HTML/CSV 응답 |
| `docs/refactoring.md` | 아키텍처 비포/애프터·의존 구조 명세 |

---

## 3. 책임 분리 요약

### FeedbackController (Boundary)

- `registerRoutes`, `parseForm`, `urlDecode`, `renderPage`, `escapeHtml`
- 라우트: `GET /`, `POST /analyze`, `POST /upload`, `POST /filter`, `GET /download`
- 비즈니스 호출 없음 — `FeedbackService` DTO만 수신·뷰에 매핑

### FeedbackService (Application)

- `TextAnalyzer`, `Filters`, `FilterResult` 인스턴스 멤버 (전역 제거)
- `AnalysisSnapshot`, `FilterOutcome` DTO로 컨트롤러에 결과 전달

### main.cpp (Composition Root)

- 초기화 + `FeedbackService` / `FeedbackController` 생성 + `server.listen`

---

## 4. 검증 로그

```text
cmake --build build --target feedback_analyzer feedback_analyzer_tests  # OK
ctest --test-dir build --output-on-failure                           # 11/11 Passed
```

---

## 5. 산출물

| 경로 | 설명 |
|------|------|
| `docs/refactoring.md` | Phase 5&6 아키텍처 비교·의존 명세 |
| `report/006_phase5_6_srp_controller_service_wrapup.md` | 본 보고서 |
| `prompt/006_phase5_6_srp_controller_service_wrapup_raw.md` | 입력 프롬프트 원문 |
| `prompt/006_phase5_6_srp_controller_service_wrapup.md` | 대화 Export Transcript |

---

## 6. 테스트 영향

- `feedback_analyzer_tests` 소스 목록 **변경 없음** — 도메인 API(`TextAnalyzer`, `Filters`, `Session`) 동작 유지.
