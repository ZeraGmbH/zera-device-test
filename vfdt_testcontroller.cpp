#include "vfdt_testcontroller.h"
#include "vfdt_testrunner.h"
#include "vfdt_testresult.h"

#include <QCoreApplication>
#include <QQmlEngine>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QDebug>
#include <QTextStream>

namespace VFDT
{
  TestController::TestController(QObject *t_parent) :
    QObject(t_parent),
    m_testRunner(new TestRunner(this))
  {
    connect(m_testRunner, &TestRunner::sigTestCountChanged, this, &TestController::updateTestCount);
    connect(m_testRunner, &TestRunner::sigTestProgressChanged, this, &TestController::updateTestProgress);
  }

  TestController::~TestController()
  {
    m_testRunner->deleteLater();
  }

  void TestController::startTest()
  {
    if(m_testReady)
    {
      m_dutTestResult = m_testRunner->dutRunTest(m_refDBFilePath, m_dutDBFilePath, m_epsilonValue);

      m_dutResultData.clear();
      const QMap<QString, QString> resultMap = m_dutTestResult->getResultMap();
      for(const QString &ecsKey : resultMap.keys())
      {
        m_dutResultData.insert(ecsKey, resultMap.value(ecsKey));
      }
      emit dutResultChanged(m_dutResultData);
    }
  }

  void TestController::connectToSCPIServer()
  {
    if(m_scpiServerIP.isEmpty() || m_scpiServerPort == 0)
    {
      qWarning() << "Invalid SCPI connection data, IP:" << m_scpiServerIP << "port:" << m_scpiServerPort;
    }
    else
    {
      ///@todo: TBD
      qWarning() << "function not implemented yet";
      //readyCheck();
    }
  }

  void TestController::writeResultFile(const QString &t_filePath)
  {
    if(m_testCount > 0 && m_testProgress == m_testCount) //test finished
    {
      QFileInfo fInfo(t_filePath);
      if(fInfo.completeSuffix() == "csv")
      {
        QFile resultFile(t_filePath);
        if(resultFile.open(QFile::WriteOnly | QFile::Truncate))
        {
          QTextStream outStream(&resultFile);
          const auto separator = ';';
          const auto endOfLine = "\r\n";//csv needs MS-DOS-style lines that end with (CR/LF) characters (optional for the last line).
          //header
          outStream << "Name:" << separator << "DUT Result:" /*<< separator "SCPI Result:"*/ << separator << "Epsilon: " << m_dutTestResult->getEpsilon() << endOfLine;

          const QMap<QString, QString> dutData = m_dutTestResult->getResultMap();
          for(const QString &ecsKey : dutData.keys())
          {
            outStream << ecsKey << separator << dutData.value(ecsKey) << endOfLine;
          }
          resultFile.close();
        }
      }
    }
  }

  double TestController::epsilonValue() const
  {
    return m_epsilonValue;
  }

  QString TestController::refDBFilePath() const
  {
    return m_refDBFilePath;
  }

  QString TestController::dutDBFilePath() const
  {
    return m_dutDBFilePath;
  }

  QString TestController::scpiServerIP() const
  {
    return m_scpiServerIP;
  }

  quint16 TestController::scpiServerPort() const
  {
    return m_scpiServerPort;
  }

  QStringList TestController::entityBlacklist() const
  {
    return m_entityBlacklist;
  }

  QStringList TestController::componentBlacklist() const
  {
    return m_componentBlacklist;
  }

  bool TestController::testReady() const
  {
    return m_testReady;
  }

  int TestController::testProgress() const
  {
    return m_testProgress;
  }

  int TestController::testCount() const
  {
    return m_testCount;
  }

  bool TestController::scpiServerConnected() const
  {
    return m_scpiServerConnected;
  }

  bool TestController::refDBReady() const
  {
    return m_refDBReady;
  }

  bool TestController::dutDBReady() const
  {
    return m_dutDBReady;
  }

  QVariantMap TestController::dutResult() const
  {
    return m_dutResultData;
  }

  void TestController::setEpsilonValue(double t_epsilonValue)
  {
    if (qFuzzyCompare(m_epsilonValue, t_epsilonValue))
      return;

    m_epsilonValue = t_epsilonValue;
    emit epsilonValueChanged(m_epsilonValue);
  }

  void TestController::setRefDBFilePath(QString t_refDBFilePath)
  {
    if (m_refDBFilePath != t_refDBFilePath)
    {
      QFileInfo refFileInfo;
      QMimeDatabase mimeDB;
      refFileInfo.setFile(t_refDBFilePath);
      if(refFileInfo.exists())
      {
        QMimeType mimeType = mimeDB.mimeTypeForFile(refFileInfo);
        if(mimeType.inherits(QStringLiteral("application/x-sqlite3")))
        {
          m_refDBFilePath = t_refDBFilePath;
          emit refDBFilePathChanged(m_refDBFilePath);
          m_refDBReady = true;
          emit refDBReadyChanged(m_refDBReady);
          readyCheck();
        }
      }
    }
  }

  void TestController::setDutDBFilePath(QString t_dutDBFilePath)
  {
    if (m_dutDBFilePath != t_dutDBFilePath)
    {
      QFileInfo dutFileInfo;
      QMimeDatabase mimeDB;
      dutFileInfo.setFile(t_dutDBFilePath);
      if(dutFileInfo.exists())
      {
        QMimeType mimeType = mimeDB.mimeTypeForFile(dutFileInfo);
        if(mimeType.inherits(QStringLiteral("application/x-sqlite3")))
        {
          m_dutDBFilePath = t_dutDBFilePath;
          emit dutDBFilePathChanged(m_dutDBFilePath);
          m_dutDBReady = true;
          emit dutDBReadyChanged(m_dutDBReady);
          readyCheck();
        }
      }
    }
  }

  void TestController::setScpiServerIP(QString t_scpiServerIP)
  {
    if (m_scpiServerIP == t_scpiServerIP)
      return;

    m_scpiServerIP = t_scpiServerIP;
    emit scpiServerIPChanged(m_scpiServerIP);
  }

  void TestController::setScpiServerPort(quint16 t_scpiServerPort)
  {
    if (m_scpiServerPort == t_scpiServerPort)
      return;

    m_scpiServerPort = t_scpiServerPort;
    emit scpiServerPortChanged(m_scpiServerPort);
  }

  void TestController::setEntityBlacklist(QStringList t_entityBlacklist)
  {
    if (m_entityBlacklist == t_entityBlacklist)
      return;

    m_entityBlacklist = t_entityBlacklist;
    m_testRunner->setBlacklists(m_entityBlacklist, m_componentBlacklist);
    emit entityBlacklistChanged(m_entityBlacklist);
  }

  void TestController::setComponentBlacklist(QStringList t_componentBlacklist)
  {
    if (m_componentBlacklist == t_componentBlacklist)
      return;

    m_componentBlacklist = t_componentBlacklist;
    m_testRunner->setBlacklists(m_entityBlacklist, m_componentBlacklist);
    emit componentBlacklistChanged(m_componentBlacklist);
  }

  void TestController::updateTestCount(int t_testCount)
  {
    if(m_testCount != t_testCount)
    {
      m_testCount = t_testCount;
      emit testCountChanged(m_testCount);
    }
  }

  void TestController::updateTestProgress(int t_testProgress)
  {
    if(m_testProgress != t_testProgress)
    {
      m_testProgress = t_testProgress;
      emit testProgressChanged(m_testProgress);
    }
  }

  void TestController::readyCheck()
  {
    m_testReady = m_refDBReady && (m_dutDBReady || m_scpiServerConnected);
    emit testReadyChanged(m_testReady);
  }

  void registerTestController()
  {
    qmlRegisterUncreatableType<TestController>("VFDT", 1, 0, "TestController", "C++ QmlContext QObject");
  }
  Q_COREAPP_STARTUP_FUNCTION(registerTestController)
} // namespace VFDT
