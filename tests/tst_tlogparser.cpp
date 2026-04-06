#include <QtTest/QtTest>
#include <QTemporaryFile>
#include <cstring>
#include <cmath>

#include "TlogParser.h"
#include "helpers/TlogBuilder.h"

static QVector<MavlinkMessage> parseBytes(const QByteArray& data, QString* err = nullptr)
{
    QTemporaryFile f;
    if (!f.open()) return {};
    f.write(data);
    f.flush();
    return TlogParser::parse(f.fileName(), err);
}

class TstTlogParser : public QObject {
    Q_OBJECT
private slots:
    void parse_emptyFile_returnsEmptyAndSetsError();
    void parse_truncatedPacket_stopsGracefully();
    void parse_junkPrefix_resyncsAndFindsPacket();
    void parse_shortPayload_doesNotCrash();
    void parse_singleHeartbeat_returnsOneMessage();
    void parse_globalPositionInt_correctLatLonAlt();
    void parse_globalPositionInt_zeroLatLon_hasPositionFalse();
    void parse_attitude_piRadians_convertedTo180Degrees();
    void parse_statusText_severityLabels();
    void parse_unknownMsgId_generatedName();
};

void TstTlogParser::parse_emptyFile_returnsEmptyAndSetsError()
{
    QString err;
    auto msgs = parseBytes(QByteArray{}, &err);
    QVERIFY(msgs.isEmpty());
    QVERIFY(!err.isEmpty());
}

void TstTlogParser::parse_truncatedPacket_stopsGracefully()
{
    TlogBuilder b;
    b.addHeartbeat(1000);
    QByteArray data = b.data();
    data.chop(1);
    QVERIFY(parseBytes(data).isEmpty());
}

void TstTlogParser::parse_junkPrefix_resyncsAndFindsPacket()
{
    TlogBuilder b;
    b.addJunk(QByteArray(5, 0x01));
    b.addHeartbeat(1 /*ts_us: all bytes 0x00/0x01, no accidental 0xFD*/);
    auto msgs = parseBytes(b.data());
    QCOMPARE(msgs.size(), 1);
    QCOMPARE(msgs[0].msgid, quint32(0));
}

void TstTlogParser::parse_shortPayload_doesNotCrash()
{
    TlogBuilder b;
    b.addRecord(1000, 33, QByteArray(10, 0xFF));
    auto msgs = parseBytes(b.data());
    QCOMPARE(msgs.size(), 1);
    QVERIFY(!msgs[0].hasPosition);
    QVERIFY(msgs[0].fieldsText.isEmpty());
}

void TstTlogParser::parse_singleHeartbeat_returnsOneMessage()
{
    TlogBuilder b;
    b.addHeartbeat(1000);
    auto msgs = parseBytes(b.data());
    QCOMPARE(msgs.size(), 1);
    QCOMPARE(msgs[0].msgName, QString("HEARTBEAT"));
}

void TstTlogParser::parse_globalPositionInt_correctLatLonAlt()
{
    TlogBuilder b;
    b.addGlobalPositionInt(1000, 473900000, 85400000, 500000);
    auto msgs = parseBytes(b.data());
    QVERIFY(msgs[0].hasPosition);
    QVERIFY(qAbs(msgs[0].lat - 47.39) < 1e-6);
    QVERIFY(qAbs(msgs[0].lon -  8.54) < 1e-6);
    QVERIFY(qAbs(msgs[0].alt - 500.0) < 0.001);
}

void TstTlogParser::parse_globalPositionInt_zeroLatLon_hasPositionFalse()
{
    TlogBuilder b;
    b.addGlobalPositionInt(1000, 0, 0, 500000);
    QVERIFY(!parseBytes(b.data())[0].hasPosition);
}

void TstTlogParser::parse_attitude_piRadians_convertedTo180Degrees()
{
    TlogBuilder b;
    b.addAttitude(1000, float(M_PI), 0.0f, 0.0f);
    auto msgs = parseBytes(b.data());
    QVERIFY(msgs[0].fieldsText.contains("180.00"));
}

void TstTlogParser::parse_statusText_severityLabels()
{
    auto check = [](quint8 sev, const QString& expected) {
        TlogBuilder b;
        b.addStatusText(1000, sev, "msg");
        auto msgs = parseBytes(b.data());
        QVERIFY(msgs[0].fieldsText.startsWith("[" + expected + "]"));
    };
    check(0, "EMERGENCY");
    check(3, "ERROR");
    check(6, "INFO");
    check(7, "DEBUG");
}

void TstTlogParser::parse_unknownMsgId_generatedName()
{
    TlogBuilder b;
    b.addRecord(1000, 9999, QByteArray(4, 0));
    auto msgs = parseBytes(b.data());
    QCOMPARE(msgs[0].msgName, QString("MSG_9999"));
    QVERIFY(msgs[0].fieldsText.startsWith("payload:"));
}

QTEST_MAIN(TstTlogParser)
#include "tst_tlogparser.moc"
