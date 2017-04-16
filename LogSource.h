#pragma ocne

#ifndef LOGSOURCE_H
#define LOGSOURCE_H

#include "LogEntry.h"

#include <QObject>

#include <memory>

namespace std {
class mutex;
}

class LogSource : public QObject {
public:
    virtual ~LogSource();

    bool empty() const;
    std::vector<LogEntry> popAll();

    virtual bool hasError() const = 0;

protected:
    LogSource();

    void append(const LogEntry& e);
    void append(const std::vector<LogEntry>&& l);

private:
    std::unique_ptr<std::mutex> mutex_;
    std::vector<LogEntry> logs_;
};

#endif
