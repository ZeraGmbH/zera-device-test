#ifndef VFDT_TESTRUNNER_H
#define VFDT_TESTRUNNER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QRegularExpression>

namespace VFDT
{
  struct TestResult;
  ///@b QSet(entityName, componentName)
  using ECSKeySet = QSet<QPair<QString, QString> >;
  ///@b Hash((entityName, componentName), value)
  using ECSDataHash = QHash<QPair<QString, QString>, QVariant>;
  class TestRunner : public QObject
  {
    Q_OBJECT
  public:
    explicit TestRunner(QObject *parent = nullptr);
    void setBlacklists(const QStringList &t_entityBlacklist, const QStringList &t_componentBlacklist);

  signals:
    void sigTestProgressChanged(int t_progress);
    void sigTestCountChanged(int t_testCount);
    void sigTestFinished(TestResult *t_result);
    void sigTestStarted();

  public slots:
    TestResult *dutRunTest(const QString &t_refDBPath, const QString &t_dutDBPath, double t_epsilon);

  private:
    QSqlDatabase openDB(const QString &t_path) const;
    ECSDataHash readDBData(QSqlDatabase t_database);

    bool isNotBlacklisted(const QString &t_data, const QList<QRegularExpression> &t_regexList) const;

    bool compareVariant(const QVariant &t_value1, const QVariant &t_value2, double t_epsilon);
    bool epsilonFuzzyCompareFloat(double t_value1, double t_value2, double t_epsilon)
    {
      Q_ASSERT(t_epsilon>0);
      return (qAbs(t_value1 - t_value2) <= t_epsilon);
    }
    bool epsilonFuzzyCompareDouble(float t_value1, float t_value2, float t_epsilon)
    {
      Q_ASSERT(t_epsilon>0);
      return (qAbs(t_value1 - t_value2) <= t_epsilon);
    }

    QList<QRegularExpression> m_entityBlacklist;
    QList<QRegularExpression> m_componentBlacklist;
  };
} // namespace VFDT

#endif // VFDT_TESTRUNNER_H
