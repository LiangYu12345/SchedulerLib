#include "packetutilityv3.h"
using namespace PacketV3;

PacketEncoderFixedV2::PacketEncoderFixedV2()
{
    m_buf.reserve(128);
    m_buf.append(sizeof (BasicPktHeader), 0);
    BasicPktHeader *header = reinterpret_cast<BasicPktHeader*>(m_buf.data());
    header->fixValue1 = 0xAA;
    header->fixValue2 = 0x55;
    header->pktType = 0;
}

char *PacketEncoderFixedV2::data()
{
    return m_buf.data();
}

unsigned int PacketEncoderFixedV2::size()
{
    return m_buf.size();
}

void PacketEncoderFixedV2::setUAVID(int id)
{
    BasicPktHeader *header = reinterpret_cast<BasicPktHeader*>(m_buf.data());
    header->UAVID = id;
}

void PacketEncoderFixedV2::setUAVType(int type)
{
    BasicPktHeader *header = reinterpret_cast<BasicPktHeader*>(m_buf.data());
    header->UAVType = type;
}

void PacketEncoderFixedV2::setRedOrBlue(quint8 type)
{
    BasicPktHeader *header = reinterpret_cast<BasicPktHeader*>(m_buf.data());
    header->warOwner = type;
}

PacketDecoderFixedV2::PacketDecoderFixedV2(const char *buf)
{
    m_buf = buf;
}

int PacketDecoderFixedV2::UAVID()
{
    const BasicPktHeader *header = reinterpret_cast<const BasicPktHeader*>(m_buf);
    return header->UAVID;
}

int PacketDecoderFixedV2::UAVType()
{
    const BasicPktHeader *header = reinterpret_cast<const BasicPktHeader*>(m_buf);
    return header->UAVType;
}

quint8 PacketDecoderFixedV2::redOrBlue()
{
    const BasicPktHeader *header = reinterpret_cast<const BasicPktHeader*>(m_buf);
    return header->warOwner;
}

PacketEncoderVariableV2::PacketEncoderVariableV2()
{
    m_buf.reserve(1024);
    m_buf.append(sizeof(BasicPktHeader), 0);
    m_buf.append(sizeof(ArrayPktHeader), 0);
    BasicPktHeader *header = reinterpret_cast<BasicPktHeader*>(m_buf.data());
    header->fixValue1 = 0xAA;
    header->fixValue2 = 0x55;
    header->pktType = 1;
}

char *PacketEncoderVariableV2::data()
{
    return m_buf.data();
}

unsigned int PacketEncoderVariableV2::size()
{
    return m_buf.size();
}

void PacketEncoderVariableV2::setUAVID(int id)
{
    BasicPktHeader *header = reinterpret_cast<BasicPktHeader*>(m_buf.data());
    header->UAVID = id;
}

void PacketEncoderVariableV2::setUAVType(int type)
{
    BasicPktHeader *header = reinterpret_cast<BasicPktHeader*>(m_buf.data());
    header->UAVType = type;
}

void PacketEncoderVariableV2::setRedOrBlue(quint8 type)
{
    BasicPktHeader *header = reinterpret_cast<BasicPktHeader*>(m_buf.data());
    header->warOwner = type;
}

PacketDecoderVariableV2::PacketDecoderVariableV2(const char *buf)
{
    m_buf = buf;
}

int PacketDecoderVariableV2::UAVID()
{
    return *reinterpret_cast<const quint8*>(m_buf+5);
}

int PacketDecoderVariableV2::UAVType()
{
    return *reinterpret_cast<const quint8*>(m_buf+6);
}

quint8 PacketDecoderVariableV2::redOrBlue()
{
    return *reinterpret_cast<const quint8*>(m_buf+7);
}
