#include <LogSource.h>

#include <mutex>

LogSource::LogSource()
    : mutex_(new std::mutex())
{
}

LogSource::~LogSource()
{
}

bool LogSource::empty() const
{
    mutex_->lock();
    const bool b = (logs_.size() == 0);
    mutex_->unlock();
    return b;
}

std::vector<LogEntry> LogSource::popAll()
{
    mutex_->lock();

    std::vector<LogEntry> e = std::move(logs_);
    logs_ = std::vector<LogEntry>();

    mutex_->unlock();
    return e;
}

void LogSource::append( const LogEntry &e )
{
    mutex_->lock();
    logs_.push_back(e);
    mutex_->unlock();
}

void LogSource::append( const std::vector<LogEntry> && l )
{
    mutex_->lock();
    std::copy( l.begin(), l.end(), std::back_inserter(logs_) );
    mutex_->unlock();
}

