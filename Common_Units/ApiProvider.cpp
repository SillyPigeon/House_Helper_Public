#include "ApiProvider.h"


ApiProvider::ApiProvider(QWidget *parent)
    : QWidget(parent)
{
    networkAccessManager = new QNetworkAccessManager(this);
}

ApiProvider::~ApiProvider()
{
}


void ApiProvider::requestNetwork(QString url, QByteArray requestData)
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

