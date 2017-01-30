#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "LogViewer.h"
#include "LogSourceFile.h"
#include "LogParser.h"
#include "Log4NetParser.h"
#include "LogViewerSettingsWidget.h"

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QLabel>
#include <QLayout>
#include <QSettings>

#define TEST_TYA 1
//#define TEST_IS

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUi();

    connect( ui->actionOpen, &QAction::triggered, this, &MainWindow::openNewSource );
    connect( ui->actionSettings, &QAction::triggered, this, &MainWindow::openSettings );

    loadSettings();
}

void MainWindow::setupUi()
{
    viewer_ = new LogViewer();

    QHBoxLayout *l = new QHBoxLayout();
    l->addWidget( viewer_ );
    ui->centralWidget->setLayout( l );

    lblStatus_ = new QLabel();
    ui->statusBar->addWidget(lblStatus_);
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

namespace {

template <typename T>
QVariantList variantListFromList( const QList<T> &l )
{
    QVariantList vl;
    std::for_each( l.begin(), l.end(), [&vl](const T &t){ vl.push_back(t); } );
    return vl;
}

template <typename T>
QList<T> listFromVariantList( const QVariantList &vl )
{
    QList<T> l;
    std::for_each( vl.begin(), vl.end(), [&l](const QVariant &v){ l.push_back(v.value<T>());} );
    return l;
}

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

    if( s.contains("Columns") )
    {
        viewer_->setColumnWidths( listFromVariantList<int>( s.value("Columns").toList() ) );
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

    s.setValue( "Columns", variantListFromList<int>(viewer_->getColumnWidths()) );
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

    ///@todo load config
#if TEST_TYA
    const QString exp("\\[(.*)\\]\\[(.*)\\]\\[(.*)\\]\\[(.*)\\]\\[(.*)\\]\\[(.*)\\]");
    std::unique_ptr<RegExpParser> parser(new RegExpParser(exp));
    parser->setMapping(LogEntry::Item::severity, 1);
    parser->setMapping(LogEntry::Item::time, 2);
    parser->setMapping(LogEntry::Item::file, 3);
    parser->setMapping(LogEntry::Item::func, 4);
    parser->setMapping(LogEntry::Item::line, 5);
    parser->setMapping(LogEntry::Item::message, 6);

    std::unique_ptr<LogSourceFile> source( new LogSourceFile(parser.release()) );
#elif defined(TEST_IS)
    const QString exp("[\\[(.*)\\]\\[(.*)\\]\\[T:(.*)\\]\\[L:(.*)\\]\\[(.*)\\]\\[(.*)\\]\\[(.*)\\](.*)");
    //                 [1478063781833][2][T:1][L:1][C:/jenkinsWork/horus_release_64bit/Horus/Libraries/Model/private/Analysis.cpp][void Analysis::triggerUpdate()][296]Run analysis  19

    std::unique_ptr<RegExpParser> parser(new RegExpParser(exp));
    parser->setMapping(LogEntry::Item::time, 1);
    parser->setMapping(LogEntry::Item::severity, 2);
    parser->setMapping(LogEntry::Item::thread, 3);
    parser->setMapping(LogEntry::Item::file, 5);
    parser->setMapping(LogEntry::Item::func, 6);
    parser->setMapping(LogEntry::Item::line, 7);
    parser->setMapping(LogEntry::Item::message, 8);

    std::unique_ptr<LogSourceFile> source( new LogSourceFile(parser.release()) );
#else
    const QString exp(
                ".*: (\\d+-\\d+-\\d+ \\d+:\\d+:\\d+,\\d+): \\[(\\d+)\\]: (.*)\\s*:\\s*" // PC name\Account Name: 2016-12-01 16:20:51,640: [1]: INFO :
                "\\[(.*)\\]\\s*-\\s*(\\d+)\\s*-\\s*(.+)\\r"                             // [FuncName] - 207 - path to the file may include space.cpp
                "\\s*(\\S.*)$"                                                          // log log log log...
                );


    std::unique_ptr<RegExpParser> parser(new RegExpParser(exp));
    parser->setMapping(LogEntry::Item::time, 1);
    parser->setMapping(LogEntry::Item::thread, 2);
    parser->setMapping(LogEntry::Item::severity, 3);
    parser->setMapping(LogEntry::Item::func, 4);
    parser->setMapping(LogEntry::Item::line, 5);
    parser->setMapping(LogEntry::Item::file, 6);
    parser->setMapping(LogEntry::Item::message, 7);
    parser->setDateTimeFormat("yyyy-MM-dd hh:mm:ss,zzz"); // 2016-11-10 14:49:22,965

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

    connect( source.get(), &LogSourceFile::waitingToBeParsed,
             [this](const QString &text){ lblStatus_->setText((text.length() > 100 ? text.left(100).append("...") : text));} );

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
        a->setToolTip( lastUsed_[i] );
        connect( a, &QAction::triggered, this, &MainWindow::openRecent );
    }
    recentMenu->setToolTipsVisible( true );
    ui->actionRecent->setMenu( recentMenu );
}

void MainWindow::openRecent()
{
    QAction *a = qobject_cast<QAction*>(sender());
    Q_ASSERT( a );
    openSource( a->data().toString() );
}

void MainWindow::openSettings()
{
    QSettings s;

    QDialog dlg;
    LogViewerSettingsWidget *w = new LogViewerSettingsWidget(&s, &dlg);
    QVBoxLayout *l = new QVBoxLayout();
    {
        dlg.setLayout(l);
        l->addWidget(w);

        QDialogButtonBox *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        l->addWidget(btns);
    }
    const int r = dlg.exec();
    if (r == QDialog::Accepted)
    {
        w->applyOn(s, viewer_);
    }
}
