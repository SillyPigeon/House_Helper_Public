#ifndef WEATHER_API_H
#define WEATHER_API_H

#include <QWidget>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QHostInfo>

#include <QFile>

class WeatherApiHandler : public QWidget
{
    Q_OBJECT

public:
    WeatherApiHandler(QWidget *parent = nullptr);
    ~WeatherApiHandler();

    /* 请求网络 */
    void requestNetwork(QString, QByteArray);

    /* 获取识别结果 */
    void getWeatherData(QString);

private:

    //请求区域code
    int mAreaCode;
    //请求区域id
    int mAreaId;
    //区域字串
    QString mAreaString;

    /* 存储serverapi地址 */
    QString weatherApiUrl;

    /* 存储serverapi地址 */
    QString areaIdApiUrl;

    /* 填写网页上申请的api_id 如 g_api_key="g8eBUMSokVB1BHGmgxxxxxx" */
    const QString apk_id = "";

    /* 填写网页上申请的APP SECRET 如 $secretKey="94dc99566550d87f8fa8ece112xxxxx" */
    const QString secret_sign = "";

    /* 天气API接口，发送语音可返回识别结果 */
    const QString weather_api = "http://route.showapi.com/9-2?showapi_appid=%1&areaCode=%2&areaid=%3&area=%4 \
                                 &needMoreDay=0&needIndex=0&need3HourForcast=0&needAlarm=0&needHourData=&showapi_sign=%5";

    /* 区域ID-API接口，发送语音可返回识别结果 */
    const QString areaId_api = "http://route.showapi.com/9-3?showapi_appid=%1&area=%2&showapi_sign=%3";

    /* 网络管理 */
    QNetworkAccessManager *networkAccessManager;

    void getAreaValueFromData(QByteArray);

    QString getWeatherTextFromData(QByteArray);

    void ClearFileCatch(void);

    /* 合成文件 */
    QFile file;

    QString fileName;

private slots:

    /* 准备读取响应返回来的数据 */
    void readyReadData();

    /* 响应完成处理 */
    void replyFinished();

signals:
    void weatherDadaReadyData(QString);

};
#endif // WEATHER_API_H
