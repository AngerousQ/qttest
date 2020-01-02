//test 3
#ifndef OVERALL_H
#define OVERALL_H

#define TCPLISTENPORT 9191

#define QFOUR 0.0625
#define QSIX 0.015625
#define QSEVEN 0.0078125
#define QNINE 0.001953125
#define QTEN 0.0009765625


#include <QAtomicInteger>






struct OVERALL_TIMELYSTRUCT
{
    QAtomicInt OTS_recvNum = 0;//当前结构体收到了几个机组
    float OTS_recvData[8][46*4];//存放数据
};
extern OVERALL_TIMELYSTRUCT OVERALL_timelyStruct[25];
extern QAtomicInt OVERALL_ConnectGeneralNum;//已经连接了几个机组
extern int OVERALL_BeginStructNum;
extern float OVERALL_RecvDataQValue[46];
extern float OVERALL_MultiplyValue[46];
extern float OVERALL_AndValue[46];
#endif // OVERALL_H
