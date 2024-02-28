#include "packetutilityv2.h"
using namespace PacketV2;

quint8 g_loopCount = 0;

PacketEncoderFixed::PacketEncoderFixed()
{
    m_buf.reserve(128);
    m_buf.append(sizeof (BasicPktHeader), 0);
    BasicPktHeader *header = reinterpret_cast<BasicPktHeader*>(m_buf.data());
    header->fixValue1 = 0xAA;
    header->fixValue2 = 0x55;
    header->pktType = 0;
}

char *PacketEncoderFixed::data()
{
    return m_buf.data();
}

unsigned int PacketEncoderFixed::size()
{
    return m_buf.size();
}

void PacketEncoderFixed::setUAVID(int id)
{
    BasicPktHeader *header = reinterpret_cast<BasicPktHeader*>(m_buf.data());
    header->UAVID = id;
}

void PacketEncoderFixed::setUAVType(int type)
{
    BasicPktHeader *header = reinterpret_cast<BasicPktHeader*>(m_buf.data());
    header->UAVType = type;
}

PacketDecoderFixed::PacketDecoderFixed(const char *buf)
{
    m_buf = buf;
}

int PacketDecoderFixed::UAVID()
{
    const BasicPktHeader *header = reinterpret_cast<const BasicPktHeader*>(m_buf);
    return header->UAVID;
}

int PacketDecoderFixed::UAVType()
{
    const BasicPktHeader *header = reinterpret_cast<const BasicPktHeader*>(m_buf);
    return header->UAVType;
}

PacketEncoderVariable::PacketEncoderVariable()
{
    m_buf.reserve(1024);
    m_buf.append(sizeof(BasicPktHeader), 0);
    m_buf.append(sizeof(ArrayPktHeader), 0);
    BasicPktHeader *header = reinterpret_cast<BasicPktHeader*>(m_buf.data());
    header->fixValue1 = 0xAA;
    header->fixValue2 = 0x55;
    header->pktType = 1;
}

char *PacketEncoderVariable::data()
{
    return m_buf.data();
}

unsigned int PacketEncoderVariable::size()
{
    return m_buf.size();
}

void PacketEncoderVariable::setUAVID(int id)
{
    BasicPktHeader *header = reinterpret_cast<BasicPktHeader*>(m_buf.data());
    header->UAVID = id;
}

void PacketEncoderVariable::setUAVType(int type)
{
    BasicPktHeader *header = reinterpret_cast<BasicPktHeader*>(m_buf.data());
    header->UAVType = type;
}

PacketDecoderVariable::PacketDecoderVariable(const char *buf)
{
    m_buf = buf;
}

int PacketDecoderVariable::UAVID()
{
    return *reinterpret_cast<const quint8*>(m_buf+5);
}

int PacketDecoderVariable::UAVType()
{
    return *reinterpret_cast<const quint8*>(m_buf+6);
}
