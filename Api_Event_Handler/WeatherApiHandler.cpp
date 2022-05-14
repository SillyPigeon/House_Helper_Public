#include "WeatherApiHandler.h"
#include <QCoreApplication>

WeatherApiHandler::WeatherApiHandler(QWidget *parent)
    : QWidget(parent)
{
    networkAccessManager = new QNetworkAccessManager(this);

    mAreaId = 0;
    mAreaCode = 0;
    mAreaString = "";
    fileName = QCoreApplication::applicationDirPath() + "/weatherData.json";

    ClearFileCatch();
}

WeatherApiHandler::~WeatherApiHandler()
{
}

void WeatherApiHandler::getWeatherData(QString region)
{
    ClearFileCatch();

    areaIdApiUrl = QString(areaId_api).arg(apk_id).arg(region).arg(secret_sign);

    QByteArray requestData;
    requestData.clear();

    requestNetwork(areaIdApiUrl, requestData);
}


void WeatherApiHandler::requestNetwork(QString url, QByteArray requestData)
{
    /* 网络请求 */
    QNetworkRequest networkRequest;

    /* 开发板需要加一些安全配置才能访问https */
    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1SslV3);
    networkRequest.setSslConfiguration(config);

    /* 以json格式返回 */
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                             "application/json;charset=UTF-8");

    /* 设置访问的地址 */
    networkRequest.setUrl(url);

    /* 网络响应 */
    QNetworkReply *newReply = networkAccessManager->post(networkRequest, requestData);

    connect(newReply, SIGNAL(finished()), this, SLOT(replyFinished()));
    connect(newReply, SIGNAL(readyRead()), this, SLOT(readyReadData()));

}

/* 读取数据 */
void WeatherApiHandler::readyReadData()
{
    QNetworkReply *reply = (QNetworkReply *)sender();
    QByteArray data = reply->readAll();

    file.setFileName(fileName);
    file.open(QIODevice::WriteOnly|QIODevice::Append);
    file.write(data);
    file.close();
}

/* 请求完成处理，释放对象 */
void WeatherApiHandler::replyFinished()
{
    QNetworkReply *reply = (QNetworkReply *)sender();
    if (reply->url() == QUrl(areaIdApiUrl) ) {
        file.setFileName(fileName);
        if (!file.exists()) {
            qDebug()<<"已返回，文件"<<fileName<<"不存在"<<endl;
            return;
        }
        QByteArray areaData;
        file.open(QIODevice::ReadOnly);
        areaData = file.readAll();
        file.close();
        getAreaValueFromData(areaData);
        // 请求天气数据
        ClearFileCatch();
        weatherApiUrl = QString(weather_api).arg(apk_id)
                                              .arg(mAreaCode)
                                              .arg(mAreaId)
                                              .arg(mAreaString)
                                              .arg(secret_sign);
        QByteArray requestData;
        requestData.clear();
        requestNetwork(weatherApiUrl, requestData);
    }

    if (reply->url() == QUrl(weatherApiUrl) ) {
        file.setFileName(fileName);
        if (!file.exists()) {
            qDebug()<<"已返回，文件"<<fileName<<"不存在"<<endl;
            return;
        }
        QByteArray weatherData;
        file.open(QIODevice::ReadOnly);
        weatherData = file.readAll();
        file.close();
        QString weatherMsg = getWeatherTextFromData(weatherData);
        emit weatherDadaReadyData(weatherMsg);
    }

    reply->deleteLater();
    reply = nullptr;
}

void WeatherApiHandler::ClearFileCatch()
{
    file.setFileName(fileName);
    if (file.exists())
    {
        file.remove();
        qDebug()<<fileName<<" delete"<<endl;
    }
}

/* Json解释分离数据 */
QString WeatherApiHandler::getWeatherTextFromData(QByteArray data){
    QJsonParseError parseError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data, &parseError);

    if (!(parseError.error == QJsonParseError::NoError)) {
        qDebug()<<"parseError.error"<<parseError.error<<endl;
        return "";
    }

    if (!jsonDocument.isObject()) {
        qDebug()<<"jsonDocument.isObject.error"<<endl;
        return "";
    }

    /* jsonDocument转化为json对象 */
    QJsonObject jsonObj = jsonDocument.object();

    if(!jsonObj.contains("showapi_res_body")){
        qDebug()<<"showapi_res_body.error"<<endl;
        return "";
    }

    QJsonObject bodyJsonObject = jsonObj.value(QString("showapi_res_body")).toObject();

    if (!bodyJsonObject.contains("f1")) {
        qDebug()<<"weather_data.error"<<endl;
        return "";
    }

    QJsonObject weatherMsgObject = bodyJsonObject.value("f1").toObject();

    QString msg = "白天的天气是" + weatherMsgObject.value("day_weather").toString()
                               + weatherMsgObject.value("day_air_temperature").toString() + "度，"
                + "晚上的天气是" + weatherMsgObject.value("night_weather").toString()
                               + weatherMsgObject.value("night_air_temperature").toString() + "度";
    qDebug()<<"Get weather msg is "<<msg<<endl;
    return msg;
}

void WeatherApiHandler::getAreaValueFromData(QByteArray data){
    QJsonParseError parseError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data, &parseError);

    if (!(parseError.error == QJsonParseError::NoError)) {
        qDebug()<<"parseError.error"<<parseError.error<<endl;
        return;
    }

    if (!jsonDocument.isObject()) {
        qDebug()<<"jsonDocument.isObject.error"<<endl;
        return;
    }

    /* jsonDocument转化为json对象 */
    QJsonObject jsonObj = jsonDocument.object();

    if(!jsonObj.contains("showapi_res_body")){
        qDebug()<<"showapi_res_body.error"<<endl;
        return;
    }

    QJsonObject bodyJsonObject = jsonObj.value(QString("showapi_res_body")).toObject();

    if (!bodyJsonObject.contains("list")) {
        qDebug()<<"list.error"<<endl;
        return;
    }

    QJsonValue areaListValue = bodyJsonObject.value("list").toArray();

    if(!areaListValue.isArray())
    {
        qDebug()<<"areaListValue.error"<<endl;
        return;
    }

    QJsonArray areaList = areaListValue.toArray();
    QJsonObject firstAreaObject = areaList.first().toObject();

    if (firstAreaObject.contains("areaCode")) {
        mAreaCode = firstAreaObject.value("areaCode").toString().toInt();
        qDebug()<<"areaCode "<<mAreaCode<<endl;
    }

    if (firstAreaObject.contains("areaid")) {
        mAreaId = firstAreaObject.value("areaid").toString().toInt();
        qDebug()<<"mAreaId "<<mAreaId<<endl;
    }

    if (firstAreaObject.contains("distric")) {
        mAreaString = firstAreaObject.value("distric").toString();
        qDebug()<<"mAreaString "<<mAreaString<<endl;
    }
}
