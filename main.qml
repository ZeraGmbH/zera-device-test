import QtQuick 2.9
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.0
import VFDT 1.0

ApplicationWindow {
  id: displayWindow
  visible: true
  width: 1024
  height: 600
  title: qsTr("Device test")
  readonly property TestController testController: VFDT_TEST_CONTROLLER;

  TabBar {
    id: bar
    width: parent.width
    TabButton {
      text: qsTr("Record")
    }
    TabButton {
      text: qsTr("Compare")
    }
    TabButton {
      text: qsTr("Result")
      enabled: testController.testCount > 0 && (testController.testProgress === testController.testCount);
    }
  }

  StackLayout {
    anchors.margins: 8;
    anchors.topMargin: bar.height+8
    anchors.fill: parent
    currentIndex: bar.currentIndex
    RecordDataPage {
      id: recordTab
    }
    CompareDataPage {
      id: compareTab
    }
    ResultPage {
      id: resultTab
    }
  }
}
