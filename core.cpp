#include "core.h"
#include "module.h"
#include "udppacketutility.h"
#include <QMutableMapIterator>
#include <QUdpSocket>
#include <QThread>

Core::Core(QObject *parent) : QObject(parent),
    m_isRun(false),
    m_udpDecoder(nullptr)
{
    connect(&m_timer, &QTimer::timeout, this, &Core::handleUpdateEvent, Qt::DirectConnection);
    qRegisterMetaType<Module*>("Module");

    if(m_udpDecoder == nullptr)
    {
        m_udpDecoder = new PacketDecoderUdp();
        m_udpDecoderThread = new QThread();
        m_udpDecoder->moveToThread(m_udpDecoderThread);
        connect(this,&Core::sigReadPacket,m_udpDecoder,&PacketDecoderUdp::onAppendPacket,Qt::QueuedConnection);
        connect(m_udpDecoder,&PacketDecoderUdp::sigDistributePacket,this,&Core::ondistributePacket,Qt::QueuedConnection);
        connect(m_udpDecoderThread,&QThread::started,m_udpDecoder,&PacketDecoderUdp::onStartDecode);
        m_udpDecoderThread->start();
    }

}

void Core::startDDS(int ddsID)
{
    connect(&m_ddsNetwork, &DDSNetwork::readyRead, this, &Core::readDDSPendingDatagrams, Qt::QueuedConnection);
    m_ddsNetwork.start(ddsID);
}

DDSNetwork *Core::getDDSNetwork()
{
    return &m_ddsNetwork;
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
    //udp->joinMulticastGroup(QHostAddress("224.2.3.1"));
    connect(udp, &QUdpSocket::readyRead, this, &Core::readUDPPendingDatagrams, Qt::DirectConnection);  //really a Qt::DirectConnection
    if(!udp->bind(QHostAddress::Any, port, QAbstractSocket::ReuseAddressHint | QAbstractSocket::ShareAddress)) {
        qWarning() << "Can't bind to UDP with port:" << port;
        delete udp;
        return false;
    }

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

void Core::regDDSCallback(Module *module, const QString &ddsTopic)
{
    if(module->objectName().isEmpty()) {
        qDebug()<<"Please setObjectName() of Callback before register topic: " << ddsTopic;
        return;
    }
    if(m_ddsModules.contains(ddsTopic, module)) {
        qDebug()<<"You don't need to register DDS twice: " << ddsTopic << "for " << module->objectName();
        return;
    }
    // if had ever been registerd, so register it to DDSNetwork
    if(!m_ddsModules.contains(ddsTopic)) {
        m_ddsNetwork.registerRead(ddsTopic.toUtf8());
    }
    m_ddsModules.insert(ddsTopic, module);
}

void Core::unregDDSCallback(Module *module, const QString &ddsTopic)
{
    m_ddsModules.remove(ddsTopic, module);
    // if empty of one topic, so register dds topic from DDSNetwork as nobody use it
    if(!m_ddsModules.contains(ddsTopic)) {
        m_ddsNetwork.unregisterRead(ddsTopic.toUtf8());
    }
}

void Core::ondistributePacket(int identity, QByteArray buf)
{
    auto callbacks = m_udpModules.values(identity);
    for(auto item : callbacks) {
        item->UDPEvent(identity, buf);
    }
}

void Core::readUDPPendingDatagrams()
{
    auto udp = dynamic_cast<QUdpSocket*>(this->sender());
    QByteArray datagram;
    while (udp->hasPendingDatagrams()) {    //retreive all pending data
        datagram.resize(udp->pendingDatagramSize());
        udp->readDatagram(datagram.data(), datagram.size());
        emit sigReadPacket(datagram);
    }
}

void Core::readDDSPendingDatagrams()
{
    while (m_ddsNetwork.hasPendingDatagrams()) {    //retreive all pending data
        DDSDatagram datagram = m_ddsNetwork.receiveDatagram();
        const QString &topic = datagram.topic();
        auto modules = m_ddsModules.values(topic);
        for(Module* item : modules) {
            item->DDSEvent(datagram);
        }
    }
}

void Core::handleUpdateEvent()
{
    for(auto module : qAsConst(m_modules)) {
        module->updateEvent(m_timer.interval());
    }
}
