import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Dialogs 1.3
import VFDT 1.0

Item {
  id: resultPage
  readonly property TestController testController: VFDT_TEST_CONTROLLER;
  readonly property var dutTestResult: testController.dutResult;
  readonly property int rowHeight: 40
  property int failCount: 0
  property int passCount: 0
  property int untestedCount: 0;

  property var resultModel: [];
  onDutTestResultChanged: {
    updateResultModel();
  }

  FileDialog {
    id: fileSaveDialog
    width: resultPage.width*0.9
    height: resultPage.height*0.9
    selectExisting: false
    selectMultiple: false
    selectFolder: false
    nameFilters: [ "Comma-separated values  (*.csv)" ];
    defaultSuffix: ".csv"
    onAccepted: {
      var path =  fileSaveDialog.fileUrl.toString();
      path = path.replace(/^(file:\/{2})/,"");
      testController.writeResultFile(path);
    }
  }

  function updateResultModel() {
    resultModel = [];
    passCount = 0;
    failCount = 0;
    untestedCount = 0;
    for (var testKey in dutTestResult) {
      var hideResult = false;
      if(dutTestResult[testKey] === "PASS")
      {
        resultPage.passCount++;
      }
      else if(dutTestResult[testKey] === "FAIL")
      {
        resultPage.failCount++;
      }
      else
      {
        untestedCount++;
        hideResult = hideUntested.checked;
      }

      if(hideResult === false)
      {
        resultModel.push({"testKey": testKey, "dutResult": dutTestResult[testKey]})
      }
    }
    resultModelChanged();
  }

  RowLayout {
    id: controlBar
    width: resultPage.width
    height: resultPage.rowHeight
    anchors.top: parent.top

    CheckBox {
      id: hideUntested
      text: "Hide untested"
      onCheckedChanged: {
        resultPage.updateResultModel()
      }
    }

    Label {
      text: "Epsilon: " + testController.epsilonValue
    }

    Label {
      text: "Results: " + testController.testCount + " Passed: " + passCount + " Failed: " + failCount +" Untested: " + untestedCount;
    }

    Button {
      text: "Save"
      onClicked: fileSaveDialog.open();
    }
  }

  ListView {
    anchors.fill: parent
    anchors.topMargin: controlBar.height+4
    spacing: 2
    model: resultModel
    clip: true
    ScrollBar.vertical: ScrollBar {
      width: 16
      active: true
      onActiveChanged: if(active === false) active = true;
    }

    delegate: Rectangle {
      height: resultPage.rowHeight
      width: resultPage.width - 16
      color: {
        switch(modelData.dutResult) {
        case 'PASS':
          return "yellowgreen"
        case 'FAIL':
          return "tomato"
        default:
          return "silver"
        }
      }

      RowLayout {
        anchors.fill: parent
        anchors.leftMargin: height/4
        anchors.rightMargin: height/4
        Label {
          text: modelData.testKey
          Layout.fillWidth: true
          font.pointSize: 16
        }
        Label {
          text: modelData.dutResult
          horizontalAlignment: Text.AlignRight
          font.pointSize: 16
        }
      }
    }
  }
}
