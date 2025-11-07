#pragma once

#include <optional>
#include <string>
#include <vector>

namespace gaden {
    enum class LogLevel;
    class Logger;
}

namespace gaden::cli {

class LoggerConfigurator {
public:
    explicit LoggerConfigurator(std::string appName = {},
        std::string appVersion = {},
        std::string appDescription = {}
    ) :
        m_appName(std::move(appName)),
        m_appVersion(std::move(appVersion)),
        m_appDescription(std::move(appDescription))
    {}

    bool process(int argc, char** argv, gaden::LogLevel defaultLogLevel);

    std::optional<std::string> jsonPath() const { return m_jsonPath; }
    const std::vector<std::string>& rest() const { return m_rest; }

private:
    static std::optional<gaden::LogLevel> parseLogLevel(std::string s);
    static const char* allowedLevelsList();
    bool applyJsonIfProvided();
    bool applyLoggerSettingsFromJson(const std::string& jsonText);
    bool parseArgs(int argc, char** argv);

private:
    std::string m_appName;
    std::string m_appVersion;
    std::string m_appDescription;

    std::optional<std::string> m_logLevel;
    std::optional<std::string> m_logFile;
    bool m_noLogFile = false;
    bool m_checkIndents = false;
    bool m_info = false;
    bool m_help = false;
    bool m_version = false;
    std::optional<std::string> m_jsonPath;
    std::vector<std::string> m_rest;
};

} // namespace gaden::cli
