#ifndef VFDT_TESTCONTROLLER_H
#define VFDT_TESTCONTROLLER_H

#include <QObject>
#include <QVariantMap>

namespace VFDT
{
  class TestResult;
  class TestRunner;
  class TestController : public QObject
  {
    Q_OBJECT
    Q_PROPERTY(double epsilonValue READ epsilonValue WRITE setEpsilonValue NOTIFY epsilonValueChanged)
    Q_PROPERTY(QString refDBFilePath READ refDBFilePath WRITE setRefDBFilePath NOTIFY refDBFilePathChanged)
    Q_PROPERTY(QString dutDBFilePath READ dutDBFilePath WRITE setDutDBFilePath NOTIFY dutDBFilePathChanged) //dut = device under test
    Q_PROPERTY(QString scpiServerIP READ scpiServerIP WRITE setScpiServerIP NOTIFY scpiServerIPChanged)
    Q_PROPERTY(quint16 scpiServerPort READ scpiServerPort WRITE setScpiServerPort NOTIFY scpiServerPortChanged)
    Q_PROPERTY(QStringList entityBlacklist READ entityBlacklist WRITE setEntityBlacklist NOTIFY entityBlacklistChanged)
    Q_PROPERTY(QStringList componentBlacklist READ componentBlacklist WRITE setComponentBlacklist NOTIFY componentBlacklistChanged)
    Q_PROPERTY(QVariantMap dutResult READ dutResult NOTIFY dutResultChanged)

    Q_PROPERTY(bool refDBReady READ refDBReady NOTIFY refDBReadyChanged)
    Q_PROPERTY(bool dutDBReady READ dutDBReady NOTIFY dutDBReadyChanged)
    Q_PROPERTY(bool scpiServerConnected READ scpiServerConnected NOTIFY scpiServerConnectedChanged)
    Q_PROPERTY(bool testReady READ testReady NOTIFY testReadyChanged)
    Q_PROPERTY(int testProgress READ testProgress NOTIFY testProgressChanged)
    Q_PROPERTY(int testCount READ testCount NOTIFY testCountChanged)

  public:
    explicit TestController(QObject *t_parent = nullptr);
    ~TestController();

    Q_INVOKABLE void startTest();
    Q_INVOKABLE void connectToSCPIServer();
    Q_INVOKABLE void writeResultFile(const QString &t_filePath);

    double epsilonValue() const;
    QString refDBFilePath() const;
    QString dutDBFilePath() const;
    QString scpiServerIP() const;
    quint16 scpiServerPort() const;
    QStringList entityBlacklist() const;
    QStringList componentBlacklist() const;

    bool testReady() const;
    int testProgress() const;
    int testCount() const;
    bool scpiServerConnected() const;
    bool refDBReady() const;
    bool dutDBReady() const;
    QVariantMap dutResult() const;

  signals:
    void epsilonValueChanged(double epsilonValue);
    void refDBFilePathChanged(QString refDBFilePath);
    void dutDBFilePathChanged(QString dutDBFilePath);
    void scpiServerIPChanged(QString scpiServerIP);
    void scpiServerPortChanged(quint16 scpiServerPort);
    void entityBlacklistChanged(QStringList entityBlacklist);
    void componentBlacklistChanged(QStringList componentBlacklist);
    void testReadyChanged(bool testReady);
    void testProgressChanged(int testProgress);
    void testCountChanged(int testCount);
    void scpiServerConnectedChanged(bool scpiServerConnected);
    void refDBReadyChanged(bool refDBReady);
    void dutDBReadyChanged(bool dutDBReady);
    void dutResultChanged(QVariantMap dutResult);

  public slots:
    void setEpsilonValue(double t_epsilonValue);
    void setRefDBFilePath(QString t_refDBFilePath);
    void setDutDBFilePath(QString t_dutDBFilePath);
    void setScpiServerIP(QString t_scpiServerIP);
    void setScpiServerPort(quint16 t_scpiServerPort);
    void setEntityBlacklist(QStringList t_entityBlacklist);
    void setComponentBlacklist(QStringList t_componentBlacklist);

  private slots:
    void updateTestCount(int t_testCount);
    void updateTestProgress(int t_testProgress);

  private:
    void readyCheck();

    double m_epsilonValue;
    QString m_refDBFilePath;
    QString m_dutDBFilePath;
    QString m_scpiServerIP;
    quint16 m_scpiServerPort = 0;
    bool m_testReady = false;
    int m_testProgress = 0;
    int m_testCount = 0;
    bool m_scpiServerConnected = false;
    bool m_refDBReady = false;
    bool m_dutDBReady;

    TestResult *m_dutTestResult = 0;
    //TestResult *m_scpiTestResult = 0;
    TestRunner *m_testRunner = 0;
    QStringList m_entityBlacklist;
    QStringList m_componentBlacklist;
    QVariantMap m_dutResultData;
  };
} // namespace VFDT

#endif // VFDT_TESTCONTROLLER_H
