#include "configcenter.h"
#include <QApplication>

QSettings ConfigCenter::m_settings("Config.ini", QSettings::IniFormat);

int ConfigCenter::ddsID()
{
    m_settings.beginGroup("DDS");
    int id = m_settings.value("ID", "1").toInt();
    m_settings.setValue("ID", id);
    m_settings.endGroup();
    return id;
}

QScreen *ConfigCenter::screen()
{
    m_settings.beginGroup("Screen");
    int screenNumber = m_settings.value("Number", "1").toInt();
    m_settings.setValue("Number", screenNumber);
    m_settings.endGroup();
    auto screens = qApp->screens();
    screenNumber = screenNumber <= screens.size() ? screenNumber : 0;
    screenNumber = screenNumber > 0 ? screenNumber : 1;
    return screens[screenNumber-1];
}

QRect ConfigCenter::geometry()
{
    m_settings.beginGroup("Screen");
    QRect rect = m_settings.value("Geometry", QRect(0, 0, 1920, 1080)).toRect();
    m_settings.setValue("Geometry", rect);
    m_settings.endGroup();
    return rect;
}

QString ConfigCenter::elevationPath()
{
    m_settings.beginGroup("DEM");
    QString demPath = m_settings.value("Path", "E:/SRTM_DEM_China").toString();
    m_settings.setValue("Path", demPath);
    m_settings.endGroup();
    return demPath;
}

QString ConfigCenter::mapPath(const QString &type)
{
    m_settings.beginGroup("Map");
    QString path = m_settings.value(type, "E:/Arcgis").toString();
    m_settings.setValue(type, path);
    m_settings.endGroup();
    return path;
}

QGeoCoordinate ConfigCenter::homePosition()
{
    m_settings.beginGroup("Map");
    double lat = m_settings.value("Latitude", 40.4015).toDouble();
    double lon = m_settings.value("Longitude", 99.7895).toDouble();
    double alt = m_settings.value("Altitude", 9000).toDouble();
    m_settings.setValue("Latitude", lat);
    m_settings.setValue("Longitude", lon);
    m_settings.setValue("Altitude", alt);
    m_settings.endGroup();
    return QGeoCoordinate(lat, lon, alt);
}
QPair<int, int> ConfigCenter::DTUBeginPort()
{
    m_settings.beginGroup("DTU");
    int servelPort = m_settings.value("ServerPort", 10001).toInt();
    int localPort = m_settings.value("LocalPort", 20001).toInt();
    m_settings.setValue("ServerPort", servelPort);
    m_settings.setValue("LocalPort", localPort);
    m_settings.endGroup();
    return qMakePair(servelPort, localPort);
}

QHostAddress ConfigCenter::DTUServerAddress()
{
    m_settings.beginGroup("DTU");
    QString addr = m_settings.value("ServerAddr", QString("192.168.0.233")).toString();
    m_settings.setValue("ServerAddr", addr);
    m_settings.endGroup();
    return QHostAddress(addr);
}

bool ConfigCenter::saveDebug()
{
    m_settings.beginGroup("Debug");
    bool save = m_settings.value("Sava", false).toBool();
    m_settings.setValue("Save", save);
    m_settings.endGroup();
    return save;
}

bool ConfigCenter::isFlySet()
{
    m_settings.beginGroup("Station");
    bool ret = m_settings.value("IsFlySeat", true).toBool();
    m_settings.setValue("IsFlySeat", ret);
    m_settings.endGroup();
    return ret;
}

int ConfigCenter::seatId()
{
	bool ret = m_settings.value("SeatId", 1).toInt();
	m_settings.setValue("SeatId", ret);
	return ret;
}

QVariant ConfigCenter::config(const QString &name)
{
    m_settings.beginGroup("Config");
    auto config = m_settings.value(name);
    m_settings.setValue(name, config);
    m_settings.endGroup();
    return config;
}

ConfigCenter::ConfigCenter(QObject *parent) : QObject(parent)
{
}
