/**
 * @file test_phase7_feature.cpp
 * @brief Phase 7 — FileHandler::saveToCsv, TextAnalyzer::classifyWeighted
 */

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "Constants.h"
#include "Feedback.h"
#include "FileHandler.h"
#include "TextAnalyzer.h"

namespace {

std::string readFileBinary(const std::filesystem::path& path) {
    std::ifstream in(path, std::ios::binary);
    return std::string((std::istreambuf_iterator<char>(in)),
                       std::istreambuf_iterator<char>());
}

}  // namespace

class Phase7FeatureFixture : public ::testing::Test {
protected:
    void SetUp() override { Constants::init(); }

    std::filesystem::path tempCsvPath() const {
        return std::filesystem::temp_directory_path() /
               ("fa_phase7_" + std::to_string(
                                    std::hash<const void*>{}(this)) +
                ".csv");
    }
};

TEST_F(Phase7FeatureFixture, ClassifyWeightedMixedTieIsNeutral) {
    const std::string label = TextAnalyzer::classifyWeighted(
        u8"품질은 최고인데 가격이 너무 비싸서 실망했어요.");
    EXPECT_EQ(u8"중립", label);
}

TEST_F(Phase7FeatureFixture, ClassifyWeightedNegativeDominates) {
    const std::string label = TextAnalyzer::classifyWeighted(
        u8"최고라고 하지만 실망스럽고 불만이에요. 최악입니다.");
    EXPECT_EQ(u8"부정", label);
}

TEST_F(Phase7FeatureFixture, ClassifyWeightedPositiveDominates) {
    const std::string label = TextAnalyzer::classifyWeighted(
        u8"정말 최고입니다. 만족스럽고 감사합니다.");
    EXPECT_EQ(u8"긍정", label);
}

TEST_F(Phase7FeatureFixture, ClassifyWeightedNoKeywordsIsNeutral) {
    const std::string label =
        TextAnalyzer::classifyWeighted(u8"오늘 날씨가 맑았습니다.");
    EXPECT_EQ(u8"중립", label);
}

TEST_F(Phase7FeatureFixture, SaveToCsvWritesBomHeaderAndRows) {
    const auto path = tempCsvPath();
    std::filesystem::remove(path);

    const std::vector<Feedback> data = {
        Feedback(u8"첫 번째 피드백"),
        Feedback(u8"두 번째 피드백"),
    };

    ASSERT_TRUE(FileHandler::saveToCsv(data, path.string()));

    const std::string content = readFileBinary(path);
    ASSERT_GE(content.size(), 3u);
    EXPECT_EQ(0xEF, static_cast<unsigned char>(content[0]));
    EXPECT_EQ(0xBB, static_cast<unsigned char>(content[1]));
    EXPECT_EQ(0xBF, static_cast<unsigned char>(content[2]));
    EXPECT_NE(content.find("text\n"), std::string::npos);
    EXPECT_NE(content.find(u8"첫 번째 피드백"), std::string::npos);
    EXPECT_NE(content.find(u8"두 번째 피드백"), std::string::npos);

    std::filesystem::remove(path);
}

TEST_F(Phase7FeatureFixture, SaveToCsvInvalidPathReturnsFalse) {
    EXPECT_FALSE(FileHandler::saveToCsv({Feedback(u8"데이터")}, ""));
}

TEST_F(Phase7FeatureFixture, SaveToCsvEmptyDataWritesHeaderOnly) {
    const auto path = tempCsvPath();
    std::filesystem::remove(path);

    ASSERT_TRUE(FileHandler::saveToCsv({}, path.string()));

    const std::string content = readFileBinary(path);
    EXPECT_NE(content.find("text\n"), std::string::npos);
    EXPECT_EQ(content.find('\n', content.find("text\n") + 5), std::string::npos)
        << "no data rows after header";

    std::filesystem::remove(path);
}
