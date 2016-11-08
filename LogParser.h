#pragma once

#ifndef LOGPARSER_H
#define LOGPARSER_H

class QString;

#include <vector>

class LogEntry;

class LogParser
{
public:
    virtual LogEntry parse( const QString &buf ) = 0;

protected:
    LogParser(){}
};

#endif

