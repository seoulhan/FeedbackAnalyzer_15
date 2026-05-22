#include "TextAnalyzer.h"

#include "TextUtils.h"

std::map<std::string, int> TextAnalyzer::globalSent;
std::map<std::string, int> TextAnalyzer::globalKw;

std::map<std::string, int> TextAnalyzer::createEmptySentimentCounts() {
    std::map<std::string, int> counts;
    counts[DomainConstants::SENTIMENT_POSITIVE] = DomainConstants::COUNT_INITIAL;
    counts[DomainConstants::SENTIMENT_NEUTRAL] = DomainConstants::COUNT_INITIAL;
    counts[DomainConstants::SENTIMENT_NEGATIVE] = DomainConstants::COUNT_INITIAL;
    return counts;
}

std::string TextAnalyzer::classifySentimentForText(const std::string& text) {
    if (TextUtils::containsAny(text,
                               Constants::SENTIMENT_KEYWORDS[DomainConstants::SENTIMENT_POSITIVE])) {
        return DomainConstants::SENTIMENT_POSITIVE;
    }
    if (TextUtils::containsAny(text,
                               Constants::SENTIMENT_KEYWORDS[DomainConstants::SENTIMENT_NEGATIVE])) {
        return DomainConstants::SENTIMENT_NEGATIVE;
    }
    return DomainConstants::SENTIMENT_NEUTRAL;
}

void TextAnalyzer::incrementSentimentCount(std::map<std::string, int>& counts,
                                           const std::string& label) {
    counts[label]++;
}

std::map<std::string, int> TextAnalyzer::createEmptyKeywordCounts() {
    std::map<std::string, int> counts;
    for (const auto& entry : Constants::CATEGORY_KEYWORDS) {
        counts[entry.first] = DomainConstants::COUNT_INITIAL;
    }
    return counts;
}

void TextAnalyzer::countKeywordsForText(const std::string& text,
                                        std::map<std::string, int>& counts) {
    for (const auto& entry : Constants::CATEGORY_KEYWORDS) {
        const std::string& category = entry.first;
        if (!entry.second.count(DomainConstants::CATEGORY_SUBKEY_MAIN)) {
            continue;
        }
        const auto& keywords = entry.second.at(DomainConstants::CATEGORY_SUBKEY_MAIN);
        if (TextUtils::containsAny(text, keywords)) {
            counts[category]++;
        }
    }
}

std::map<std::string, int> TextAnalyzer::analyzeSentiment(
    const std::vector<Feedback>& feedbacks) {
    std::map<std::string, int> result = createEmptySentimentCounts();

    for (const auto& feedback : feedbacks) {
        const std::string label = classifySentimentForText(feedback.getText());
        incrementSentimentCount(result, label);
    }

    globalSent = result;
    return result;
}

std::map<std::string, int> TextAnalyzer::analyzeKeywords(
    const std::vector<Feedback>& feedbacks) {
    std::map<std::string, int> result = createEmptyKeywordCounts();

    for (const auto& feedback : feedbacks) {
        countKeywordsForText(feedback.getText(), result);
    }

    globalKw = result;
    return result;
}
