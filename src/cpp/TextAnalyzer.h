#pragma once

#include <map>
#include <string>
#include <vector>

#include "Constants.h"
#include "Feedback.h"

class TextAnalyzer {
private:
    static std::map<std::string, int> globalSent;
    static std::map<std::string, int> globalKw;

    static std::map<std::string, int> createEmptySentimentCounts();
    static std::string classifySentimentForText(const std::string& text);
    static void incrementSentimentCount(std::map<std::string, int>& counts,
                                        const std::string& label);

    static std::map<std::string, int> createEmptyKeywordCounts();
    static void countKeywordsForText(const std::string& text,
                                     std::map<std::string, int>& counts);

public:
    std::map<std::string, int> analyzeSentiment(const std::vector<Feedback>& feedbacks);
    std::map<std::string, int> analyzeKeywords(const std::vector<Feedback>& feedbacks);
};
