#ifndef FKMTHREAD_H
#define FKMTHREAD_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QUdpSocket>
#include <overall.h>
class FKmThread : public QThread
{
    Q_OBJECT
public:
    explicit FKmThread(QThread *parent = 0);
public:
    void quit();
    void setThreadAttribute(QString ipAddress,int generalNum,int localNum);
private:
    bool m_beginThreadFlag;
    bool m_runFlag;
    qint32 m_localNum;
    qint32 m_generalNum;
    QString m_goalIPaddress;
    qint32 m_goalPort;
    qint32 m_connectPort;
    qint32 m_localPort;
    QUdpSocket * m_dataSocket;
    QByteArray sendMessageArray;
    char m_recvBuf[1256];
private:
    void setGoal(int receive);
    void mathTimelyData();
signals:

public slots:
    void run();
    void sendData(char * data,int size,QString ipaddress,quint32 port);
    void receiveData();
};

#endif // FKMTHREAD_H
