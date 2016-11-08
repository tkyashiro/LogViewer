#pragma once

#ifndef LOGMODEL_H
#define LOGMODEL_H

#include "LogEntry.h"

#include <QAbstractTableModel>

#include <vector>


class LogModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    void append( const LogEntry &e );
    void append( const std::vector<LogEntry> && l );
    bool insertRow(int row, const QModelIndex &parent);
    bool insertRows(int row, int count, const QModelIndex &parent);
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    QVariant data( const QModelIndex &index, int role ) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    int rowCount( const QModelIndex &parent ) const;
    int columnCount( const QModelIndex &parent ) const;

    Q_SIGNAL void logsAdded( int n );
private:
    std::vector<LogEntry> entries_;
private:
    void setData( int r, const LogEntry &e );
};

#endif
