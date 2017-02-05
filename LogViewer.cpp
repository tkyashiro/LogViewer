#include "LogViewer.h"

#include "LogModel.h"
#include "LogSource.h"
#include "LogModelProxy.h"

#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QScrollBar>
#include <QTableView>
#include <QTextBrowser>
#include <QTimer>
#include <QUrl>
#include <QInputDialog>
#include <QCheckBox>
#include <QDesktopServices>
#include <QDateTimeEdit>
#include <QDialogButtonBox>
#include <QMessageBox>

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

    connect( table_, &QTableView::doubleClicked, this, &LogViewer::onCellDoubleClicked );

    connect( table_->horizontalHeader(), &QHeaderView::sectionDoubleClicked, this, &LogViewer::inputFilter );
}

LogViewer::~LogViewer()
{
    timer_->stop();
}

void LogViewer::inputFilter(int idx)
{
    switch (idx)
    {
    case LogModel::eFile:
    case LogModel::eFunc:
    case LogModel::eMessage:
    case LogModel::eSeverity:
        textFilterDialog(idx);
        break;
    case LogModel::eLine  : break; ///@todo
    case LogModel::eThread: break; ///@todo
    case LogModel::eTime  :
        timeFilterDialog();
        break;
    case LogModel::eSentinel:
    default:
        break;
    }
}

void LogViewer::textFilterDialog(int idx)
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

namespace
{
class DateRangeDialog : public QDialog
{
public:
    DateRangeDialog(const QDateTime &from, const QDateTime &to)
        : chkFrom(new QCheckBox()),
          chkTo(new QCheckBox()),
          editFrom(new QDateTimeEdit(from.isValid() ? from : QDateTime::currentDateTime())),
          editTo(new QDateTimeEdit(to.isValid() ? to : QDateTime::currentDateTime().addDays(1)))
    {
        QVBoxLayout *l = new QVBoxLayout();

        {
            QGridLayout *gl = new QGridLayout();
            gl->addWidget(new QLabel("From:"), 0, 0);
            gl->addWidget(chkFrom, 0, 1);
            gl->addWidget(editFrom, 0, 2);
            gl->addWidget(new QLabel("To:"), 1, 0);
            gl->addWidget(chkTo, 1, 1);
            gl->addWidget(editTo, 1, 2);
            l->addLayout(gl);

            chkFrom->setChecked(from.isValid());
            chkTo->setChecked(to.isValid());
        }

        QDialogButtonBox *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        l->addWidget(btns);

        setLayout(l);

        connect( btns, &QDialogButtonBox::accepted, this, &DateRangeDialog::onOkClicked );
        connect( btns, &QDialogButtonBox::rejected, this, &QDialog::reject );
    }
    const QDateTime fromDateTime() const
    {
        return chkFrom->isChecked() ? editFrom->dateTime()
                                    : QDateTime();
    }
    const QDateTime toDateTime() const
    {
        return chkTo->isChecked() ? editTo->dateTime()
                                  : QDateTime();
    }
private:
    QCheckBox *chkFrom, *chkTo;
    QDateTimeEdit *editFrom;
    QDateTimeEdit *editTo;

    void onOkClicked()
    {
        if (chkFrom->isChecked() && chkTo->isChecked())
        {
            if (editFrom->dateTime() > editTo->dateTime())
            {
                QMessageBox::warning(this, tr("Invalid range"), tr("Please set valid range"));
                return;
            }
        }
        accept();
    }
};
}

void LogViewer::timeFilterDialog()
{
    QVariantMap vm = proxy_->headerData(LogModel::eTime, Qt::Horizontal, LogModelProxy::FilterRole).toMap();

    DateRangeDialog dlg(vm["minDateTime"].toDateTime(), vm["maxDateTime"].toDateTime());
    const int ok = dlg.exec();

    if (ok == QDialog::Accepted)
    {
        QVariantMap ret;
        ret["minDateTime"] = dlg.fromDateTime();
        ret["maxDateTime"] = dlg.toDateTime();
        proxy_->setHeaderData(LogModel::eTime, Qt::Horizontal, ret, LogModelProxy::FilterRole);
    }
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

namespace
{
void showTextBrowser(const QString &string, QWidget *parent);
}

void LogViewer::onCellDoubleClicked(const QModelIndex &index)
{
    switch (index.column()) {
    case LogModel::eFile:
    {
        tryOpenFile(index);
        break;
    }
    case LogModel::eMessage:
    {
        showTextBrowser(model_->data(index, Qt::DisplayRole).toString(), table_);
        break;
    }
    default:
        break;
    }
}

void LogViewer::tryOpenFile(const QModelIndex &index)
{
    QString file = model_->data(index, Qt::DisplayRole).toString();
    const QString &from = sourceMapping_.first;
    if (from.isEmpty())
    {
        // do nothing, use as it is
    }
    else
    {
        const Qt::CaseSensitivity cs = Qt::CaseInsensitive;
        if (!file.startsWith(from, cs))
        {
            QMessageBox::warning(this, tr("Failed to open file"), tr("Could not open %1").arg(file));
            return;
        }
        file.replace(from, sourceMapping_.second, cs);
    }

    const QUrl url = QUrl::fromLocalFile(file);
    const bool b = QDesktopServices::openUrl(url);
    if (!b)
    {
        QMessageBox::warning(this, tr("Failed to open file"), tr("Could not open %1").arg(file));
    }
}

namespace
{
void showTextBrowser(const QString &string, QWidget *parent)
{
    QDialog dlg(parent);
    {
        QVBoxLayout *vl = new QVBoxLayout();
        QTextBrowser *browser = new QTextBrowser(&dlg);
        browser->setText(string);
        vl->addWidget(browser);

        dlg.setLayout(vl);
    }

    dlg.exec();
}

}
