#include "LogViewer.h"

#include "LogModel.h"
#include "LogSource.h"
#include "LogModelProxy.h"

#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QScrollBar>
#include <QTableView>
#include <QTimer>
#include <QInputDialog>

#include <assert.h>

LogViewer::LogViewer( QWidget *parent )
    : QWidget(parent),
    source_(nullptr),
    model_(new LogModel()),
    proxy_(new LogModelProxy()),
    timer_(new QTimer()),
    total_(0),
    lbl_(new QLabel()),
    lblFailed_(new QLabel()),
    table_(new QTableView())
{
    timer_->setInterval(500);
    connect( timer_.get(), &QTimer::timeout, this, &LogViewer::onTimeOut );

    proxy_->setSourceModel(model_.get());
    table_->setModel(proxy_.get());

    QVBoxLayout *vl = new QVBoxLayout();
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
    connect( model_.get(), &LogModel::logsAdded, this, &LogViewer::maybeScroll );

    connect( table_->horizontalHeader(), &QHeaderView::sectionDoubleClicked, this, &LogViewer::inputFilter );

}

void LogViewer::inputFilter(int idx)
{
    switch (idx)
    {
    case LogModel::eFile:
    case LogModel::eFunc:
    case LogModel::eMessage:
    case LogModel::eSeverity:
    {
        bool ok = false;
        const QString lastPattern = proxy_->headerData(idx, Qt::Horizontal, LogModelProxy::FilterRole).toRegExp().pattern();
        QString filter = QInputDialog::getText(nullptr, tr("Input regular expression"),
                                               tr("Please input reqular expression to search for."),
                                               QLineEdit::Normal,
                                               lastPattern,
                                               &ok);
        if (ok)
        {
            proxy_->setHeaderData(idx, Qt::Horizontal, QRegExp(filter), LogModelProxy::FilterRole);
        }
    }
        break;
    case LogModel::eLine  : break; ///@todo
    case LogModel::eThread: break; ///@todo
    case LogModel::eTime  : break; ///@todo
    case LogModel::eSentinel:
    default:
        break;
    }
}

LogViewer::~LogViewer()
{
    timer_->stop();
}

QMap<QString,QColor> LogViewer::getColors() const
{
    return model_->getColors();
}

void LogViewer::setColors(const QMap<QString,QColor> &colors)
{
    model_->setColors(colors);
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

QList<int> LogViewer::getColumnWidths() const
{
    QList<int> ws;

    QHeaderView *h = table_->horizontalHeader();
    Q_ASSERT( h );
    const int n = h->count();

    for( int c = 0; c < n; ++c )
    {
        ws.append( h->sectionSize(c) );
    }

    return ws;
}

void LogViewer::setColumnWidths( const QList<int> &widths )
{
    const int n = widths.size();
    for( int c = 0; c < n; ++c )
    {
        table_->setColumnWidth( c, widths[c] );
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

void LogViewer::clear()
{
    Q_ASSERT( model_ );
    model_->clear();
}

void LogViewer::maybeScroll()
{
    QScrollBar *vs = table_->verticalScrollBar();
    Q_ASSERT(vs);
    if( vs->value() == vs->maximum() )
    {
        table_->scrollToBottom();
    }
}
