#ifndef LOCALINTROSPECTION_H
#define LOCALINTROSPECTION_H

#include <ve_eventsystem.h>
#include <QJsonObject>

namespace VeinStorage
{
  class VeinHash;
}

namespace VFDT
{
  class LocalIntrospection : public VeinEvent::EventSystem
  {
    Q_OBJECT
  public:
    explicit LocalIntrospection(VeinStorage::VeinHash *t_storage, QObject *parent = 0);

    // EventSystem interface
  public:
    bool processEvent(QEvent *t_event) override;
    QJsonObject getJsonIntrospection(int t_entityId) const;

  signals:

  public slots:

  private:
    VeinStorage::VeinHash *m_storage=nullptr;
  };
}
#endif // LOCALINTROSPECTION_H
