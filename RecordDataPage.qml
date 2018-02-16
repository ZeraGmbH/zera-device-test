import QtQuick 2.9
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.0
import QtQuick.Dialogs 1.3
import VeinEntity 1.0
import VeinLogger 1.0
import VFDT 1.0
import "qrc:/datalogger"

Item {
  id: recordDataPage
  readonly property NetworkController netController: VFDT_NET_CONTROLLER;
  property bool loggerReady: false;
  property QtObject logEntity;
  //used to take a snapshot of the actual data
  readonly property bool loggingEnabled: logEntity ? logEntity.LoggingEnabled : false
  onLoggingEnabledChanged: {
    if(loggingEnabled === true)
    {
      dbPathSelectionButton.enabled = false
      startRecordButton.enabled = false;
      actualLogger.startLogging();
      actualLogger.stopLogging();
      logEntity.LoggingEnabled=false;
      logStatusLabel.text = "Finished";
    }
  }

  property VeinLogger actualLogger;
  onActualLoggerChanged: console.log(actualLogger)

  Loader {
    sourceComponent: SnapshotLogger {
      id: testSL
      Binding {
        target: recordDataPage;
        property: 'loggerReady';
        value: testSL.isInitialized
      }
      onLogEntityChanged: recordDataPage.logEntity=logEntity;
      Component.onCompleted: recordDataPage.actualLogger = logger
    }
    active: VFDT_CONNECTED
  }

  FileDialog {
    id: dbSaveFileDialog
    width: recordDataPage.width*0.9
    height: recordDataPage.height*0.9
    selectExisting: false
    selectMultiple: false
    selectFolder: false
    nameFilters: [ "SQLite3 database file (*.db *.sqlite3)" ]
    defaultSuffix: ".db"
    onAccepted: {
      var path =  dbSaveFileDialog.fileUrl.toString();
      path = path.replace(/^(file:\/{2})/,"");
      dbFilepath.text = path
      logEntity.DatabaseFile = path
    }
  }

  GridLayout {
    id: grid
    anchors.fill: parent
    columns: 3

    //row
    Label {
      text: "Modulemanager settings:"
      horizontalAlignment: Label.AlignHCenter
      font.bold: true
      Layout.fillWidth: true
      Layout.columnSpan: 3
    }
    //row
    Text { text: "IP:"; }
    TextField {
      id: hostName
      text: "127.0.0.1"
      selectByMouse: true;
      Layout.columnSpan: 2;
      validator: RegExpValidator {
        regExp: /^(?:(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]?|[1-9]?[0-9])\.){3}(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]?|[1-9]?[0-9])$/
      }
    }
    //row
    Text { text: "Port:"; }
    TextField {
      id: hostPort
      text: "12000";
      selectByMouse: true;
      Layout.columnSpan: 2;
      validator: IntValidator { bottom: 1; top: 65535 }
    }
    //row
    Label {
      text: "Status:"
    }
    Label {
      text: netController ? netController.connectionStatus : ""
    }
    Button {
      text: "Connect"
      enabled: hostName.acceptableInput && hostPort.acceptableInput && !VFDT_CONNECTED
      onClicked: {
        netController.hostName=hostName.text
        netController.hostPort=hostPort.text
        netController.startConnection();
      }
    }
    //row
    Label {
      text: "Database settings:"
      horizontalAlignment: Label.AlignHCenter
      font.bold: true
      Layout.fillWidth: true
      Layout.columnSpan: 3
    }
    //row
    Label {
      text: "Database file path:"
    }
    TextField {
      id: dbFilepath
      readOnly: true
      Layout.fillWidth: true
    }
    Button {
      id: dbPathSelectionButton
      text: "Select path"
      enabled: recordDataPage.loggerReady
      onClicked: {
        dbSaveFileDialog.open()
      }
    }
    //row
    Item {
      Layout.fillHeight: true
      Layout.fillWidth: true
      Layout.columnSpan: 3
    }
    //row
    Label {
      text: "Status:"
    }
    Label {
      id: logStatusLabel
      text: logEntity ? logEntity.LoggingStatus : ""
    }
    Button {
      id: startRecordButton
      text: "Record values"
      enabled: recordDataPage.loggerReady && logEntity.DatabaseFile.length>0
      onClicked: {
        logEntity.LoggingEnabled=true;
      }
    }
  }
}
