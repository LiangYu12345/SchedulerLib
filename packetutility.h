#ifndef PACKETUTILITY_H
#define PACKETUTILITY_H
#include <QObject>
#include "scheduler_global.h"

/*!
 * \brief 组包工具
 * \details 组包方式为4 + Header + 4 + struct1 + 4 + struct2 + ...
 */
class SCHEDULERSHARED_EXPORT PacketEncoder{
public:
    PacketEncoder();
    char *data();           ///<获取内存数据
    unsigned int size();    ///<数据长度
    void setAppID(int id);  ///<设置数据包来源软件标号
    template<class T>
    /*!
     * \brief 压入一个结构体
     * \param st 结构体首地址
     * \param dataLen 为满足变长数组的特性，数据长度不一定等于结构体的大小，例如下面的结构体
     * \code struct{ int id, Array arr[1024]; } 这里的arr就是变长数组
     */
    void pushStruct(const T *st, unsigned int dataLen);

    ~PacketEncoder();
private:
    char *m_buf;            ///<数据首地址
    unsigned int m_bufLen;  ///<内存长度
    unsigned int m_dataLen; ///<数据长度
};

/*!
 * \brief 解包工具
 * \details 解包方式为4 + Header + 4 + struct1 + 4 + struct2 + ...
 */
class SCHEDULERSHARED_EXPORT PacketDecoder{
public:
    PacketDecoder(const char *buf);
    int appID();            ///<数据包来源软件编号
    template<class T>
    const T *popStruct();         ///<取出一个结构体

private:
    const char *m_data;
    const char *m_index;          ///<当前指向内存位置
};

template<class T>
void PacketEncoder::pushStruct(const T *st, unsigned int dataLen)
{
    unsigned int requrestLen = dataLen + m_dataLen + 4;

    //reallocate memery
    if(requrestLen > m_bufLen) {
        char *bufBefore = m_buf;
        m_bufLen = requrestLen * 2;
        m_buf = static_cast<char*>(malloc(m_bufLen));
        memcpy(m_buf, bufBefore, m_dataLen);
        free(bufBefore);
    }

    //push struct data
    *reinterpret_cast<unsigned int*>(m_buf+m_dataLen) = dataLen;
    m_dataLen += sizeof (int);

    memcpy(m_buf+m_dataLen, st, dataLen);
    m_dataLen += dataLen;

    *reinterpret_cast<unsigned int*>(m_buf+9) = m_dataLen - 13;
}

template<class T>
const T *PacketDecoder::popStruct()
{
    // 4 + struct + ...
    unsigned int len = *reinterpret_cast<const unsigned int*>(m_index);
    m_index += sizeof(int);
    const T* st = reinterpret_cast<const T*>(m_index);
    m_index += len;

    return st;
}

#endif // PACKETUTILITY_H
