#pragma once

#ifndef LOGSOURCEFILE_H
#define LOGSOURCEFILE_H

#include "LogSource.h"

class LogParser;
class QTimer;
class QFileSystemWatcher;

class LogSourceFile : public LogSource {
    Q_OBJECT
public:
    LogSourceFile(LogParser* parser);
    ~LogSourceFile();

    bool setPath(const QString& path);
    bool hasError() const { return false; }

    void setSeparator(const QString& sep);

    void read();

    Q_SIGNAL void failedToParse(const QString& str);
    Q_SIGNAL void waitingToBeParsed(const QString& s);

private:
    std::unique_ptr<LogParser> parser_;
    std::unique_ptr<QFileSystemWatcher> fileWatcher_;
    QTimer* timer_;

    QString path_;
    QString separator_;

    QString buf_;
    int64_t nextPos_;

private:
    void onFileChanged(const QString& path);
    void onTimeout();
};

#endif
