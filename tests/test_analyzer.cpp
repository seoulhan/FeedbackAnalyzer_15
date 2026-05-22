/**
 * @file test_analyzer.cpp
 * @brief Google Test 기반 TextAnalyzer::analyzeSentiment() 레거시 동작 안전망
 *
 * Phase 2: 프로덕션 코드 변경 없이 현재 비즈니스 규칙을 고정합니다.
 * - 긍정 키워드 우선 매칭 → "긍정"
 * - 긍정 없을 때 부정 키워드 → "부정"
 * - 둘 다 없으면 → "중립"
 *
 * 리팩토링·버그 수정·가중치 분석(classifyWeighted) 도입 후에도
 * 본 테스트를 GREEN으로 유지하거나, 의도적 스펙 변경 시에만 갱신합니다.
 */

#include <gtest/gtest.h>

#include <map>
#include <string>
#include <vector>

#include "Constants.h"
#include "Feedback.h"
#include "TextAnalyzer.h"

// ---------------------------------------------------------------------------
// Test Fixture — 매 테스트 전 Constants::init() 보장
// ---------------------------------------------------------------------------

class AnalyzerTestFixture : public ::testing::Test {
protected:
    TextAnalyzer analyzer;

    void SetUp() override {
        Constants::init();
    }

    static std::vector<Feedback> makeFeedbacks(
        std::initializer_list<std::string> texts) {
        std::vector<Feedback> feedbacks;
        feedbacks.reserve(texts.size());
        for (const auto& text : texts) {
            feedbacks.emplace_back(text);
        }
        return feedbacks;
    }

    static const std::string& kPositive() {
        static const std::string value{u8"긍정"};
        return value;
    }
    static const std::string& kNeutral() {
        static const std::string value{u8"중립"};
        return value;
    }
    static const std::string& kNegative() {
        static const std::string value{u8"부정"};
        return value;
    }
};

// ---------------------------------------------------------------------------
// 단일 책임 검증 케이스
// ---------------------------------------------------------------------------

TEST_F(AnalyzerTestFixture, PositiveSentiment) {
    const auto feedbacks = makeFeedbacks({
        u8"정말 최고입니다. 만족스럽고 감사합니다.",
    });

    const std::map<std::string, int> result = analyzer.analyzeSentiment(feedbacks);

    EXPECT_EQ(1, result.at(kPositive()));
    EXPECT_EQ(0, result.at(kNeutral()));
    EXPECT_EQ(0, result.at(kNegative()));
}

TEST_F(AnalyzerTestFixture, NegativeSentiment) {
    const auto feedbacks = makeFeedbacks({
        u8"배송이 늦어서 실망스럽고 불만이에요.",
    });

    const std::map<std::string, int> result = analyzer.analyzeSentiment(feedbacks);

    EXPECT_EQ(0, result.at(kPositive()));
    EXPECT_EQ(0, result.at(kNeutral()));
    EXPECT_EQ(1, result.at(kNegative()));
}

TEST_F(AnalyzerTestFixture, EmptyFeedback) {
    const std::vector<Feedback> feedbacks;

    const std::map<std::string, int> result = analyzer.analyzeSentiment(feedbacks);

    EXPECT_EQ(0, result.at(kPositive()));
    EXPECT_EQ(0, result.at(kNeutral()));
    EXPECT_EQ(0, result.at(kNegative()));
    EXPECT_EQ(3u, result.size());
}

TEST_F(AnalyzerTestFixture, MixedSentiment) {
    // 레거시: containsAny(긍정)이 먼저 평가되므로 긍·부정 키워드가 공존하면 "긍정"으로 분류
    const auto feedbacks = makeFeedbacks({
        u8"품질은 최고인데 가격이 너무 비싸서 실망했어요.",
    });

    const std::map<std::string, int> result = analyzer.analyzeSentiment(feedbacks);

    EXPECT_EQ(1, result.at(kPositive()))
        << "legacy first-match: positive keywords take precedence over negative";
    EXPECT_EQ(0, result.at(kNeutral()));
    EXPECT_EQ(0, result.at(kNegative()));
}

TEST_F(AnalyzerTestFixture, UnknownKeywords) {
    const auto feedbacks = makeFeedbacks({
        u8"오늘 날씨가 맑고 산책하기 무난했습니다.",
    });

    const std::map<std::string, int> result = analyzer.analyzeSentiment(feedbacks);

    EXPECT_EQ(0, result.at(kPositive()));
    EXPECT_EQ(1, result.at(kNeutral()));
    EXPECT_EQ(0, result.at(kNegative()));
}

// ---------------------------------------------------------------------------
// gtest 진입점 (GTest::gtest_main 링크 시 이 블록은 제거 가능)
// ---------------------------------------------------------------------------

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// =============================================================================
// CMake 연동 가이드 (프로젝트 루트 CMakeLists.txt 또는 tests/CMakeLists.txt)
// =============================================================================
//
// [사전 조건]
//   - Google Test 설치 (vcpkg, Conan, 시스템 패키지, 소스 빌드 등)
//   - CMAKE_PREFIX_PATH 또는 GTest_DIR가 find_package(GTest)를 찾도록 설정
//
// [루트 CMakeLists.txt 예시 — 기존 feedback_analyzer 타겟 아래에 추가]
//
//   enable_testing()
//
//   find_package(GTest REQUIRED)
//
//   add_executable(feedback_analyzer_tests
//       tests/test_analyzer.cpp
//       src/cpp/Constants.cpp
//       src/cpp/TextAnalyzer.cpp
//   )
//
//   target_include_directories(feedback_analyzer_tests PRIVATE src/cpp)
//
//   target_link_libraries(feedback_analyzer_tests PRIVATE
//       GTest::gtest
//       # 또는 진입점 중복을 피하려면: GTest::gtest_main (이 경우 main() 제거)
//   )
//
//   include(GoogleTest)
//   gtest_discover_tests(feedback_analyzer_tests)
//
// [Windows / MinGW 참고]
//   - 본 프로젝트와 동일하게 WINVER 정의가 필요하면 feedback_analyzer_tests에도 적용
//
// [빌드 및 실행]
//   cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
//   cmake --build build --target feedback_analyzer_tests
//   ctest --test-dir build --output-on-failure
//   # 또는: build/feedback_analyzer_tests.exe (Windows)
//
// [vcpkg 사용 시 예시]
//   vcpkg install gtest:x64-windows
//   cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=[vcpkg]/scripts/buildsystems/vcpkg.cmake
//
// =============================================================================
