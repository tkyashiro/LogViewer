#include "LogModel.h"
#include "LogEntry.h"

namespace
{
enum {
    eSeverity = 0,
    eFunc,
    eFile,
    eLine,
    eMessage,
    eThread,
    eTime,
    eSentinel
};
std::array<const char*,7> sHeaders = {
    "Severity",
    "Func",
    "File",
    "Line",
    "Message",
    "Thread",
    "Time",
};
}

int LogModel::rowCount( const QModelIndex & ) const
{
    return entries_.size();
}

int LogModel::columnCount( const QModelIndex & ) const
{
    return eSentinel;
}

QVariant LogModel::data( const QModelIndex &index, int role ) const
{
    if( role != Qt::DisplayRole )
    {
        return QVariant();
    }

    const LogEntry &e = entries_[index.row()];
    switch( index.column() )
    {
    case eSeverity: return e.getSeverity();
    case eFunc: return e.getFunc();
    case eFile: return e.getFile();
    case eLine: return e.getLine();
    case eMessage: return e.getMessage();
    case eThread: return e.getThread();
    case eTime: return e.getTime();
    default:
        Q_ASSERT(false);
        return QVariant();
    }
}

QVariant LogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( role != Qt::DisplayRole )
    {
        return QVariant();
    }

    if( orientation == Qt::Horizontal )
    {
        return sHeaders[section];
    }
    else
    {
        Q_ASSERT( orientation == Qt::Vertical );
        return QVariant(section);
    }
}

void LogModel::append( const LogEntry &e )
{
    const int r = entries_.size();
    QModelIndex idx = index( r, 0 );
    insertRow( r, idx );
    setData( r, e );

    emit logsAdded( 1 );
}

void LogModel::append( const std::vector<LogEntry> && l )
{
    int r = entries_.size();
    const int n = l.size();
    if( n <= 0 )
    {
        return;
    }

    insertRows( r, n, index(r, 0) );

    for( int i = 0; i < n; ++i )
    {
        setData( r+i, l[i] );
    }

    emit logsAdded( n );
}

bool LogModel::insertRow(int row, const QModelIndex &parent)
{
    beginInsertRows( parent, row, row );
    entries_.push_back( LogEntry() );
    endInsertRows();
}

bool LogModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows( parent, row, row+count-1 );
    for( int i = 0; i < count; ++i )
    {
        entries_.push_back( LogEntry() );
    }
    endInsertRows();
}

void LogModel::setData( int r, const LogEntry &e )
{
    Q_ASSERT( r < entries_.size() );
    entries_[r] = e;
    emit dataChanged( index(r,0), index(r,eSentinel) );
}

bool LogModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if( role != Qt::DisplayRole )
    {
        return false;
    }

    const int r = index.row();
    Q_ASSERT( r < entries_.size() );
    LogEntry &e = entries_[r];
    switch( index.column() )
    {
    case eSeverity: e.setSeverity( value.toString() ); break;
    case eFunc: e.setFunc( value.toString() ); break;
    case eFile: e.setFile( value.toString() ); break;
    case eLine: e.setLine( value.toInt() ); break;
    case eMessage: e.setMessage( value.toString() ); break;
    case eThread: e.setThread( value.toLongLong() ); break;
    case eTime: e.setTime( value.toDateTime() ); break;
    default:
        return false;
    }

    emit dataChanged( index, index );
    return true;
}

void LogModel::clear()
{
    if( entries_.size() > 0 )
    {
        beginRemoveRows( QModelIndex(), 0, entries_.size()-1 );
        entries_.clear();
        endRemoveRows();
    }
}
