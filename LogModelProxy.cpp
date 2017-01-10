#include "LogModelProxy.h"
#include "LogModel.h"

LogModelProxy::LogModelProxy()
{
}

QVariant LogModelProxy::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == FilterRole && orientation == Qt::Horizontal)
    {
        switch (section) {
        case LogModel::eTime    : return QVariant(); ///@todo
        case LogModel::eThread  : return QVariant();///@todo
        case LogModel::eSeverity: return severityFilter_;
        case LogModel::eMessage : return messageFilter_;
        case LogModel::eFunc    : return funcFilter_;
        case LogModel::eFile    : return fileFilter_;
        case LogModel::eLine    : return QVariant();
        case LogModel::eSentinel:
        default:
            return QVariant();
        }
    }

    auto withRegExp = [](const QVariant &original, const QRegExp &regexp)
    {
        return regexp.isEmpty() ? original
                                : QVariant(QString("%1 (%2)").arg(original.toString()).arg(regexp.pattern()));
    };

    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        const QVariant srcData = sourceModel()? sourceModel()->headerData(section, orientation, role)
                                              : QVariant();
        switch (section) {
        case LogModel::eTime    : return srcData; ///@todo
        case LogModel::eThread  : return srcData; ///@todo
        case LogModel::eSeverity: return withRegExp(srcData, severityFilter_);
        case LogModel::eMessage : return withRegExp(srcData, messageFilter_);
        case LogModel::eFunc    : return withRegExp(srcData, funcFilter_);
        case LogModel::eFile    : return withRegExp(srcData, fileFilter_);
        case LogModel::eLine    : return srcData;
        case LogModel::eSentinel:
        default:
            return srcData;
        }
    }

    return sourceModel()? sourceModel()->headerData(section, orientation, role)
                        : QVariant();
}

bool LogModelProxy::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (orientation == Qt::Horizontal)
    {
        switch (section) {
        case LogModel::eTime    : break; ///@todo
        case LogModel::eThread  : break; ///@todo
        case LogModel::eSeverity: setSeverityFilter(value.toRegExp()); break;;
        case LogModel::eMessage : setMessageFilter(value.toRegExp()); break;;
        case LogModel::eFunc    : setFuncFilter(value.toRegExp()); break;;
        case LogModel::eFile    : setFileFilter(value.toRegExp()); break;;
        case LogModel::eLine    : break;
        case LogModel::eSentinel:
        default:
            break;
        }
        return true;
    }
    else
    {
        return sourceModel()->setHeaderData(section, orientation, value, role);
    }
}

void LogModelProxy::setSeverityFilter(const QRegExp& severityFilter)
{
    severityFilter_ = severityFilter;
    emit headerDataChanged(Qt::Horizontal, LogModel::eSeverity, LogModel::eSeverity);
    invalidateFilter();
}

void LogModelProxy::setFuncFilter(const QRegExp& funcFilter)
{
    funcFilter_ = funcFilter;
    emit headerDataChanged(Qt::Horizontal, LogModel::eFunc, LogModel::eFunc);
    invalidateFilter();
}

void LogModelProxy::setFileFilter(const QRegExp& fileFilter)
{
    fileFilter_ = fileFilter;
    emit headerDataChanged(Qt::Horizontal, LogModel::eFile, LogModel::eFile);
    invalidateFilter();
}

void LogModelProxy::setMessageFilter(const QRegExp& messageFilter)
{
    messageFilter_ = messageFilter;
    emit headerDataChanged(Qt::Horizontal, LogModel::eMessage, LogModel::eMessage);
    invalidateFilter();
}

void LogModelProxy::setThreadsFilter( const QList<int> &list )
{
    listThread_ = list;
    emit headerDataChanged(Qt::Horizontal, LogModel::eThread, LogModel::eThread);
    invalidateFilter();
}

void LogModelProxy::setMinDateTime( const QDateTime &minDate )
{
    minDateTime_ = minDate;
    emit headerDataChanged(Qt::Horizontal, LogModel::eTime, LogModel::eTime);
    invalidateFilter();
}

void LogModelProxy::setMaxDateTime( const QDateTime &maxDate )
{
    maxDateTime_ = maxDate;
    emit headerDataChanged(Qt::Horizontal, LogModel::eTime, LogModel::eTime);
    invalidateFilter();
}

bool LogModelProxy::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (!severityFilter_.isEmpty())
    {
        QModelIndex i = sourceModel()->index(source_row, LogModel::eSeverity, source_parent);
        if ( !sourceModel()->data(i).toString().contains(severityFilter_) )
        {
            return false;
        }
    }

    if (!funcFilter_.isEmpty())
    {
        QModelIndex i = sourceModel()->index(source_row, LogModel::eFunc, source_parent);
        if ( !sourceModel()->data(i).toString().contains(funcFilter_) )
        {
            return false;
        }
    }

    if (!messageFilter_.isEmpty())
    {
        QModelIndex i = sourceModel()->index(source_row, LogModel::eMessage, source_parent);
        if ( !sourceModel()->data(i).toString().contains(messageFilter_) )
        {
            return false;
        }
    }

    if (!fileFilter_.isEmpty())
    {
        QModelIndex i = sourceModel()->index(source_row, LogModel::eFile, source_parent);
        if ( !sourceModel()->data(i).toString().contains(fileFilter_) )
        {
            return false;
        }
    }

    if (!listThread_.isEmpty())
    {
        QModelIndex i = sourceModel()->index(source_row, LogModel::eThread, source_parent);
        if ( ! listThread_.contains(sourceModel()->data(i).toInt()) )
        {
            return false;
        }
    }

    if (minDateTime_.isValid())
    {
        QModelIndex i = sourceModel()->index(source_row, LogModel::eTime, source_parent);
        if ( sourceModel()->data(i).toDateTime() < minDateTime_ )
        {
            return false;
        }
    }

    if (maxDateTime_.isValid())
    {
        QModelIndex i = sourceModel()->index(source_row, LogModel::eTime, source_parent);
        if ( sourceModel()->data(i).toDateTime() < maxDateTime_ )
        {
            return false;
        }
    }

    return true;
}
