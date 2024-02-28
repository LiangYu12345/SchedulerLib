#include "packetutility.h"

PacketEncoder::PacketEncoder()
{
    m_bufLen = 64;
    m_buf = static_cast<char*>(malloc(m_bufLen));
    // Header = appID[1] + SimTime[4] + DataLen[4]
    *reinterpret_cast<unsigned int*>(m_buf) = 9;
    m_dataLen = 4 + 9;  //Header is 9 byte
}

char *PacketEncoder::data()
{
    return m_buf;
}

unsigned int PacketEncoder::size()
{
    return m_dataLen;
}

PacketEncoder::~PacketEncoder()
{
    free(m_buf);
}

void PacketEncoder::setAppID(int id)
{
    // 4 + Header
    // Header = appID[1] + SimTime[4] + DataLen[4]
    *reinterpret_cast<unsigned char*>(m_buf + 4) = static_cast<unsigned char>(id);
}


PacketDecoder::PacketDecoder(const char *buf)
{
    m_data = buf;
    m_index = m_data;

    unsigned int headerLen = *reinterpret_cast<const unsigned int*>(m_data);                  //the first payload is Z_ZRHeader
    m_index += sizeof(int) + static_cast<unsigned int>(headerLen);    //move to the second payload
}

int PacketDecoder::appID()
{
    // 4 + Header
    // Header = appID[1] + SimTime[4] + DataLen[4]
    return  *reinterpret_cast<const unsigned char*>(m_data + 4);
}
