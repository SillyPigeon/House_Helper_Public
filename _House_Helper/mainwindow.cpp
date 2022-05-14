/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 1990-2021. All rights reserved.
* @projectName   02_asr_demo
* @brief         mainwindow.cpp
* @author        Deng Zhimao
* @email         1252699831@qq.com
* @net           www.openedv.com
* @date          2021-06-04
*******************************************************************/
#include "mainwindow.h"
#include <QDebug>
#include <QSound>
#include <unistd.h>

#define TTS_EVENT_ASR    0
#define TTS_EVENT_PLAY_ONCE  1

static int TTSEventType;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setGeometry(0, 0, 800, 480);
    this->setStyleSheet("background:#14161a");

    mainWidget = new QWidget();
    movieWidget = new QWidget();

    movieLabel = new QLabel();
    textLabel = new QLabel();

    movieWidget->setMinimumWidth(240);
    textLabel->setMinimumSize(240, 50);
    textLabel->setStyleSheet("color: white; font-size: 20px");
    textLabel->setText("请点击，开始说话...");
    textLabel->setAlignment(Qt::AlignCenter);

    /* 安装事件过滤器 */
    movieLabel->installEventFilter(this);
    movieLabel->setFixedSize(240, 240);

    vBoxLayout = new QVBoxLayout();
    hBoxLayout = new QHBoxLayout();

    vBoxLayout->addWidget(movieWidget);
    vBoxLayout->addWidget(textLabel);
    vBoxLayout->setAlignment(Qt::AlignCenter);

    mainWidget->setLayout(vBoxLayout);

    hBoxLayout->addWidget(movieLabel);
    movieWidget->setLayout(hBoxLayout);

    setCentralWidget(mainWidget);

    myMovie = new QMovie(":/gif/voice_effect.gif");
    /* 设置播放速度，值越大越快 */
    myMovie->setSpeed(50);
    movieLabel->setMovie(myMovie);
    movieLabel->show();
    /* 设置设置化时显示第一帧 */
    myMovie->jumpToNextFrame();

    movieLabel->setAlignment(Qt::AlignHCenter);

    timer1 = new QTimer(this);
    timer2 = new QTimer(this);
    timer3 = new QTimer(this);

    connect(timer1, SIGNAL(timeout()), this, SLOT(onTimer1TimeOut()));
    connect(timer2, SIGNAL(timeout()), this, SLOT(onTimer2TimeOut()));
    connect(timer3, SIGNAL(timeout()), this, SLOT(onTimer3TimeOut()));

    mRegion = "广州";

    /* 自定义的录音类 */
    myAudioRecorder = new AudioRecorder(this);

    /* 自定义的百度云识别录音类 */
    myAsr = new Asr(this);

    /* 自定义的百度云合成类 */
    myTTS = new tts(this);

    /* LED */
    myLed = new Led(this);

    /* 天气管理器 */
    myWeatherApiHandler = new WeatherApiHandler(this);

    /* 预约事件管理器 */
    myAlarmEventHandler = new AlarmEventHandler();

    connect(myAsr, SIGNAL(asrReadyData(QString)), this, SLOT(onAsrReadyData(QString)));
    connect(myTTS, SIGNAL(ttsReadyData(QString)), this, SLOT(onTTSReadyData(QString)));
    connect(myAlarmEventHandler, SIGNAL(timeEventStart(QString)), this, SLOT(onTimeEventStart(QString)));
    connect(myWeatherApiHandler, SIGNAL(weatherDadaReadyData(QString)), this, SLOT(onWeatherDadaReadyData(QString)));

}

MainWindow::~MainWindow()
{
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event){

    if (watched == movieLabel && event->type() == QEvent::MouseButtonPress) {
        QSound::play(":/audio/sound.wav");
        if (myMovie->state() != QMovie::Running) {
            /* 等待QSound播放完,1.5s后再录音 */
            textLabel->setText("正在听您说话，请继续...");
            TTSEventType = TTS_EVENT_ASR;
            myTTS->getVoiceByText("什么事");
            myMovie->start();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void MainWindow::onTimer1TimeOut()
{
    /* 停止录音，8s钟的短语音 */
    myAudioRecorder->stopRecorder();
    textLabel->setText("正在识别，请稍候...");
    timer1->stop();
    myMovie->stop();
    QString fileName = QCoreApplication::applicationDirPath() + "/16k.wav";
    myAsr->getTextByVoice(fileName);
    timer3->start(30000);
}

void MainWindow::onTimer2TimeOut()
{
    timer1->start(5000);
    /* 开始录音 */
    myAudioRecorder->startRecorder();
    timer2->stop();
}

void MainWindow::onTimer3TimeOut()
{
    textLabel->setText("请点击，开始说话...");
    timer3->stop();
}

void MainWindow::onAsrReadyData(QString str)
{
    if (str.contains("分钟后")
    || str.contains("分钟之后")){
        myAlarmEventHandler->AlarmEvent_AddOneEvent(str);
    }else {
        if (str.contains("开灯"))
            myLed->setLedState(true);
        else if (str.contains("关灯"))
            myLed->setLedState(false);
        else if(str.contains("天气")){
            myWeatherApiHandler->getWeatherData(mRegion);
        }
    }

    textLabel->setText("识别结果是:\n" + str);
    textLabel->adjustSize();
}

void MainWindow::onTTSReadyData(QString filePath)
{
    //play music
    qDebug()<<"play file is "<<filePath<<endl;
    QSound::play(filePath);
    if (TTSEventType == TTS_EVENT_ASR)
    {
        timer2->start(500);
    }
}

void MainWindow::onTimeEventStart(QString eventText)
{
    //handle event
    qDebug()<<"Get event text is "<<eventText<<endl;
    QString requestText = myAlarmEventHandler->AlarmEvent_GetTipTextByEvent(eventText);
    TTSEventType = TTS_EVENT_PLAY_ONCE;
    myTTS->getVoiceByText(requestText);

}

void MainWindow::onWeatherDadaReadyData(QString weatherMsg)
{
    //handle event
    QString requestText = "主人，今天" + weatherMsg;
    qDebug()<<"Today weather msg is "<<requestText<<endl;
    TTSEventType = TTS_EVENT_PLAY_ONCE;
    myTTS->getVoiceByText(requestText);
}




