#pragma once

#include <string>
#include <vector>

namespace TextUtils {

inline bool containsAny(const std::string& text,
                        const std::vector<std::string>& keywords) {
    for (const auto& kw : keywords) {
        if (text.find(kw) != std::string::npos) {
            return true;
        }
    }
    return false;
}

inline int countMatchingKeywords(const std::string& text,
                                 const std::vector<std::string>& keywords) {
    int count = 0;
    for (const auto& kw : keywords) {
        if (text.find(kw) != std::string::npos) {
            ++count;
        }
    }
    return count;
}

}  // namespace TextUtils
