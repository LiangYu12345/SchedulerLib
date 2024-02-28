#include "core.h"
#include "module.h"
#include <QMutableMapIterator>
#include <QUdpSocket>

Core::Core(QObject *parent) : QObject(parent),
    m_isRun(false)
{
    connect(&m_timer, &QTimer::timeout, this, &Core::handleUpdateEvent, Qt::DirectConnection);
    qRegisterMetaType<Module*>("Module");
}

void Core::run()
{
    if(m_isRun)
        return;;
    m_isRun = true;
    auto modules = m_modules;
    for(auto module : qAsConst(modules)) {
        module->preLoad();
    }
    for(auto module : qAsConst(modules)) {
        module->load();
    }
    for(auto module : qAsConst(modules)) {
        module->postLoad();
    }
}

void Core::stop()
{
    if(!m_isRun)
        return;
    auto modules = m_modules;
    for(auto module : qAsConst(modules)) {
        module->preUnload();
    }
    for(auto module : qAsConst(modules)) {
        module->unload();
    }
    for(auto module : qAsConst(modules)) {
        module->postUnload();
    }
    m_isRun = false;
}

void Core::setUpdateInterval(int msec)
{
    if(msec == 0)
        m_timer.stop();
    else {
        m_timer.start(msec);
    }
}

void Core::addModule(Module *module, int priority)
{
    if(m_isRun) {
        QMetaObject::invokeMethod(this, "addLoadModule", Qt::QueuedConnection, Q_ARG(Module* ,module));
    }
    else {
        m_modules.insert(priority, module);
        module->setCore(this);
        emit added(module);
    }
}

void Core::removeModule(Module *module)
{
    // unload module
    module->preUnload();
    module->unload();
    module->postUnload();

    {   // remove udp callback
        QMutableMapIterator<int, Module*> iter(m_udpModules);
        while(iter.hasNext()) {
            if(iter.next().value() == module)
                iter.remove();
        }
    }
    {   // remove module
        QMutableMapIterator<int, Module*> iter(m_modules);
        while(iter.hasNext()) {
            if(iter.next().value() == module) {
                iter.remove();
                break;
            }
        }
        module->setCore(nullptr);
    }

    emit removed(module);
}

void Core::addLoadModule(Module *module)
{
    m_modules.insert(0, module);
    module->setCore(this);
    module->preLoad();
    module->load();
    module->postLoad();
    emit added(module);
}

bool Core::bindUDP(int port)
{
    auto udp = new QUdpSocket(this);
   //really a Qt::DirectConnection
    if(!udp->bind(QHostAddress::Any, port, QAbstractSocket::ReuseAddressHint | QAbstractSocket::ShareAddress)) {
        qWarning() << "Can't bind to UDP with port:" << port;
        delete udp;
        return false;
    }
    connect(udp, &QUdpSocket::readyRead, this, &Core::readUDPPendingDatagrams, Qt::DirectConnection);
    return true;
}

bool Core::registerUDP(Module *module, int identity)
{
    if(!module)
        return false;
    if(m_udpModules.contains(identity, module)) {
        return false;
    }

    m_udpModules.insert(identity, module);
    return true;
}

void Core::readUDPPendingDatagrams()
{
    auto udp = dynamic_cast<QUdpSocket*>(this->sender());
    QByteArray datagram;
    while (udp->hasPendingDatagrams()) {    //retreive all pending data
        datagram.resize(udp->pendingDatagramSize());
        udp->readDatagram(datagram.data(), datagram.size());
        int identity= *reinterpret_cast<int*>(datagram.data());   //标识
        auto callbacks = m_udpModules.values(identity);
        for(auto item : callbacks) {
            item->UDPEvent(identity, datagram);
        }
    }
}

void Core::handleUpdateEvent()
{
    for(auto module : qAsConst(m_modules)) {
        module->updateEvent(m_timer.interval());
    }
}
