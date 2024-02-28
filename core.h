#ifndef CORE_H
#define CORE_H
#include "scheduler_global.h"

#include <QObject>
#include <QMultiMap>
#include <QTimer>
#include "ddsnetwork.h"

class Module;
class PacketDecoderUdp;
class QThread;

/*!
 * \brief 内核
 * \details 该类可以同时监听多个端口的UDP数据，解析UDP数据的第一个int位，
 * 作为标志位调用需要该数据的模块UDP处理事件
 */
class SCHEDULERSHARED_EXPORT Core : public QObject
{
    Q_OBJECT
public:
    explicit Core(QObject *parent = nullptr);

    void startDDS(int ddsID);

    DDSNetwork *getDDSNetwork();

    /// 开始运行,模块执行加载操作
    void run();
    /// 结束运行,模块执行卸载操作
    void stop();
    /// 设置更新事件间隔(0代表关闭更新事件)
    void setUpdateInterval(int msec);

    /// 添加模块，如果正在执行run或者已经执行了run，那么将会添加并加载模块 \see Core::addLoadModule
    /// \param priority代表当前模块加载优先顺序,值越小越先加载
    void addModule(Module *module, int priority = 0);
    /// 卸载模块
    void removeModule(Module *module);
    /// 添加模块，并立即加载
    Q_INVOKABLE void addLoadModule(Module *module);
    /// 查询模块
    template<class T>
    T *findModule();

    /// 添加UDP监听端口,可同时监听多个端口
    bool bindUDP(int port);
    /// 给模块注册UDP监听事件
    bool registerUDP(Module *module, int identity);

    void regDDSCallback(Module *module, const QString &ddsTopic);
    void unregDDSCallback(Module *module, const QString &ddsTopic);
private slots:
    /// 分发数据槽函数
    void ondistributePacket(int identity,QByteArray buf);
signals:
    /// 模块添加信号
    void added(Module *module);
    /// 模块卸载信号
    void removed(Module *module);

    /// UDP数据信号
    void sigReadPacket(QByteArray);

private:
    void readUDPPendingDatagrams();

    void readDDSPendingDatagrams();

    void handleUpdateEvent();

private:
    //
    bool   m_isRun;
    QTimer m_timer;

    //
    QMultiMap<int, Module*> m_modules;
    QMultiMap<int, Module*> m_udpModules;

    PacketDecoderUdp      *m_udpDecoder;       ///< udp解包器
    QThread               *m_udpDecoderThread; ///< udp解包线程

    QMultiHash<QString, Module*>  m_ddsModules;  ///<用来处理DDS数据的回调<topic,Module>
    DDSNetwork         m_ddsNetwork;
};

template<class T>
T *Core::findModule()
{
    for(auto item : m_modules) {
        auto module = qobject_cast<T*>(item);
        if(module)
            return module;
    }
    return nullptr;
}

#endif // CORE_H
