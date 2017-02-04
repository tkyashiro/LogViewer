#include "LogEntry.h"

LogEntry::LogEntry()
    : line_(-1), thread_(-1)
{
}

LogEntry::LogEntry( QString severity,
                    QString func, QString file, int32_t line,
                    QString message,
                    int64_t thread, QDateTime time )
    : severity_(severity),
      func_(func), file_(file), line_(line),
      message_(message),
      thread_(thread), time_(time)
{
}

QString LogEntry::getSeverity() const
{
	return severity_;
}

void LogEntry::setSeverity(const QString &severity)
{
	severity_ = severity;
}

QString LogEntry::getFunc() const
{
	return func_;
}

void LogEntry::setFunc(const QString &func)
{
	func_ = func;
}

QString LogEntry::getFile() const
{
	return file_;
}

void LogEntry::setFile(const QString &file)
{
	file_ = file;
}

int32_t LogEntry::getLine() const
{
	return line_;
}

void LogEntry::setLine(int32_t line)
{
	line_ = line;
}

QString LogEntry::getMessage() const
{
	return message_;
}

void LogEntry::setMessage(const QString &message)
{
	message_ = message;
}

int64_t LogEntry::getThread() const
{
	return thread_;
}

void LogEntry::setThread(int64_t th)
{
	thread_ = th;
}

QDateTime LogEntry::getTime() const
{
	return time_;
}

void LogEntry::setTime(const QDateTime &t)
{
	time_ = t;
}
