#ifndef MODULE_H
#define MODULE_H
#include "scheduler_global.h"

#include "core.h"
#include <QObject>
#include <QUdpSocket>
#include <QScreen>

/*!
 * \brief 模块
 * \details 作为Core类的核心组成;内部默认集成一个UDP套接字
 */
class SCHEDULERSHARED_EXPORT Module : public QObject
{
    Q_OBJECT
    friend Core;
public:
    explicit Module(QObject *parent = nullptr);

protected:
    /// 为当前模块注册UDP监听事件
    /// 参见 \see Core::registerUDP
    bool registerUDP(int identity);

    /// 获取内置UDP套接字
    inline QUdpSocket *udpSocket() {return m_udp;}
    /// 使用内置UDP发送广播数据 \see Module::udpSocket
    void sendBroadcast(const char *data, qint64 size, quint16 port);
    /// 使用内置UDP发送点对点或者广播数据 \see Module::udpSocket
    void sendDatagram(const char *data, qint64 size, const QHostAddress &address, quint16 port);;

    /// 设置显示区域
    void setGeometry(const QRect &rect, bool frameless);
    /// 全屏显示到某个屏幕
    void setScreen(const QScreen *screen);

    /// 设置背景层显示或隐藏
    void setBackgroundVisible(bool visible);
    /// 设置背景层显示或隐藏
    void setForegroundVisible(bool visible);
    /// 设置弹窗层显示或隐藏
    void setPopupVisible(bool visible);

    /// 在背景层添加一个控件, 控件的填充策略由QSizePolicy::Policy决定
    void addBackgroundWidget(QWidget *widget, Qt::Alignment align);
    /// 替换背景层的控件, 控件的填充策略由QSizePolicy::Policy决定
    void replaceBackgroundWidget(QWidget *from, QWidget *to, Qt::Alignment align);
    /// 移除背景层的控件
    void removeBackgroundWidget(QWidget *widget);
    /// 在前景层添加一个控件, 控件的填充策略由QSizePolicy::Policy决定
    void addForegroundWidget(QWidget *widget, Qt::Alignment align);
    /// 替换前景层的控件, 控件的填充策略由QSizePolicy::Policy决定
    void replaceForegroundWidget(QWidget *from, QWidget *to, Qt::Alignment align);
    /// 移除前景层的控件
    void removForegroundWidget(QWidget *widget);
    /// 设置弹窗控件(同一时刻只允许一个弹窗), 控件的填充策略由QSizePolicy::Policy决定
    void setPopupWidget(QWidget *widget, Qt::Alignment align);

protected:
    /// 设置当前模块归属的Core(子类不因该调用)
    inline void setCore(Core *core) {m_core = core;}
    /// 获取当前模块归属的Core
    inline Core *core() {return m_core;}

protected:
    /// 加载前处理
    virtual void preLoad() {};
    /// 加载处理
    virtual void load() {};
    /// 加载后处理
    virtual void postLoad() {};

    /// 卸载前处理
    virtual void preUnload() {};
    /// 卸载处理
    virtual void unload() {};
    /// 卸载后处理
    virtual void postUnload() {};

    /// UDP数据事件,仅监听的数据才会触发该事件
    virtual void UDPEvent(int identity, const QByteArray &datagram){};

    /// 更新事件
    /// \param deltaMS 上一次更新距离本次更新的毫秒差
    virtual void updateEvent(int deltaMS) {};

private:
    Core       *m_core;
    QUdpSocket *m_udp;
};

#endif // MODULE_H
