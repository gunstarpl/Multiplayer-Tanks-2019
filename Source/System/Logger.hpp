#pragma once

#include "Common/Debug.hpp"

enum class LogType
{
    Trace,
    Info,
    Warning,
    Error,
    Fatal,
};

class Logger
{
public:
    // Message structure.
    struct Message
    {
        LogType type;
        std::string text;
    };
    
    // Type declarations.
    using MessageList = std::deque<Message>;

public:
    Logger();
    ~Logger();

    bool initialize();
    void setNextIndent(int indent);
    void write(LogType type, const char* format, ...);
    const MessageList& getMessages() const;

private:
    // Message history.
    const int MaxMessageCount = 1000;
    MessageList m_messages;

    // Message indent.
    int m_nextIndent;

    // File output.
    const char* OutputLogFile = "Log.txt";
    std::ofstream m_logFile;
};

// Logging macros.
extern Logger* g_logger;

#define LOG_INDENT(indent) \
        assert(g_logger != nullptr && "Logger has not been initialized!"); \
        g_logger->setNextIndent(indent);

#define LOG(type, format, ...) \
        assert(g_logger != nullptr && "Logger has not been initialized!"); \
        g_logger->write(type, format, ## __VA_ARGS__);

#define LOG_INFO(format, ...) LOG(LogType::Info, format, ## __VA_ARGS__)
#define LOG_WARNING(format, ...) LOG(LogType::Warning, format, ## __VA_ARGS__)
#define LOG_ERROR(format, ...) LOG(LogType::Error, format, ## __VA_ARGS__)
#define LOG_FATAL(format, ...) LOG(LogType::Fatal, format, ## __VA_ARGS__)

#ifndef NDEBUG
    #define LOG_TRACE(format, ...) LOG(LogType::Trace, format, ## __VA_ARGS__)
#else
    #define LOG_TRACE(format, ...)
#endif