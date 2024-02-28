#include "module.h"
#include "udppacketutility.h"

Module::Module(QObject *parent) : QObject(parent),
    m_core(nullptr),
    m_udp(new QUdpSocket(this)),
    udpEncoder(new PacketEncoderUdp())
{

}

bool Module::registerUDP(int identity)
{
    return core()->registerUDP(this, identity);
}

void Module::setReserveSize(int size)
{
    udpEncoder->setReserveSize(size);
}

void Module::setMsgId(int id)
{
    udpEncoder->setMsgId(id);
}

void Module::setSimTime(long long time)
{
    udpEncoder->setSimTime(time);
}

void Module::setUTCTime(long long time)
{
    udpEncoder->setUTCTime(time);
}

void Module::sendBroadcast(const char *data, qint64 size, quint16 port)
{
    QVector<QByteArray> vecPackets = udpEncoder->appendPaylod(data,size);
    foreach(auto packet , vecPackets)
    {
        m_udp->writeDatagram(packet.data(), packet.length(), QHostAddress::Broadcast, port);
    }
}

void Module::sendDatagram(const char *data, qint64 size, const QHostAddress &address, quint16 port)
{
    m_udp->writeDatagram(data, size, address, port);
}
