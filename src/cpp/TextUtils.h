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

}  // namespace TextUtils
