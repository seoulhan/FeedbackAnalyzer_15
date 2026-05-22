/**
 * @file test_bugfix_red.cpp
 * @brief Phase 3-RED — 버그 수정 전 실패해야 하는 회귀 테스트
 *
 * BUG ① Constants::SENTIMENT_KEYWORDS 중복 문자열
 * BUG ② containsAny 단일 구현 (TextUtils.h) + TextAnalyzer/Filters 매칭 일치
 * BUG ③ Session / FeedbackSession 테스트 가능 API 및 정적 상태 격리
 *
 * 프로덕션 src/cpp/ 비즈니스 로직은 변경하지 않습니다.
 * GREEN 단계(C_15/green)에서 구현 후 본 테스트를 PASS로 전환합니다.
 */

#include <gtest/gtest.h>

#include <map>
#include <set>
#include <string>
#include <vector>

#include "Constants.h"
#include "Feedback.h"
#include "Filters.h"
#include "Session.h"
#include "TextAnalyzer.h"

#if defined(__has_include)
#  if __has_include("TextUtils.h")
#    define HAVE_TEXTUTILS_H 1
#    include "TextUtils.h"
#  endif
#endif

namespace {

bool vectorHasNoDuplicateStrings(const std::vector<std::string>& vec) {
    return vec.size() == std::set<std::string>(vec.begin(), vec.end()).size();
}

std::string sentimentViaTextAnalyzer(const std::string& text) {
    TextAnalyzer analyzer;
    const std::map<std::string, int> counts =
        analyzer.sent({Feedback(text)});
    if (counts.at(u8"긍정") > 0) {
        return u8"긍정";
    }
    if (counts.at(u8"부정") > 0) {
        return u8"부정";
    }
    return u8"중립";
}

std::string sentimentViaFilters(const std::string& text) {
    Filters filters;
    const std::vector<Feedback> one = {Feedback(text)};
    if (!filters.fil(one, u8"긍정", u8"전체").empty()) {
        return u8"긍정";
    }
    if (!filters.fil(one, u8"부정", u8"전체").empty()) {
        return u8"부정";
    }
    if (!filters.fil(one, u8"중립", u8"전체").empty()) {
        return u8"중립";
    }
    return u8"미분류";
}

}  // namespace

// ---------------------------------------------------------------------------
// BUG ① — Constants 중복 키워드
// ---------------------------------------------------------------------------

class ConstantsRedFixture : public ::testing::Test {
protected:
    void SetUp() override { Constants::init(); }
};

TEST_F(ConstantsRedFixture, SentimentKeywordsPositiveHasNoDuplicates) {
    ASSERT_TRUE(Constants::SENTIMENT_KEYWORDS.count(u8"긍정"));
    const auto& keywords = Constants::SENTIMENT_KEYWORDS.at(u8"긍정");
    EXPECT_TRUE(vectorHasNoDuplicateStrings(keywords))
        << "BUG ①: u8\"긍정\" vector has duplicate entries (size="
        << keywords.size() << ", unique="
        << std::set<std::string>(keywords.begin(), keywords.end()).size() << ")";
}

TEST_F(ConstantsRedFixture, SentimentKeywordsNegativeHasNoDuplicates) {
    ASSERT_TRUE(Constants::SENTIMENT_KEYWORDS.count(u8"부정"));
    const auto& keywords = Constants::SENTIMENT_KEYWORDS.at(u8"부정");
    EXPECT_TRUE(vectorHasNoDuplicateStrings(keywords))
        << "BUG ①: u8\"부정\" vector has duplicate entries (size="
        << keywords.size() << ", unique="
        << std::set<std::string>(keywords.begin(), keywords.end()).size() << ")";
}

// ---------------------------------------------------------------------------
// BUG ② — containsAny 단일 구현 (TextUtils) 및 매칭 일치
// ---------------------------------------------------------------------------

class TextUtilsRedFixture : public ::testing::Test {
protected:
    void SetUp() override {
        Constants::init();
        Filters::initFilterKeywords();
    }
};

TEST_F(TextUtilsRedFixture, TextUtilsHeaderMustExist) {
#ifndef HAVE_TEXTUTILS_H
    FAIL() << "BUG ②: TextUtils.h missing — extract inline containsAny into "
              "namespace TextUtils and include from TextAnalyzer/Filters";
#else
    SUCCEED();
#endif
}

TEST_F(TextUtilsRedFixture, ContainsAnyUnifiedSentimentParity) {
    // GREEN 스펙: TextUtils::containsAny 단일 구현 후 동일 키워드 소스로
    // TextAnalyzer::sent() 분류와 Filters::fil() 감성 필터가 일치해야 함.
    const std::vector<std::string> samples = {
        u8"친절한 응대였습니다.",
        u8"정말 최고입니다. 만족스럽고 감사합니다.",
        u8"배송이 늦어서 실망스럽고 불만이에요.",
        u8"오늘 날씨가 맑고 산책하기 좋았습니다.",
    };

    for (const auto& text : samples) {
        EXPECT_EQ(sentimentViaTextAnalyzer(text), sentimentViaFilters(text))
            << "BUG ②: sentiment mismatch for sample text (awaiting TextUtils "
               "and shared keyword source)";
    }
}

// ---------------------------------------------------------------------------
// BUG ③ — Session / FeedbackSession 테스트 가능 API
// ---------------------------------------------------------------------------

class SessionRedFixture : public ::testing::Test {
protected:
    void TearDown() override { FeedbackSession::clear(); }
};

TEST_F(SessionRedFixture, GetOldDataFromSessionIgnoresKey) {
    Session::updateCurrentFeedbacks({Feedback(u8"단일 세션 데이터")});

    std::vector<Feedback>& viaAlpha = Session::getOldDataFromSession("alpha");
    std::vector<Feedback>& viaBeta = Session::getOldDataFromSession("beta");

    EXPECT_NE(&viaAlpha, &viaBeta)
        << "BUG ③: getOldDataFromSession(key) ignores key; distinct keys must "
           "not alias the same storage (GREEN: FeedbackSession per key)";
}

TEST_F(SessionRedFixture, UpdateWithoutClearLeavesDataVisible) {
    // GREEN: FeedbackSession::clear() isolates static state between tests/requests.
    FeedbackSession::update({Feedback(u8"세션 데이터")});
    ASSERT_FALSE(FeedbackSession::getCurrent().empty());

    FeedbackSession::clear();
    EXPECT_TRUE(FeedbackSession::getCurrent().empty())
        << "BUG ③: FeedbackSession::clear() must reset current session storage";
}
