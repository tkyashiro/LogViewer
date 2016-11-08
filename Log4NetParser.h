#pragma once

#ifndef LOG4NETPARSER_H
#define LOG4NETPARSER_H

#include "LogParser.h"
#include "LogEntry.h"

#include <QString>
#include <QStringList>
#include <QRegExp>

#include <map>

class RegExpParser : public LogParser
{
public:
    RegExpParser(const QString &exp);
    LogEntry parse( const QString &buf );

    void setMapping( LogEntry::Item item, int cap );
    void setDateTimeFormat( const QString &str );

private:
    QRegExp regex_;
    std::map<LogEntry::Item,int> mapping_;

    QString dateTimeFormat_;
private:
    LogEntry entryFromRegex() const;
};

#endif
