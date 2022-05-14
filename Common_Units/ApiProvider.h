#ifndef API_PROVIDER_H
#define API_PROVIDER_H

#include <QFile>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QWidget>

class ApiProvider : public QWidget
{


public:
    ApiProvider(QWidget *parent = nullptr);
    ~ApiProvider();

    /* 请求网络 */
    void requestNetwork(QString, QByteArray);

    QString apiType;

private:

    /* 存储serverapi地址 */
    QString serverApiUrl;

    /* 网络管理 */
    QNetworkAccessManager *networkAccessManager;

private slots:

    /* 准备读取响应返回来的数据 */
    virtual void readyReadData();

    /* 响应完成处理 */
    virtual void replyFinished();

signals:
    void apiReadyData(QString);

};
#endif // API_PROVIDER_H
