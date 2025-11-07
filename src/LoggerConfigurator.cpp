#include "gaden/LoggerConfigurator.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <regex>
#include <sstream>
#include <string>

#include <gaden/Logger.hpp>

namespace gaden::cli {

static std::string lower_nospace(std::string s) {
    s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

const char* LoggerConfigurator::allowedLevelsList() {
    return "Debug4 | Debug3 | Debug2 | Debug | Info | Warning | Error";
}

std::optional<gaden::LogLevel> LoggerConfigurator::parseLogLevel(std::string s) {
    s = lower_nospace(std::move(s));
    if (s == "debug4" || s == "d4" || s == "4") return gaden::LogLevel::Debug4;
    if (s == "debug3" || s == "d3" || s == "3") return gaden::LogLevel::Debug3;
    if (s == "debug2" || s == "d2" || s == "2") return gaden::LogLevel::Debug2;
    if (s == "debug"  || s == "d"  || s == "1") return gaden::LogLevel::Debug;
    if (s == "info"   || s == "i"  || s == "0") return gaden::LogLevel::Info;
    if (s == "warning"|| s == "w")              return gaden::LogLevel::Warning;
    if (s == "error"  || s == "e")              return gaden::LogLevel::Error;
    return std::nullopt;
}

bool LoggerConfigurator::parseArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        auto needValue = [&](std::optional<std::string>& out)->bool{
            if (i + 1 >= argc) return false;
            out = std::string(argv[++i]);
            return true;
        };

        if (a == "-l" || a == "--log-level") {
            if (!needValue(m_logLevel)) return false;
        } else if (a == "-f" || a == "--log-file") {
            if (!needValue(m_logFile)) return false;
        } else if (a == "--no-log-file") {
            m_noLogFile = true;
        } else if (a == "--check-indents") {
            m_checkIndents = true;
        } else if (a == "-i" || a == "--input") {
            if (!needValue(m_jsonPath)) return false;
        } else if (a == "--info") {
            m_info = true;
        } else if (a == "--help" || a == "-h" || a == "/?") {
            m_help = true;
        } else if (a == "--version") {
            m_version = true;
        } else {
            m_rest.push_back(std::move(a));
        }
    }
    return true;
}

static std::vector<std::string> parse_string_array(const std::string& s) {
    std::vector<std::string> out;
    std::regex qstr(R"xx("([^"\\]*(?:\\.[^"\\]*)*)")xx");
    auto it = std::sregex_iterator(s.begin(), s.end(), qstr);
    auto end = std::sregex_iterator();
    for (; it != end; ++it) {
        out.push_back((*it)[1].str());
    }
    return out;
}

bool LoggerConfigurator::applyLoggerSettingsFromJson(const std::string& json) {
    using gaden::Logger;
    Logger& log = Logger::instance();

    std::smatch m;
    std::regex loggerObj(R"("Logger"\s*:\s*\{([\s\S]*?)\})");
    std::string body = json;
    if (std::regex_search(json, m, loggerObj)) {
        body = m[1].str();
    }

    std::regex reLevel(R"xx("Level"\s*:\s*"([^"]+)")xx");
    if (std::regex_search(body, m, reLevel)) {
        if (auto lvl = parseLogLevel(m[1].str())) {
            log.setLevel(*lvl);
        } else {
            std::cerr << "Invalid log level in JSON: " << m[1].str()
                      << "\nAllowed: " << allowedLevelsList() << "\n";
            return false;
        }
    }

    std::regex reCI(R"("CheckIndents"\s*:\s*(true|false))", std::regex::icase);
    if (std::regex_search(body, m, reCI)) {
        std::string v = m[1].str();
        std::transform(v.begin(), v.end(), v.begin(), ::tolower);
        if (v == "true") {
            log.enableIndentChecking();
        }
    }

    std::regex reOut(R"xx("Output"\s*:\s*"([^"]*)")xx");
    if (std::regex_search(body, m, reOut)) {
        const std::string outp = m[1].str();
        if (!outp.empty()) {
            log.setOutputToFile(std::filesystem::path{outp});
        }
    }

    std::regex reFilter(R"("Filter"\s*:\s*\{([\s\S]*?)\})");
    if (std::regex_search(body, m, reFilter)) {
        const std::string fbody = m[1].str();

        std::smatch mi;
        std::regex reInc(R"("Include"\s*:\s*\[([\s\S]*?)\])");
        std::regex reExc(R"("Exclude"\s*:\s*\[([\s\S]*?)\])");

        if (std::regex_search(fbody, mi, reInc)) {
            auto arr = parse_string_array(mi[1].str());
            if (!arr.empty()) {
                log.setIncludes(arr);
            }
        } else if (std::regex_search(fbody, mi, reExc)) {
            auto arr = parse_string_array(mi[1].str());
            if (!arr.empty()) {
                log.setExcludes(arr);
            }
        }
    }

    return true;
}

bool LoggerConfigurator::applyJsonIfProvided() {
    if (!m_jsonPath) return true;
    std::ifstream f(*m_jsonPath);
    if (!f) {
        std::cerr << "Failed to open settings file: " << *m_jsonPath << "\n";
        return false;
    }
    std::ostringstream oss;
    oss << f.rdbuf();
    const std::string text = oss.str();
    if (!applyLoggerSettingsFromJson(text)) {
        std::cerr << "Ignoring settings file due to errors: " << *m_jsonPath << "\n";
        return false;
    }
    return true;
}

bool LoggerConfigurator::process(int argc, char** argv, LogLevel defaultLogLevel) {
    using gaden::Logger;
    Logger& log = Logger::instance();

    log.setLevel(gaden::LogLevel::Info);

    if (!parseArgs(argc, argv)) {
        std::cerr << "Invalid arguments. Use --help.\n";
        return false;
    }

    if (m_help) {
        std::cout << "\n" << m_appName << " " << m_appVersion << "\n";
        if (!m_appDescription.empty()) std::cout << m_appDescription << "\n";
        std::cout << "\nOptions:\n"
                  << "  -l, --log-level <level>     " << allowedLevelsList() << "\n"
                  << "  -f, --log-file  <path>      Write logs to file\n"
                  << "      --no-log-file           Do not write logs to a file\n"
                  << "      --check-indents         Monitor missing indent calls\n"
                  << "  -i, --input     <json>      Apply settings from JSON\n"
                  << "      --info                  Show app info & flags\n"
                  << "      --version               Show version\n"
                  << "      --help                  Show this help\n";
        return false;
    }

    if (m_version) {
        std::cout << m_appName << " " << m_appVersion << "\n";
        return false;
    }

    if (m_info) {
#ifdef GADEN_RELEASE_TYPE
        constexpr const char* kReleaseType = GADEN_RELEASE_TYPE;
#else
  #ifdef GADEN_DEBUG
        constexpr const char* kReleaseType = "Debug";
  #else
        constexpr const char* kReleaseType = "Release";
  #endif
#endif

#ifdef GADEN_DEBUG
        constexpr int kgadenDebug = 1;
#else
        constexpr int kgadenDebug = 0;
#endif

#ifdef GADEN_LOGS
        constexpr int kgadenLogs = 1;
#else
        constexpr int kgadenLogs = 0;
#endif

        std::cout << "\n" << m_appName << " Version " << m_appVersion
                  << ", " << kReleaseType
                  << ", with options:\n"
                  << "    GADEN_DEBUG:\t"  << kgadenDebug << "\n"
                  << "    GADEN_LOGS: \t" << kgadenLogs  << "\n"
                  << std::endl;
        return false;
    }

    if (m_logLevel) {
        if (auto lvl = parseLogLevel(*m_logLevel)) {
            log.setLevel(*lvl);
        } else {
            std::cerr << "Invalid log level: " << *m_logLevel << "\n"
                      << "Allowed: " << allowedLevelsList() << "\n";
            return false;
        }
    } else {
        // Apply default log level
        log.setLevel(defaultLogLevel);
    }

    if (m_noLogFile) {
        // no file logging
    } else if (m_logFile) {
        log.setOutputToFile(std::filesystem::path{*m_logFile});
    } else {
        // default: no file logging (match original's pre-CLI default)
        // log.setOutputToFile(); // enable if you prefer default file logging
    }

    if (m_checkIndents) {
        log.enableIndentChecking();
    }

    if (!applyJsonIfProvided()) {
        // warning already printed; continue
    }

    return true;
}

} // namespace gaden::cli
