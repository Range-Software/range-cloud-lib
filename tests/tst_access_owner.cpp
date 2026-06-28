#include <QtTest>

#include "rcl_access_owner.h"

class TestAccessOwner : public QObject
{
    Q_OBJECT

private slots:

    void settersAndGetters();
    void validity();
    void jsonRoundTrip();
    void stringRoundTrip();
};

void TestAccessOwner::settersAndGetters()
{
    RAccessOwner owner;
    owner.setUser("alice");
    owner.setGroup("engineers");
    QCOMPARE(owner.getUser(), QString("alice"));
    QCOMPARE(owner.getGroup(), QString("engineers"));
}

void TestAccessOwner::validity()
{
    RAccessOwner owner;
    owner.setUser("alice");
    owner.setGroup("engineers");
    QVERIFY(owner.isValid());

    QVERIFY(RAccessOwner::isUserValid("user.name-1@host"));
    QVERIFY(!RAccessOwner::isUserValid(""));
    QVERIFY(!RAccessOwner::isUserValid("bad name with spaces"));
    QVERIFY(!RAccessOwner::isGroupValid("with/slash"));
}

void TestAccessOwner::jsonRoundTrip()
{
    RAccessOwner owner;
    owner.setUser("bob");
    owner.setGroup("admins");

    RAccessOwner restored = RAccessOwner::fromJson(owner.toJson());
    QCOMPARE(restored.getUser(), QString("bob"));
    QCOMPARE(restored.getGroup(), QString("admins"));
}

void TestAccessOwner::stringRoundTrip()
{
    RAccessOwner owner;
    owner.setUser("carol");
    owner.setGroup("users");

    RAccessOwner restored = RAccessOwner::fromString(owner.toString());
    QCOMPARE(restored.getUser(), QString("carol"));
    QCOMPARE(restored.getGroup(), QString("users"));
}

QTEST_APPLESS_MAIN(TestAccessOwner)

#include "tst_access_owner.moc"
