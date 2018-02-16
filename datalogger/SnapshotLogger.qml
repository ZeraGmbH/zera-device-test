import QtQuick 2.9
import VeinEntity 1.0
import VeinLogger 1.0

Item {
  readonly property int loggerEntId: 200000;
  readonly property bool isInitialized: dataLogger.isInitialized;
  readonly property alias logEntity: dataLogger.logEntity;
  readonly property alias logger: dataLogger;
  VeinLogger {
    id: dataLogger
    recordName: "default"
    initializeValues: true
    //only run script manually
    property var requiredIds: [];
    property var resolvedIds: []; //may only contain ids that are also in requiredIds

    readonly property bool debugBuild: true
    property bool entitiesLoaded: false
    property var loggedValues: ({});
    property QtObject logEntity;
    property bool isInitialized: false;

    onLoggedValuesChanged: {
      clearLoggerEntries();
      for(var it in loggedValues)
      {
        var entData = loggedValues[it];
        for(var i=0; i<entData.length; ++i)
        {
          addLoggerEntry(it, entData[i]);
        }
      }
    }

    function takeSnapshot() {
      console.log("starting logging at", new Date().toLocaleTimeString());
      startLogging();
      console.log("stopped logging at", new Date().toLocaleTimeString());
      stopLogging();
    }

    property string session;
    onSessionChanged: {
      var entIds = VeinEntity.getEntity("_System")["Entities"];
      if(entIds !== undefined)
      {
        entIds.push(0);
        entIds.push(loggerEntId);
      }
      else
      {
        entIds = [0, loggerEntId];
      }
      VeinEntity.setRequiredIds(entIds)
      entitiesLoaded = true
    }

    Binding on session {
      when: VeinEntity.hasEntity("_System")
      value: VeinEntity.getEntity("_System").Session
    }

    Connections {
      target: VeinEntity
      onSigEntityAvailable: {
        var entId = VeinEntity.getEntity(t_entityName).entityId()
        if(dataLogger.requiredIds.indexOf(entId) > -1) //required
        {
          if(dataLogger.resolvedIds.indexOf(entId) < 0) //resolved
          {
            resolvedIds.push(entId);
            checkRequired = true;
          }
        }

        if(entId === loggerEntId)
        {
          dataLogger.logEntity = Qt.binding(function(){
            return VeinEntity.getEntity("_BinaryLoggingSystem");
          });
          dataLogger.isInitialized = true;
        }
        else if(dataLogger.entitiesLoaded === true)
        {
          //log all components of all entities
          var tmpEntity = VeinEntity.getEntity(t_entityName);
          var tmpEntityId = tmpEntity.entityId()
          var loggedComponents = [];
          dataLogger.loggedValues[String(tmpEntityId)] = tmpEntity.keys();
          dataLogger.loggedValuesChanged();
        }
      }
    }
  }
}
