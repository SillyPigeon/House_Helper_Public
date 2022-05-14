/******************************************************************
Copyright © Huangzhenhang Co., Ltd. 1990-2021. All rights reserved.
* @projectName   HouseHelper
* @brief         mainwindow.h
* @author        Huangzhenhang | Deng Zhimao
* @email         biechusheng233@gmail.com | 1252699831@qq.com
* @date          2021-06-04
*******************************************************************/
#ifndef TTS_H
#define TTS_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWidget>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QHostInfo>

#include <QFile>


class tts: public QWidget
{
    Q_OBJECT
public:
    tts(QWidget *parent = nullptr);
    ~tts();

    /* 请求网络 */
    void requestNetwork(QString, QByteArray);

    /* 获取识别结果 */
    void getVoiceByText(QString transText);

private:
    /* 存储获取tokenUrl地址 */
    QString tokenUrl;

    /* 存储serverapi地址 */
    QString serverApiUrl;

    /* 最终需要访问token的地址 */
    QString accessToken;

    /* 获取token的接口*/
    const QString token_org = "https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=%1&client_secret=%2&";

    /* 填写网页上申请的appkey 如 g_api_key="g8eBUMSokVB1BHGmgxxxxxx" */
    const QString api_key = "";

    /* 填写网页上申请的APP SECRET 如 $secretKey="94dc99566550d87f8fa8ece112xxxxx" */
    const QString secret_key = "";

    /* 百度服务器API接口，发送语音可返回识别结果 */
    const QString server_api = "http://tsn.baidu.com/text2audio?lan=zh&ctp=1&cuid=%1&tok=%2&tex=%3&vol=9&per=0&spd=5&pit=5&aue=6";

    /* 网络管理 */
    QNetworkAccessManager *networkAccessManager;

    QString getJsonValue(QByteArray ba, QString key);

    /* 合成文件 */
    QFile file;

    QString fileName;

private slots:

    /* 准备读取响应返回来的数据 */
    void readyReadData();

    /* 响应完成处理 */
    void replyFinished();

signals:
    void ttsReadyData(QString);

};

#endif // TTS_H
