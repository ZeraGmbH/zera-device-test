import QtQuick 2.9
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.0
import QtQuick.Dialogs 1.3
import VeinEntity 1.0
import VeinLogger 1.0
import VFDT 1.0

Item {
  id: compareDataPage

  readonly property TestController testController: VFDT_TEST_CONTROLLER;
  property real testEpsilon;
  onTestEpsilonChanged: {
    if(testEpsilon && testEpsilon > 0)
    {
      testController.setEpsilonValue(testEpsilon)
    }
  }

  Component.onCompleted: {
    refDBPathField.setPath("/work/qt_projects/vein-framework/bin/test2.db");
    dutDBPathField.setPath("/work/qt_projects/vein-framework/bin/test3.db");

    testController.entityBlacklist = ["_System", "_LoggingSystem", "_ScriptSystem", "CustomerData", "SCPIModule1", "AdjustmentModule1", "StatusModule1", "SEC1Module1"];
    testController.componentBlacklist = ["INF_.*", "SIG_.*" , "PAR_Interval", "PAR_ChannelGrouping", "PAR_RangeAutomatic"];
  }

  FileDialog {
    id: dbOpenFileDialog
    width: compareDataPage.width*0.9
    height: compareDataPage.height*0.9
    selectExisting: true
    selectMultiple: false
    selectFolder: false
    nameFilters: [ "SQLite3 database file (*.db *.sqlite3)" ];
    defaultSuffix: ".db"
    property QtObject targetPointer;
    onAccepted: {
      var path =  dbOpenFileDialog.fileUrl.toString();
      path = path.replace(/^(file:\/{2})/,"");
      targetPointer.setPath(path);
    }
  }

  GridLayout {
    id: grid
    anchors.fill: parent
    columns: 4
    //row
    Label {
      text: "Reference data:"
    }
    TextField {
      id: refDBPathField
      readOnly: true
      Layout.fillWidth: true
      text: testController.refDBFilePath
      function setPath(path) {
        testController.refDBFilePath = path;
      }
    }
    Button {
      text: "Browse"
      onClicked: {
        dbOpenFileDialog.targetPointer = refDBPathField;
        dbOpenFileDialog.open()
      }
    }
    Label {
      font.pointSize: 20
      text: testController.refDBReady ? "\u2713" : "X" //unicode checkmark
      color: testController.refDBReady ? "darkgreen" : "darkred"
    }
    //row
    Label {
      text: "DUT data:"
    }
    TextField {
      id: dutDBPathField
      readOnly: true
      Layout.fillWidth: true
      text: testController.dutDBFilePath
      function setPath(path) {
        testController.dutDBFilePath = path;
      }
    }
    Button {
      text: "Browse"
      onClicked: {
        dbOpenFileDialog.targetPointer = dutDBPathField;
        dbOpenFileDialog.open()
      }
    }
    Label {
      font.pointSize: 20
      text: testController.dutDBReady ? "\u2713" : "X" //unicode checkmark
      color: testController.dutDBReady ? "darkgreen" : "darkred"
    }
    //row
    //    Label {
    //      text: "SCPI server:"
    //    }
    //    TextField {
    //      selectByMouse: true
    //      Layout.fillWidth: true
    //      text: "127.0.0.1:12345"
    //    }
    //    Button {
    //      text: "Connect"
    //    }
    //    Label {
    //      font.pointSize: 20
    //      text: "\u2713" //unicode checkmark
    //      color: "darkgreen"
    //    }
    //row
    Label {
      text: "Epsilon:"
    }
    TextField {
      id: epsilonField
      selectByMouse: true
      Layout.fillWidth: true
      text: "1e-6"

      Component.onCompleted: {
          compareDataPage.testEpsilon = parseFloat(epsilonField.text);
      }

      onTextChanged: {
          compareDataPage.testEpsilon = parseFloat(epsilonField.text);
      }

      validator: DoubleValidator {
        top: 1
        bottom: 1e-7
      }
    }
    Label {
      id: epsilonValue
      text: parseFloat(epsilonField.text)
    }
    Label {
      font.pointSize: 20
      text: epsilonValue.text !== "nan" && epsilonField.acceptableInput ? "\u2713" : "X" //unicode checkmark
      color: epsilonValue.text !== "nan" && epsilonField.acceptableInput ? "darkgreen" : "darkred"
    }
    //row
    GridLayout {
      Layout.columnSpan: 4
      Layout.fillHeight: true
      Layout.fillWidth:true
      columns: 2
      Label {
        text: "Module blacklist:"
      }
      Label {
        text: "Component blacklist:"
      }
      Frame {
        Layout.fillHeight: true
        implicitWidth: compareDataPage.width/2
        ListView {
          anchors.fill: parent
          clip: true
          model: testController.entityBlacklist
          delegate: Label {
            text: modelData
          }
        }
      }
      Frame {
        Layout.fillHeight: true
        implicitWidth:  compareDataPage.width/2 * 0.99
        ListView {
          anchors.fill: parent
          clip: true
          model: testController.componentBlacklist
          delegate: Label {
            text: modelData
          }
        }
      }
    }
    //row
    Label {
      text: "Test progress:"
    }
    ProgressBar {
      from: 0
      to: testController.testCount
      value: testController.testProgress
      Layout.fillWidth: true
    }
    Button {
      text: "Start test"
      enabled: testController.testReady;
      onClicked: testController.startTest();
    }
  }
}
