#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include <QObject>

namespace VFDT
{
  class NetworkController : public QObject
  {
    Q_OBJECT
    Q_PROPERTY(QString hostName READ hostName WRITE setHostName NOTIFY hostNameChanged)
    Q_PROPERTY(quint16 hostPort READ hostPort WRITE setHostPort NOTIFY hostPortChanged)
    Q_PROPERTY(QString connectionStatus READ connectionStatus WRITE setConnectionStatus NOTIFY connectionStatusChanged)

    QString m_hostName;
    quint16 m_hostPort=0;
    QString m_connectionStatus;

  public:
    explicit NetworkController(QObject *t_parent = nullptr);

    Q_INVOKABLE bool startConnection();

    QString hostName() const;
    quint16 hostPort() const;
    QString connectionStatus() const;

  signals:
    void hostNameChanged(QString t_hostName);
    void hostPortChanged(quint16 t_hostPort);
    void connectionStatusChanged(QString t_connectionStatus);
    void sigStartConnection(const QString &t_ip, quint16 t_port);

  public slots:
    void setHostName(QString t_hostName);
    void setHostPort(quint16 t_hostPort);
    void setConnectionStatus(QString t_connectionStatus);
  };

} // namespace VFDT

#endif // NETWORKCONTROLLER_H
