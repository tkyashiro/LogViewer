#ifndef LOGMODELPROXY_H
#define LOGMODELPROXY_H

#include <QDateTime>
#include <QSortFilterProxyModel>

class LogModelProxy : public QSortFilterProxyModel {
public:
    enum { FilterRole = Qt::UserRole + 1 };

public:
    LogModelProxy();

    virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role);

    void clearFilter();

    void setSeverityFilter(const QRegExp& severityFilter);
    void setFuncFilter(const QRegExp& funcFilter);
    void setFileFilter(const QRegExp& fileFilter);
    void setMessageFilter(const QRegExp& messageFilter);

    void setThreadsFilter(const QList<int>& list);

    void setMinDateTime(const QDateTime& minDate);
    void setMaxDateTime(const QDateTime& maxDate);

private:
    QRegExp severityFilter_;
    QRegExp funcFilter_;
    QRegExp fileFilter_;
    QRegExp messageFilter_;

    QList<int> listThread_;

    QDateTime minDateTime_;
    QDateTime maxDateTime_;
};

#endif // LOGMODELPROXY_H
