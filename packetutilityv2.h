#ifndef PACKETUTILITYV2_H
#define PACKETUTILITYV2_H

#include "scheduler_global.h"
#include <QObject>
extern quint8 g_loopCount;

#pragma pack(1)
/// 定长数据包、变长数据包公共包头
namespace PacketV2 {
struct BasicPktHeader
{
    quint8  fixValue1;    //固定值 0xAA
    quint8  fixValue2;    //固定值 0x55
    quint16 pktLen;       //包长度 本数据帧的节数总字
    quint8  loopCount;    //循环计数器
    quint8  UAVID;        //无人机ID
    quint8  UAVType;      //无人机类型
    quint8  pktType;      //数据包类型 0-定长包 1-变长包
};

/// 变长数据包包头
struct ArrayPktHeader
{
    quint8 publicLen;                //公共数据块字节数
    quint8 oneArrayLen;              //单个周期数据块字节数
    quint8 arrayCountAll;            //周期数据块总数
    quint8 arrayCountInPkt;          //当次周期数据块个数
    quint8 startArrayIndexInPkt;     //当次起始周期数据块序号
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

/*!
 * \brief 定长组包工具 移植张松ZrDDSNetwork的算法
 * \details 组包方式为Header + FixedStruct + Tail
 */
class SCHEDULERSHARED_EXPORT PacketEncoderFixed{
public:
    PacketEncoderFixed();
    char *data();           ///<获取内存数据
    unsigned int size();    ///<数据长度
    void setUAVID(int id);  ///<设置无人机ID
    void setUAVType(int type); ///<设置无人机类型
    template<class T>
    void setFixed(T *st); ///<压入一个结构体

private:
    QByteArray m_buf;            ///<数据首地址
    int m_id;
    int m_type;
};

/*!
 * \brief 定长解包工具 移植张松ZrDDSNetwork的算法
 * \details 解包方式为Header + FixedStruct + Tail
 */
class SCHEDULERSHARED_EXPORT PacketDecoderFixed{
public:
    PacketDecoderFixed(const char* buf);
    int UAVID();         ///<获取无人机ID
    int UAVType();       ///<获取无人机类型
    template<class T>
    const T *getFixed(); ///<取出一个结构体

private:
    const char *m_buf;            ///<数据首地址
};

/*!
 * \brief 变长组包工具 移植张松ZrDDSNetwork的算法
 * \details 组包方式为Header + ArrayHeader + FixedStruct + StructArray[N] + Tail
 * \warning 如果不是使用变长数据，请不要使用该类，该类认定@setVariable为开始组包标志
 */
class SCHEDULERSHARED_EXPORT PacketEncoderVariable{
public:
    PacketEncoderVariable();
    char *data();           ///<获取内存数据
    unsigned int size();    ///<数据长度
    void setUAVID(int id);  ///<设置无人机ID
    void setUAVType(int type); ///<设置无人机类型
    template<class T>
    void setFixed(T *st);    ///<压入一个定长结构体
    template<class T>
    void setVariable(T *st, int count); ///<压入一个结构体数组

private:
    QByteArray m_buf;            ///<数据首地址
    int m_id;
    int m_type;
};

/*!
 * \brief 变长解包工具 移植张松ZrDDSNetwork的算法
 * \details 解包方式为Header + ArrayHeader + FixedStruct + StructArray[N] + Tail
 */
class SCHEDULERSHARED_EXPORT PacketDecoderVariable{
public:
    PacketDecoderVariable(const char* buf);
    int UAVID();         ///<获取无人机ID
    int UAVType();       ///<获取无人机类型
    template<class T>
    const T *getFixed();    ///<取出一个结构体
    template<class T>
    const QPair<int, const T*> getVariable(); ///<取出一个结构体数组
private:
    const char *m_buf;            ///<数据首地址
};


template<class T>
void PacketEncoderFixed::setFixed(T *st)
{
    // T
    m_buf.append((char*)st, sizeof(T));
    // Tail
    m_buf.append(CRC8(m_buf.data(), m_buf.size() - 1));
    // Header
    PacketV2::BasicPktHeader *header = reinterpret_cast<PacketV2::BasicPktHeader*>(m_buf.data());
    header->pktLen = m_buf.size();
}

template<class T>
const T *PacketDecoderFixed::getFixed()
{
    return reinterpret_cast<const T*>(m_buf + sizeof (PacketV2::BasicPktHeader));
}

template<class T>
void PacketEncoderVariable::setFixed(T *st)
{
    PacketV2::ArrayPktHeader *header = reinterpret_cast<PacketV2::ArrayPktHeader*>(m_buf.data()+sizeof (PacketV2::BasicPktHeader));
    header->publicLen = sizeof(T);
    // T
    m_buf.append((char*)st, sizeof (T));
}

template<class T>
void PacketEncoderVariable::setVariable(T *st, int count)
{
    // T[]
    m_buf.append((char*)st, count * sizeof (T));
    //ArrayHeader
    PacketV2::ArrayPktHeader *header = reinterpret_cast<PacketV2::ArrayPktHeader*>(m_buf.data()+sizeof (PacketV2::BasicPktHeader));
    header->oneArrayLen = quint8(sizeof (T));
    header->arrayCountAll = count;
    header->arrayCountInPkt = count;
    header->startArrayIndexInPkt = 0;
    //Tail
    m_buf.append(CRC8(m_buf.data(), m_buf.size() - 1));
    // Header
    PacketV2::BasicPktHeader *bHeader = reinterpret_cast<PacketV2::BasicPktHeader*>(m_buf.data());
    bHeader->pktLen = m_buf.size();
}

template<class T>
const T *PacketDecoderVariable::getFixed()
{
    if(*reinterpret_cast<const quint8*>(m_buf + sizeof (PacketV2::BasicPktHeader)) == 0)
        return nullptr;
    else
        return reinterpret_cast<const T*>(m_buf + sizeof (PacketV2::BasicPktHeader) + sizeof(PacketV2::ArrayPktHeader));
}

template<class T>
const QPair<int, const T *> PacketDecoderVariable::getVariable()
{
    const PacketV2::ArrayPktHeader *header = reinterpret_cast<const PacketV2::ArrayPktHeader*>(m_buf + sizeof (PacketV2::BasicPktHeader));
    int publicLen = header->publicLen;

    QPair<int, const T*> ret;
    ret.first = header->arrayCountAll;
    ret.second = reinterpret_cast<const T*>(m_buf + sizeof (PacketV2::BasicPktHeader) + sizeof (PacketV2::ArrayPktHeader) + publicLen);
    return ret;
}
#endif // PACKETUTILITYV2_H
