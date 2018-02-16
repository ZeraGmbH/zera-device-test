#include "vfdt_testresult.h"

namespace VFDT
{
  //constexpr definition, see: https://stackoverflow.com/questions/8016780/undefined-reference-to-static-constexpr-char
  constexpr QLatin1String TestResult::s_extraText;
  constexpr QLatin1String TestResult::s_missingText;
  constexpr QLatin1String TestResult::s_blacklistedText;
  constexpr QLatin1String TestResult::s_passedText;
  constexpr QLatin1String TestResult::s_failedText;

  TestResult::TestResult(double t_epsilon, QObject *t_parent) : QObject(t_parent), m_epsilon(t_epsilon)
  {

  }

  QMap<QString, QString> TestResult::getResultMap() const
  {
    return m_resultMap;
  }

  void TestResult::insertExtraKeys(const QList<ResultKey> &t_extraList)
  {
    for(ResultKey resultKey : t_extraList)
    {
      m_resultMap.insert(QString("%1::%2").arg(resultKey.first).arg(resultKey.second), TestResult::s_extraText);
    }
  }

  void TestResult::insertMissingKeys(const QList<ResultKey> &t_missingList)
  {
    for(ResultKey resultKey : t_missingList)
    {
      m_resultMap.insert(QString("%1::%2").arg(resultKey.first).arg(resultKey.second), TestResult::s_missingText);
    }
  }

  void TestResult::insertBlacklistedKeys(const QList<ResultKey> &t_skippedList)
  {
    for(ResultKey resultKey : t_skippedList)
    {
      m_resultMap.insert(QString("%1::%2").arg(resultKey.first).arg(resultKey.second), TestResult::s_blacklistedText);
    }
  }

  void TestResult::insertResult(const ResultKey &t_resultKey, bool t_passed)
  {
    m_resultMap.insert(QString("%1::%2").arg(t_resultKey.first).arg(t_resultKey.second), (t_passed ? TestResult::s_passedText : TestResult::s_failedText));
  }

  double TestResult::getEpsilon() const
  {
    return m_epsilon;
  }
} // namespace VFDT
