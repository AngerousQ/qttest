//5555555555555555555
#include "fkmthread.h"

FKmThread::FKmThread(QThread *parent) : QThread(parent)
{
    m_dataSocket = new QUdpSocket();
    connect(m_dataSocket,SIGNAL(readyRead()),this,SLOT(receiveData()));
    m_dataSocket->setReadBufferSize(6000);
    m_beginThreadFlag = true;
}

void FKmThread::setThreadAttribute(QString ipAddress, int generalNum, int localNum)
{
    m_goalIPaddress = ipAddress;
    m_connectPort = 20001+generalNum;
    m_localPort = 30000+generalNum*100+localNum;
    m_goalPort = 10000+generalNum*100+localNum;
    m_localNum = localNum;
    m_generalNum = generalNum - 1;
    qDebug()<<m_connectPort<<m_localPort<<m_goalPort;
}

void FKmThread::run()
{
    uchar m_connectBuf[] = {0x02,0x04,0x08,uchar(m_localNum),0xaa,0xaa};
    uchar m_disconnectBuf[] = {0x01,0x03,0x07,uchar(m_localNum),0x55,0x55};
    m_runFlag = true;
    m_dataSocket->bind(m_localPort);
    sendData((char *)m_connectBuf,sizeof(m_connectBuf),m_goalIPaddress,m_connectPort);
    while(m_runFlag)
    {
        if(!m_dataSocket->waitForReadyRead(100))
        {
            qDebug()<<"receiveError";
        }

        msleep(5);
    }
    m_beginThreadFlag = true;
    OVERALL_ConnectGeneralNum -= 1;
    sendData((char *)m_disconnectBuf,sizeof(m_connectBuf),m_goalIPaddress,m_goalPort);
    m_dataSocket->close();

}

void FKmThread::quit()
{
    m_runFlag = false;
}

void FKmThread::sendData(char * data,int size,QString ipaddress,quint32 port)
{
    QByteArray sendArray = QByteArray(data,size);
    m_dataSocket->writeDatagram(sendArray,QHostAddress(ipaddress),port);

}

void FKmThread::receiveData()
{
    //qDebug()<<m_localNum<<" receiveLenth "<<m_dataSocket->bytesAvailable();
    switch (m_dataSocket->bytesAvailable())
    {
    case 1256:
        m_dataSocket->readDatagram(m_recvBuf,1256);
        mathTimelyData();
        break;
    default:
        m_dataSocket->readDatagram(m_recvBuf,56);
        break;
    }
}

void FKmThread::mathTimelyData()
{
    //计算当前发送的数据包应该存储在哪个结构体
    int mathNum = (uchar)m_recvBuf[6]*256+(uchar)m_recvBuf[7];
    if((mathNum%80)>0)
    {
        mathNum = mathNum/80+(mathNum%80)/40+12;
    }
    else
    {
        mathNum = mathNum/80;
    }
    //计算每个量的值
    short _recvValue = 0;

    for(int i=0; i<4; i++)
    {
        for(int j=0; j<46; j++)
        {
            memcpy(&_recvValue,m_recvBuf+8+312*i+12+j*2,sizeof(short));
            OVERALL_timelyStruct[mathNum].OTS_recvData[m_generalNum][46*i+j]=float(_recvValue)*OVERALL_RecvDataQValue[j]*OVERALL_MultiplyValue[j]+OVERALL_AndValue[j];
        }
    }
    if(mathNum == 20)
    {
        uchar m_hearBuf[] = {0xa1,0x21,0x01,0x11,0x11,0x01,0xbb,0xbb,0xa5,0xa5};
        sendData((char *)m_hearBuf,sizeof(m_hearBuf),m_goalIPaddress,m_connectPort);
    }
    //初始值
    if(m_beginThreadFlag)
    {
        m_beginThreadFlag = false;
        if(OVERALL_ConnectGeneralNum == 0)
        {
            OVERALL_BeginStructNum = mathNum;
        }
        OVERALL_ConnectGeneralNum += 1;
    }
    OVERALL_timelyStruct[mathNum].OTS_recvNum += 1;
    //如果在结束线程时收到数据，那么清空对应的结构体
    if(!m_runFlag)
    {
        OVERALL_timelyStruct[mathNum].OTS_recvNum = 0;
    }
    //qDebug()<<mathNum;
}
