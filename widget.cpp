#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    m_sendDataStructNum = 0;
    m_localNum = 1;
    QString localHostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(localHostName);
    m_IpAddressStr = info.addresses().last().toString();
    m_PortNum = 9191;
    m_PowerPlantNum = 2;
    m_DataThread = new FKmThread[8];
    m_BroadcastSocket = new QUdpSocket();
    m_BroadcastSocket->bind(20000);
    connect(m_BroadcastSocket,SIGNAL(readyRead()),this,SLOT(receiveUdpData()));
    ui->setupUi(this);
    ui->IPAddressEdit->setText(m_IpAddressStr);
    ui->IPAddressEdit->setEnabled(false);
    ui->PortNumEdit->setText(QString::number(m_PortNum));
    ui->PowerPlantNumEdit->setText(QString::number(m_PowerPlantNum));
    ui->LocalNumEdit->setText(QString::number(m_localNum));
    m_startOrStopFlag = false;
    m_SendDataTimer = new QTimer();
    connect(m_SendDataTimer,SIGNAL(timeout()),this,SLOT(sendDataTimer()));
    m_SendDataTimer->start(80);
    m_TcpServer = new QTcpServer();
    connect(m_TcpServer,SIGNAL(newConnection()),this,SLOT(acceptSocket()));
    m_TcpSocket = NULL;
    connect(m_TcpSocket,SIGNAL(disconnected()),this,SLOT(disconnectSocket()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::sendDataTimer()
{
    //    if((OVERALL_ConnectGeneralNum <= OVERALL_timelyStruct[OVERALL_BeginStructNum].OTS_recvNum) && OVERALL_ConnectGeneralNum != 0)
    //    {
    //        //复制数据
    //        long int onePowerPlantBufSize = OVERALL_ConnectGeneralNum*46*4*sizeof(float);//一个发电厂的数据长度
    //        char * sendbuf = new char[m_PowerPlantNum*onePowerPlantBufSize];
    //        for (int i=0; i<m_PowerPlantNum; i++)
    //        {
    //            memcpy(sendbuf+i*onePowerPlantBufSize,OVERALL_timelyStruct[m_sendDataStructNum].OTS_recvData,onePowerPlantBufSize);
    //        }
    //        //发送数据
    //        if(m_TcpSocket != NULL)
    //        {
    //            m_TcpSocket->write(sendbuf,m_PowerPlantNum*onePowerPlantBufSize);
    //        }
    //        //qDebug()<<"发送数据："<<OVERALL_BeginStructNum;
    //        //当前数据包机组数置为0
    //        OVERALL_timelyStruct[OVERALL_BeginStructNum].OTS_recvNum = 0;
    //        //准备发送下一报数据
    //        OVERALL_BeginStructNum = OVERALL_BeginStructNum == 24 ? 0 : OVERALL_BeginStructNum+1;
    //    }
    char temp[2944*m_PowerPlantNum];
    memset(temp,0,2944*m_PowerPlantNum);
    if(m_TcpSocket != NULL)
    {
        m_TcpSocket->write(temp,2944*m_PowerPlantNum);
        m_TcpSocket->waitForBytesWritten(10);
    }

}

void Widget::receiveUdpData()
{
    QHostAddress srcAddress;
    char receiveBuf[24];
    m_BroadcastSocket->readDatagram(receiveBuf,24,&srcAddress);
    int ipBuf[6];
    memcpy(ipBuf,receiveBuf,24);
    if (!m_DataThread[ipBuf[0]-1].isRunning() && m_startOrStopFlag)
    {
        QString ipAddress = QString("%1.%2.%3.%4").arg(ipBuf[1]).arg(ipBuf[2]).arg(ipBuf[3]).arg(ipBuf[4]);
        m_DataThread[ipBuf[0]-1].setThreadAttribute(ipAddress,ipBuf[0],m_localNum);
        m_DataThread[ipBuf[0]-1].start();
    }
}

void Widget::on_startButton_clicked()
{
    if(m_startOrStopFlag)
    {
        for(int i = 0; i<8; i++)
        {
            if(m_DataThread[i].isRunning())
                m_DataThread[i].quit();
        }
        //TCP断开
        disconnectSocket();

        ui->startButton->setText("启动");
        ui->PowerPlantNumEdit->setEnabled(true);
        ui->LocalNumEdit->setEnabled(true);
        ui->PortNumEdit->setEnabled(true);
    }
    else
    {
        m_TcpServer->listen(QHostAddress::Any,m_PortNum);
        ui->startButton->setText("停止");
        ui->PowerPlantNumEdit->setEnabled(false);
        ui->LocalNumEdit->setEnabled(false);
        ui->PortNumEdit->setEnabled(false);
        m_PowerPlantNum = ui->PowerPlantNumEdit->text().toInt();
        m_localNum = ui->LocalNumEdit->text().toInt();
    }
    m_startOrStopFlag = !m_startOrStopFlag;
}

void Widget::acceptSocket()
{
    m_TcpSocket = m_TcpServer->nextPendingConnection();
    //获取对方IP和端口
    QString ip = m_TcpSocket->peerAddress().toString();
    int port = m_TcpSocket->peerPort();

    qDebug()<<ip<<"  "<<port;
}

void Widget::disconnectSocket()
{
    if(m_TcpSocket != NULL)
    {
        m_TcpSocket->disconnectFromHost();
        m_TcpSocket->close();
        m_TcpSocket = NULL;
        qDebug()<<"disconnect";
    }
}
