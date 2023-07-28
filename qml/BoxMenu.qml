import QtQuick
import QtQuick.Layouts
import MyDesigns
import nodeConection
import account
import AirDropper
import QtQuick.Controls

Rectangle
{
    id:root
    property alias settings:settings_
    property bool incolum: height>300
    signal showSettings()
    color:CustomStyle.backColor2.lighter(1.5)
    radius:3
    ColumnLayout
    {
        anchors.fill: parent
        TextAddress
        {
            id:addr
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            Layout.margins: 5
            description: qsTr("Account")
            address: (Node_Conection.state)?Account.addr_bech32([0,0,0],Node_Conection.info().protocol.bech32Hrp):''
            color: CustomStyle.frontColor1
        }
        Text
        {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            font:CustomStyle.h3
            text:qsTr("Available Balance: ")
            horizontalAlignment:Text.AlignHCenter
            color: CustomStyle.frontColor1
            fontSizeMode:Text.Fit
        }
        AmountText
        {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            font:CustomStyle.h2
            jsob:(Node_Conection.state)?DropCreator.funds:{}
            horizontalAlignment:Text.AlignHCenter
            fontSizeMode:Text.Fit
        }

        GridLayout
        {
            rowSpacing: 10
            columnSpacing: 15
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            Layout.minimumHeight: 45+100*root.incolum
            columns: root.incolum ? 1 : 4
            MySettButton
            {
                id:settings_
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.maximumWidth: 50+150*root.incolum
                Layout.maximumHeight:  50
                onReleased: root.showSettings();
                text:qsTr("Settings")
            }
            PlusButton
            {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.maximumWidth: 50+150*root.incolum
                Layout.maximumHeight:  50
                text:qsTr("Add New")
                enabled:(Node_Conection.state&&DropCreator.state)
                onClicked:
                {
                    DropCreator.model.newBox();
                }
            }
            MyButton
            {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.maximumWidth: 50+150*root.incolum
                Layout.maximumHeight:  50
                text:qsTr("Send")
                enabled:(Node_Conection.state&&DropCreator.state&&DropCreator.model.countNotSent)
                onClicked:
                {
                    DropCreator.create();
                }
            }
            MyButton
            {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.maximumWidth: 50+150*root.incolum
                Layout.maximumHeight:  50
                text:qsTr("Add Drops")
                enabled:(Node_Conection.state&&DropCreator.state)
                onClicked:
                {
                    addArray.open();
                }
            }

        }
    }

    Connections {
        target: Account
        function onSeedChanged() {
            if(Node_Conection.state===Node_Conection.Connected)
            {
                addr.address=Account.addr_bech32([0,0,0],Node_Conection.info().protocol.bech32Hrp);
            }
        }
    }
    Connections {
        target: Node_Conection
        function onStateChanged() {
            if(Node_Conection.state===Node_Conection.Connected)
            {
                addr.address=Account.addr_bech32([0,0,0],Node_Conection.info().protocol.bech32Hrp);
            }
        }
    }
    Popup
    {
        id:addArray
        closePolicy: Popup.CloseOnPressOutside
        anchors.centerIn: Overlay.overlay
        background: Rectangle
        {
            id:bck
            color:CustomStyle.backColor1
            border.width:1
            border.color:CustomStyle.frontColor1
        }
        modal: true
        focus: true

        ColumnLayout
        {
            anchors.fill: parent

            MyTextArea
            {
                id:texArr
                label.text: "New drops(Json)"
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.maximumHeight: 800
                Layout.maximumWidth: 500
                Layout.minimumHeight: 400
                Layout.minimumWidth: 250
                Layout.preferredWidth: 450
                Layout.alignment: Qt.AlignCenter
                textarea.placeholderText:'[{\n "alias":"Best Subscriber",\n "address":"rms1qprfutdav6d0ynlxz3dmuykanze9drjrrqug8k7c5n0redn24sfcw5mxh7q",\n "message":"A present from estervtech",\n "amount": 100000\n}]'
            }
            MyButton
            {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.maximumHeight: 40
                Layout.maximumWidth: 70
                text:qsTr("Add")
                enabled: texArr.textarea.text!==""
                onClicked: DropCreator.setDropArray(texArr.textarea.text);
                Layout.alignment: Qt.AlignVCenter|Qt.AlignRight
            }
        }

    }
}
