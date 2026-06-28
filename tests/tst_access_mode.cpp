#include <QtTest>

#include "rcl_access_mode.h"

class TestAccessMode : public QObject
{
    Q_OBJECT

private slots:

    void defaultIsValidAndNone();
    void humanConversion();
    void toHumanString();
    void modeMaskValidity();
    void filterModeMask();
    void jsonRoundTrip();
    void stringRoundTrip();
};

void TestAccessMode::defaultIsValidAndNone()
{
    RAccessMode mode;
    QVERIFY(mode.isValid());
    QCOMPARE(mode.getUserModeMask(), int(RAccessMode::None));
    QCOMPARE(mode.getGroupModeMask(), int(RAccessMode::None));
    QCOMPARE(mode.getOtherModeMask(), int(RAccessMode::None));
}

void TestAccessMode::humanConversion()
{
    QCOMPARE(RAccessMode::modeToHuman(RAccessMode::Read), 'r');
    QCOMPARE(RAccessMode::modeToHuman(RAccessMode::Write), 'w');
    QCOMPARE(RAccessMode::modeToHuman(RAccessMode::Execute), 'x');
    QCOMPARE(RAccessMode::modeToHuman(RAccessMode::None), '-');

    QCOMPARE(RAccessMode::humanToMode('r'), RAccessMode::Read);
    QCOMPARE(RAccessMode::humanToMode('w'), RAccessMode::Write);
    QCOMPARE(RAccessMode::humanToMode('x'), RAccessMode::Execute);
    QCOMPARE(RAccessMode::humanToMode('?'), RAccessMode::None);
}

void TestAccessMode::toHumanString()
{
    RAccessMode mode;
    mode.setUserModeMask(RAccessMode::RWX);
    mode.setGroupModeMask(RAccessMode::R);
    mode.setOtherModeMask(RAccessMode::None);
    QCOMPARE(mode.toHuman(), QString("rwxr-----"));

    QCOMPARE(RAccessMode::modeMaskToHuman(RAccessMode::RW), QString("rw-"));
}

void TestAccessMode::modeMaskValidity()
{
    QVERIFY(RAccessMode::modeMaskIsValid(RAccessMode::None));
    QVERIFY(RAccessMode::modeMaskIsValid(RAccessMode::RWX));
    QVERIFY(RAccessMode::modeMaskIsValid(RAccessMode::RW));
    QVERIFY(!RAccessMode::modeMaskIsValid(8));
    QVERIFY(!RAccessMode::modeMaskIsValid(-1));
}

void TestAccessMode::filterModeMask()
{
    QCOMPARE(RAccessMode::filterModeMask(RAccessMode::RWX, RAccessMode::RW), int(RAccessMode::RW));
    QCOMPARE(RAccessMode::filterModeMask(RAccessMode::RWX, RAccessMode::R), int(RAccessMode::R));
    QCOMPARE(RAccessMode::filterModeMask(RAccessMode::R, RAccessMode::W), int(RAccessMode::None));
}

void TestAccessMode::jsonRoundTrip()
{
    RAccessMode mode;
    mode.setUserModeMask(RAccessMode::RWX);
    mode.setGroupModeMask(RAccessMode::RX);
    mode.setOtherModeMask(RAccessMode::R);

    RAccessMode restored = RAccessMode::fromJson(mode.toJson());
    QCOMPARE(restored.getUserModeMask(), mode.getUserModeMask());
    QCOMPARE(restored.getGroupModeMask(), mode.getGroupModeMask());
    QCOMPARE(restored.getOtherModeMask(), mode.getOtherModeMask());
}

void TestAccessMode::stringRoundTrip()
{
    RAccessMode mode;
    mode.setUserModeMask(RAccessMode::RW);
    mode.setGroupModeMask(RAccessMode::R);
    mode.setOtherModeMask(RAccessMode::X);

    RAccessMode restored = RAccessMode::fromString(mode.toString());
    QCOMPARE(restored.getUserModeMask(), mode.getUserModeMask());
    QCOMPARE(restored.getGroupModeMask(), mode.getGroupModeMask());
    QCOMPARE(restored.getOtherModeMask(), mode.getOtherModeMask());
}

QTEST_APPLESS_MAIN(TestAccessMode)

#include "tst_access_mode.moc"
