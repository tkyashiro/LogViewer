#include <QTest>

class LogSourceTest : public QObject {
    Q_OBJECT
public:
private slots:
    void test() { QVERIFY(true); }
};

QTEST_APPLESS_MAIN(LogSourceTest)
#include "LogSourceTest.moc"
