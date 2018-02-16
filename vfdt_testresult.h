#ifndef VFDT_TESTRESULT_H
#define VFDT_TESTRESULT_H

#include <QObject>
#include <QMap>

namespace VFDT
{
  using ResultKey = QPair<QString, QString>;

  class TestResult : public QObject
  {
    Q_OBJECT
  public:
    explicit TestResult(double t_epsilon, QObject *t_parent = nullptr);

    QMap<QString, QString> getResultMap() const;
    void insertExtraKeys(const QList<ResultKey> &t_extraList);
    void insertMissingKeys(const QList<ResultKey> &t_missingList);
    void insertBlacklistedKeys(const QList<ResultKey> &t_skippedList);
    void insertResult(const ResultKey &t_resultKey, bool t_passed);

    double getEpsilon() const;

  signals:
    //void sigResultModelChanged(QAbstractTableModel *t_resultModel);
  public slots:

  private:
    const double m_epsilon;
    //sorted (key, result) as strings
    QMap<QString, QString> m_resultMap;

    static constexpr QLatin1String s_extraText = QLatin1String("EXTRA DATA");
    static constexpr QLatin1String s_missingText = QLatin1String("MISSING DATA");
    static constexpr QLatin1String s_blacklistedText = QLatin1String("BLACKLISTED");
    static constexpr QLatin1String s_passedText = QLatin1String("PASS");
    static constexpr QLatin1String s_failedText = QLatin1String("FAIL");
  };
} // namespace VFDT

#endif // VFDT_TESTRESULT_H
