#include "LogViewerSettingsWidget.h"

#include "LogViewer.h"

#include <QColorDialog>
#include <QGridLayout>
#include <QPushButton>
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

    int r = 0;
    for (auto it = colors.begin(); it != colors.end(); ++it, ++r)
    {
        table_->setItem( r, C_SEVERITY, new QTableWidgetItem( it.key() ) );

        QPushButton *p = new QPushButton( it.value().name() );
        table_->setCellWidget( r, C_COLOR, p );
        QColor defaultcolor = it.value();
        connect( p, &QPushButton::clicked,
                 [&p, defaultcolor](){
            QColor c = QColorDialog::getColor( defaultcolor );
            if( c.isValid() )
            {
                //p->setText( c );
            }
        }
        );
    }
}

namespace
{
QMap<QString, QColor> colorsFromTable( QTableWidget *w )
{
    QMap<QString, QColor> m;

    return m;
}
}

void LogViewerSettingsWidget::applyOn( LogViewer *viewer ) const
{
    QMap<QString, QColor> colors = colorsFromTable(table_);

    viewer->setColors( colors );
}
