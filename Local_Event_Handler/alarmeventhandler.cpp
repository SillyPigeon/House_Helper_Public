#include "alarmeventhandler.h"
#include <QDebug>
#include <QSqlError>
#include <QMap>
#include <QDateTime>
#include <QMutex>
#include <QSound>

static QMap<QString, int> numStringMap = {
    { "一", 1 },
    { "二", 2 },
    { "两", 2 },
    { "三", 3 },
    { "四", 4 },
    { "五", 5 },
    { "六", 6 },
    { "七", 7 },
    { "八", 8 },
    { "九", 9 },
    { "十", 10 },
};

QMutex g_dbMutex;

AlarmEventHandler::AlarmEventHandler()
{
    //Init Alarm Timer
    mTimer = new QTimer(this);

    connect(mTimer, SIGNAL(timeout()), this, SLOT(AlarmEvent_ExecTimerEvent()));

    //Init SQLite DB
    /* 查看本机可用的数据库驱动 */
    QStringList drivers = QSqlDatabase::drivers();
    foreach(QString driver, drivers) {
        qDebug()<<driver;
    }

    /* 以QSQLITE驱动方式打开或者创建数据库 */
    sqlDatabase = QSqlDatabase::addDatabase("QSQLITE");
    sqlDatabase.setDatabaseName("alarm.db");
    /* 以open的方式打开alarm.db数据库，则会创建一个alarm.db */
    if (!sqlDatabase.open())
        qDebug()<<"连接数据库错误"<<sqlDatabase.lastError()<<endl;
    else
        qDebug()<<"连接数据库成功"<<endl;

    QSqlQuery query(sqlDatabase);
    /* 使用指令式创建表 */
    query.exec("create table alarm (id int primary key, time int, event vchar(30))");

    model = new QSqlTableModel(this, sqlDatabase);

    /* 模型设置表的名字，需要与数据库的表的名字相同 */
    model->setTable("alarm");

    /* 如果有修改则同步修改到数据库，
     * 注意这个规则需要与tabview这样的控件才生效，
     * 因为tabview可以直接编辑表里的内容 */
    model->setEditStrategy(QSqlTableModel::OnFieldChange);

    /* 成功则返回true，查看数据库里是否有alarm这个表格 */
    model->select();
}

bool AlarmEventHandler::AlarmEvent_AddOneEvent(QString timeEvent)
{
    int timeIndex = timeEvent.indexOf("分钟后");
    int timeMinute = numStringMap[timeEvent.mid(timeIndex-1,1)];

    QString event = timeEvent.mid(timeIndex+3,timeEvent.length()-timeIndex-3);

    g_dbMutex.lock();
    int row = model->rowCount();
    /* 插入数据到数据库 */
    model->insertRow(row);
    model->setData(model->index(row, 0), row + 1);
    model->setData(model->index(row, 1), timeMinute);
    model->setData(model->index(row, 2), event);
    model->submit();
    /* 再确保提交 */
    if (model->isDirty())
        model->submitAll();
    model->setSort(1,Qt::AscendingOrder);//sort by time;
    model->select();
    int nextMinute = model->data(model->index(0, 1)).toInt();
    g_dbMutex.unlock();

    AlarmEvent_SetTimerMinuteStart(nextMinute);
    return true;
}

void AlarmEventHandler::AlarmEvent_SetTimerMinuteStart(int minute)
{
    mTimer->stop();
    mTimer->start(minute * 60 * 1000);
    qDebug()<<"Set event after minute "<< minute <<endl;
}

void AlarmEventHandler::AlarmEvent_ExecTimerEvent(void)
{
    QSound::play(":/audio/sound.wav");
    mTimer->stop();
    //Get First Event and delete
    g_dbMutex.lock();
    model->setSort(1,Qt::AscendingOrder);//sort by time;
    model->select();
    QString event = model->data(model->index(0, 2)).toString();
    /* 删除数据库整一行数据 */
    model->removeRow(0);
    model->submit();
    /* 执行上面语句 */
    model->select();
    /* 再确保提交 */
    if (model->isDirty())
        model->submitAll();
    g_dbMutex.unlock();
    qDebug()<<"first event is "<< event <<endl;
    emit timeEventStart(event);
    //Set next Timer
}

QString AlarmEventHandler::AlarmEvent_GetTipTextByEvent(QString event)
{
    int commandIndex = event.indexOf("我");
    QString command = event.mid(commandIndex+1,event.length()-commandIndex-2);
    return "主人，到了" + command + "的时间了";
}


