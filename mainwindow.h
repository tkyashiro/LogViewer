#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class LogViewer;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    LogViewer *viewer_;
    QList<QString> lastUsed_;
private:
    void openNewSource();
    void openSource( const QString &path );
    Q_SLOT void openRecent();
    void updateLastUsedFile( const QString &path );
    void updateLastUsedFileMenu();
};

#endif // MAINWINDOW_H
