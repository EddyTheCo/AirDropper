import QtQuick
import QtQuick.Layouts
import MyDesigns
import QtQuick.Controls
import nodeConection

Rectangle
{
    id:root
    required property string alias;
    required property string address;
    required property string metdata;
    required property int index;
    required property bool sent;
    required property bool validAddr;
    required property var amountJson;
    required property var depositJson;
    required property int amount;
    property bool selected:false;

    radius:5

    height:250+((root.selected)?50:0)
    color:CustomStyle.backColor1.lighter(1.4)

    RowLayout
    {
        id:rows
        width:parent.width*0.95
        height:parent.height*0.95
        anchors.centerIn: parent

        ColumnLayout
        {
            id:dat
            Layout.fillWidth: true
            Layout.fillHeight:  true
            Layout.minimumWidth: 150
            spacing:5
            MyTextField
            {
                id:aliF
                Layout.fillWidth: true
                Layout.fillHeight:  true
                Layout.maximumHeight: 60
                Layout.maximumWidth: 300
                Layout.alignment: Qt.AlignTop
                label.text: "Name"
                textfield.text: (root.alias)?root.alias:"Drop " + root.index
                textfield.placeholderText: "The best follower"
                visible: root.selected
            }
            MyTextField
            {
                id:addrF
                Layout.fillWidth: true
                Layout.fillHeight:  true
                Layout.alignment: Qt.AlignTop
                Layout.maximumHeight: 60
                Layout.maximumWidth: 450
                label.text: "Address"
                textfield.text: (root.address)?root.address:''
                textfield.placeholderText:(Node_Conection.state)?Node_Conection.info().protocol.bech32Hrp+"1...(mandatory)":""
                visible: root.selected

            }
            Text
            {
                id:aliasT
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop
                text: (root.alias)?root.alias:"Drop "+ root.index
                fontSizeMode:Text.Fit
                font: CustomStyle.h1
                color: CustomStyle.frontColor2
                visible:!root.selected
            }
            TextAddress
            {
                id:addrT
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
                Layout.margins: 5
                address: (root.address)?root.address:(Node_Conection.state)?Node_Conection.info().protocol.bech32Hrp+"1...(mandatory)":""
                fontSizeMode:Text.VerticalFit
                font: CustomStyle.h3
                color: (root.validAddr)?CustomStyle.frontColor2:"red"
                visible:!root.selected
            }
            MyFrame {
                id:metframe
                description: qsTr("Message")
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.maximumHeight: 30 + collapsed*60
                visible: root.metdata!==""|| root.selected
                ScrollView {
                    id:scrl
                    visible:metframe.collapsed
                    anchors.fill: parent
                    TextArea
                    {
                        id:metadata
                        visible:metframe.collapsed
                        readOnly:root.sent||!root.selected
                        placeholderText: qsTr('A present from estervtech')
                        anchors.fill: parent
                        text: root.metdata;
                        onEditingFinished: root.ListView.view.model.setProperty(root.index,"metdata",metadata.text);
                        color:CustomStyle.frontColor1
                        placeholderTextColor:CustomStyle.midColor1
                    }
                }
            }
            MyTextField
            {
                id:amouF
                Layout.fillWidth: true
                Layout.fillHeight:  true
                Layout.maximumHeight: 60
                Layout.maximumWidth: 100
                label.text: "Amount"+((Node_Conection.state)?"("+Node_Conection.info().baseToken.subunit+")":"")
                textfield.text: (root.amount)?root.amount:''
                textfield.validator: IntValidator {bottom: 1}
                visible: root.selected
            }


        }
        ColumnLayout
        {
            Layout.fillWidth: true
            Layout.minimumWidth: 100
            Layout.alignment: Qt.AlignRight| Qt.AlignVCenter
            AmountText
            {
                Layout.fillWidth: true
                font:CustomStyle.h2
                jsob:(Node_Conection.state)?root.amountJson:{}
                horizontalAlignment:Text.AlignHCenter
                fontSizeMode:Text.Fit
                visible:!root.selected
            }
            AmountText
            {
                Layout.fillWidth: true
                font:CustomStyle.h3
                jsob:(Node_Conection.state)?root.depositJson:{}
                horizontalAlignment:Text.AlignHCenter
                fontSizeMode:Text.Fit
                visible:!root.selected&&(root.depositJson.largeValue.value>root.amountJson.largeValue.value)
            }
        }



    }


    CloseButton
    {
        id: rmv
        anchors.right: root.right
        anchors.top: root.top
        anchors.margins: 10
        width:14
        height:14
        onClicked:
        {
            root.ListView.view.model.rmBox(root.index);
        }

    }
    Text
    {
        id:invalid
        anchors.horizontalCenter:  root.horizontalCenter
        anchors.bottom: root.bottom
        text: "Invalid address"
        fontSizeMode:Text.Fit
        font: CustomStyle.h3
        color: "red"
        visible:root.address&&!root.validAddr&&!root.selected
    }
    MyButton
    {
        id: edit
        anchors.right: root.right
        anchors.bottom: root.bottom
        anchors.margins: 10
        width:60
        height:30
        visible: !root.sent
        text:qsTr((root.selected)?"Save":"Edit")
        onClicked:
        {

            if(root.selected)
            {
                root.ListView.view.model.setProperty(root.index,"address",addrF.textfield.text);
                if(amouF.textfield.text)root.ListView.view.model.setProperty(root.index,"amount",parseInt(amouF.textfield.text,10));
                root.ListView.view.model.setProperty(root.index,"metdata",metadata.text);
                root.ListView.view.model.setProperty(root.index,"alias",aliF.textfield.text);
            }
            root.selected=!root.selected;
        }

    }
}


