#pragma once
#include <vector>
#include <map>
#include <string>
#include "Feedback.h"

class FeedbackSession {
private:
    static std::vector<Feedback> currentFeedbacks;
    static std::map<std::string, std::vector<Feedback>> keyedFeedbacks;

public:
    static std::vector<Feedback>& getCurrent() { return currentFeedbacks; }

    static void update(const std::vector<Feedback>& feedbacks) {
        currentFeedbacks = feedbacks;
    }

    static void clear() {
        currentFeedbacks.clear();
        keyedFeedbacks.clear();
    }

    static std::vector<Feedback>& getByKey(const std::string& key) {
        return keyedFeedbacks[key];
    }
};

class Session {
private:
    static std::map<std::string, std::string> internalData;
    static std::map<std::string, std::string> filterOptions;

public:
    static void initSessionStateUgly() {
        FeedbackSession::clear();
    }

    static std::vector<Feedback>& getOldDataFromSession(const std::string& key) {
        return FeedbackSession::getByKey(key);
    }

    static void updateCurrentFeedbacks(const std::vector<Feedback>& feedbacks) {
        FeedbackSession::update(feedbacks);
    }

    static std::vector<Feedback>& getCurrentFeedbacks() {
        return FeedbackSession::getCurrent();
    }
};
