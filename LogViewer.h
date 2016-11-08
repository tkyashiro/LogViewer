#pragma once

#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QWidget>

#include <memory>

class LogSource;
class LogModel;
class QLabel;
class QTableView;

class LogViewer : public QWidget
{
    Q_OBJECT
public:
    LogViewer( QWidget *parent = 0 );
    ~LogViewer();

    void setLogSource( LogSource *source );
private:
    std::unique_ptr<LogSource> source_;

    std::unique_ptr<LogModel> model_;
    std::unique_ptr<QTimer> timer_;

    int total_;
    QLabel *lbl_;
    QTableView *table_;
private:
    void onTimeOut();
    void logsAdded( int n );
};


#endif


