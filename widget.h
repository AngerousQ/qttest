///////////angerous 2
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include <fkmthread.h>
#include <QUdpSocket>
#include <QTimer>
#include <QtNetwork/QNetworkInterface>
#include <QHostInfo>
#include <QAtomicInteger>
#include <QTimer>
#include <overall.h>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
namespace Ui  {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;
    bool m_startOrStopFlag;
    int m_sendDataStructNum;//发送到哪一组数据
private:
    qint32 m_localNum;//后台机组号
    QString m_IpAddressStr;//本机IP地址
    qint32 m_PortNum;//端口
    qint32 m_PowerPlantNum;//模拟发电厂个数
    FKmThread * m_DataThread;//数据收发线程
    QUdpSocket * m_BroadcastSocket;//接受广播的线程
    QTcpServer * m_TcpServer;//监听套接字
    QTcpSocket * m_TcpSocket;//数据收发套接字
    QTimer * m_SendDataTimer;

private slots:
    void sendDataTimer();
    void receiveUdpData();
    void acceptSocket();
    void disconnectSocket();
    void on_startButton_clicked();
};

#endif //  WIDGET_H


