#include <QtTest>

#include "rcl_file_quota.h"

class TestFileQuota : public QObject
{
    Q_OBJECT

private slots:

    void settersAndGetters();
    void quotaNotExceeded();
    void quotaExceeded();
    void unlimitedQuota();
    void jsonRoundTrip();
};

void TestFileQuota::settersAndGetters()
{
    RFileQuota quota(1000, 100, 10);
    QCOMPARE(quota.getStoreSize(), qint64(1000));
    QCOMPARE(quota.getFileSize(), qint64(100));
    QCOMPARE(quota.getFileCount(), qint64(10));

    quota.setStoreSize(2000);
    QCOMPARE(quota.getStoreSize(), qint64(2000));
}

void TestFileQuota::quotaNotExceeded()
{
    RFileQuota limit(1000, 100, 10);
    RFileQuota usage(500, 50, 5);
    QVERIFY(!limit.quotaExceeded(usage));
}

void TestFileQuota::quotaExceeded()
{
    RFileQuota limit(1000, 100, 10);

    QVERIFY(limit.quotaExceeded(RFileQuota(1500, 50, 5)));  // store too big
    QVERIFY(limit.quotaExceeded(RFileQuota(500, 200, 5)));  // file too big
    QVERIFY(limit.quotaExceeded(RFileQuota(500, 50, 20)));  // too many files
}

void TestFileQuota::unlimitedQuota()
{
    // Negative limits mean unlimited.
    RFileQuota limit(-1, -1, -1);
    QVERIFY(!limit.quotaExceeded(RFileQuota(1000000, 1000000, 1000000)));
}

void TestFileQuota::jsonRoundTrip()
{
    RFileQuota quota(1234, 567, 89);
    RFileQuota restored = RFileQuota::fromJson(quota.toJson());
    QCOMPARE(restored.getStoreSize(), qint64(1234));
    QCOMPARE(restored.getFileSize(), qint64(567));
    QCOMPARE(restored.getFileCount(), qint64(89));
}

QTEST_APPLESS_MAIN(TestFileQuota)

#include "tst_file_quota.moc"
