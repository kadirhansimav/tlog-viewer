#include "TlogParser.h"

#include <QFile>
#include <cstring>
#include <cmath>

quint32 TlogParser::readU32(const QByteArray& d, int off)
{
    if (off + 4 > d.size()) return 0;
    quint32 v = 0;
    std::memcpy(&v, d.constData() + off, 4);
    return v;
}

qint32 TlogParser::readI32(const QByteArray& d, int off)
{
    return static_cast<qint32>(readU32(d, off));
}

quint16 TlogParser::readU16(const QByteArray& d, int off)
{
    if (off + 2 > d.size()) return 0;
    quint16 v = 0;
    std::memcpy(&v, d.constData() + off, 2);
    return v;
}

qint16 TlogParser::readI16(const QByteArray& d, int off)
{
    return static_cast<qint16>(readU16(d, off));
}

float TlogParser::readF32(const QByteArray& d, int off)
{
    float v = 0.0f;
    if (off + 4 > d.size()) return v;
    std::memcpy(&v, d.constData() + off, 4);
    return v;
}

quint64 TlogParser::readU64(const QByteArray& d, int off)
{
    if (off + 8 > d.size()) return 0;
    quint64 v = 0;
    std::memcpy(&v, d.constData() + off, 8);
    return v;
}

const QHash<quint32, TlogParser::MessageDef>& TlogParser::registry()
{
    static const QHash<quint32, MessageDef> reg = {
        {   0, { "HEARTBEAT",                   parseHeartbeat             }},
        {  22, { "PARAM_VALUE",                 parseParamValue            }},
        {  24, { "GPS_RAW_INT",                 parseGpsRawInt             }},
        {  25, { "GPS_STATUS",                  nullptr                    }},
        {  27, { "RAW_IMU",                     nullptr                    }},
        {  29, { "SCALED_PRESSURE",             nullptr                    }},
        {  30, { "ATTITUDE",                    parseAttitude              }},
        {  31, { "ATTITUDE_QUATERNION",         parseAttitudeQuaternion    }},
        {  32, { "LOCAL_POSITION_NED",          parseLocalPositionNed      }},
        {  33, { "GLOBAL_POSITION_INT",         parseGlobalPositionInt     }},
        {  36, { "SERVO_OUTPUT_RAW",            parseServoOutputRaw        }},
        {  65, { "RC_CHANNELS",                 nullptr                    }},
        {  74, { "VFR_HUD",                     parseVfrHud                }},
        {  83, { "ATTITUDE_TARGET",             parseAttitudeTarget        }},
        {  85, { "POSITION_TARGET_LOCAL_NED",   parsePositionTargetLocalNed}},
        {  87, { "POSITION_TARGET_GLOBAL_INT",  nullptr                    }},
        { 105, { "HIGHRES_IMU",                 nullptr                    }},
        { 110, { "FILE_TRANSFER_PROTOCOL",      nullptr                    }},
        { 141, { "ALTITUDE",                    parseAltitude              }},
        { 147, { "BATTERY_STATUS",              parseBatteryStatus         }},
        { 193, { "EKF_STATUS_REPORT",           nullptr                    }},
        { 241, { "VIBRATION",                   parseVibration             }},
        { 242, { "HOME_POSITION",               nullptr                    }},
        { 253, { "STATUSTEXT",                  parseStatusText            }},
        { 290, { "UAVCAN_NODE_STATUS",          nullptr                    }},
        { 291, { "UAVCAN_NODE_INFO",            nullptr                    }},
    };
    return reg;
}

void TlogParser::parseHeartbeat(MavlinkMessage& msg)
{
    const auto& p = msg.payload;
    if (p.size() < 9) return;
    const quint8 type   = static_cast<quint8>(p[4]);
    const quint8 ap     = static_cast<quint8>(p[5]);
    const quint8 bm     = static_cast<quint8>(p[6]);
    const quint8 ss     = static_cast<quint8>(p[7]);
    const quint8 mv     = static_cast<quint8>(p[8]);
    msg.fieldsText = QString("custom_mode: %1, type: %2, autopilot: %3, "
                             "base_mode: %4, system_status: %5, mavlink_version: %6")
        .arg(readU32(p, 0)).arg(type).arg(ap).arg(bm).arg(ss).arg(mv);
}

void TlogParser::parseParamValue(MavlinkMessage& msg)
{
    const auto& p = msg.payload;
    if (p.size() < 25) return;
    const QString id = QString::fromLatin1(p.constData() + 8, 16).trimmed().remove(QChar('\0'));
    msg.fieldsText = QString("param_id: %1, param_value: %2, param_count: %3, param_index: %4")
        .arg(id)
        .arg(readF32(p, 0), 0, 'g', 6)
        .arg(readU16(p, 4))
        .arg(readU16(p, 6));
}

void TlogParser::parseGpsRawInt(MavlinkMessage& msg)
{
    const auto& p = msg.payload;
    if (p.size() < 30) return;
    msg.fieldsText = QString("lat: %1, lon: %2, alt: %3 m, fix_type: %4, satellites: %5")
        .arg(readI32(p, 8)  / 1e7, 0, 'f', 7)
        .arg(readI32(p, 12) / 1e7, 0, 'f', 7)
        .arg(readI32(p, 16) / 1000.0, 0, 'f', 2)
        .arg(static_cast<quint8>(p[28]))
        .arg(static_cast<quint8>(p[29]));
}

void TlogParser::parseAttitude(MavlinkMessage& msg)
{
    const auto& p = msg.payload;
    if (p.size() < 28) return;
    const double r2d = 180.0 / M_PI;
    msg.fieldsText = QString("roll: %1°, pitch: %2°, yaw: %3°, "
                             "rollspeed: %4 rad/s, pitchspeed: %5 rad/s, yawspeed: %6 rad/s")
        .arg(readF32(p, 4)  * r2d, 0, 'f', 2)
        .arg(readF32(p, 8)  * r2d, 0, 'f', 2)
        .arg(readF32(p, 12) * r2d, 0, 'f', 2)
        .arg(readF32(p, 16), 0, 'f', 4)
        .arg(readF32(p, 20), 0, 'f', 4)
        .arg(readF32(p, 24), 0, 'f', 4);
}

void TlogParser::parseAttitudeQuaternion(MavlinkMessage& msg)
{
    const auto& p = msg.payload;
    if (p.size() < 32) return;
    msg.fieldsText = QString("q1: %1, q2: %2, q3: %3, q4: %4, "
                             "rollspeed: %5, pitchspeed: %6, yawspeed: %7")
        .arg(readF32(p,  4), 0, 'f', 4)
        .arg(readF32(p,  8), 0, 'f', 4)
        .arg(readF32(p, 12), 0, 'f', 4)
        .arg(readF32(p, 16), 0, 'f', 4)
        .arg(readF32(p, 20), 0, 'f', 4)
        .arg(readF32(p, 24), 0, 'f', 4)
        .arg(readF32(p, 28), 0, 'f', 4);
}

void TlogParser::parseLocalPositionNed(MavlinkMessage& msg)
{
    const auto& p = msg.payload;
    if (p.size() < 28) return;
    msg.fieldsText = QString("x: %1 m, y: %2 m, z: %3 m, vx: %4 m/s, vy: %5 m/s, vz: %6 m/s")
        .arg(readF32(p,  4), 0, 'f', 3)
        .arg(readF32(p,  8), 0, 'f', 3)
        .arg(readF32(p, 12), 0, 'f', 3)
        .arg(readF32(p, 16), 0, 'f', 3)
        .arg(readF32(p, 20), 0, 'f', 3)
        .arg(readF32(p, 24), 0, 'f', 3);
}

void TlogParser::parseGlobalPositionInt(MavlinkMessage& msg)
{
    const auto& p = msg.payload;
    if (p.size() < 28) return;

    const qint32 lat_raw = readI32(p, 4);
    const qint32 lon_raw = readI32(p, 8);
    const qint32 alt_raw = readI32(p, 12);
    const qint32 rel_raw = readI32(p, 16);

    msg.hasPosition = (lat_raw != 0 || lon_raw != 0);
    msg.lat = lat_raw / 1e7;
    msg.lon = lon_raw / 1e7;
    msg.alt = alt_raw / 1000.0;

    msg.fieldsText = QString("lat: %1, lon: %2, alt: %3 m, rel_alt: %4 m, "
                             "vx: %5 cm/s, vy: %6 cm/s, vz: %7 cm/s, hdg: %8°")
        .arg(msg.lat, 0, 'f', 7)
        .arg(msg.lon, 0, 'f', 7)
        .arg(alt_raw / 1000.0, 0, 'f', 2)
        .arg(rel_raw / 1000.0, 0, 'f', 2)
        .arg(readI16(p, 20))
        .arg(readI16(p, 22))
        .arg(readI16(p, 24))
        .arg(readU16(p, 26) / 100.0, 0, 'f', 1);
}

void TlogParser::parseServoOutputRaw(MavlinkMessage& msg)
{
    const auto& p = msg.payload;
    if (p.size() < 21) return;
    msg.fieldsText = QString("s1:%1 s2:%2 s3:%3 s4:%4 s5:%5 s6:%6 s7:%7 s8:%8")
        .arg(readU16(p,  4)).arg(readU16(p,  6))
        .arg(readU16(p,  8)).arg(readU16(p, 10))
        .arg(readU16(p, 12)).arg(readU16(p, 14))
        .arg(readU16(p, 16)).arg(readU16(p, 18));
}

void TlogParser::parseVfrHud(MavlinkMessage& msg)
{
    const auto& p = msg.payload;
    if (p.size() < 20) return;
    msg.fieldsText = QString("airspeed: %1 m/s, groundspeed: %2 m/s, alt: %3 m, "
                             "climb: %4 m/s, heading: %5°, throttle: %6%")
        .arg(readF32(p,  0), 0, 'f', 2)
        .arg(readF32(p,  4), 0, 'f', 2)
        .arg(readF32(p,  8), 0, 'f', 2)
        .arg(readF32(p, 12), 0, 'f', 2)
        .arg(readI16(p, 16))
        .arg(readU16(p, 18));
}

void TlogParser::parseAttitudeTarget(MavlinkMessage& msg)
{
    const auto& p = msg.payload;
    if (p.size() < 37) return;
    const double r2d = 180.0 / M_PI;
    msg.fieldsText = QString("q: [%1,%2,%3,%4], roll_rate: %5°/s, pitch_rate: %6°/s, "
                             "yaw_rate: %7°/s, thrust: %8")
        .arg(readF32(p,  4), 0, 'f', 3).arg(readF32(p,  8), 0, 'f', 3)
        .arg(readF32(p, 12), 0, 'f', 3).arg(readF32(p, 16), 0, 'f', 3)
        .arg(readF32(p, 20) * r2d, 0, 'f', 2)
        .arg(readF32(p, 24) * r2d, 0, 'f', 2)
        .arg(readF32(p, 28) * r2d, 0, 'f', 2)
        .arg(readF32(p, 32), 0, 'f', 3);
}

void TlogParser::parsePositionTargetLocalNed(MavlinkMessage& msg)
{
    const auto& p = msg.payload;
    if (p.size() < 51) return;
    msg.fieldsText = QString("x: %1 m, y: %2 m, z: %3 m, vx: %4, vy: %5, vz: %6, "
                             "yaw: %7°, frame: %8")
        .arg(readF32(p,  4), 0, 'f', 3).arg(readF32(p,  8), 0, 'f', 3)
        .arg(readF32(p, 12), 0, 'f', 3).arg(readF32(p, 16), 0, 'f', 3)
        .arg(readF32(p, 20), 0, 'f', 3).arg(readF32(p, 24), 0, 'f', 3)
        .arg(readF32(p, 40) * 180.0 / M_PI, 0, 'f', 1)
        .arg(static_cast<quint8>(p[50]));
}

void TlogParser::parseAltitude(MavlinkMessage& msg)
{
    const auto& p = msg.payload;
    if (p.size() < 32) return;
    msg.fieldsText = QString("monotonic: %1 m, amsl: %2 m, local: %3 m, "
                             "relative: %4 m, terrain: %5 m")
        .arg(readF32(p,  8), 0, 'f', 2)
        .arg(readF32(p, 12), 0, 'f', 2)
        .arg(readF32(p, 16), 0, 'f', 2)
        .arg(readF32(p, 20), 0, 'f', 2)
        .arg(readF32(p, 24), 0, 'f', 2);
}

void TlogParser::parseBatteryStatus(MavlinkMessage& msg)
{
    const auto& p = msg.payload;
    if (p.size() < 36) return;
    const qint16 temp     = readI16(p, 8);
    const quint16 v0      = readU16(p, 10);
    const qint16  current = readI16(p, 30);
    const qint8   remain  = static_cast<qint8>(p[35]);
    msg.fieldsText = QString("voltage: %1 V, current: %2 A, temp: %3°C, remaining: %4%")
        .arg(v0 / 1000.0, 0, 'f', 3)
        .arg(current / 100.0, 0, 'f', 2)
        .arg(temp == std::numeric_limits<qint16>::max() ? 0 : temp / 100)
        .arg(remain);
}

void TlogParser::parseVibration(MavlinkMessage& msg)
{
    const auto& p = msg.payload;
    if (p.size() < 32) return;
    msg.fieldsText = QString("vib_x: %1, vib_y: %2, vib_z: %3, clipping: %4/%5/%6")
        .arg(readF32(p,  8), 0, 'f', 4)
        .arg(readF32(p, 12), 0, 'f', 4)
        .arg(readF32(p, 16), 0, 'f', 4)
        .arg(readU32(p, 20)).arg(readU32(p, 24)).arg(readU32(p, 28));
}

void TlogParser::parseStatusText(MavlinkMessage& msg)
{
    const auto& p = msg.payload;
    if (p.size() < 2) return;
    const QString text = QString::fromLatin1(p.constData() + 1,
                                             qMin(p.size() - 1, 50))
                             .trimmed().remove(QChar('\0'));
    const quint8 sev = static_cast<quint8>(p[0]);
    const QString sevStr = sev == 0 ? "EMERGENCY" : sev == 1 ? "ALERT"
                         : sev == 2 ? "CRITICAL"  : sev == 3 ? "ERROR"
                         : sev == 4 ? "WARNING"   : sev == 5 ? "NOTICE"
                         : sev == 6 ? "INFO"       : "DEBUG";
    msg.fieldsText = QString("[%1] %2").arg(sevStr, text);
}

void TlogParser::parseFields(MavlinkMessage& msg)
{
    const auto it = registry().find(msg.msgid);
    if (it != registry().end() && it->parser) {
        it->parser(msg);
        return;
    }

    const int show = qMin(msg.payload.size(), 16);
    QString hex;
    for (int i = 0; i < show; ++i) {
        if (i) hex += ' ';
        hex += QString("%1").arg(static_cast<quint8>(msg.payload[i]),
                                 2, 16, QLatin1Char('0'));
    }
    msg.fieldsText = "payload: " + hex + (msg.payload.size() > 16 ? " ..." : "");
}

QVector<MavlinkMessage> TlogParser::parse(const QString& filePath, QString* error)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        if (error) *error = "Dosya açılamadı: " + file.errorString();
        return {};
    }

    const QByteArray raw = file.readAll();
    file.close();

    QVector<MavlinkMessage> messages;
    messages.reserve(100'000);

    int pos = 0;
    const int size = raw.size();
    const auto* bytes = reinterpret_cast<const quint8*>(raw.constData());

    while (pos < size) {
        if (pos + 20 > size) break;

        quint64 ts = 0;
        for (int i = 0; i < 8; ++i)
            ts = (ts << 8) | bytes[pos + i];

        if (bytes[pos + 8] != 0xFD) {
            ++pos;
            continue;
        }

        const int hdrPos     = pos + 8;
        const quint8 plen    = bytes[hdrPos + 1];
        const quint8 incompat= bytes[hdrPos + 2];
        const quint8 compat  = bytes[hdrPos + 3];
        const quint8 seq     = bytes[hdrPos + 4];
        const quint8 sysid   = bytes[hdrPos + 5];
        const quint8 compid  = bytes[hdrPos + 6];
        const quint32 msgid  = static_cast<quint32>(bytes[hdrPos + 7])
                             | (static_cast<quint32>(bytes[hdrPos + 8]) << 8)
                             | (static_cast<quint32>(bytes[hdrPos + 9]) << 16);

        int packet_total = 8 + 10 + plen + 2;
        if (incompat & 0x01) packet_total += 13;

        if (pos + packet_total > size) break;

        MavlinkMessage msg;
        msg.timestamp_us   = ts;
        msg.incompat_flags = incompat;
        msg.compat_flags   = compat;
        msg.seq            = seq;
        msg.sysid          = sysid;
        msg.compid         = compid;
        msg.msgid          = msgid;
        msg.payload        = raw.mid(hdrPos + 10, plen);
        const auto it = registry().find(msgid);
        msg.msgName = (it != registry().end()) ? it->name : QString("MSG_%1").arg(msgid);

        parseFields(msg);

        messages.append(std::move(msg));
        pos += packet_total;
    }

    if (messages.isEmpty() && error)
        *error = "Geçerli MAVLink v2 mesajı bulunamadı.";

    return messages;
}
