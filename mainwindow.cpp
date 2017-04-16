#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Log4NetParser.h"
#include "LogParser.h"
#include "LogSourceFile.h"
#include "LogViewer.h"
#include "LogViewerSettings.h"
#include "LogViewerSettingsDialog.h"

#include <QDebug>
#include <QSettings>
#include <QtWidgets>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUi();

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openNewSource);
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::openSettings);

    loadSettings();
}

void MainWindow::setupUi()
{
    viewer_ = new LogViewer();

    QHBoxLayout* l = new QHBoxLayout();
    l->addWidget(viewer_);
    ui->centralWidget->setLayout(l);

    lblStatus_ = new QLabel();
    ui->statusBar->addWidget(lblStatus_);
}

void MainWindow::loadSettings()
{
    QSettings s;

    loadLastUsedFile(s);
    restoreLastSize();
}

void MainWindow::loadLastUsedFile(QSettings& s)
{
    const int n = s.beginReadArray("LastUsed");
    for (int i = 0; i < n; ++i) {
        s.setArrayIndex(i);
        lastUsed_.push_back(s.value("Path").toString());
    }
    s.endArray();
    updateLastUsedFileMenu();
}

namespace {

template <typename T>
QVariantList variantListFromList(const QList<T>& l)
{
    QVariantList vl;
    std::for_each(l.begin(), l.end(), [&vl](const T& t) { vl.push_back(t); });
    return vl;
}

template <typename T>
QList<T> listFromVariantList(const QVariantList& vl)
{
    QList<T> l;
    std::for_each(vl.begin(), vl.end(), [&l](const QVariant& v) { l.push_back(v.value<T>()); });
    return l;
}
}

void MainWindow::restoreLastSize()
{
    QSettings s;
    s.beginGroup("Window");

    if (s.contains("Size")) {
        resize(s.value("Size").toSize());
    }
    if (s.value("Maximized", false).toBool()) {
        showMaximized();
    }

    if (s.contains("Columns")) {
        viewer_->setColumnWidths(listFromVariantList<int>(s.value("Columns").toList()));
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
    s.beginGroup("Window");
    s.setValue("Size", size());
    s.setValue("Maximized", isMaximized());

    s.setValue("Columns", variantListFromList<int>(viewer_->getColumnWidths()));
}

void MainWindow::openNewSource()
{
    static QString path;
    path = QFileDialog::getOpenFileName();
    if (!path.isEmpty()) {
        openSource(path);
    }
}

void MainWindow::openSource(const QString& path)
{
    if (!QFile::exists(path)) {
        return;
    }

    qDebug() << "Active parse: " << LogViewerSettings::get()->getActiveParser();
    auto s = LogViewerSettings::get()->getActiveParserSettings();
    std::unique_ptr<RegExpParser> parser(new RegExpParser(s));
    std::unique_ptr<LogSourceFile> source(new LogSourceFile(parser.release()));
    source->setSeparator(LogViewerSettings::get()->getSeparator());

    if (!source->setPath(path)) {
        return;
    }

    source->read();

    connect(source.get(), &LogSourceFile::waitingToBeParsed,
        [this](const QString& text) { lblStatus_->setText((text.length() > 100 ? text.left(100).append("...") : text)); });

    viewer_->clear();
    viewer_->setLogSource(source.release());

    updateLastUsedFile(path);
}

void MainWindow::updateLastUsedFile(const QString& path)
{
    if (lastUsed_.contains(path)) {
        lastUsed_.removeOne(path);
    }
    lastUsed_.prepend(path);

    saveLastUsedFile();

    updateLastUsedFileMenu();
}

void MainWindow::saveLastUsedFile()
{
    QSettings s;
    const int n = std::min(lastUsed_.size(), 5);
    s.beginWriteArray("LastUsed");
    for (int i = 0; i < n; ++i) {
        s.setArrayIndex(i);
        s.setValue("Path", lastUsed_[i]);
    }
    s.endArray();
}

void MainWindow::updateLastUsedFileMenu()
{
    const int n = lastUsed_.size();
    if (n == 0) {
        return;
    }

    QMenu* recentMenu = new QMenu();
    for (int i = 0; i < n; ++i) {
        QAction* a = recentMenu->addAction(QFileInfo(lastUsed_[i]).fileName());
        a->setData(lastUsed_[i]);
        a->setToolTip(lastUsed_[i]);
        connect(a, &QAction::triggered, this, &MainWindow::openRecent);
    }
    recentMenu->setToolTipsVisible(true);
    ui->actionRecent->setMenu(recentMenu);
}

void MainWindow::openRecent()
{
    QAction* a = qobject_cast<QAction*>(sender());
    Q_ASSERT(a);
    openSource(a->data().toString());
}

void MainWindow::openSettings()
{
    QSettings s;

    LogViewerSettingsDialog dlg(&s, viewer_, this);
    dlg.exec();
}
