#include "vfdt_networkcontroller.h"

#include <QCoreApplication>
#include <QQmlEngine>
#include <QDebug>

namespace VFDT
{
  NetworkController::NetworkController(QObject *t_parent) : QObject(t_parent)
  {
    setConnectionStatus("Not connected");
  }

  bool NetworkController::startConnection()
  {
    bool retVal=false;
    if(m_hostName.isEmpty())
    {
      setConnectionStatus("Invalid hostname");
    }
    else if(m_hostPort==0)
    {
      setConnectionStatus("Invalid port");
    }
    else
    {
      setConnectionStatus("Connecting...");
      emit sigStartConnection(m_hostName, m_hostPort);
      retVal = true;
    }
    return retVal;
  }

  QString NetworkController::hostName() const
  {
    return m_hostName;
  }

  quint16 NetworkController::hostPort() const
  {
    return m_hostPort;
  }

  QString NetworkController::connectionStatus() const
  {
    return m_connectionStatus;
  }

  void NetworkController::setHostName(QString t_hostName)
  {
    if (m_hostName == t_hostName)
      return;

    m_hostName = t_hostName;
    emit hostNameChanged(m_hostName);
  }

  void NetworkController::setHostPort(quint16 t_hostPort)
  {
    if (m_hostPort == t_hostPort)
      return;

    m_hostPort = t_hostPort;
    emit hostPortChanged(m_hostPort);
  }

  void NetworkController::setConnectionStatus(QString t_connectionStatus)
  {
    if (m_connectionStatus == t_connectionStatus)
      return;

    m_connectionStatus = t_connectionStatus;
    emit connectionStatusChanged(m_connectionStatus);
  }

  void registerNetworkController()
  {
    qmlRegisterUncreatableType<NetworkController>("VFDT", 1, 0, "NetworkController", "C++ QmlContext QObject");
  }
  Q_COREAPP_STARTUP_FUNCTION(registerNetworkController)
} // namespace VFDT
