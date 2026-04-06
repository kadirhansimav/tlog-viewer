#include "MessageListModel.h"

MessageListModel::MessageListModel(QObject* parent)
    : QAbstractListModel(parent)
{}

void MessageListModel::setMessages(const QVector<MavlinkMessage>& messages,
                                   quint64 startTime)
{
    beginResetModel();
    m_messages   = messages;
    m_startTime  = startTime;
    m_currentIndex = -1;
    endResetModel();
}

void MessageListModel::setCurrentIndex(int index)
{
    const int prev = m_currentIndex;
    m_currentIndex = index;

    if (prev >= 0 && prev < m_messages.size()) {
        const QModelIndex mi = createIndex(prev, 0);
        emit dataChanged(mi, mi, {IsActiveRole});
    }
    if (m_currentIndex >= 0 && m_currentIndex < m_messages.size()) {
        const QModelIndex mi = createIndex(m_currentIndex, 0);
        emit dataChanged(mi, mi, {IsActiveRole});
    }
}

int MessageListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return m_messages.size();
}

QString MessageListModel::formatRelativeTime(quint64 timestamp_us) const
{
    if (m_startTime == 0 || timestamp_us < m_startTime) return "00:00";
    const quint64 delta_s = (timestamp_us - m_startTime) / 1'000'000ULL;
    const quint64 mm = delta_s / 60;
    const quint64 ss = delta_s % 60;
    return QString("%1:%2")
        .arg(mm, 2, 10, QLatin1Char('0'))
        .arg(ss, 2, 10, QLatin1Char('0'));
}

QVariant MessageListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_messages.size())
        return {};

    const MavlinkMessage& msg = m_messages[index.row()];

    switch (role) {
    case TimeRole:       return formatRelativeTime(msg.timestamp_us);
    case MsgIdRole:      return msg.msgid;
    case MsgNameRole:    return msg.msgName;
    case FieldsTextRole: return msg.fieldsText;
    case IsActiveRole:   return (index.row() == m_currentIndex);
    default:             return {};
    }
}

QHash<int, QByteArray> MessageListModel::roleNames() const
{
    return {
        { TimeRole,       "msgTime"    },
        { MsgIdRole,      "msgId"      },
        { MsgNameRole,    "msgName"    },
        { FieldsTextRole, "fieldsText" },
        { IsActiveRole,   "isActive"   },
    };
}
