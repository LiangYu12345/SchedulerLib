#include "udppacketutility.h"
#include <QtMath>
#include<iostream>
#include<algorithm>
#include <QDebug>

PacketEncoderUdp::PacketEncoderUdp():
    m_unitFixLen(sizeof(BasicPktHeader)+sizeof(BasicPktTail)),
    m_nMsgCricle(0)
{
    setReserveSize(1200);
    m_header = BasicPktHeader();
    m_nMsgCricle = qrand();
}

void PacketEncoderUdp::setReserveSize(int len)
{
    m_unitMaxLen = qBound(m_unitFixLen, len, 1200);
    m_buf.reserve(m_unitMaxLen+m_unitFixLen);
}

void PacketEncoderUdp::setMsgId(int id)
{
    m_header.msgId = id;
}

void PacketEncoderUdp::setSimTime(quint64 time)
{
    m_header.simTime = time;
}

void PacketEncoderUdp::setUTCTime(quint64 time)
{
    m_header.utcTime = time;
}

QVector<QByteArray> PacketEncoderUdp::appendPaylod(const char *buf,int len)
{
    QVector<QByteArray> vecBuf;
    int arrCount = qCeil(len*1.0 / m_unitMaxLen);
    int seq = 1;
    int curLen = len;
    m_nMsgCricle ++;
    m_header.arrAll = arrCount;
    m_header.msgCircle = m_nMsgCricle;
    while(seq <= arrCount)
    {
        if(seq < arrCount)
        {
            m_header.arrSeq = seq;
            m_header.length = m_unitMaxLen;
            m_buf.append((char*)&m_header,sizeof(m_header));
            m_buf.append(buf+m_unitMaxLen*(seq-1),m_unitMaxLen);
            m_buf.append(UDPCRC8(m_buf.data(),m_buf.length()));
            vecBuf.append(m_buf);
            curLen -= m_unitMaxLen;
        }
        else
        {
            m_header.arrSeq = seq;
            m_header.length = curLen;
            m_buf.append((char*)&m_header,sizeof(m_header));
            m_buf.append(buf+m_unitMaxLen*(seq-1),curLen);
            m_buf.append(UDPCRC8(m_buf.data(),m_buf.length()));
            vecBuf.append(m_buf);
        }
        ++seq;
        m_buf.clear();
    }
    return vecBuf;
}

PacketDecoderUdp::PacketDecoderUdp()
{

}

quint64 PacketDecoderUdp::simTime()
{
    return m_simTime;
}

quint64 PacketDecoderUdp::utcTime()
{
    return m_utcTime;
}

#if 0
void PacketDecoderUdp::onAppendPacket(QByteArray buf)
{
    m_byteArray.append(buf);
    _onParseData();
}
#else
void PacketDecoderUdp::onAppendPacket(QByteArray buf)
{
    char * strBuffer = buf.data();
    int nStrLen = buf.size();
    int nStrDataLen = nStrLen - 1;
    int nDataCheckLen = nStrLen - sizeof(BasicPktHeader) - sizeof(BasicPktTail);
    BasicPktHeader * pHeader = (BasicPktHeader *)strBuffer;
    if((pHeader->fixValue1 == 0xAA) && (pHeader->fixValue2 == 0x55) &&
            (pHeader->fixValue3 == 0xFF) && (pHeader->fixValue4 == 0x32))
    {
        if(pHeader->length != nDataCheckLen){
            qDebug()<<"Msg Length Error!";
            return;
        }

        char crc = UDPCRC8(strBuffer, nStrDataLen);
        if(crc != strBuffer[nStrDataLen]){
            qDebug()<<"Msg Crc Error!";
            return;
        }
    }
    else{
        qDebug()<<"Msg Header Error!";
        return;
    }

    m_utcTime = pHeader->utcTime;
    m_simTime = pHeader->simTime;
    int msgId = pHeader->msgId;
    int arrAll = pHeader->arrAll;
    int arrSeq = pHeader->arrSeq;
    int payLoadLen = pHeader->length;
    int msgCicle = pHeader->msgCircle;

    if(1 == arrAll){
        QByteArray packet;
        packet.append(strBuffer + sizeof(BasicPktHeader), payLoadLen);
        emit sigDistributePacket(msgId, packet);
    }
    else{
        auto & msgBuffer = m_mapMsgs[msgId];
        if(msgCicle != msgBuffer.msgCicle){
            msgBuffer.vecBufs.clear();
            msgBuffer.msgCicle = msgCicle;
        }

        if(arrAll >= arrSeq){
            unitBuf uint;
            uint.seq = arrSeq;
            uint.buf.append(strBuffer + sizeof(BasicPktHeader), payLoadLen);

            msgBuffer.arrCount = arrAll;
            msgBuffer.vecBufs.append(uint);
            msgBuffer.size = msgBuffer.vecBufs.size();
        }

        if(msgBuffer.size == msgBuffer.arrCount){
            std::sort(msgBuffer.vecBufs.begin(), msgBuffer.vecBufs.end(), [&](unitBuf a, unitBuf b){
                return a.seq < b.seq;
            });

            QByteArray packet;

            foreach(unitBuf uint, msgBuffer.vecBufs){
                packet.append(uint.buf, uint.buf.length());
            }

            emit sigDistributePacket(msgId, packet);
        }
    }
}
#endif

void PacketDecoderUdp::onStartDecode()
{
    BasicPktHeader header = BasicPktHeader();
    m_headerCode.append(header.fixValue1);
    m_headerCode.append(header.fixValue2);
    m_headerCode.append(header.fixValue3);
    m_headerCode.append(header.fixValue4);
    m_utcTime = 0;
    m_simTime = 0;
    m_unitFixLen = sizeof(BasicPktHeader)+sizeof(BasicPktTail);
//    m_pTimer = new QTimer(this);
//    connect(m_pTimer,&QTimer::timeout,this,&PacketDecoderUdp::_onParseData);
//    m_pTimer->start();
}

void PacketDecoderUdp::_onParseData()
{
    int nCurHeaderIndex = m_byteArray.indexOf(m_headerCode);
    int nNextHeaderIndex = -1;
    while (-1 != nCurHeaderIndex)
    {
        if (!m_msgData.isEmpty() && 0 != nCurHeaderIndex)
        {
            m_msgData.append(m_byteArray.mid(0, nCurHeaderIndex));
            //去掉第一个包头前面的数据
            m_byteArray.remove(0, nCurHeaderIndex);
            if (m_msgData.size() >= m_unitFixLen)
            {
                int len = 0;
                memcpy(&len,m_msgData.data()+32,sizeof (int));
                if (len == m_msgData.length() - m_unitFixLen)
                    _parseData();
            }
            nCurHeaderIndex = m_byteArray.indexOf(m_headerCode);
            continue;
        }
        else if (m_msgData.isEmpty() && 0 != nCurHeaderIndex)
        {
            m_msgData.append(m_byteArray.mid(0, nCurHeaderIndex));
            //去掉第一个包头前面的数据
            m_byteArray.remove(0, nCurHeaderIndex);
            if (m_msgData.size() >= m_unitFixLen)
            {
                int len = 0;
                memcpy(&len,m_msgData.data()+32,sizeof (int));
                if (len == m_msgData.length() - m_unitFixLen)
                    _parseData();
            }
            nCurHeaderIndex = m_byteArray.indexOf(m_headerCode);
            continue;
        }
        else if (!m_msgData.isEmpty() && 0 == nCurHeaderIndex)
        {
            m_msgData.clear();
        }

        nNextHeaderIndex = m_byteArray.indexOf(m_headerCode, nCurHeaderIndex + m_headerCode.size());
        if (-1 != nNextHeaderIndex)
        {
            m_msgData.clear();
            m_msgData.append(m_byteArray.mid(nCurHeaderIndex,
                                             nNextHeaderIndex - nCurHeaderIndex));
            m_byteArray.remove(0, nNextHeaderIndex);
            if (m_msgData.size() >= m_unitFixLen)
            {
                int len = 0;
                memcpy(&len,m_msgData.data()+32,sizeof (int));
                if (len == m_msgData.length() - m_unitFixLen)
                    _parseData();
            }
        }
        else
        {
            m_msgData.clear();
            m_msgData.append(m_byteArray);
            m_byteArray.remove(0,m_msgData.size());

            if (m_msgData.size() >= m_unitFixLen)
            {
                int len = 0;
                memcpy(&len,m_msgData.data()+32,sizeof (int));
                if (len == m_msgData.length() - m_unitFixLen)
                    _parseData();
            }
        }

        nCurHeaderIndex = m_byteArray.indexOf(m_headerCode);
    }
}

void PacketDecoderUdp::_parseData()
{
    int len = m_msgData.size();
    if (UDPCRC8(m_msgData.data(),len) == m_msgData[len-1])
    {
        m_msgData.clear();
        return;
    }

    BasicPktHeader header;
    memcpy(&header, m_msgData.data(), sizeof(BasicPktHeader));
    m_utcTime = header.utcTime;
    m_simTime = header.simTime;
    int msgId = header.msgId;
    int arrAll = header.arrAll;
    int arrSeq = header.arrSeq;
    int payLoadLen = header.length;

    QByteArray payLoad;
    payLoad.append(m_msgData.data() + sizeof(BasicPktHeader),payLoadLen);

    if(arrAll >= arrSeq)
    {
        unitBuf unit;
        unit.seq = arrSeq;
        unit.buf = payLoad;
        m_mapMsgs[msgId].arrCount = arrAll;
        m_mapMsgs[msgId].vecBufs.append(unit);
        m_mapMsgs[msgId].size = m_mapMsgs[msgId].vecBufs.size();
    }

    if(m_mapMsgs[msgId].size == m_mapMsgs[msgId].arrCount)
    {
        std::sort(m_mapMsgs[msgId].vecBufs.begin(),m_mapMsgs[msgId].vecBufs.end(), [&](unitBuf a,unitBuf b)
        {
            return a.seq<b.seq;
        });
        QByteArray packet;
        foreach(unitBuf unit,m_mapMsgs[msgId].vecBufs)
        {
            packet.append(unit.buf,unit.buf.length());
        }
        m_mapMsgs.remove(msgId);
        emit sigDistributePacket(msgId,packet);
    }
    m_msgData.clear();
}
