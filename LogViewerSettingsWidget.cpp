#include "LogViewerSettingsWidget.h"

#include "LogViewer.h"

#include <QGridLayout>
#include <QTableWidget>

namespace
{
enum { C_SEVERITY = 0, C_COLOR, C_NUM_COLUMN };
}

LogViewerSettingsWidget::LogViewerSettingsWidget(LogViewer *viewer, QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *vl = new QVBoxLayout();

    vl->addWidget( table_ = new QTableWidget(this) );
    table_->setColumnCount( C_NUM_COLUMN );

    setLayout( vl );

    toTable( viewer );
}

void LogViewerSettingsWidget::toTable(LogViewer *viewer)
{
    const QMap<QString,QColor> colors = viewer->getColors();
    table_->setRowCount( colors.size() );

}

void LogViewerSettingsWidget::applyOn( LogViewer *viewer ) const
{
}
