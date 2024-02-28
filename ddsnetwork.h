#ifndef DDSNETWORK_H
#define DDSNETWORK_H

#include "scheduler_global.h"
#include <QHash>
#include <QObject>
#include <QQueue>
#include <QMutex>
#include <dds_dcps/dcps.h>

class SCHEDULERSHARED_EXPORT DDSDatagram
{
public:
    DDSDatagram();
    int domainID() const;
    void setDomainID(int domainID);
    const QString &topic() const;
    void setTopic(const QString &topic);
    const QByteArray &data() const;
    void setData(const QByteArray &payload);
    bool isValid();
private:
    bool       m_isValid;
    int        m_domainID;
    QString    m_topic;
    QByteArray m_data;
};

///对DDS封装一层，主要用于内部软件通信；主要是将DDS线程的数据存放在队列里面，由Qt线程读取。
class SCHEDULERSHARED_EXPORT DDSNetwork : public QObject , public dds::DataReaderListener
{
    Q_OBJECT

public:
    explicit DDSNetwork(QObject *parent = nullptr);
    bool start(dds::Long domainID = 1);
    void quit();
    void registerRead(const char* topic);
    void unregisterRead(const char* topic);
    void write(const char *topic, char *data, unsigned int size);
    void write(const QString &topicName, const QByteArray& data);
    bool hasPendingDatagrams() const;
    DDSDatagram receiveDatagram();
    ~DDSNetwork();

signals:
    void readyRead();   ///<信号从DDS线程发出，务必使用QueuedConnection保证在Qt线程处理
private:
    virtual void on_data_available(dds::Long domainId, const std::string &topicName, char *buf, int len) override;
    void initNetwork();

private:
    //DDS
    bool                        m_isStarted;
    int                         m_domainId;
    dds::DomainParticipant_ptr  m_pDP;
    dds::Publisher_ptr          m_pPub;
    dds::Subscriber_ptr         m_pSub;
    QHash<QString,dds::DataReader_ptr> m_readers;
    //
    QMutex               m_mutex;       ///多个DDS线程同时收到数据时按顺序处理
    QQueue<DDSDatagram>  m_dataQueue;
};

#endif // DDSNETWORK_H
