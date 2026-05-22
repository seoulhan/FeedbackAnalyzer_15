#pragma once
#include <string>
#include <vector>
#include <map>

/** Domain string labels and numeric tuning values (no magic literals in analyzers). */
struct DomainConstants {
    static inline const std::string SENTIMENT_POSITIVE{u8"긍정"};
    static inline const std::string SENTIMENT_NEUTRAL{u8"중립"};
    static inline const std::string SENTIMENT_NEGATIVE{u8"부정"};

    static inline const std::string FILTER_ALL{u8"전체"};
    static inline const std::string CATEGORY_SUBKEY_MAIN{"main"};

    static constexpr int COUNT_INITIAL = 0;
    static constexpr int HTTP_SERVER_PORT = 8080;
    static constexpr int URL_ENCODED_HEX_DIGIT_COUNT = 2;
    static constexpr int URL_ENCODED_HEX_SKIP_AFTER_PERCENT = 2;
    static constexpr int CSV_FIRST_FIELD_INDEX = 0;
};

class Constants {
public:
    static std::map<std::string, std::vector<std::string>> SENTIMENT_KEYWORDS;
    static std::map<std::string, std::map<std::string, std::vector<std::string>>> CATEGORY_KEYWORDS;

    static void init();
};
