#include <QtTest/QtTest>
#include <QSignalSpy>

#include "MessageListModel.h"
#include "MavlinkMessage.h"

static QVector<MavlinkMessage> makeMsgs(int count,
                                         quint64 startTs = 0,
                                         quint64 stepUs  = 1'000'000ULL)
{
    QVector<MavlinkMessage> v;
    v.reserve(count);
    for (int i = 0; i < count; ++i) {
        MavlinkMessage m;
        m.timestamp_us = startTs + quint64(i) * stepUs;
        m.msgid        = quint32(i);
        m.msgName      = QString("MSG_%1").arg(i);
        m.fieldsText   = QString("field: %1").arg(i);
        v.append(m);
    }
    return v;
}

class TstMessageListModel : public QObject {
    Q_OBJECT
private slots:
    void rowCount_freshModel_isZero();
    void rowCount_afterSetMessages_matchesInputSize();
    void data_outOfBoundsRow_returnsNull();
    void data_rolesReturnCorrectValues();
    void data_isActive_defaultFalseForAll();
    void data_isActive_trueOnlyForCurrentIndex();
    void setCurrentIndex_emitsDataChanged_forBothOldAndNew();
    void setCurrentIndex_doesNotEmitForEveryRow();
    void setMessages_resetsCurrentIndexAndEmitsModelReset();
    void timeRole_relativeTimeFormattedCorrectly();
};

void TstMessageListModel::rowCount_freshModel_isZero()
{
    MessageListModel model;
    QCOMPARE(model.rowCount(), 0);
}

void TstMessageListModel::rowCount_afterSetMessages_matchesInputSize()
{
    MessageListModel model;
    model.setMessages(makeMsgs(5), 0);
    QCOMPARE(model.rowCount(), 5);
}

void TstMessageListModel::data_outOfBoundsRow_returnsNull()
{
    MessageListModel model;
    model.setMessages(makeMsgs(3), 0);
    QVERIFY(!model.data(model.index(3, 0), MessageListModel::MsgNameRole).isValid());
    QVERIFY(!model.data(QModelIndex{}, MessageListModel::MsgNameRole).isValid());
}

void TstMessageListModel::data_rolesReturnCorrectValues()
{
    MessageListModel model;
    model.setMessages(makeMsgs(3), 0);
    QCOMPARE(model.data(model.index(2, 0), MessageListModel::MsgNameRole).toString(),
             QString("MSG_2"));
    QCOMPARE(model.data(model.index(2, 0), MessageListModel::MsgIdRole).toInt(), 2);
    QCOMPARE(model.data(model.index(2, 0), MessageListModel::FieldsTextRole).toString(),
             QString("field: 2"));
}

void TstMessageListModel::data_isActive_defaultFalseForAll()
{
    MessageListModel model;
    model.setMessages(makeMsgs(5), 0);
    for (int i = 0; i < 5; ++i)
        QVERIFY(!model.data(model.index(i, 0), MessageListModel::IsActiveRole).toBool());
}

void TstMessageListModel::data_isActive_trueOnlyForCurrentIndex()
{
    MessageListModel model;
    model.setMessages(makeMsgs(5), 0);
    model.setCurrentIndex(2);
    QVERIFY( model.data(model.index(2, 0), MessageListModel::IsActiveRole).toBool());
    QVERIFY(!model.data(model.index(1, 0), MessageListModel::IsActiveRole).toBool());
    model.setCurrentIndex(4);
    QVERIFY(!model.data(model.index(2, 0), MessageListModel::IsActiveRole).toBool());
    QVERIFY( model.data(model.index(4, 0), MessageListModel::IsActiveRole).toBool());
}

void TstMessageListModel::setCurrentIndex_emitsDataChanged_forBothOldAndNew()
{
    MessageListModel model;
    model.setMessages(makeMsgs(5), 0);
    model.setCurrentIndex(1);

    QSignalSpy spy(&model, &QAbstractListModel::dataChanged);
    model.setCurrentIndex(3);

    QCOMPARE(spy.count(), 2);
    QSet<int> rows;
    for (const auto& args : spy)
        rows.insert(args.at(0).value<QModelIndex>().row());
    QVERIFY(rows.contains(1));
    QVERIFY(rows.contains(3));
}

void TstMessageListModel::setCurrentIndex_doesNotEmitForEveryRow()
{
    MessageListModel model;
    model.setMessages(makeMsgs(100), 0);
    model.setCurrentIndex(10);

    QSignalSpy spy(&model, &QAbstractListModel::dataChanged);
    model.setCurrentIndex(50);

    QCOMPARE(spy.count(), 2);
}

void TstMessageListModel::setMessages_resetsCurrentIndexAndEmitsModelReset()
{
    MessageListModel model;
    QSignalSpy resetSpy(&model, &QAbstractListModel::modelReset);

    model.setMessages(makeMsgs(5), 0);
    model.setCurrentIndex(3);
    model.setMessages(makeMsgs(5), 0);

    QCOMPARE(resetSpy.count(), 2);
    for (int i = 0; i < 5; ++i)
        QVERIFY(!model.data(model.index(i, 0), MessageListModel::IsActiveRole).toBool());
}

void TstMessageListModel::timeRole_relativeTimeFormattedCorrectly()
{
    MessageListModel model;
    const quint64 start = 1'000'000'000'000'000ULL;
    model.setMessages(makeMsgs(1, start + 90'000'000ULL, 0), start);
    QCOMPARE(model.data(model.index(0, 0), MessageListModel::TimeRole).toString(),
             QString("01:30"));
}

QTEST_MAIN(TstMessageListModel)
#include "tst_messagelistmodel.moc"
