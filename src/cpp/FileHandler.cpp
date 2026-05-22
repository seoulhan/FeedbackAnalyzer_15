#include "FileHandler.h"

#include <fstream>

namespace {

std::string escapeCsvField(const std::string& text) {
    if (text.find_first_of(",\"\n\r") == std::string::npos) {
        return text;
    }
    std::string escaped;
    escaped.reserve(text.size() + 2);
    escaped.push_back('"');
    for (char c : text) {
        if (c == '"') {
            escaped.append("\"\"");
        } else {
            escaped.push_back(c);
        }
    }
    escaped.push_back('"');
    return escaped;
}

}  // namespace

bool FileHandler::saveToCsv(const std::vector<Feedback>& data, const std::string& path) {
    if (path.empty()) {
        return false;
    }

    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out.is_open()) {
        return false;
    }

    out << "\xEF\xBB\xBF";
    out << "text\n";

    for (const auto& item : data) {
        out << escapeCsvField(item.getText()) << "\n";
        if (!out.good()) {
            return false;
        }
    }

    out.flush();
    return out.good();
}
