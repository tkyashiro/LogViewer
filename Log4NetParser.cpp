#include "Log4NetParser.h"

#include "LogEntry.h"

RegExpParser::RegExpParser(const QString& exp)
    : regex_(exp)
    , dateTimeFormat_("yy/MM/dd hh:mm:ss")
{
    mapping_[LogEntry::Item::severity] = -1;
    mapping_[LogEntry::Item::func] = -1;
    mapping_[LogEntry::Item::file] = -1;
    mapping_[LogEntry::Item::line] = -1;
    mapping_[LogEntry::Item::message] = -1;
    mapping_[LogEntry::Item::thread] = -1;
    mapping_[LogEntry::Item::time] = -1;
}

RegExpParser::RegExpParser(const Settings& settings)
{
    regex_ = QRegExp(settings.pattern);
    mapping_ = settings.mapping;
    dateTimeFormat_ = settings.dateTimeFormat;
}

void RegExpParser::setMapping(LogEntry::Item item, int cap)
{
    mapping_[item] = cap;
}

void RegExpParser::setDateTimeFormat(const QString& str)
{
    dateTimeFormat_ = str;
}

LogEntry RegExpParser::parse(const QString& buf)
{
    if (regex_.exactMatch(buf)) {
        return entryFromRegex();
    } else {
        throw std::exception(); ///@todo use dedicated exception
    }
}

LogEntry RegExpParser::entryFromRegex() const
{
    LogEntry e;
    int pos = -1;

    if ((pos = mapping_.at(LogEntry::Item::severity)) > 0) {
        e.setSeverity(regex_.cap(pos));
    }

    if ((pos = mapping_.at(LogEntry::Item::func)) > 0) {
        e.setFunc(regex_.cap(pos));
    }

    if ((pos = mapping_.at(LogEntry::Item::file)) > 0) {
        e.setFile(regex_.cap(pos));
    }

    if ((pos = mapping_.at(LogEntry::Item::line)) > 0) {
        e.setLine(regex_.cap(pos).toInt());
    }

    if ((pos = mapping_.at(LogEntry::Item::message)) > 0) {
        e.setMessage(regex_.cap(pos).replace(QChar::CarriageReturn, QChar::LineFeed));
    }

    if ((pos = mapping_.at(LogEntry::Item::thread)) > 0) {
        e.setThread(regex_.cap(pos).toInt());
    }

    if ((pos = mapping_.at(LogEntry::Item::time)) > 0) {
        e.setTime(QDateTime::fromString(regex_.cap(pos), dateTimeFormat_));
    }

    return e;
}
