/******************************************************************
Copyright © Huangzhenhang Co., Ltd. 1990-2021. All rights reserved.
* @projectName   HouseHelper
* @brief         mainwindow.h
* @author        Huangzhenhang | Deng Zhimao
* @email         biechusheng233@gmail.com | 1252699831@qq.com
* @date          2021-06-04
*******************************************************************/

#include "tts.h"
#include <QCoreApplication>


tts::tts(QWidget *parent)
    : QWidget(parent)
{
    networkAccessManager = new QNetworkAccessManager(this);

    fileName = QCoreApplication::applicationDirPath() + "/tts_16k.wav";
    file.setFileName(fileName);

    if (file.exists())
    {
        file.remove();
        qDebug()<<fileName<<" delete"<<endl;
    }


    tokenUrl = QString(token_org).arg(api_key).arg(secret_key);

    QByteArray requestData;
    requestData.clear();

    requestNetwork(tokenUrl, requestData);
}

tts::~tts()
{
}

/* 请求网络 */
void tts::requestNetwork(QString url, QByteArray requestData)
{
    /* 网络请求 */
    QNetworkRequest networkRequest;

    /* 开发板需要加一些安全配置才能访问https */
    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1SslV3);
    networkRequest.setSslConfiguration(config);

    /* 以json格式返回 */
    if (url == tokenUrl) {
        networkRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                                 "application/json;charset=UTF-8");

    }

    if (url == serverApiUrl){

        networkRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                                 "audio/wav");
    }

    /* 设置访问的地址 */
    networkRequest.setUrl(url);

    /* 网络响应 */
    QNetworkReply *newReply = networkAccessManager->post(networkRequest, requestData);

    connect(newReply, SIGNAL(finished()), this, SLOT(replyFinished()));
    connect(newReply, SIGNAL(readyRead()), this, SLOT(readyReadData()));
}


/* 读取数据 */
void tts::readyReadData()
{
    QNetworkReply *reply = (QNetworkReply *)sender();
    QByteArray data = reply->readAll();

    if (reply->url() == QUrl(tokenUrl)) {

        qDebug()<<QString(data)<<endl;
        QString key = "access_token";

        QString temp = getJsonValue(data, key);
        accessToken = temp;

        qDebug()<<accessToken<<endl;
        qDebug()<<"TTS 获取token成功，可以调用getTheResult设置语音路径返回结果"<<endl;
    }

    if (reply->url() == QUrl(serverApiUrl)) {
        //handler the tts data

        QString reply_content = reply->header(QNetworkRequest::ContentTypeHeader).toString();

        qDebug()<<"Get reply from server content type is "<<reply_content<<endl;

        if(reply_content == "audio/wav"){
            fileName = QCoreApplication::applicationDirPath() + "/tts_16k.wav";
            file.setFileName(fileName);
            file.open(QIODevice::WriteOnly|QIODevice::Append);
            file.write(data);
            file.close();
        }else {
            fileName = QCoreApplication::applicationDirPath() + "/error_rcv.txt";
            file.setFileName(fileName);
            file.open(QIODevice::WriteOnly);
            file.write(data);
            file.close();
            qDebug()<<"file type is wrong"<<endl;
        }
    }

}

/* 请求完成处理，释放对象 */
void tts::replyFinished()
{
    QNetworkReply *reply = (QNetworkReply *)sender();
    if (reply->url() == QUrl(tokenUrl) ) {
        //        QByteArray requestData;

        //        file.open(QIODevice::ReadOnly);
        //        requestData = file.readAll();
        //        file.close();

        //        serverApiUrl = QString(server_api).arg(QHostInfo::localHostName()).arg(accessToken);

        //        requestNetwork(serverApiUrl, requestData);
    }

    if (reply->url() == QUrl(serverApiUrl) ) {
        fileName = QCoreApplication::applicationDirPath() + "/tts_16k.wav";
        file.setFileName(fileName);
        if (file.exists())
        {
            qDebug()<<"tts file get and reply is finished"<<endl;
            emit ttsReadyData(fileName);
        }
    }

    reply->deleteLater();
    reply = nullptr;
}

/* Json解释分离数据 */
QString tts::getJsonValue(QByteArray ba, QString key)
{
    QJsonParseError parseError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(ba, &parseError);

    if (parseError.error == QJsonParseError::NoError) {

        if (jsonDocument.isObject()) {
            /* jsonDocument转化为json对象 */
            QJsonObject jsonObj = jsonDocument.object();

            if (jsonObj.contains(key)) {
                QJsonValue jsonVal= jsonObj.value(key);

                if (jsonVal.isString()) {
                    return jsonVal.toString();

                } else if (jsonVal.isArray()) {
                    /* 转换成jsonArray */
                    QJsonArray arr = jsonVal.toArray();
                    /* 获取第一个元素 */
                    QJsonValue jv = arr.at(0);
                    return jv.toString();
                }
            }
        }
    }

    return nullptr;
}

void tts::getVoiceByText(QString text)
{
    QByteArray requestData;
    qDebug()<<"请求合成文字:"<<text<<endl;
    fileName = QCoreApplication::applicationDirPath() + "/tts_16k.wav";
    file.setFileName(fileName);
    if (file.exists())
    {
        file.remove();
        qDebug()<<fileName<<" delete"<<endl;
    }
    serverApiUrl = QString(server_api).arg(QHostInfo::localHostName()).arg(accessToken).arg(text);

    qDebug()<<"请求url:"<<serverApiUrl<<endl;

    requestNetwork(serverApiUrl, requestData);
}
