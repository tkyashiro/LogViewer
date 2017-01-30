#include "LogViewerSettingsWidget.h"

#include "LogViewer.h"

#include <QColorDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>
#include <QPushButton>
#include <QTableWidget>

namespace
{
enum { C_SEVERITY = 0, C_COLOR, C_NUM_COLUMN };
}

LogViewerSettingsWidget::LogViewerSettingsWidget(QSettings *settings, QWidget *parent) : QWidget(parent)
{
    Q_ASSERT( settings );

    setupUi();

    lineSource_->setText(settings->value("source").toString());
    lineLocal_->setText(settings->value("local").toString());

    //toTable( viewer );
}

void LogViewerSettingsWidget::setupUi()
{
    QVBoxLayout *vl = new QVBoxLayout();

    // vl->addWidget( table_ = new QTableWidget(this) );
    // table_->setColumnCount( C_NUM_COLUMN );

    QGroupBox *grpSourceMap = new QGroupBox();
    {
        QGridLayout *g = new QGridLayout();
        g->addWidget(new QLabel(tr("Source")), 0, 0);
        g->addWidget(lineSource_ = new QLineEdit(), 0, 1);

        g->addWidget(new QLabel(tr("Local")), 1, 0);
        g->addWidget(lineLocal_ = new QLineEdit(), 1, 1);
        grpSourceMap->setLayout(g);
    }
    vl->addWidget( grpSourceMap );

    setLayout( vl );
}

#if 0
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
#endif

void LogViewerSettingsWidget::applyOn( QSettings &settings, LogViewer *viewer ) const
{
    // QMap<QString, QColor> colors = colorsFromTable(table_);
    // viewer->setColors( colors );

    settings.setValue("source", lineSource_->text());
    settings.setValue("local", lineLocal_->text());

    viewer->setMapping( lineSource_->text(), lineLocal_->text() );
}
