#include "Session.h"

std::vector<Feedback> FeedbackSession::currentFeedbacks;
std::map<std::string, std::vector<Feedback>> FeedbackSession::keyedFeedbacks;
std::map<std::string, std::string> Session::internalData;
std::map<std::string, std::string> Session::filterOptions;
