#pragma once

#include <cstdint>
#include <QString>
#include <QByteArray>

struct MavlinkMessage {
    quint64    timestamp_us = 0;
    quint8     incompat_flags = 0;
    quint8     compat_flags  = 0;
    quint8     seq   = 0;
    quint8     sysid = 0;
    quint8     compid= 0;
    quint32    msgid = 0;
    QByteArray payload;

    QString msgName;
    QString fieldsText;

    bool   hasPosition = false;
    double lat = 0.0;
    double lon = 0.0;
    double alt = 0.0;
};
