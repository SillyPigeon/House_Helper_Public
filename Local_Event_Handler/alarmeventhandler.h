#ifndef ALARMEVENTHANDLER_H
#define ALARMEVENTHANDLER_H


#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QTimer>

class AlarmEventHandler: public QObject
{
    Q_OBJECT
public:
    AlarmEventHandler();
    bool AlarmEvent_AddOneEvent(QString timeEvent);
    QString AlarmEvent_GetTipTextByEvent(QString event);

private:

    /* 数据库连接类 */
    QSqlDatabase sqlDatabase;

    /* 数据库操作模型 */
    QSqlTableModel *model;

    void AlarmEvent_SetTimerMinuteStart(int minute);

    QTimer* mTimer;

private slots:
    void AlarmEvent_ExecTimerEvent(void);

signals:
    void timeEventStart(QString);
};

#endif // ALARMEVENTHANDLER_H
