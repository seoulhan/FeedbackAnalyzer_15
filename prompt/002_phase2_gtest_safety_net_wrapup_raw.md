# [Phase 2] Google Test (C++) 기반 단위 테스트 안전망 설계

**User — 입력 프롬프트 원문**

---

# [Phase 2] Google Test (C++) 기반 단위 테스트 안전망 설계
당신은 시니어 C++ QA 엔지니어입니다. 
개선 작업을 진행하기 전, 기존 레거시 비즈니스 로직을 완벽히 보호할 단위 테스트 프레임워크를 `tests/test_analyzer.cpp`에 설계하려 합니다.

구현 규칙 및 요구사항:
1. Google Test (gtest) 프레임워크를 기반으로 작성합니다.
2. `TEST_F` 및 `SetUp()` Fixture를 활용하여 테스트 실행 전 `Constants::init()` 공통 초기화 로직이 매번 수행되도록 보장합니다.
3. 단일 책임 검증 원칙에 따라 아래 독립 케이스들을 설계해 주세요:
 - PositiveSentiment: "긍정" 피드백 정상 분류 검증
 - NegativeSentiment: "부정" 피드백 정상 분류 검증
 - EmptyFeedback: 빈 피드백 목록 입력 시의 경계값 대응 검증
 - MixedSentiment: 긍정과 부정이 혼합된 텍스트 처리 검증
 - UnknownKeywords: 사전에 없는 키워드 처리 검증
4. CMake 연동을 위한 `find_package(GTest)` 및 `target_link_libraries` 구성 설정 방안을 하단에 친절히 덧붙여 주세요.

이 단계에서는 오직 단위 테스트 파일의 스켈레톤 및 설계 코드만 생성하며, 프로덕션 레거시 코드는 일절 변경하지 마십시오.

---

**User — wrap-up (`refs/step_manual_edit.md` 24–27)**

실행이 완료되면 다음과 같은 마무리 작업을 실행하세요.
 - 프로젝트 루트 아래 report\ 디렉토리에 프롬프트 처리 결과에 대한 보고서 파일을 markdown 포맷의 텍스트로 생성하세요. 파일명의 경우 순서대로 번호 접두사를 붙여 생성하세요.
 - 프로젝트 루트 아래 prompt\ 디렉토리에 입력 프롬프트 원문 markdown과 대화 전체를 Export Transcript 하세요. 파일명의 경우 보고서의 제목에서 report 를 제외하고, 순서대로 번호 접두사를 붙여 생성하세요.
 - 프로젝트 변경사항을 git에 checkout 하고 commit하세요.
