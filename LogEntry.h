#pragma once

#ifndef LOGENTRY_H
#define LOGENTRY_H

#include <QString>
#include <QDateTime>

class LogEntry
{
public:
    enum class Item {
        severity,
        func,
        file,
        line,
        message,
        thread,
        time,
    };
public:
	LogEntry();
    LogEntry( QString severity,
              QString func, QString file, int32_t line,
              QString message,
              int64_t thread, QDateTime time );

	QString getSeverity() const;
	void setSeverity(const QString &severity);

	QString getFunc() const;
	void setFunc(const QString &func);

	QString getFile() const;
	void setFile(const QString &file);

	int32_t getLine() const;
	void setLine(int32_t line);

	QString getMessage() const;
	void setMessage(const QString &message);

	int64_t getThread() const;
	void setThread(int64_t th);

	QDateTime getTime() const;
	void setTime(const QDateTime &t) ;
private:
	QString severity_;
	QString func_;
	QString file_;
	int32_t line_;
	QString message_;

	int64_t thread_;
	QDateTime time_;
};

#endif
