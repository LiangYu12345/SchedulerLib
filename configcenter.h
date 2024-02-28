#ifndef CONFIGCENTER_H
#define CONFIGCENTER_H

#include "scheduler_global.h"
#include <QObject>
#include <QSettings>
#include <QHostAddress>
#include <QGeoCoordinate>
#include <QScreen>

/*!
 * \brief 配置管理
 * \details 1.提供常见配置接口 2.提供config()实现自定义接口作为补充
 */
class SCHEDULERSHARED_EXPORT ConfigCenter : public QObject
{
    Q_OBJECT
public:
    /// DDS ID
    static int ddsID();
    /// 显示屏幕
    static QScreen *screen();
    /// 显示位置及大小
    static QRect geometry();
    /// 高程数据路径
    static QString elevationPath();
    /// 地图资源路径
    static QString mapPath(const QString &type = "arcgis");
    /// 地图起始位置
    static QGeoCoordinate homePosition();
    /// DTU开始端口
    static QPair<int,int> DTUBeginPort();
    /// DTU服务器地址
    static QHostAddress DTUServerAddress();
    /// 是否保存调试数据
    static bool saveDebug();
    /// 当前是否为飞行席位
    static bool isFlySet();
	/// 席位id
	static int seatId();
    /// 自定义配置
    static QVariant config(const QString &name);
private:
    explicit ConfigCenter(QObject *parent = nullptr);

private:
    static QSettings      m_settings;
};

#endif // CONFIGCENTER_H
