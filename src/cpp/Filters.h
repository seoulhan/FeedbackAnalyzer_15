#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Constants.h"
#include "Feedback.h"

class Filters {
private:
    static std::map<std::string, std::vector<std::string>> S_KEYWORDS;

    static std::string inferSentimentLabel(const std::string& text);
    static std::vector<Feedback> applySentimentFilter(const std::vector<Feedback>& dataList,
                                                    const std::string& sentimentFilter);
    static std::vector<Feedback> applyKeywordFilter(const std::vector<Feedback>& dataList,
                                                    const std::string& keywordFilter);
    static void logFilteredItems(const std::vector<Feedback>& items);

public:
    static void initFilterKeywords();

    std::vector<Feedback> filter(const std::vector<Feedback>& dataList,
                                 const std::string& sentimentFilter,
                                 const std::string& keywordFilter);
};
