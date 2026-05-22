#pragma once

#include <map>
#include <string>
#include <vector>

#include "Feedback.h"
#include "FilterResult.h"
#include "Filters.h"
#include "TextAnalyzer.h"

/** Application/business layer: session, analysis, filtering, export cache. */
class FeedbackService {
public:
    struct AnalysisSnapshot {
        std::map<std::string, int> sentimentCounts;
        std::map<std::string, int> keywordCounts;
        std::vector<Feedback> feedbacks;
    };

    struct FilterOutcome {
        bool hasFilteredItems = false;
        std::string warningMessage;
        AnalysisSnapshot snapshot;
    };

    void resetSession();

    std::vector<Feedback>& currentFeedbacks();

    void appendFeedbackText(const std::string& rawText);

    void importFromCsvContent(const std::string& content);

    AnalysisSnapshot analyzeCurrentFeedbacks();

    AnalysisSnapshot analyzeFeedbacks(const std::vector<Feedback>& items);

    FilterOutcome runFilter(const std::string& sentimentFilter,
                            const std::string& keywordFilter);

    const std::vector<Feedback>& downloadItems() const;

private:
    TextAnalyzer textAnalyzer_;
    Filters filters_;
    FilterResult filterResult_;

    static std::string trimText(const std::string& text);
    static std::vector<std::string> parseCsvLine(const std::string& line);
};
