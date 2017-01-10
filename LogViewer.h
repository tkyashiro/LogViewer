#pragma once

#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QWidget>

#include <memory>

class LogSource;
class LogModel;
class LogModelProxy;
class QLabel;
class QTableView;

class LogViewer : public QWidget
{
    Q_OBJECT
public:
    LogViewer( QWidget *parent = 0 );
    ~LogViewer();

    void setLogSource( LogSource *source );
    void clear();

    QMap<QString,QColor> getColors() const;
    void setColors(const QMap<QString,QColor> &colors);

    QList<int> getColumnWidths() const;
    void setColumnWidths( const QList<int> &widths );
private:
    std::unique_ptr<LogSource> source_;

    std::unique_ptr<LogModel> model_;
    std::unique_ptr<LogModelProxy> proxy_;
    std::unique_ptr<QTimer> timer_;

    int total_;
    QLabel *lbl_;
    QLabel *lblFailed_;
    QTableView *table_;
private:
    void onTimeOut();
    void logsAdded( int n );
    void maybeScroll();
    void inputFilter(int idx);
};


#endif


