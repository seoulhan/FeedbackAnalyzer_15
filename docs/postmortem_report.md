# 코드 리뷰 및 개선 보고서

**프로젝트:** FeedbackAnalyzer_15  
**기간:** Phase 1 ~ Phase 8 (2026-05-22)  
**참조:** `docs/analysis.md`, `docs/bug_fix.md`, `docs/refactoring.md`, `docs/new_feature.md`, `report/001` ~ `report/007`

---

## 1. 분석 배경 (프로젝트 대상 및 개선 목적 설명)

### 1.1 프로젝트 대상

`FeedbackAnalyzer_15`는 C++17 기반의 단일 프로세스 HTTP 피드백 분석 데모 애플리케이션이다. `httplib`로 폼·CSV 입력을 받고, 키워드 사전 기반 감성·카테고리 집계 및 필터링 결과를 HTML/CSV로 제공한다.

**초기 분석 범위 (13개 경로):** `main.cpp`, `TextAnalyzer`, `Filters`, `Session`, `Constants`, `Logger`, `UIComponents`, `FileHandler`, `Feedback` 등.

### 1.2 개선 목적

| 목적 | 설명 |
|------|------|
| **레거시 가시화** | 축약 API·중복 키워드·이중 데이터 저장소 등 6대 이슈를 문서화하고 심각도를 분류 |
| **회귀 방지** | Google Test 안전망 → RED/GREEN TDD로 BUG ①~③을 고정 |
| **구조 개선** | God Object `main.cpp` 해체, SRP 기반 Controller/Service 분리, 네이밍·상수화 |
| **기능 정합** | `FileHandler` stub 제거, 디스크 CSV 저장, 가중치 감성으로 분석·필터 일치 |
| **운영 가능성** | `ctest` 18건 GREEN, 단계별 `report/`·`docs/` 산출로 추적 가능한 리팩토링 이력 확보 |

### 1.3 단계별 진행 요약

| Phase | 내용 | 핵심 산출 |
|-------|------|-----------|
| 1 | 레거시 정밀 분석 (코드 미변경) | `docs/analysis.md` |
| 2 | GTest 안전망 스켈레톤 | `tests/test_analyzer.cpp` |
| 3-RED | CMake + BUG ①~③ RED | `tests/test_bugfix_red.cpp` |
| 3-GREEN | BUG 수정 | `TextUtils.h`, `FeedbackSession` |
| 4 | 네이밍·`DomainConstants`·SRP 헬퍼 분리 | `TextAnalyzer.cpp`, `Filters.cpp` |
| 5~6 | Controller/Service 분리 | `FeedbackController`, `FeedbackService`, `main` ~25줄 |
| 7 | CSV 물리 저장·가중치 감성 | `FileHandler.cpp`, `classifyWeighted` |
| 8 | 전후 비교 포스트모템 | 본 문서 |

---

## 2. Cursor AI 분석 요약 (항목별 심각도)

Phase 1 `docs/analysis.md` 및 Cursor AI 정밀 리뷰 결과를 심각도별로 정리한다.

| ID | 이슈 | 심각도 | 핵심 영향 | Phase에서의 처리 |
|----|------|--------|-----------|------------------|
| ① | `sent`, `kw`, `fil`, `fil_data` 축약명 | **Major** | API·전역명 가독성 저하, 데이터 소스 혼동 | Phase 4: `analyzeSentiment`, `analyzeKeywords`, `filter`, `FilterResult` |
| ② | `Constants.cpp` 감성 키워드 중복 등록 | **Major** | 설정 신뢰성 훼손, 메모리 낭비 | Phase 3-GREEN: 중복 블록 제거 |
| ③ | `containsAny()` TextAnalyzer/Filters 중복 | **Major** | 분석·필터 감성 불일치 | Phase 3-GREEN: `TextUtils::containsAny` 단일화 |
| ④ | 전역·static 상태 (`fil_data`, Session, Analyzer static 등) | **Critical** | 테스트 격리 불가, `fil_data`/Session 이원화 | Phase 5~6: 서비스 멤버 캡슐화; Phase 3: `FeedbackSession` |
| ⑤ | Session `key` 무시, dead API | **Major** | 거짓 추상화, 다중 세션 불가 | Phase 3-GREEN: `getByKey` / `clear` |
| ⑥ | `FileHandler::saveResult` stub, main 미연동 | **Major** | 저장 계약 미이행, dead instance | Phase 7: `saveToCsv` + `FeedbackService` 연동 |
| — | `kw` vs `fil` 카테고리 서브키 규칙 불일치 | **Warning** | UI 카테고리 대비 집계·필터 어긋남 가능 | Phase 4: `CATEGORY_SUBKEY_MAIN` 상수로 `"main"` 경로 통일 (필터는 Phase 4 이후 `main` 기준 정렬) |
| — | `Logger::debugMode = true` 기본값 | **Minor** | 운영 로그 노이즈 | 미변경 (범위 외) |
| — | `main.cpp` God Object (~383줄) | **Critical** (구조) | HTTP·뷰·비즈니스 결합 | Phase 5~6: Controller/Service, `main` 24줄 |

**심각도 분포 (초기 6대 + 구조):** Critical 2건, Major 5건, Warning 1건, Minor 1건 — **Critical/Major 7건은 Phase 3~7에서 해소 또는 완화.**

---

## 3. 개선/처리 내역 분석표

| 피드백 사항 | 수정 조치 내역 | 개선 결과 |
|-------------|----------------|-----------|
| BUG ① Constants 긍정·부정 키워드 2회 등록 | `Constants.cpp` 중복 `push_back` 블록 삭제 | RED 2건 → GREEN; 벡터 unique 보장 |
| BUG ② `containsAny` 이중 정의·`S_KEYWORDS` vs `Constants` 불일치 | `TextUtils.h` 공통화; `Filters` 감성 분기를 `Constants::SENTIMENT_KEYWORDS`로 통일 | Analyzer/Filters 동일 문장 동일 라벨 (`ContainsAnyUnifiedSentimentParity` PASS) |
| BUG ③ `getOldDataFromSession(key)` key 무시 | `FeedbackSession` (`getCurrent`, `update`, `clear`, `getByKey`); `Session` 위임 | 키별 저장소 분리·`clear()` 격리 검증 PASS |
| ① 축약 메서드명 | `analyzeSentiment`, `analyzeKeywords`, `filter`, `FilterResult` | 도메인 의도가 API·변수명에 반영 |
| ④ 전역 `fil_data`, `textAnalyzer`, `filters` | `FeedbackService` 인스턴스 멤버, DTO 반환 | `main` 전역 제거; Composition Root만 유지 |
| ④ God `main.cpp` | `FeedbackController` + `FeedbackService` | `main.cpp` 383줄 → **24줄**; HTTP/비즈니스 분리 (`docs/refactoring.md`) |
| ⑥ FileHandler stub | `FileHandler::saveToCsv` 구현; `saveResult` 제거 | 필터 성공 시 `filtered_feedback.csv` 디스크 기록; GTest 3건 |
| 레거시 first-match 감성 (긍정 우선) | `classifyWeighted` + `countMatchingKeywords` | 혼합·동점 문장 **중립**; 부정 우세 시 부정 (Phase 7 스펙) |
| 테스트 부재 | GTest 5 → 11 → **18**건, FetchContent 연동 | `ctest` **18/18 PASS** (2026-05-22 검증) |
| 매직 문자열·숫자 | `DomainConstants` (`SENTIMENT_*`, `HTTP_SERVER_PORT` 등) | 분석·필터·HTTP 설정 단일 상수 소스 |

---

## 4. 전후 코드 비교

### 4.1 Constants — 감성 키워드 중복 제거 (BUG ①)

**개선 전 (레거시, 중복 블록):**

```cpp
void Constants::init() {
    SENTIMENT_KEYWORDS[u8"긍정"] = {
        u8"좋아요", u8"만족", u8"감사", u8"최고", /* ... 1차 블록 ... */
        u8"좋아요", u8"만족", u8"감사", u8"최고"  /* 동일 세트 2차 반복 → size 33, unique 20 */
    };
    SENTIMENT_KEYWORDS[u8"부정"] = {
        u8"나쁘", u8"불만", /* ... */
        u8"나쁘", u8"불만"  /* 부정도 동일 패턴 중복 */
    };
}
```

**개선 후:**

```cpp
void Constants::init() {
    SENTIMENT_KEYWORDS[u8"긍정"] = {
        u8"좋아요", u8"만족", u8"감사", u8"최고", u8"좋은", u8"훌륭", u8"추천", u8"좋았어요",
        u8"좋습니다", u8"최고입니다", u8"감사합니다", u8"만족스럽", u8"좋았습니다",
        u8"최고에요", u8"기뻐요", u8"만족합니다", u8"굿", u8"최고다", u8"와우", u8"아주 좋아"
    };
    SENTIMENT_KEYWORDS[u8"부정"] = {
        u8"나쁘", u8"불만", u8"실망", u8"최악", u8"별로", u8"불편", u8"불만족", u8"문제",
        /* ... 단일 세트만 유지 ... */
    };
}
```

`Constants.h`에는 `DomainConstants`로 감성 라벨·HTTP 포트·CSV 인덱스 등이 상수화되어 분석 코드의 매직 문자열이 제거되었다.

---

### 4.2 TextUtils — `containsAny` 공통화 (BUG ②)

**개선 전 (`TextAnalyzer.h` / `Filters.h` 각각 동일 private static):**

```cpp
class TextAnalyzer {
private:
    static bool containsAny(const std::string& text,
                            const std::vector<std::string>& keywords) {
        for (const auto& kw : keywords) {
            if (text.find(kw) != std::string::npos) return true;
        }
        return false;
    }
    // Filters::fil()은 Filters::S_KEYWORDS로 감성 추론 → Constants와 불일치
};
```

**개선 후 (`src/cpp/TextUtils.h`):**

```cpp
namespace TextUtils {

inline bool containsAny(const std::string& text,
                        const std::vector<std::string>& keywords) {
    for (const auto& kw : keywords) {
        if (text.find(kw) != std::string::npos) {
            return true;
        }
    }
    return false;
}

inline int countMatchingKeywords(const std::string& text,
                                 const std::vector<std::string>& keywords) {
    int count = 0;
    for (const auto& kw : keywords) {
        if (text.find(kw) != std::string::npos) {
            ++count;
        }
    }
    return count;
}

}  // namespace TextUtils
```

`TextAnalyzer`·`Filters`는 private 중복을 제거하고 `TextUtils` + `Constants::SENTIMENT_KEYWORDS`만 참조한다.

---

### 4.3 Session — key 기반 저장 및 clear (BUG ③)

**개선 전:**

```cpp
class Session {
public:
    static std::vector<Feedback>& getOldDataFromSession(const std::string& key) {
        return currentFeedbacks;  // key 무시, 항상 동일 벡터
    }
    static void initSessionStateUgly() { /* no-op */ }
    // internalData, filterOptions — 미사용 dead field
};
```

**개선 후:**

```cpp
class FeedbackSession {
public:
    static std::vector<Feedback>& getCurrent() { return currentFeedbacks; }
    static void update(const std::vector<Feedback>& feedbacks) {
        currentFeedbacks = feedbacks;
    }
    static void clear() {
        currentFeedbacks.clear();
        keyedFeedbacks.clear();
    }
    static std::vector<Feedback>& getByKey(const std::string& key) {
        return keyedFeedbacks[key];
    }
};

class Session {
public:
    static void initSessionStateUgly() { FeedbackSession::clear(); }
    static std::vector<Feedback>& getFeedbacksByKey(const std::string& key) {
        return FeedbackSession::getByKey(key);
    }
    static std::vector<Feedback>& getCurrentFeedbacks() {
        return FeedbackSession::getCurrent();
    }
};
```

`GET /`는 `clear()`로 신규 세션을 보장하고, 키별 스냅샷은 `getFeedbacksByKey`로 분리된다.

---

### 4.4 FileHandler — CSV 실제 저장 (이슈 ⑥)

**개선 전 (`FileHandler.h` 인라인 stub):**

```cpp
class FileHandler {
public:
    void saveResult(const std::vector<Feedback>& data) {
        std::cout << "saveResult: " << data.size() << " items\n";
        for (const auto& item : data) {
            std::cout << item.getText() << "\n";
        }
    }
    void save(const std::vector<Feedback>& data) { saveResult(data); }
};
// main.cpp: static FileHandler fileHandler; — 호출 없음
// 실제 다운로드: GET /download가 fil_data만 직렬화
```

**개선 후 (`FileHandler.cpp`):**

```cpp
bool FileHandler::saveToCsv(const std::vector<Feedback>& data, const std::string& path) {
    if (path.empty()) return false;
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out.is_open()) return false;
    out << "\xEF\xBB\xBF";
    out << "text\n";
    for (const auto& item : data) {
        out << escapeCsvField(item.getText()) << "\n";
        if (!out.good()) return false;
    }
    out.flush();
    return out.good();
}
```

`FeedbackService::runFilter` 성공 시 작업 디렉터리 `filtered_feedback.csv`에 기록한다. HTTP `/download`는 메모리 `FilterResult` 직렬화를 유지한다.

---

### 4.5 가중치 감성 분석 (`classifyWeighted`)

**개선 전 (`TextAnalyzer::sent` / first-match):**

```cpp
std::map<std::string, int> sent(const std::vector<Feedback>& feedbacks) {
    // ...
    if (containsAny(text, SENTIMENT_KEYWORDS[u8"긍정"])) {
        result[u8"긍정"]++;
    } else if (containsAny(text, SENTIMENT_KEYWORDS[u8"부정"])) {
        result[u8"부정"]++;
    } else {
        result[u8"중립"]++;
    }
    // "최고" + "실망" 동시 포함 → 긍정 우선 → 긍정
}
```

**개선 후:**

```cpp
std::string TextAnalyzer::classifyWeighted(const std::string& text) {
    const int positiveCount = TextUtils::countMatchingKeywords(
        text, Constants::SENTIMENT_KEYWORDS[DomainConstants::SENTIMENT_POSITIVE]);
    const int negativeCount = TextUtils::countMatchingKeywords(
        text, Constants::SENTIMENT_KEYWORDS[DomainConstants::SENTIMENT_NEGATIVE]);

    if (positiveCount > negativeCount) {
        return DomainConstants::SENTIMENT_POSITIVE;
    }
    if (negativeCount > positiveCount) {
        return DomainConstants::SENTIMENT_NEGATIVE;
    }
    return DomainConstants::SENTIMENT_NEUTRAL;
}

std::string TextAnalyzer::classifySentimentForText(const std::string& text) {
    return classifyWeighted(text);
}
```

`Filters::inferSentimentLabel`도 동일 경로를 사용해 분석·필터 라벨이 일치한다.

---

### 4.6 Composition Root — `main.cpp` 슬림화

**개선 전:** ~383줄 — 라우트 람다, `renderPage`, `parseForm`, `urlDecode`, 전역 `fil_data` / `textAnalyzer` / `filters` / 미사용 `fileHandler`.

**개선 후:**

```cpp
int main() {
    Constants::init();
    Filters::initFilterKeywords();

    FeedbackService feedbackService;
    FeedbackController controller(feedbackService);

    httplib::Server server;
    controller.registerRoutes(server);

    Logger::logInfo(u8"서버가 http://localhost:" +
                    std::to_string(DomainConstants::HTTP_SERVER_PORT) + u8" 에서 시작됩니다.");
    server.listen("0.0.0.0", DomainConstants::HTTP_SERVER_PORT);
    return 0;
}
```

---

## 5. 정량적 효과 및 향후 과제

### 5.1 코드 스멜·구조 지표

| 지표 | 개선 전 (Phase 1 기준) | 개선 후 (Phase 8) | 변화 |
|------|------------------------|-------------------|------|
| 문서화된 Critical/Major 이슈 | 6 (+ God Object) | 0건 미해결 Major BUG | **6/6 BUG 항목 GREEN** |
| `main.cpp` 줄 수 | ~383 | **24** | **−94%** |
| `containsAny` 구현 수 | 2 (클래스별 복제) | 1 (`TextUtils`) | **−50%** |
| 감성 키워드 중복 등록 | 긍정·부정 각 2블록 | 1블록 | 중복 **제거** |
| 전역 서비스 객체 (`main`) | 4 (`fil_data`, analyzer, filters, fileHandler) | 0 (스택 `FeedbackService`) | **전역 제거** |
| 단위 테스트 수 | 0 | **18** (`ctest`) | +18 |
| 신규 프로덕션 모듈 | — | `TextUtils`, `FeedbackSession`, `FeedbackController`, `FeedbackService`, `FileHandler.cpp`, `FilterResult` | 책임 분리 |

**코드 스멜 감소량 (정성·정량 혼합):** Phase 1에서 식별한 6대 레거시 이슈 + God Object 1건 ≈ **7건 구조·품질 부채 → 7건 처리 완료**. 잔여: `TextAnalyzer::globalSent/globalKw` static 동기화(읽기 거의 없음), `Session` dead field(`internalData`, `filterOptions`) — **잔여 스멜 약 2건 (Minor 수준)**.

### 5.2 테스트·커버리지 성과

| 구분 | 수치 |
|------|------|
| `ctest` 통과율 | **18/18 (100%)** |
| 테스트 구성 | Phase 2 안전망 5 + BUG ①~③ 6 + Phase 7 기능 7 |
| 테스트 타깃에 링크된 도메인 TU | `Constants`, `TextAnalyzer`, `Filters`, `Session`, `FileHandler` (**5/5**) |
| 앱 전용 TU (미링크) | `FeedbackController`, `FeedbackService`, `Logger`, `UIComponents`, `main` |

**모듈 기준 커버리지 추정:** 테스트 executable에 포함된 핵심 도메인 파일 5개 / 프로덕션 `src/cpp` 구현 단위 9개 ≈ **56% (파일 수 기준)**.  
**행위 기준 추정:** 감성·키워드·필터·세션·CSV 저장 경로는 GTest로 직접 검증 → 핵심 비즈니스 규칙 **약 65~70%** (Controller/Service HTTP 경로는 통합 테스트 미포함).

> 정밀 커버리지 %는 `gcov`/`llvm-cov` 미실행; 위 수치는 링크 단위·테스트 매핑 기반 추정치이다.

### 5.3 AI 활용 회고

| 활용 영역 | 효과 | 한계·주의 |
|-----------|------|-----------|
| Phase 1 정밀 분석 | 13파일 흐름·6대 이슈·Mermaid 다이어그램을 `docs/analysis.md`로 일괄 정리 | 분석 단계 코드 변경 금지 준수 필요 |
| RED/GREEN TDD | 실패 6건으로 BUG 재현 후 최소 수정으로 11→18 GREEN | 스펙 변경 시(`MixedSentiment` 중립) 테스트·문서 동시 갱신 필요 |
| 리팩토링 (Phase 4~6) | Before/After 표·의존 그래프를 `docs/refactoring.md`에 유지 | Controller 326줄 — Boundary 층 추가 테스트 권장 |
| 신규 기능 (Phase 7) | `saveToCsv`·`classifyWeighted` 명세·7 테스트 자동 설계 | `/download` vs 디스크 CSV 이중 경로 잔존 |
| Phase 8 포스트모템 | `report/`·`docs/` 교차 참조로 전 단계 비교 보고서 자동 합성 | 수치는 빌드·ctest 실행으로 검증 필요 |

**결론:** Cursor AI는 **진단 → 테스트 고정 → 최소 수정 → 점진적 SRP 분리** 순서의 레거시 개선 파이프라인에 적합했으며, 사람 검토 포인트는 **의도적 스펙 변경(가중치 중립)** 과 **HTTP 통합 테스트 부재**이다.

### 5.4 향후 과제

1. **통합 테스트:** `FeedbackController` 라우트(`POST /filter`, `GET /download`)에 대한 httplib in-process 테스트 추가.
2. **CSV 경로 통합:** `/download` 응답과 `filtered_feedback.csv` 단일 소스 정책 검토.
3. **Dead state 제거:** `TextAnalyzer::globalSent/globalKw`, `Session::internalData` / `filterOptions` 정리.
4. **커버리지 측정:** CMake에 `--coverage` 옵션 추가 후 목표치(예: line 80%) 설정.
5. **카테고리 필터 규칙:** `kw`는 `"main"`만, 레거시 `fil`은 `"main"` 제외 서브키 — Phase 4 이후 필터 경로 재검증 및 UI 정합 문서화.
6. **CI:** `cmake` + `ctest`를 파이프라인에 고정; `build/`는 저장소 제외 유지.

---

*문서 생성: Phase 8 — 리팩토링 전후 비교 포스트모템 (2026-05-22)*
