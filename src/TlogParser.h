#pragma once

#include "MavlinkMessage.h"
#include <QVector>
#include <QString>
#include <QHash>
#include <functional>

class TlogParser {
public:
    static QVector<MavlinkMessage> parse(const QString& filePath,
                                         QString*        error = nullptr);

private:
    struct MessageDef {
        QString name;
        std::function<void(MavlinkMessage&)> parser;
    };

    static const QHash<quint32, MessageDef>& registry();
    static void parseFields(MavlinkMessage& msg);

    static void parseHeartbeat(MavlinkMessage& msg);
    static void parseParamValue(MavlinkMessage& msg);
    static void parseGpsRawInt(MavlinkMessage& msg);
    static void parseAttitude(MavlinkMessage& msg);
    static void parseAttitudeQuaternion(MavlinkMessage& msg);
    static void parseLocalPositionNed(MavlinkMessage& msg);
    static void parseGlobalPositionInt(MavlinkMessage& msg);
    static void parseServoOutputRaw(MavlinkMessage& msg);
    static void parseVfrHud(MavlinkMessage& msg);
    static void parseAttitudeTarget(MavlinkMessage& msg);
    static void parsePositionTargetLocalNed(MavlinkMessage& msg);
    static void parseAltitude(MavlinkMessage& msg);
    static void parseBatteryStatus(MavlinkMessage& msg);
    static void parseVibration(MavlinkMessage& msg);
    static void parseStatusText(MavlinkMessage& msg);

    static quint32 readU32(const QByteArray& d, int off);
    static qint32  readI32(const QByteArray& d, int off);
    static quint16 readU16(const QByteArray& d, int off);
    static qint16  readI16(const QByteArray& d, int off);
    static float   readF32(const QByteArray& d, int off);
    static quint64 readU64(const QByteArray& d, int off);
};
