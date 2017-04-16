#include "LogViewerSettings.h"

LogViewerSettings* LogViewerSettings::get()
{
    static LogViewerSettings theSettings;

    return &theSettings;
}

LogViewerSettings::LogViewerSettings()
{
    {
        RegExpParser::Settings s;
        s.pattern = QString("\\[(.*)\\]\\[(.*)\\]\\[(.*)\\]\\[(.*)\\]\\[(.*)\\]\\[(.*)\\]");
        s.mapping[LogEntry::Item::severity] = 1;
        s.mapping[LogEntry::Item::time] = 2;
        s.mapping[LogEntry::Item::file] = 3;
        s.mapping[LogEntry::Item::func] = 4;
        s.mapping[LogEntry::Item::line] = 5;
        s.mapping[LogEntry::Item::message] = 6;

        this->parserSettings_["testing"] = s;
    }
    {
        RegExpParser::Settings s;
        s.pattern = QString("[\\[(.*)\\]\\[(.*)\\]\\[T:(.*)\\]\\[L:(.*)\\]\\[(.*)\\]\\[(.*)\\]\\[(.*)\\](.*)");
        //                   [1478063781833][2][T:1][L:1][C:/jenkinsWork/horus_release_64bit/Horus/Libraries/Model/private/Analysis.cpp][void Analysis::triggerUpdate()][296]Run analysis  19

        s.mapping[LogEntry::Item::time] = 1;
        s.mapping[LogEntry::Item::severity] = 2;
        s.mapping[LogEntry::Item::thread] = 3;
        s.mapping[LogEntry::Item::file] = 5;
        s.mapping[LogEntry::Item::func] = 6;
        s.mapping[LogEntry::Item::line] = 7;
        s.mapping[LogEntry::Item::message] = 8;

        this->parserSettings_["IS"] = s;
    }

    {
        RegExpParser::Settings s;
        s.pattern = QString(".*: (\\d+-\\d+-\\d+ \\d+:\\d+:\\d+,\\d+): \\[(\\d+)\\]: (.*)\\s*:\\s*" // PC name\Account Name: 2016-12-01 16:20:51,640: [1]: INFO :
                            "\\[(.*)\\]\\s*-\\s*(\\d+)\\s*-\\s*([^\\r]+)\\r" // [FuncName] - 207 - path to the file may include space.cpp
                            "\\s*(\\S.*)$" // log log log log...
            );

        s.mapping[LogEntry::Item::time] = 1;
        s.mapping[LogEntry::Item::thread] = 2;
        s.mapping[LogEntry::Item::severity] = 3;
        s.mapping[LogEntry::Item::func] = 4;
        s.mapping[LogEntry::Item::line] = 5;
        s.mapping[LogEntry::Item::file] = 6;
        s.mapping[LogEntry::Item::message] = 7;

        s.dateTimeFormat = QString("yyyy-MM-dd hh:mm:ss,zzz"); // 2016-11-10 14:49:22,965

        this->parserSettings_["TEST2"] = s;

    }

    active_ = "TEST2";
    separator_ = "=====================================================================================================================";
}
