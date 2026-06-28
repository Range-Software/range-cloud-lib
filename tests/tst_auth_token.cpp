#include <QtTest>

#include "rcl_auth_token.h"

class TestAuthToken : public QObject
{
    Q_OBJECT

private slots:

    void nullByDefault();
    void settersMakeNonNull();
    void generatedContentIsUnique();
    void jsonRoundTrip();
};

void TestAuthToken::nullByDefault()
{
    RAuthToken token;
    // No content and no resource name yet.
    QVERIFY(token.isNull());
    // The id is generated and therefore not null.
    QVERIFY(!token.getId().isNull());
}

void TestAuthToken::settersMakeNonNull()
{
    RAuthToken token;
    token.setContent("secret");
    token.setResourceName("project-x");
    token.setValidityDate(123456789);

    QVERIFY(!token.isNull());
    QCOMPARE(token.getContent(), QString("secret"));
    QCOMPARE(token.getResourceName(), QString("project-x"));
    QCOMPARE(token.getValidityDate(), qint64(123456789));
}

void TestAuthToken::generatedContentIsUnique()
{
    QString a = RAuthToken::generateTokenContent();
    QString b = RAuthToken::generateTokenContent();
    QVERIFY(!a.isEmpty());
    QVERIFY(!b.isEmpty());
    QVERIFY(a != b);
}

void TestAuthToken::jsonRoundTrip()
{
    RAuthToken token;
    token.setContent("token-content");
    token.setResourceName("resource");
    token.setValidityDate(987654321);

    RAuthToken restored = RAuthToken::fromJson(token.toJson());
    QCOMPARE(restored.getContent(), QString("token-content"));
    QCOMPARE(restored.getResourceName(), QString("resource"));
    QCOMPARE(restored.getValidityDate(), qint64(987654321));
}

QTEST_APPLESS_MAIN(TestAuthToken)

#include "tst_auth_token.moc"
