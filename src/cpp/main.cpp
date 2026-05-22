#include "httplib.h"

#include "Constants.h"
#include "FeedbackController.h"
#include "FeedbackService.h"
#include "Filters.h"
#include "Logger.h"

int main() {
    Constants::init();
    Filters::initFilterKeywords();

    FeedbackService feedbackService;
    FeedbackController controller(feedbackService);

    httplib::Server server;
    controller.registerRoutes(server);

    Logger::logInfo(u8"서버가 http://localhost:" +
                    std::to_string(DomainConstants::HTTP_SERVER_PORT) + u8" 에서 시작됩니다.");
    server.listen("0.0.0.0", DomainConstants::HTTP_SERVER_PORT);

    return 0;
}
