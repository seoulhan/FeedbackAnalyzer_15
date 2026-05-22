#pragma once

#include "FeedbackService.h"
#include "httplib.h"

/** HTTP boundary: routing, form parsing, HTML/CSV response assembly. */
class FeedbackController {
public:
    explicit FeedbackController(FeedbackService& service);

    void registerRoutes(httplib::Server& server);

private:
    FeedbackService& service_;

    static std::string urlDecode(const std::string& str);
    static std::map<std::string, std::string> parseForm(const std::string& body);
    static std::string getCurrentTimestamp();
    static std::string escapeHtml(const std::string& s);

    static std::string renderPage(const std::string& success,
                                  const std::string& warning,
                                  const std::string& error,
                                  const std::map<std::string, int>& sentimentResults,
                                  const std::map<std::string, int>& keywordResults,
                                  const std::vector<Feedback>& feedbacks);

    void handleGetRoot(const httplib::Request& req, httplib::Response& res);
    void handlePostAnalyze(const httplib::Request& req, httplib::Response& res);
    void handlePostUpload(const httplib::Request& req, httplib::Response& res);
    void handlePostFilter(const httplib::Request& req, httplib::Response& res);
    void handleGetDownload(const httplib::Request& req, httplib::Response& res);

    void respondHtml(httplib::Response& res,
                     const std::string& success,
                     const std::string& warning,
                     const std::string& error,
                     const FeedbackService::AnalysisSnapshot& snapshot);
};
