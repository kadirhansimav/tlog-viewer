#include "AppController.h"
#include "TlogParser.h"

#include <QUrl>
#include <QGeoCoordinate>
#include <algorithm>
#include <cmath>

AppController::AppController(QObject* parent)
    : QObject(parent)
    , m_model(new MessageListModel(this))
{}

bool AppController::loadFile(const QString& urlOrPath)
{
    QString path = urlOrPath;
    if (path.startsWith("file://"))
        path = QUrl(path).toLocalFile();

    QString error;
    const auto messages = TlogParser::parse(path, &error);

    if (messages.isEmpty()) {
        emit loadError(error.isEmpty() ? "Dosya boş veya geçersiz." : error);
        return false;
    }

    m_messages     = messages;
    m_currentIndex = 0;
    m_startTime    = m_messages.first().timestamp_us;
    m_endTime      = m_messages.last().timestamp_us;
    m_fileLoaded   = true;

    m_posCache.resize(m_messages.size());
    PosEntry lastPos{0.0, 0.0};
    for (const auto& msg : m_messages) {
        if (msg.hasPosition) { lastPos = {msg.lat, msg.lon}; break; }
    }
    for (int i = 0; i < m_messages.size(); ++i) {
        const auto& msg = m_messages[i];
        if (msg.hasPosition) {
            lastPos.lat = msg.lat;
            lastPos.lon = msg.lon;
        }
        m_posCache[i] = lastPos;
    }

    m_flightPath.clear();
    {
        constexpr double MIN_DIST_M  = 1.0;
        constexpr double DEG_TO_M    = 111320.0;

        double prevLat = 0.0, prevLon = 0.0;
        bool first = true;

        for (const auto& msg : m_messages) {
            if (!msg.hasPosition) continue;

            if (first) {
                m_flightPath.append(QVariant::fromValue(QGeoCoordinate(msg.lat, msg.lon)));
                prevLat = msg.lat;
                prevLon = msg.lon;
                first = false;
                continue;
            }

            const double dlat = (msg.lat - prevLat) * DEG_TO_M;
            const double dlon = (msg.lon - prevLon) * DEG_TO_M
                                * std::cos(prevLat * M_PI / 180.0);
            const double dist = std::sqrt(dlat * dlat + dlon * dlon);

            if (dist >= MIN_DIST_M) {
                m_flightPath.append(QVariant::fromValue(QGeoCoordinate(msg.lat, msg.lon)));
                prevLat = msg.lat;
                prevLon = msg.lon;
            }
        }
    }

    m_model->setMessages(m_messages, m_startTime);
    m_model->setCurrentIndex(0);

    emit fileLoadedChanged();
    emit messagesChanged();
    emit currentIndexChanged();
    return true;
}

void AppController::setCurrentIndex(int index)
{
    index = qBound(0, index, qMax(0, m_messages.size() - 1));
    if (index == m_currentIndex) return;
    m_currentIndex = index;
    m_model->setCurrentIndex(index);
    emit currentIndexChanged();
}

double AppController::currentLat() const
{
    if (m_posCache.isEmpty()) return 0.0;
    return m_posCache[qBound(0, m_currentIndex, m_posCache.size()-1)].lat;
}

double AppController::currentLon() const
{
    if (m_posCache.isEmpty()) return 0.0;
    return m_posCache[qBound(0, m_currentIndex, m_posCache.size()-1)].lon;
}

QString AppController::formatTime(quint64 ts_us) const
{
    if (m_startTime == 0 || ts_us < m_startTime) return "00:00";
    const quint64 delta_s = (ts_us - m_startTime) / 1'000'000ULL;
    return QString("%1:%2")
        .arg(delta_s / 60, 2, 10, QLatin1Char('0'))
        .arg(delta_s % 60, 2, 10, QLatin1Char('0'));
}

QString AppController::currentTimeStr() const
{
    if (m_messages.isEmpty()) return "00:00";
    return formatTime(m_messages[qBound(0, m_currentIndex,
                                        m_messages.size()-1)].timestamp_us);
}

QString AppController::startTimeStr() const { return "00:00"; }

QString AppController::endTimeStr() const
{
    if (m_messages.isEmpty()) return "00:00";
    return formatTime(m_endTime);
}

QVariantList AppController::flightPath() const { return m_flightPath; }
