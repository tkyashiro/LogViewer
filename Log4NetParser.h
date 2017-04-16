#pragma once

#ifndef LOG4NETPARSER_H
#define LOG4NETPARSER_H

#include "LogEntry.h"
#include "LogParser.h"

#include <QRegExp>
#include <QString>
#include <QStringList>

#include <map>

class RegExpParser : public LogParser {
public:
    struct Settings {
        QString pattern;
        std::map<LogEntry::Item, int> mapping;
        QString dateTimeFormat;
    };

public:
    RegExpParser(const QString& exp);
    RegExpParser(const Settings& settings);

    LogEntry parse(const QString& buf);

    void setMapping(LogEntry::Item item, int cap);
    void setDateTimeFormat(const QString& str);

private:
    QRegExp regex_;
    std::map<LogEntry::Item, int> mapping_;

    QString dateTimeFormat_;

private:
    LogEntry entryFromRegex() const;
};

#endif
