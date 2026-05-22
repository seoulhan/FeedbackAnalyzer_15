#pragma once

#include <vector>

#include "Feedback.h"

/** Last successful filter operation; used by /download CSV export. */
struct FilterResult {
    std::vector<Feedback> feedbacks;

    void assign(std::vector<Feedback> items) { feedbacks = std::move(items); }

    const std::vector<Feedback>& items() const { return feedbacks; }

    bool empty() const { return feedbacks.empty(); }

    void clear() { feedbacks.clear(); }
};
