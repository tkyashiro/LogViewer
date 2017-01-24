#include "LogSourceFile.h"

#include "LogParser.h"

#include <QFile>
#include <QFileSystemWatcher>
#include <QTextStream>
#include <QTimer>

#include <assert.h>

#define USE_FILEWATCHER 0

namespace
{
class TimerResetter
{
public:
    TimerResetter( QTimer *timer ) : timer_(timer) { timer_->stop(); }
    ~TimerResetter() { timer_->start(); }
private:
    QTimer *timer_;
};
}

LogSourceFile::LogSourceFile( LogParser *parser )
    : parser_(parser),
      fileWatcher_(new QFileSystemWatcher()),
      separator_(QChar::CarriageReturn),
      nextPos_(0)
{
    assert( parser_ );
#if USE_FILEWATCHER
    QObject::connect( fileWatcher_.get(), &QFileSystemWatcher::fileChanged,
                      this, &LogSourceFile::onFileChanged );
    QObject::connect( fileWatcher_.get(), &QFileSystemWatcher::directoryChanged,
                      this, &LogSourceFile::onFileChanged );
#else
    timer_ = new QTimer(this);
    timer_->setInterval( 1/*sec*/ * 1000 );
    connect( timer_, &QTimer::timeout, this, &LogSourceFile::onTimeout );
    timer_->start();
#endif
}

LogSourceFile::~LogSourceFile()
{
}

void LogSourceFile::setSeparator( const QString &sep )
{
    separator_ = sep;
}

bool LogSourceFile::setPath( const QString &path )
{
#if USE_FILEWATCHER
    if( fileWatcher_->addPath( path ) )
    {
        if( ! path_.isEmpty() )
        {
            fileWatcher_->removePath(path_);
        }

        path_ = path;
        nextPos_ = 0;
        buf_.clear();
        return true;
    }

    return false;
#else
    path_ = path;
    nextPos_ = 0;
    buf_.clear();
    return true;
#endif
}

void LogSourceFile::onFileChanged( const QString &path )
{
    if( path_ != path )
    {
        return;
    }

    read();
}

void LogSourceFile::onTimeout()
{
    read();
}

void LogSourceFile::read()
{
    TimerResetter resetter(timer_);

    QFile file( path_ );
    if ( ! file.open( QIODevice::ReadOnly ) )
    {
        ///@todo error handling
        return;
    }

    if( ! file.seek( nextPos_ ) )
    {
        ///@todo error handling
        return;
    }

    QTextStream st( &file );
    std::vector<LogEntry> list;
    int idx = 0;

    while( ! st.atEnd() )
    {
        buf_.append( st.readLine() ).append( QChar::CarriageReturn );

        if( (idx = buf_.indexOf(separator_)) >= 0 )
        {
            QString s = buf_.left(idx);
            buf_.remove( 0, idx + separator_.length() );
            try
            {
                list.push_back(parser_->parse( s ));
            }
            catch(const std::exception &e)
            {
                LogEntry le;
                le.setMessage( s );
                list.push_back(le);
                emit failedToParse(s);
            }
        }

        if( list.size() >= 100 )
        {
            append( std::move(list) );
            list = std::vector<LogEntry>();
        }
    }

    append( std::move(list) );

    emit waitingToBeParsed(buf_);

    nextPos_ = st.pos();
}

