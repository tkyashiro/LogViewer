#include <memory>

#include <QTest>

#include <LogModel.h>
#include <LogModelProxy.h>

class LogModelProxyTest : public QObject
{
    Q_OBJECT
public:

private slots:
    void init();

    void testFilterSeverity_data();
    void testFilterSeverity();

    void testFilterTime_data();
    void testFilterTime();

private:
    std::unique_ptr<LogModel> model_;
    std::unique_ptr<LogModelProxy> proxy_;
};

namespace
{
const char *sevError = "ERROR";
const char *sevWarn  = "WARN";
const char *sevTrace = "TRACE";
}

void LogModelProxyTest::init()
{
    model_.reset(new LogModel());
    proxy_.reset(new LogModelProxy());

    proxy_->setSourceModel(model_.get());

    std::vector<LogEntry> entries = {
        LogEntry( sevTrace, "Hoge::Hoge()", "c:\\Hoge.cpp", 120, "Test", 1, QDateTime(QDate(1983,  4, 15) ,QTime(15, 59)) ),
        LogEntry( sevWarn , "Hoge::Hoge()", "c:\\Hoge.cpp", 130, "Test", 1, QDateTime(QDate(1983, 12, 30) ,QTime( 5,  0)) ),
        LogEntry( sevError, "Hoge::Test()", "c:\\Hoge.cpp", 140, "Test", 2, QDateTime(QDate(1983, 12, 30) ,QTime( 5,  1)) ),
        LogEntry( sevError, "Fuga::Test()", "c:\\Fuga.cpp", 120, "Test", 3, QDateTime(QDate(1983, 12, 30) ,QTime( 5,  2)) ),
        LogEntry( sevTrace, "Fuga::Exec()", "c:\\Fuga.cpp", 130, "Test", 3, QDateTime(QDate(2010,  8, 10) ,QTime( 6,  0)) ),
        LogEntry( sevTrace, "Fuga::Exec()", "c:\\Fuga.cpp", 120, "Test", 1, QDateTime(QDate(2017, 12, 30) ,QTime(12, 59)) ),
    };
    model_->append(std::move(entries));
}

void LogModelProxyTest::testFilterSeverity_data()
{
    QTest::addColumn<QString>("severity");
    QTest::addColumn<int>("expected");

    QTest::newRow("Error") << sevError << 2;
    QTest::newRow("Warn")  << sevWarn  << 1;
    QTest::newRow("Trace") << sevTrace << 3;
}

void LogModelProxyTest::testFilterSeverity()
{
    QFETCH(QString, severity);

    proxy_->setSeverityFilter(QRegExp(severity));

    QTEST( proxy_->rowCount(), "expected" );
}

void LogModelProxyTest::testFilterTime_data()
{
    QTest::addColumn<QDateTime>("from");
    QTest::addColumn<QDateTime>("to");
    QTest::addColumn<int>("expected");

    QTest::newRow("invalid=all") << QDateTime() << QDateTime() << model_->rowCount(model_->index(0,0));
    QTest::newRow("2010.8.10 0:00~") << QDateTime(QDate(2010,8,10)) << QDateTime() << 2;
    QTest::newRow("~2010.8.10 0:00") << QDateTime() << QDateTime(QDate(2010,8,10)) << 4;
    QTest::newRow("1983.12.30") << QDateTime(QDate(1983,12,30)) << QDateTime(QDate(1983,12,30),QTime(23,59,59)) << 3;
}

void LogModelProxyTest::testFilterTime()
{
    QFETCH(QDateTime, from);
    QFETCH(QDateTime, to);

    proxy_->setMinDateTime(from);
    proxy_->setMaxDateTime(to);

    qDebug() << proxy_->headerData(LogModel::eTime, Qt::Horizontal, Qt::DisplayRole);

    QTEST( proxy_->rowCount(), "expected" );
}

QTEST_APPLESS_MAIN(LogModelProxyTest)
#include "LogModelProxyTest.moc"
