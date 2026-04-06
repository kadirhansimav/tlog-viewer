#pragma once

#include "MavlinkMessage.h"
#include <QAbstractListModel>
#include <QVector>

class MessageListModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        TimeRole       = Qt::UserRole + 1,
        MsgIdRole,
        MsgNameRole,
        FieldsTextRole,
        IsActiveRole,
    };

    explicit MessageListModel(QObject* parent = nullptr);

    void setMessages(const QVector<MavlinkMessage>& messages, quint64 startTime);
    void setCurrentIndex(int index);

    int      rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    QString formatRelativeTime(quint64 timestamp_us) const;

    QVector<MavlinkMessage> m_messages;
    quint64 m_startTime = 0;
    int     m_currentIndex = -1;
};
