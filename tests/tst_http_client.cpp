#include <QTest>
#include <QThread>

#include <rbl_error.h>

#include "rcl_http_client.h"

class TestHttpClient : public QObject
{
    Q_OBJECT

private slots:

    void senderIsReleasedWhenApplicationQuits();
    void requestAfterQuitFailsImmediately();
};

//! Send a request from a worker thread to a client living in this thread and
//! return true if the worker has returned within given time.
static bool sendFromWorker(RHttpClient &client, RHttpMessage &reply, bool quitWhileWaiting, unsigned long waitMs)
{
    RHttpMessage request;

    QThread *worker = QThread::create([&client,&request,&reply]()
    {
        client.sendRequest(request,reply);
    });
    worker->start();

    if (quitWhileWaiting)
    {
        // Let the worker start waiting for the reply. Events of this thread are
        // not processed, the same as after the application event loop has ended.
        QTest::qSleep(300);
        QMetaObject::invokeMethod(&client,"onAboutToQuit",Qt::DirectConnection);
    }

    bool finished = worker->wait(waitMs);
    if (!finished)
    {
        worker->terminate();
        worker->wait();
    }
    delete worker;
    return finished;
}

void TestHttpClient::senderIsReleasedWhenApplicationQuits()
{
    RHttpClientSettings settings;
    settings.setUrl("http://localhost:1");
    RHttpClient client(RHttpClient::Public,settings);

    RHttpMessage reply;
    // A sender left waiting keeps a thread of the global thread pool busy,
    // which keeps the process running after its windows are closed.
    QVERIFY(sendFromWorker(client,reply,true,5000));
    QCOMPARE(reply.getErrorType(),RError::Connection);
}

void TestHttpClient::requestAfterQuitFailsImmediately()
{
    RHttpClientSettings settings;
    settings.setUrl("http://localhost:1");
    RHttpClient client(RHttpClient::Public,settings);

    QMetaObject::invokeMethod(&client,"onAboutToQuit",Qt::DirectConnection);

    RHttpMessage reply;
    QVERIFY(sendFromWorker(client,reply,false,5000));
    QCOMPARE(reply.getErrorType(),RError::Connection);
}

QTEST_GUILESS_MAIN(TestHttpClient)
#include "tst_http_client.moc"
