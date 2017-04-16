#include "RegExpParserSettingsWidget.h"

#include <QtWidgets>

#include "LogEntry.h"

namespace {
enum {
    C_ITEM,
    C_MAPPING,
    C_TESTPATTERN,
    C_SENTINEL
};

enum {
    R_severity,
    R_func,
    R_file,
    R_line,
    R_message,
    R_thread,
    R_time,
    R_SENTINEL
};
}

RegExpParserSettingsWidget::RegExpParserSettingsWidget(const RegExpParser::Settings& settings, QWidget* parent)
    : QWidget(parent)
    , model_(settings)
{
    setupUi();

    fromData();
}

void RegExpParserSettingsWidget::setupUi()
{
    auto l = new QFormLayout(this);

    auto vl = new QVBoxLayout();
    {
        vl->addWidget(textPattern_ = new QTextEdit());
        textPattern_->setMinimumHeight(20);
        textPattern_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

        vl->addWidget(tblMapping_ = new QTableWidget());
        tblMapping_->setColumnCount(C_SENTINEL);
        tblMapping_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        const auto labels = QStringList{ "Item", "Match", "Matched pattern" };
        Q_ASSERT(C_SENTINEL == labels.size());
        tblMapping_->setHorizontalHeaderLabels(labels);
        tblMapping_->setRowCount(R_SENTINEL);

        vl->addWidget(new QLabel(tr("Test pattern")));
        vl->addWidget( textTestPattern_ = new QTextEdit());
        textTestPattern_->setMinimumHeight(20);
        textTestPattern_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    }
    l->addRow(tr("Regular expression"), vl);

    l->addRow(tr("Date/Time format"), lineDateTimeFormat_ = new QLineEdit());

    setLayout(l);
}

void RegExpParserSettingsWidget::fromData()
{
    textPattern_->setText(model_.pattern);
    lineDateTimeFormat_->setText(model_.dateTimeFormat);

    const auto labels = QStringList{ "Severity", "Function", "File", "Line", "Message", "Thread", "Time" };
    const auto items = QList<LogEntry::Item>{
        LogEntry::Item::severity, LogEntry::Item::func, LogEntry::Item::file, LogEntry::Item::line, LogEntry::Item::message, LogEntry::Item::thread, LogEntry::Item::time,
    };
    for (int r = 0; r < R_SENTINEL; ++r) {
        tblMapping_->setItem(r, C_ITEM, new QTableWidgetItem(labels[r]));
        tblMapping_->setItem(r, C_MAPPING, new QTableWidgetItem(QString::number(model_.mapping[items[r]])));
    }
}
