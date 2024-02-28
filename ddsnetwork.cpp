#include "ddsnetwork.h"


DDSNetwork::DDSNetwork(QObject *parent) : QObject(parent),
    m_isStarted(false)
{
}

bool DDSNetwork::start(dds::Long domainID)
{
    m_domainId = domainID;
    if(m_isStarted)
        return false;
    initNetwork();
    m_isStarted = true;
    return  true;
}

void DDSNetwork::quit()
{
    if(!m_isStarted)
        return;
    m_pDP->delete_contained_entities();
    m_isStarted = false;
    m_pDP = nullptr;
}

void DDSNetwork::registerRead(const char *topic)
{
    if(!m_pDP)
        return;
    if(!m_readers.contains(topic)) {
        dds::Topic_ptr ptopic = m_pDP->create_topic(topic, "", dds::TopicQos(), nullptr, 0);
        auto reader = m_pSub->create_datareader(ptopic, dds::DataReaderQos(), this, 0); //创建 监听
        m_readers.insert(topic, reader);
    }
}

void DDSNetwork::unregisterRead(const char *topic)
{
    auto reader = m_readers.take(topic);
    if(reader) {
        m_pSub->delete_datareader(reader);
    }
}

void DDSNetwork::write(const char *topic, char *data, unsigned int size)
{
    m_pPub->pub_data(topic, data, size);
}

void DDSNetwork::write(const QString &topicName, const QByteArray &data)
{
    m_pPub->pub_data(topicName.toStdString().c_str(), data.data(), data.size());
}

bool DDSNetwork::hasPendingDatagrams() const
{
    return !m_dataQueue.isEmpty();
}

DDSDatagram DDSNetwork::receiveDatagram()
{
    QMutexLocker locker(&m_mutex);
    return m_dataQueue.dequeue();
}

DDSNetwork::~DDSNetwork()
{
    quit();
}

void DDSNetwork::on_data_available(dds::Long domainId, const std::string &topicName, char *buf, int len)
{
    //收到数据全部放在队列里面，发送信号，让Qt线程去处理数据
    DDSDatagram datagram;
    datagram.setDomainID(domainId);
    datagram.setTopic(QString::fromStdString(topicName));
    datagram.setData(QByteArray(buf, len));

    QMutexLocker locker(&m_mutex);
    m_dataQueue.enqueue(datagram);
    if(m_dataQueue.size() == 1)
        emit readyRead();
}

void DDSNetwork::initNetwork()
{
    dds::StatusMask mask = 0;
    dds::DomainParticipantQos qos;

    m_pDP = dds::DomainParticipantFactory::get_instance()->create_participant(m_domainId, qos, nullptr, mask);
    m_pPub = m_pDP->create_publisher(dds::PublisherQos(), nullptr, 0); //注册发布者
    m_pSub = m_pDP->create_subscriber(dds::SubscriberQos(), nullptr, 0); //注册订阅者
}

DDSDatagram::DDSDatagram():
    m_isValid(false)
{
}

int DDSDatagram::domainID() const
{
    return m_domainID;
}

void DDSDatagram::setDomainID(int domainID)
{
    m_domainID = domainID;
}

const QString &DDSDatagram::topic() const
{
    return m_topic;
}

void DDSDatagram::setTopic(const QString &topic)
{
    m_topic = topic;
}

const QByteArray &DDSDatagram::data() const
{
    return m_data;
}

void DDSDatagram::setData(const QByteArray &payload)
{
    m_data = payload;
    m_isValid = true;
}

bool DDSDatagram::isValid()
{
    return m_isValid;
}
