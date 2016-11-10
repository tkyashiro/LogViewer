#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "LogViewer.h"
#include "LogSourceFile.h"
#include "LogParser.h"
#include "Log4NetParser.h"

#include <QFileDialog>
#include <QLayout>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    viewer_ = new LogViewer();

    QHBoxLayout *l = new QHBoxLayout();
    l->addWidget( viewer_ );
    ui->centralWidget->setLayout( l );

    connect( ui->actionOpen, &QAction::triggered, this, &MainWindow::openNewSource );

    loadSettings();
}

void MainWindow::loadSettings()
{
    QSettings s;

    loadLastUsedFile( s );
    restoreLastSize();
}

void MainWindow::loadLastUsedFile( QSettings &s )
{
    const int n = s.beginReadArray("LastUsed");
    for( int i = 0; i < n; ++i )
    {
        s.setArrayIndex(i);
        lastUsed_.push_back( s.value("Path").toString() );
    }
    s.endArray();
    updateLastUsedFileMenu();

}

void MainWindow::restoreLastSize()
{
    QSettings s;

    if( s.contains("Size") )
    {
        resize( s.value("Size").toSize() );
    }
    if( s.value("Maximized", false).toBool() )
    {
        showMaximized();
    }
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::saveSettings()
{
    saveLastUsedFile();
    saveCurrentSize();
}

void MainWindow::saveCurrentSize()
{
    QSettings s;
    s.setValue( "Size", size() );
    s.setValue( "Maximized", isMaximized() );
}

void MainWindow::openNewSource()
{
    static QString path;
    path = QFileDialog::getOpenFileName();
    if( ! path.isEmpty() )
    {
        openSource(path);
    }
}

void MainWindow::openSource( const QString &path )
{
    if( ! QFile::exists(path) )
    {
        return;
    }

#if TEST_TYA

    ///@todo load config
    const QString exp("(\\[.*\\])(\\[.*\\])(\\[.*\\])(\\[.*\\])(\\[.*\\])(\\[.*\\])");
    std::unique_ptr<RegExpParser> parser(new RegExpParser(exp));
    parser->setMapping(LogEntry::Item::severity, 1);
    parser->setMapping(LogEntry::Item::time, 2);
    parser->setMapping(LogEntry::Item::file, 3);
    parser->setMapping(LogEntry::Item::func, 4);
    parser->setMapping(LogEntry::Item::line, 5);
    parser->setMapping(LogEntry::Item::message, 6);

    std::unique_ptr<LogSourceFile> source( new LogSourceFile(parser.release()) );
#else
    const QString exp(
                ".*: (\\d+-\\d+-\\d+ \\d+:\\d+:\\d+,\\d+): \\[(\\d+)\\]: (.*)\\s*:\\s*"
                "\\[(.*)\\]\\s*-\\s*(\\d+)\\s*-\\s*(\\S+)\\s*(\\S.*)$"
                );
    std::unique_ptr<RegExpParser> parser(new RegExpParser(exp));
    parser->setMapping(LogEntry::Item::time, 1);
    parser->setMapping(LogEntry::Item::thread, 2);
    parser->setMapping(LogEntry::Item::severity, 3);
    parser->setMapping(LogEntry::Item::func, 4);
    parser->setMapping(LogEntry::Item::line, 5);
    parser->setMapping(LogEntry::Item::file, 6);
    parser->setMapping(LogEntry::Item::message, 7);

    std::unique_ptr<LogSourceFile> source( new LogSourceFile(parser.release()) );
    source->setSeparator(
                "====================================================================================================================="
                );
#endif

    if( ! source->setPath( path ) )
    {
        return;
    }

    source->read();

    viewer_->clear();
    viewer_->setLogSource( source.release() );

    updateLastUsedFile( path );
}


void MainWindow::updateLastUsedFile( const QString &path )
{
    if( lastUsed_.contains( path ) )
    {
        lastUsed_.removeOne( path );
    }
    lastUsed_.prepend( path );

    saveLastUsedFile();

    updateLastUsedFileMenu();
}

void MainWindow::saveLastUsedFile()
{
    QSettings s;
    const int n = std::min(lastUsed_.size(), 5);
    s.beginWriteArray("LastUsed");
    for( int i = 0; i < n; ++i )
    {
        s.setArrayIndex(i);
        s.setValue( "Path", lastUsed_[i] );
    }
    s.endArray();
}

void MainWindow::updateLastUsedFileMenu()
{
    const int n = lastUsed_.size();
    if( n == 0 )
    {
        return;
    }

    QMenu *recentMenu = new QMenu();
    for( int i = 0; i < n; ++i )
    {
        QAction *a = recentMenu->addAction(QFileInfo(lastUsed_[i]).fileName());
        a->setData( lastUsed_[i] );
        connect( a, &QAction::triggered, this, &MainWindow::openRecent );
    }
    ui->actionRecent->setMenu( recentMenu );
}

void MainWindow::openRecent()
{
    QAction *a = qobject_cast<QAction*>(sender());
    Q_ASSERT( a );
    openSource( a->data().toString() );
}
