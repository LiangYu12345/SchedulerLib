#ifndef UDPPACKETUTILITY_H
#define UDPPACKETUTILITY_H

#include "scheduler_global.h"
#include <QObject>
#include <QVector>
#include <QMap>
#include <QByteArray>
#include <QTimer>

#pragma pack(1)
/// 定长数据包、变长数据包公共包头
namespace PacketUdp {
struct BasicPktHeader
{
    quint8  fixValue1;    ///< 固定值 0xAA
    quint8  fixValue2;    ///< 固定值 0x55
    quint8  fixValue3;    ///< 固定值 0xFF
    quint8  fixValue4;    ///< 固定值 0x32
    quint64 simTime;      ///< 仿真时间戳
    quint64 utcTime;      ///< UTC时间戳
    quint32 msgId;        ///< 消息id
    quint32 msgCircle;    ///< 消息循环数,用于区分消息序列
    quint32 arrAll;       ///< 总包数
    quint32 arrSeq;       ///< 当前包所属序号
    quint32 length;       ///< 包长度 负载字段的节数总字

    BasicPktHeader()
    {
        fixValue1 = 0xAA;
        fixValue2 = 0x55;
        fixValue3 = 0xFF;
        fixValue4 = 0x32;
        simTime   = 0;
        utcTime   = 0;
        msgId     = 0;
        msgCircle = 0;
        arrAll    = 0;
        arrSeq    = 0;
        length    = 0;
    }
};

struct BasicPktTail
{
    quint8  crc;      ///< 校验位 前所有字节累加和取低八位
};

}
#pragma pack()

inline quint8 CRC8(const char *buf, int length)
{
    quint8 sum = 0;
    unsigned int nCrc = 0;
    for (int i = 0; i < length; i++) {
        nCrc += (unsigned char)buf[i];
    }
    sum = nCrc & 0xFF;
    return sum;
}

using namespace PacketUdp;

/*!
 * \brief UDP打包工具
 * \details 组包方式为BasicPktHeader + 负载数据字段 + BasicPktTail
 */
class SCHEDULERSHARED_EXPORT PacketEncoderUdp :public QObject
{
    Q_OBJECT
public:
    PacketEncoderUdp();
    void setReserveSize(int len);                         ///< 设置单包最大长度
    void setMsgId(int id);                                ///< 设置消息编号
    void setSimTime(quint64 time);                            ///< 设置仿真时间戳
    void setUTCTime(quint64 time);                            ///< 设置UTC时间戳
    QVector<QByteArray> appendPaylod(const char *buf,int len);  ///< 压入数据

private:
    QByteArray       m_buf;                  ///< 数据首地址
    int              m_unitMaxLen;           ///< 单包负载字段最大长度
    int              m_unitFixLen;           ///< 单包固定字节长度
    int              m_nMsgCricle;           ///< 消息包循环计数
    BasicPktHeader   m_header;               ///< 数据包头
};

namespace ParseCombination {
struct unitBuf
{
    int seq;        ///< 单包编号
    QByteArray buf; ///< 单包数据
};

struct msgBufs
{
    int size;                 ///< 当前大小
    int arrCount;             ///< 网络包中记录的总包大小
    int msgCicle;             ///< 消息循环计数
    QVector<unitBuf> vecBufs; ///< 数据包集合

    msgBufs(){
        size = 0;
        arrCount = 0;
        msgCicle = 0;
    }
};
};
using namespace  ParseCombination;
/*!
 * \brief  UDP解析工具
 * \details 解包方式为BasicPktHeader + 负载数据字段 + BasicPktTail
 */
class SCHEDULERSHARED_EXPORT PacketDecoderUdp :public QObject
{
    Q_OBJECT
public:
    PacketDecoderUdp();
    quint64 simTime();           ///<获取仿真时间戳
    quint64 utcTime();           ///<获取UTC时间戳

signals:
    /// 解析完成的数据分发信号
    void sigDistributePacket(int,QByteArray);
public slots:
    /// 添加数据包槽函数
    void onAppendPacket(QByteArray buf);
    /// 线程启动槽函数
    void onStartDecode();

    /// 解析数据槽函数
    void _onParseData();

    void _parseData();
private:
    QByteArray m_headerCode;                ///< 协议包头
    QByteArray m_byteArray;                 ///< 待解析的数据
    QByteArray m_msgData;                   ///< 已解析的数据
    quint64        m_utcTime;                   ///< UTC时间戳
    quint64        m_simTime;                   ///< 仿真时间戳
    QTimer    *m_pTimer;                    ///< 解析数据定时器
    int        m_unitFixLen;                ///< 单包固定字节长度
    QMap<int,msgBufs>  m_mapMsgs;           ///< 解析完成的数据

};
#endif // UDPPACKETUTILITY_H
