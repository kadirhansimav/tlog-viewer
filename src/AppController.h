#pragma once

#include "MavlinkMessage.h"
#include "MessageListModel.h"

#include <QObject>
#include <QVector>
#include <QVariantList>
#include <QPair>

class AppController : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool   fileLoaded        READ fileLoaded        NOTIFY fileLoadedChanged)
    Q_PROPERTY(int    messageCount      READ messageCount      NOTIFY messagesChanged)
    Q_PROPERTY(int    currentIndex      READ currentIndex      WRITE setCurrentIndex
                                                               NOTIFY currentIndexChanged)
    Q_PROPERTY(double currentLat        READ currentLat        NOTIFY currentIndexChanged)
    Q_PROPERTY(double currentLon        READ currentLon        NOTIFY currentIndexChanged)
    Q_PROPERTY(QString currentTimeStr   READ currentTimeStr    NOTIFY currentIndexChanged)
    Q_PROPERTY(QString startTimeStr     READ startTimeStr      NOTIFY messagesChanged)
    Q_PROPERTY(QString endTimeStr       READ endTimeStr        NOTIFY messagesChanged)
    Q_PROPERTY(QVariantList flightPath  READ flightPath        NOTIFY messagesChanged)
    Q_PROPERTY(MessageListModel* messageModel READ messageModel CONSTANT)

public:
    explicit AppController(QObject* parent = nullptr);

    bool   fileLoaded()   const { return m_fileLoaded; }
    int    messageCount() const { return m_messages.size(); }
    int    currentIndex() const { return m_currentIndex; }
    double currentLat()   const;
    double currentLon()   const;
    QString currentTimeStr() const;
    QString startTimeStr()   const;
    QString endTimeStr()     const;
    QVariantList flightPath() const;
    MessageListModel* messageModel() const { return m_model; }

    Q_INVOKABLE bool loadFile(const QString& urlOrPath);

public slots:
    void setCurrentIndex(int index);

signals:
    void fileLoadedChanged();
    void messagesChanged();
    void currentIndexChanged();
    void loadError(const QString& msg);

private:
    QString formatTime(quint64 ts_us) const;

    struct PosEntry { double lat = 0.0; double lon = 0.0; };

    bool   m_fileLoaded    = false;
    int    m_currentIndex  = 0;
    QVector<MavlinkMessage> m_messages;
    QVector<PosEntry>       m_posCache;
    QVariantList            m_flightPath;
    quint64 m_startTime    = 0;
    quint64 m_endTime      = 0;

    MessageListModel* m_model;
};
