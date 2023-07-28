import QtQuick.Controls
import QtQuick
import QtQuick.Layouts
import nodeConection
import account
import CustomStyle 1.0
Drawer
{
    id: drawer
    closePolicy: Popup.CloseOnPressOutside
    focus:true
    modal:true
    background: Rectangle
    {
        color:CustomStyle.backColor1
    }

    ColumnLayout  //is better to use a ScrollView here
    {
        width:parent.width
        height:100
        Node_Connections
        {
            id:conn_
            Layout.fillWidth: true
            Layout.minimumHeight: 30
            collapsed:1.0
        }
        AccountQml
        {
            id:acc_
            Layout.fillWidth: true
            Layout.minimumHeight: 30
        }

    }

}
