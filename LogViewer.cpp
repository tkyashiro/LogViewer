#include "LogViewer.h"

#include "LogModel.h"
#include "LogSource.h"

#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QTableView>
#include <QTimer>

#include <assert.h>

LogViewer::LogViewer( QWidget *parent )
    : QWidget(parent),
    source_(nullptr),
    model_(new LogModel()),
    timer_(new QTimer()),
    total_(0),
    lbl_(new QLabel()),
    lblFailed_(new QLabel()),
    table_(new QTableView())
{
    timer_->setInterval(500);
    connect( timer_.get(), &QTimer::timeout, this, &LogViewer::onTimeOut );

    QVBoxLayout *vl = new QVBoxLayout();

    table_->setModel( model_.get() );
    vl->addWidget( table_ );

    QHBoxLayout *hl = new QHBoxLayout();
    {
        hl->addWidget( lbl_ );
        hl->addStretch();
        hl->addWidget( lblFailed_ );
    }
    vl->addLayout( hl );

    setLayout(vl);

    connect( model_.get(), &LogModel::logsAdded, this, &LogViewer::logsAdded );
}

LogViewer::~LogViewer()
{
    timer_->stop();
}

void LogViewer::setLogSource( LogSource *source )
{
    timer_->stop();

    source_.reset(source);

    if ( source_ )
    {
        timer_->start();
    }
}

void LogViewer::onTimeOut()
{
    assert( source_.get() );
    if ( ! source_->empty() )
    {
        model_->append( std::move( source_->popAll() ) );
    }
}

void LogViewer::logsAdded( int n )
{
    total_ += n;
    lbl_->setText( QString("added : %1 / total : %2").arg(n).arg(total_));
}
