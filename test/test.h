//
// This header is used across tests to include all the valid headers
//

#ifndef TEST_HELPERS_H_
#define TEST_HELPERS_H_

#define _ELPP_ENABLE_THREADING
#define _ELPP_DISABLE_VMODULES_EXTENSION
#define _ELPP_STACKTRACE_ON_CRASH
#define _ELPP_STRICT_SIZE_CHECK

#include <gtest/gtest.h>
#include "easylogging++.h"

using namespace el;
using namespace el::base;
using namespace el::base::utils;

_INITIALIZE_EASYLOGGINGPP

static const char* logfile = "/tmp/logs/el.gtest.log";

static void reconfigureLoggersForTest(void) {
    Configurations c;
    c.setGlobally(ConfigurationType::Format, "%datetime{%a %b %d, %H:%m} %log");
    c.setGlobally(ConfigurationType::Filename, "/tmp/logs/el.gtest.log");
    c.setGlobally(ConfigurationType::MaxLogFileSize, "2097152"); // 2MB
    c.setGlobally(ConfigurationType::ToStandardOutput, "false");
    c.setGlobally(ConfigurationType::PerformanceTracking, "true");
    Loggers::setDefaultConfigurations(c, true);
    // We do not want to reconfgure syslog with date/time
    Loggers::reconfigureLogger(consts::kSysLogLoggerId, ConfigurationType::Format, "%level: %log");

    if (!Helpers::hasFlag(LoggingFlag::DisableApplicationAbortOnFatalLog)) {
        Helpers::addFlag(LoggingFlag::DisableApplicationAbortOnFatalLog);
    }
}

static std::string tail(unsigned int n, const char* filename = logfile) {
    if (n == 0) return std::string();
    std::ifstream fstr(filename);
    if (!fstr.is_open()) {
        return std::string();
    }
    fstr.seekg(0, fstr.end);
    int size = static_cast<int>(fstr.tellg());
    int ncopy = n + 1;
    for (int i = (size - 1); i >= 0; --i) {
        fstr.seekg(i);
        char c = fstr.get();
        if (c == '\n' && --ncopy == 0) {
            break;
        }
        if (i == 0) {
            fstr.seekg(i); // fstr.get() increments buff, so we reset it
        }
    }
    std::stringstream ss;
    char c = fstr.get();
    while (fstr.good()) {
        ss << c;
        c = fstr.get();
    }
    return ss.str();
}

static std::string getDate(const char* format = "%a %b %d, %H:%m") {
    return DateTime::getDateTime(format);
}

static void cleanLogFile(const char* filename = logfile) {
    std::cout << "Cleaning out log file...[" << logfile << "]\n";
    std::ofstream fstr(filename, std::ofstream::trunc);
}

#undef BUILD_STR
#define BUILD_STR(strb) [&]() -> std::string { std::stringstream ssb; ssb << strb; return ssb.str(); }()

static const char* kSysLogIndent = "Easylogging++ Unit Tests";
#endif // TEST_HELPERS_H_
