#pragma once

#include <QByteArray>
#include <QString>
#include <cstring>
#include <cmath>

class TlogBuilder {
public:
    void addRecord(quint64    ts_us,
                   quint32    msgid,
                   const QByteArray& payload,
                   quint8     seq     = 1,
                   quint8     sysid   = 1,
                   quint8     compid  = 1,
                   quint8     incompat = 0,
                   quint8     compat   = 0)
    {
        for (int i = 7; i >= 0; --i)
            m_data.append(char((ts_us >> (i * 8)) & 0xFF));

        m_data.append(char(0xFD));
        m_data.append(char(payload.size() & 0xFF));
        m_data.append(char(incompat));
        m_data.append(char(compat));
        m_data.append(char(seq));
        m_data.append(char(sysid));
        m_data.append(char(compid));
        m_data.append(char(msgid         & 0xFF));
        m_data.append(char((msgid >>  8) & 0xFF));
        m_data.append(char((msgid >> 16) & 0xFF));

        m_data.append(payload);

        m_data.append(char(0x00));
        m_data.append(char(0x00));
    }

    void addHeartbeat(quint64 ts_us,
                      quint8 type      = 2,
                      quint8 autopilot = 3,
                      quint8 base_mode = 0,
                      quint8 sys_status = 4)
    {
        QByteArray p(9, 0);
        p[4] = char(type);
        p[5] = char(autopilot);
        p[6] = char(base_mode);
        p[7] = char(sys_status);
        addRecord(ts_us, 0, p);
    }

    void addGlobalPositionInt(quint64 ts_us,
                              qint32  lat_1e7,
                              qint32  lon_1e7,
                              qint32  alt_mm,
                              qint32  rel_alt_mm = 0)
    {
        QByteArray p(28, 0);
        writeI32(p,  0, 0);
        writeI32(p,  4, lat_1e7);
        writeI32(p,  8, lon_1e7);
        writeI32(p, 12, alt_mm);
        writeI32(p, 16, rel_alt_mm);
        addRecord(ts_us, 33, p);
    }

    void addAttitude(quint64 ts_us,
                     float roll_rad  = 0.0f,
                     float pitch_rad = 0.0f,
                     float yaw_rad   = 0.0f)
    {
        QByteArray p(28, 0);
        writeF32(p,  4, roll_rad);
        writeF32(p,  8, pitch_rad);
        writeF32(p, 12, yaw_rad);
        addRecord(ts_us, 30, p);
    }

    void addStatusText(quint64 ts_us, quint8 severity, const QString& text)
    {
        QByteArray p(51, 0);
        p[0] = char(severity);
        QByteArray tb = text.toLatin1().left(50);
        std::memcpy(p.data() + 1, tb.constData(), tb.size());
        addRecord(ts_us, 253, p);
    }

    void addJunk(const QByteArray& bytes) { m_data.append(bytes); }

    QByteArray data() const { return m_data; }

private:
    static void writeI32(QByteArray& d, int off, qint32 v) {
        std::memcpy(d.data() + off, &v, 4);
    }
    static void writeF32(QByteArray& d, int off, float v) {
        std::memcpy(d.data() + off, &v, 4);
    }

    QByteArray m_data;
};
