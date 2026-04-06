#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QGeoCoordinate>

#include "AppController.h"
#include "helpers/TlogBuilder.h"

static QString writeTempTlog(QTemporaryFile& f, const QByteArray& data)
{
    f.open();
    f.write(data);
    f.flush();
    return f.fileName();
}

static QByteArray minimalTlog()
{
    TlogBuilder b;
    b.addHeartbeat(0);
    b.addGlobalPositionInt(1'000'000ULL, 473900000, 85400000, 500000);
    return b.data();
}

class TstAppController : public QObject {
    Q_OBJECT
private slots:
    void initial_state_isClean();
    void loadFile_nonexistentPath_returnsFalseAndEmitsError();
    void loadFile_validFile_setsStateAndEmitsSignals();
    void loadFile_acceptsFileUrlScheme();
    void loadFile_secondLoad_resetsCurrentIndexToZero();
    void setCurrentIndex_negative_clampsToZero();
    void setCurrentIndex_tooLarge_clampsToLastMessage();
    void setCurrentIndex_sameValue_doesNotEmitSignal();
    void posCache_beforeGpsFix_seedsFromFirstFix();
    void endTimeStr_formattedAsMMSS();
};

void TstAppController::initial_state_isClean()
{
    AppController c;
    QVERIFY(!c.fileLoaded());
    QCOMPARE(c.messageCount(), 0);
    QCOMPARE(c.currentLat(), 0.0);
    QCOMPARE(c.currentLon(), 0.0);
    QCOMPARE(c.startTimeStr(), QString("00:00"));
    QCOMPARE(c.endTimeStr(),   QString("00:00"));
    QVERIFY(c.flightPath().isEmpty());
    QVERIFY(c.messageModel() != nullptr);
}

void TstAppController::loadFile_nonexistentPath_returnsFalseAndEmitsError()
{
    AppController c;
    QSignalSpy spy(&c, &AppController::loadError);
    QVERIFY(!c.loadFile("/does/not/exist.tlog"));
    QVERIFY(!c.fileLoaded());
    QCOMPARE(spy.count(), 1);
}

void TstAppController::loadFile_validFile_setsStateAndEmitsSignals()
{
    AppController c;
    QSignalSpy loadedSpy(&c, &AppController::fileLoadedChanged);
    QSignalSpy msgSpy   (&c, &AppController::messagesChanged);

    QTemporaryFile f;
    QVERIFY(c.loadFile(writeTempTlog(f, minimalTlog())));

    QVERIFY(c.fileLoaded());
    QVERIFY(c.messageCount() > 0);
    QCOMPARE(loadedSpy.count(), 1);
    QCOMPARE(msgSpy.count(),    1);
}

void TstAppController::loadFile_acceptsFileUrlScheme()
{
    AppController c;
    QTemporaryFile f;
    writeTempTlog(f, minimalTlog());
    const QString url = QUrl::fromLocalFile(f.fileName()).toString();
    QVERIFY(c.loadFile(url));
    QVERIFY(c.fileLoaded());
}

void TstAppController::loadFile_secondLoad_resetsCurrentIndexToZero()
{
    AppController c;
    QTemporaryFile f1;
    c.loadFile(writeTempTlog(f1, minimalTlog()));

    if (c.messageCount() > 1)
        c.setCurrentIndex(c.messageCount() - 1);

    QTemporaryFile f2;
    c.loadFile(writeTempTlog(f2, minimalTlog()));
    QCOMPARE(c.currentIndex(), 0);
}

void TstAppController::setCurrentIndex_negative_clampsToZero()
{
    AppController c;
    QTemporaryFile f;
    c.loadFile(writeTempTlog(f, minimalTlog()));
    c.setCurrentIndex(-5);
    QCOMPARE(c.currentIndex(), 0);
}

void TstAppController::setCurrentIndex_tooLarge_clampsToLastMessage()
{
    AppController c;
    QTemporaryFile f;
    c.loadFile(writeTempTlog(f, minimalTlog()));
    c.setCurrentIndex(999999);
    QCOMPARE(c.currentIndex(), c.messageCount() - 1);
}

void TstAppController::setCurrentIndex_sameValue_doesNotEmitSignal()
{
    AppController c;
    QTemporaryFile f;
    c.loadFile(writeTempTlog(f, minimalTlog()));

    QSignalSpy spy(&c, &AppController::currentIndexChanged);
    c.setCurrentIndex(0);
    QCOMPARE(spy.count(), 0);
}

void TstAppController::posCache_beforeGpsFix_seedsFromFirstFix()
{
    AppController c;
    QTemporaryFile f;
    c.loadFile(writeTempTlog(f, minimalTlog()));
    c.setCurrentIndex(0);
    QVERIFY(qAbs(c.currentLat() - 47.39) < 1e-5);
    QVERIFY(qAbs(c.currentLon() -  8.54) < 1e-5);
}

void TstAppController::endTimeStr_formattedAsMMSS()
{
    const quint64 start = 1'000'000'000'000'000ULL;
    TlogBuilder b;
    b.addHeartbeat(start);
    b.addHeartbeat(start + 65'000'000ULL);

    AppController c;
    QTemporaryFile f;
    c.loadFile(writeTempTlog(f, b.data()));

    QCOMPARE(c.endTimeStr(), QString("01:05"));
}

QTEST_MAIN(TstAppController)
#include "tst_appcontroller.moc"
