#pragma once

#include <print>
#include <string>

namespace Exray
{
enum class LogType
{
    Info,
    Warning,
    Error
};

inline void log(const std::string &message, LogType type = LogType::Info)
{
    std::string messageType = "";
    switch (type)
    {
    case LogType::Warning:
        messageType = "WARNING";
        break;

    case LogType::Error:
        messageType = "ERROR";
        break;

    default:
        messageType = "INFO";
        break;
    }
    std::println("[{}] {}", messageType, message);
}
}