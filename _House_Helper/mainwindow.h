/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 1990-2021. All rights reserved.
* @projectName   02_asr_demo
* @brief         mainwindow.h
* @author        Deng Zhimao
* @email         1252699831@qq.com
* @net           www.openedv.com
* @date          2021-06-04
*******************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QMovie>
#include <QTimer>
#include <QHBoxLayout>
#include "../Central_System/audiorecorder.h"
#include "../Central_System/asr.h"
#include "../Central_System/tts.h"
#include "../Local_Event_Handler/led.h"
#include "../Local_Event_Handler/alarmeventhandler.h"
#include "../Api_Event_Handler/WeatherApiHandler.h"

class Asr;
class AudioRecorder;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

    /* 主Widget */
    QWidget *mainWidget;

    /* gif底下的Widget */
    QWidget *movieWidget;

    /* gif动画画布 */
    QLabel *movieLabel;

    /* 用于显示识别结果 */
    QLabel *textLabel;

    /* 垂直布局 */
    QVBoxLayout *vBoxLayout;

    /* 水平布局 */
    QHBoxLayout *hBoxLayout;

    /* 用于显示GIF动画 */
    QMovie *myMovie;

    /* 定时器 */
    QTimer *timer1;
    QTimer *timer2;
    QTimer *timer3;

    /* 事件过滤器 */
    bool eventFilter(QObject *watched, QEvent *event);

    /* 录音类 */
    AudioRecorder *myAudioRecorder;

    /* 主意识别类 */
    Asr *myAsr;

    /* 主语言合成 */
    tts *myTTS;

    /* 开发板LED */
    Led *myLed;

    /* 天气管理器 */
    WeatherApiHandler* myWeatherApiHandler;

    /* 预约事件管理器 */
    AlarmEventHandler* myAlarmEventHandler;

    /*成员变量*/
    QString mRegion;


private slots:
    void onTimer1TimeOut();
    void onTimer2TimeOut();
    void onTimer3TimeOut();
    void onAsrReadyData(QString);
    void onTTSReadyData(QString);
    void onTimeEventStart(QString);
    void onWeatherDadaReadyData(QString);


};
#endif // MAINWINDOW_H
