#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <ve_eventhandler.h>
#include <vn_networksystem.h>
#include <vn_tcpsystem.h>
#include <veinqml.h>
#include <veinqmlwrapper.h>
#include <vl_databaselogger.h>
#include <vl_datasource.h>
#include <vl_qmllogger.h>
#include <vl_sqlitedb.h>
#include <vn_networkstatusevent.h>
#include <vs_veinhash.h>

#include <QDataStream>
#include <QList>
#include <QMetaType>
#include <QQmlContext>
#include <QTimer>

#include "vfdt_localintrospection.h"
#include "vfdt_networkcontroller.h"
#include "vfdt_testcontroller.h"

int main(int argc, char *argv[])
{
  using namespace VFDT;
  bool loadedOnce=false;

  QString categoryLoggingFormat = "%{if-debug}DD%{endif}%{if-warning}WW%{endif}%{if-critical}EE%{endif}%{if-fatal}FATAL%{endif} %{category} %{message}";

  QStringList loggingFilters = QStringList() << QString("%1.debug=false").arg(VEIN_EVENT().categoryName()) <<
                                                QString("%1.debug=false").arg(VEIN_NET_VERBOSE().categoryName()) <<
                                                QString("%1.debug=false").arg(VEIN_NET_INTRO_VERBOSE().categoryName()) << //< Introspection logging is still enabled
                                                QString("%1.debug=false").arg(VEIN_NET_TCP_VERBOSE().categoryName()) <<
                                                QString("%1.debug=false").arg(VEIN_STORAGE_HASH_VERBOSE().categoryName()) <<
                                                QString("%1.debug=false").arg(VEIN_API_QML_VERBOSE().categoryName());

  const VeinLogger::DBFactory sqliteFactory = [](){
    return new VeinLogger::SQLiteDB();
  };

  QLoggingCategory::setFilterRules(loggingFilters.join("\n"));
  qSetMessagePattern(categoryLoggingFormat);

  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty(QStringLiteral("VFDT_CONNECTED"), false);
  NetworkController netController;
  engine.rootContext()->setContextProperty(QStringLiteral("VFDT_NET_CONTROLLER"), &netController);
  TestController *testController = new TestController(&engine);
  engine.rootContext()->setContextProperty(QStringLiteral("VFDT_TEST_CONTROLLER"), testController);

  VeinEvent::EventHandler *evHandler = new VeinEvent::EventHandler(&app);
  VeinStorage::VeinHash *storSystem = new VeinStorage::VeinHash(&app);
  LocalIntrospection *lIntrospectionSys = new LocalIntrospection(storSystem, &app);
  VeinNet::NetworkSystem *netSystem = new VeinNet::NetworkSystem(&app);
  VeinNet::TcpSystem *tcpSystem = new VeinNet::TcpSystem(&app);
  VeinApiQml::VeinQml *qmlApi = new VeinApiQml::VeinQml(&app);
  VeinLogger::DatabaseLogger *binaryDataLogger = new VeinLogger::DatabaseLogger(new VeinLogger::DataSource(qmlApi, &app), sqliteFactory, &app, VeinLogger::AbstractLoggerDB::STORAGE_MODE::BINARY);

  VeinApiQml::VeinQml::setStaticInstance(qmlApi);
  VeinLogger::QmlLogger::setStaticLogger(binaryDataLogger);
  //only store data for the binary data logger
  storSystem->setAcceptableOrigin({VeinEvent::EventData::EventOrigin::EO_LOCAL});

  QList<VeinEvent::EventSystem*> subSystems;

  QObject::connect(qmlApi,&VeinApiQml::VeinQml::sigStateChanged, [&](VeinApiQml::VeinQml::ConnectionState t_state){
    if(t_state == VeinApiQml::VeinQml::ConnectionState::VQ_LOADED && loadedOnce == false)
    {
      netController.setConnectionStatus("Connected");
      engine.rootContext()->setContextProperty(QStringLiteral("VFDT_CONNECTED"), true);
      loadedOnce=true;
    }
    else if(t_state == VeinApiQml::VeinQml::ConnectionState::VQ_ERROR)
    {
      netController.setConnectionStatus("Connection error");
      engine.rootContext()->setContextProperty(QStringLiteral("VFDT_CONNECTED"), false);
    }
  });

  qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");
  qRegisterMetaTypeStreamOperators<QList<float> >("QList<float>");
  qRegisterMetaTypeStreamOperators<QList<double> >("QList<double>");
  qRegisterMetaTypeStreamOperators<QList<QString> >("QList<QString>");
  qRegisterMetaTypeStreamOperators<QVector<QString> >("QVector<QString>");

  QObject::connect(tcpSystem, &VeinNet::TcpSystem::sigSendEvent, [&](QEvent *t_event){
    if(t_event->type()==VeinNet::NetworkStatusEvent::getEventType())
    {
      engine.rootContext()->setContextProperty(QStringLiteral("VFDT_CONNECTED"), false);
      netController.setConnectionStatus("Network failed");
    }
  });

  netSystem->setOperationMode(VeinNet::NetworkSystem::VNOM_PASS_THROUGH);

  //do not reorder
  subSystems.append(lIntrospectionSys);
  subSystems.append(storSystem);
  subSystems.append(netSystem);
  subSystems.append(tcpSystem);
  subSystems.append(qmlApi);
  subSystems.append(binaryDataLogger);

  evHandler->setSubsystems(subSystems);

  QObject::connect(&netController, &NetworkController::sigStartConnection, tcpSystem, &VeinNet::TcpSystem::connectToServer);

  QObject::connect(tcpSystem, &VeinNet::TcpSystem::sigConnnectionEstablished, [=]() {
    qmlApi->setRequiredIds(QList<int>()<<0);
  });

  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

  return app.exec();
}
