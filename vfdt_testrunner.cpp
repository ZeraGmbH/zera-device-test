#include "vfdt_testrunner.h"
#include "vfdt_testresult.h"

#include <QDataStream>
#include <QSqlError>
#include <QDebug>
#include <QMetaType>


namespace VFDT
{
  TestRunner::TestRunner(QObject *parent) : QObject(parent)
  {

  }

  void TestRunner::setBlacklists(const QStringList &t_entityBlacklist, const QStringList &t_componentBlacklist)
  {
    m_entityBlacklist.clear();
    m_componentBlacklist.clear();
    for(const QString &entry : t_entityBlacklist)
    {
      m_entityBlacklist.append(QRegularExpression(entry));
    }
    for(const QString &entry : t_componentBlacklist)
    {
      m_componentBlacklist.append(QRegularExpression(entry));
    }
  }

  TestResult *TestRunner::dutRunTest(const QString &t_refDBPath, const QString &t_dutDBPath, double t_epsilon)
  {
    qDebug() << "Test started";
    TestResult *result = new TestResult(t_epsilon, this);
    ECSDataHash refData;
    ECSDataHash dutData;
    {//<<< to limit QSqlDatabase lifetime
      QSqlDatabase refDatabase = openDB(t_refDBPath);
      QSqlDatabase dutDatabase = openDB(t_dutDBPath);
      refData = readDBData(refDatabase);
      dutData = readDBData(dutDatabase);
      refDatabase.close();
      dutDatabase.close();
    }//<<< to limit QSqlDatabase lifetime
    QSqlDatabase::removeDatabase(t_refDBPath);
    QSqlDatabase::removeDatabase(t_dutDBPath);
    if(refData.isEmpty() || dutData.isEmpty())
    {
      qWarning() << "Invalid test data";
    }
    else
    {
      emit sigTestStarted();
      emit sigTestCountChanged(refData.count());
      const ECSKeySet refKeys = refData.keys().toSet();
      const ECSKeySet dutKeys = dutData.keys().toSet();
      const ECSKeySet commonKeys = ECSKeySet(refKeys).intersect(dutKeys);
      const ECSKeySet missingKeys = ECSKeySet(refKeys).subtract(dutKeys);
      const ECSKeySet extraKeys = ECSKeySet(dutKeys).subtract(refKeys);
      ECSKeySet blacklistedKeys;

      qDebug() << "Missing entries:" << missingKeys.count();
      qDebug() << "Extra entries:" << extraKeys.count();
      qDebug() << "Common entries:" << commonKeys.count();
      qDebug() << "epsilon:" << t_epsilon;

      for(const QPair<QString, QString> &ecsKey : commonKeys)
      {
        if((isNotBlacklisted(ecsKey.first, m_entityBlacklist) && isNotBlacklisted(ecsKey.second, m_componentBlacklist)))
        {
          QByteArray refValueArray = refData.value(ecsKey).toByteArray();
          QByteArray dutValueArray = dutData.value(ecsKey).toByteArray();
          QDataStream refValueStream(&refValueArray, QIODevice::ReadOnly);
          QDataStream dutValueStream(&dutValueArray, QIODevice::ReadOnly);
          QVariant refValue, dutValue;
          refValueStream >> refValue;
          dutValueStream >> dutValue;

          result->insertResult(ecsKey, compareVariant(refValue, dutValue, t_epsilon));
        }
        else
        {
          blacklistedKeys.insert(ecsKey);
        }
      }
      result->insertMissingKeys(missingKeys.toList());
      result->insertExtraKeys(extraKeys.toList());
      result->insertBlacklistedKeys(blacklistedKeys.toList());

      ///@note currently this function is blocking, if proper progress info is required use multithreading
      emit sigTestProgressChanged(commonKeys.count());
      emit sigTestFinished(result);
    }
    return result;
  }





  QSqlDatabase TestRunner::openDB(const QString &t_path) const
  {
    QSqlError dbError;
    QSqlDatabase retVal = QSqlDatabase::addDatabase("QSQLITE", t_path);;
    retVal.setDatabaseName(t_path);
    if (!retVal.open())
    {
      dbError = retVal.lastError();
      qWarning() << "Error opening database:" << dbError.text();
    }

    if(dbError.type() != QSqlError::NoError)
    {
      qWarning() << "Database error:" << retVal.lastError().text();
    }
    else
    {
      QSqlQuery schemaVersionQuery(retVal);

      if(schemaVersionQuery.exec("pragma schema_version;") == true) //check if the file is valid (empty or a valid database)
      {
        schemaVersionQuery.first();
        if(schemaVersionQuery.value(0) == 0) //if there is no database schema or if the file does not exist, then this will create the database and initialize the schema
        {
          qWarning() << "Database schema is not supported";
          retVal.close();
        }
      }
      else
      {
        retVal.close();
      }
      schemaVersionQuery.finish();
    }
    return retVal;
  }

  ECSDataHash TestRunner::readDBData(QSqlDatabase t_database)
  {
    ECSDataHash retVal;
    if(t_database.isOpen())
    {
      QSqlError queryError;
      QSqlQuery dataQuery = QSqlQuery(t_database);
      dataQuery.prepare("SELECT entity_name, component_name, component_value FROM valueview ORDER BY 1;");
      dataQuery.exec();

      queryError = dataQuery.lastError();
      if(queryError.type() != QSqlError::NoError)
      {
        qWarning() << "Error in initQuery:" << queryError.text();
      }


      while(dataQuery.next())
      {
        retVal.insert(qMakePair<QString, QString>(dataQuery.value(0).toString(), dataQuery.value(1).toString()), dataQuery.value(2));
      }
      dataQuery.finish();
    }

    return retVal;
  }

  bool TestRunner::isNotBlacklisted(const QString &t_data, const QList<QRegularExpression> &t_regexList) const
  {
    bool retVal = true;
    for(const QRegularExpression &regex : t_regexList)
    {
      auto match = regex.match(t_data);
      if(match.hasMatch())
      {
        retVal = false;
        break;
      }
    }
    return retVal;
  }

  bool TestRunner::compareVariant(const QVariant &t_value1, const QVariant &t_value2, double t_epsilon)
  {
    bool retVal = false;
    if(t_value1.type() == t_value2.type())
    {

      switch(static_cast<QMetaType::Type>(t_value1.type()))
      {
        case QMetaType::Float:
        {
          bool convTest1, convTest2;
          const float f1 = t_value1.toFloat(&convTest1);
          const float f2 = t_value2.toFloat(&convTest2);
          retVal = (convTest1 && convTest2 && epsilonFuzzyCompareFloat(f1, f2, t_epsilon));
          break;
        }
        case QMetaType::Double:
        {
          bool convTest1, convTest2;
          const double d1 = t_value1.toDouble(&convTest1);
          const double d2 = t_value2.toDouble(&convTest2);
          retVal = (convTest1 && convTest2 && epsilonFuzzyCompareDouble(d1, d2, t_epsilon));
          break;
        }
        default:
        {
          int type1 = QMetaType::type(t_value1.typeName());
          if(type1 == QMetaType::type("QList<double>"))
          {
            const QList<double> list1 = t_value1.value<QList<double> >();
            const QList<double> list2 = t_value2.value<QList<double> >();
            if(list1.count() == list2.count())
            {
              for(int listCounter = 0; listCounter<list1.count(); ++listCounter)
              {
                const double d1 = list1.at(listCounter);
                const double d2 = list2.at(listCounter);
                retVal = epsilonFuzzyCompareDouble(d1, d2, t_epsilon);
                if(retVal == false)
                {
                  break;
                }
              }
            }
          }
          else
          {
            retVal = (t_value1 == t_value2);
          }
          break;
        }
      }
    }
    else
    {
      qDebug() << "invalid types" << t_value1.typeName() << t_value2.typeName();
    }
    return retVal;
  }
} // namespace VFDT
