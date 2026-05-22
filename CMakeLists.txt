cmake_minimum_required(VERSION 3.14)
project(FeedbackAnalyzer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(feedback_analyzer
    src/cpp/main.cpp
    src/cpp/Constants.cpp
    src/cpp/Filters.cpp
    src/cpp/Logger.cpp
    src/cpp/Session.cpp
    src/cpp/TextAnalyzer.cpp
    src/cpp/UIComponents.cpp
)

target_include_directories(feedback_analyzer PRIVATE src/cpp)

# Windows: httplib mmap uses CreateFile2 / CreateFileMappingFromApp (Win8+).
# MinGW defaults to older WINVER unless raised here.
if(WIN32)
    target_compile_definitions(feedback_analyzer PRIVATE
        _WIN32_WINNT=0x0A00
        WINVER=0x0A00
    )
    target_link_libraries(feedback_analyzer PRIVATE ws2_32)
endif()

# ---------------------------------------------------------------------------
# Google Test — feedback_analyzer_tests (Phase 2 safety net + Phase 3 RED)
# ---------------------------------------------------------------------------

enable_testing()

find_package(GTest QUIET)
if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

add_executable(feedback_analyzer_tests
    tests/test_analyzer.cpp
    tests/test_bugfix_red.cpp
    src/cpp/Constants.cpp
    src/cpp/Filters.cpp
    src/cpp/Session.cpp
    src/cpp/TextAnalyzer.cpp
)

target_include_directories(feedback_analyzer_tests PRIVATE src/cpp)

target_link_libraries(feedback_analyzer_tests PRIVATE
    GTest::gtest
)

if(WIN32)
    target_compile_definitions(feedback_analyzer_tests PRIVATE
        _WIN32_WINNT=0x0A00
        WINVER=0x0A00
    )
endif()

include(GoogleTest)
gtest_discover_tests(feedback_analyzer_tests)
