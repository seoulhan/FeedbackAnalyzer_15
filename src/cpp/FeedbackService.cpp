#include "FeedbackService.h"

#include <sstream>

#include "Constants.h"
#include "Logger.h"
#include "Session.h"

void FeedbackService::resetSession() {
    Session::initSessionStateUgly();
}

std::vector<Feedback>& FeedbackService::currentFeedbacks() {
    return Session::getCurrentFeedbacks();
}

std::string FeedbackService::trimText(const std::string& text) {
    const auto start = text.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return {};
    }
    const auto end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}

void FeedbackService::appendFeedbackText(const std::string& rawText) {
    const std::string trimmed = trimText(rawText);
    if (!trimmed.empty()) {
        currentFeedbacks().push_back(Feedback(trimmed));
    }
}

std::vector<std::string> FeedbackService::parseCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;
    for (char c : line) {
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.push_back(field);
            field.clear();
        } else {
            field += c;
        }
    }
    fields.push_back(field);
    return fields;
}

void FeedbackService::importFromCsvContent(const std::string& content) {
    if (content.empty()) {
        return;
    }

    std::istringstream stream(content);
    std::string line;
    bool firstLine = true;
    auto& feedbacks = currentFeedbacks();

    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (firstLine) {
            firstLine = false;
            continue;
        }
        if (line.empty()) {
            continue;
        }

        const auto fields = parseCsvLine(line);
        if (!fields.empty() && !fields[DomainConstants::CSV_FIRST_FIELD_INDEX].empty()) {
            feedbacks.push_back(Feedback(fields[DomainConstants::CSV_FIRST_FIELD_INDEX]));
        }
    }
}

FeedbackService::AnalysisSnapshot FeedbackService::analyzeFeedbacks(
    const std::vector<Feedback>& items) {
    AnalysisSnapshot snapshot;
    snapshot.feedbacks = items;
    if (items.empty()) {
        return snapshot;
    }

    snapshot.sentimentCounts = textAnalyzer_.analyzeSentiment(items);
    snapshot.keywordCounts = textAnalyzer_.analyzeKeywords(items);
    return snapshot;
}

FeedbackService::AnalysisSnapshot FeedbackService::analyzeCurrentFeedbacks() {
    auto& feedbacks = currentFeedbacks();

    for (const auto& fb : feedbacks) {
        Logger::logInfo(fb.getText());
    }
    Logger::logInfo(u8"현재 " + std::to_string(feedbacks.size()) + u8"개의 피드백이 입력되었습니다.");

    auto snapshot = analyzeFeedbacks(feedbacks);
    if (!feedbacks.empty()) {
        Logger::logInfo(u8"감성 분석 완료");
        Logger::logInfo(u8"키워드 분석 완료");
    }
    return snapshot;
}

FeedbackService::FilterOutcome FeedbackService::runFilter(const std::string& sentimentFilter,
                                                          const std::string& keywordFilter) {
    FilterOutcome outcome;
    auto& feedbacks = currentFeedbacks();

    if (feedbacks.empty()) {
        outcome.warningMessage = u8"분석할 피드백이 없습니다.";
        Logger::logWarning(outcome.warningMessage);
        return outcome;
    }

    const auto filtered = filters_.filter(feedbacks, sentimentFilter, keywordFilter);
    if (filtered.empty()) {
        outcome.warningMessage = u8"필터링 결과가 없습니다.";
        Logger::logWarning(outcome.warningMessage);
        return outcome;
    }

    filterResult_.assign(filtered);
    outcome.hasFilteredItems = true;
    outcome.snapshot = analyzeFeedbacks(filterResult_.items());
    Logger::logInfo(u8"필터링 결과: " +
                    std::to_string(filterResult_.items().size()) + u8"개의 피드백");
    return outcome;
}

const std::vector<Feedback>& FeedbackService::downloadItems() const {
    return filterResult_.items();
}
