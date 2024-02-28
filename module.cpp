#include "module.h"

Module::Module(QObject *parent) : QObject(parent),
    m_core(nullptr),
    m_udp(new QUdpSocket(this))
{

}

bool Module::registerUDP(int identity)
{
    return core()->registerUDP(this, identity);
}

void Module::sendBroadcast(const char *data, qint64 size, quint16 port)
{
    m_udp->writeDatagram(data, size, QHostAddress::Broadcast, port);
}

void Module::sendDatagram(const char *data, qint64 size, const QHostAddress &address, quint16 port)
{
    m_udp->writeDatagram(data, size, address, port);
}
