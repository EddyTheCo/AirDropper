#pragma once

#include <QAbstractListModel>
#include<QObject>
#include<QString>
#include <QtQml/qqmlregistration.h>

#include<account.hpp>
#include"nodeConnection.hpp"
#include"outmonitor.hpp"
#include<QHash>


class DropBox :public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString  alias MEMBER m_alias NOTIFY  aliasChanged)
    Q_PROPERTY(QString  address MEMBER m_address NOTIFY addressChanged)
    Q_PROPERTY(bool  validAddr READ isValidAddr NOTIFY validAddrChanged)
    Q_PROPERTY(bool  sent READ isSent WRITE setSent NOTIFY sentChanged)
    Q_PROPERTY(QString  metdata READ metdata WRITE setMetdata NOTIFY metdataChanged)
    Q_PROPERTY(QJsonObject  amountJson READ getAmountJson  NOTIFY amountJsonChanged)
    Q_PROPERTY(QJsonObject  depositJson READ getDepositJson  NOTIFY depositJsonChanged)
    Q_PROPERTY(quint64 amount MEMBER m_amount NOTIFY amountChanged);
    QML_ELEMENT

public:
    DropBox(QObject *parent = nullptr);
    DropBox(const QJsonValue &prop,QObject *parent);
    QString metdata()const{return (m_data.isEmpty())?QString():QString(m_data);}
    c_array data_array()const{return m_data;}
    void setMetdata(QString data);
    QJsonObject getAmountJson(void)const{return m_amount_json;}
    QJsonObject getDepositJson(void)const{return m_deposit_json;}
    bool isValidAddr()const{return m_validAddr;}
    bool isSent()const{return m_sent;}
    void fillout();
    void setSent(bool se){   
        if(m_validAddr&&m_sent!=se)
        {
            m_sent=se;
            emit sentChanged();
        }
    }

signals:
    void aliasChanged();
    void addressChanged();
    void metdataChanged();
    void amountChanged();
    void amountJsonChanged();
    void depositJsonChanged();
    void validAddrChanged();
    void sentChanged();

public:
    QString m_alias,m_address;
    QJsonObject m_amount_json,m_deposit_json;
    quint64 m_amount;
    bool m_validAddr,m_sent;
    void checkAddress(void);
    c_array m_data;
    std::shared_ptr<Output> out;

};

class BoxModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int countNotSent MEMBER m_countNotSent NOTIFY countNotSentChanged)
    Q_PROPERTY(QJsonObject  amountJson READ getAmountJson  NOTIFY amountJsonChanged)
    QML_ELEMENT

public:
    enum ModelRoles {
        aliasRole = Qt::UserRole + 1,
        metdataRole,addressRole,validAddrRole,amountRole,amountJsonRole,sentRole,depositJsonRole};

    int count() const;
    explicit BoxModel(QObject *parent = nullptr);
    Q_INVOKABLE void clearBoxes();
    QJsonObject getAmountJson()const{return m_amount_json;}
    Q_INVOKABLE void newBox(void)
    {
        addBox(new DropBox((QObject*)this));
    };
    void newBoxFromArray(const QJsonArray& drops)
    {
        for(const auto v:drops)
        {
            addBox(new DropBox(v,(QObject*)this));
        }
    };
    void addBox(DropBox* nbox);
    pvector<const Output> newDrops(Node_info* info);
    Q_INVOKABLE void rmBox(int i);
    void setSent();

    Q_INVOKABLE bool setProperty(int i, QString role, const QVariant value);

    int rowCount(const QModelIndex &p) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
    QHash<int, QByteArray> roleNames() const;

signals:
    void countChanged(int count);
    void countNotSentChanged();
    void amountChanged();
    void amountJsonChanged();

private:
    void getTotal(void);
    void checkAmount(void);
    int m_count,m_countNotSent;
    QList<DropBox*> boxes;
    quint64  amount;
    QJsonObject m_amount_json;
};


class DropCreator : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QJsonObject  funds READ funds  NOTIFY fundsChanged)
    Q_PROPERTY(BoxModel*  model READ model  CONSTANT)
    Q_PROPERTY(Stte  state READ state  NOTIFY stateChanged)
    Q_PROPERTY(QString  dropArray READ getDropArray WRITE setDropArray NOTIFY dropArrayChanged)
    QML_ELEMENT
    QML_SINGLETON

public:
    DropCreator(QObject *parent = nullptr);
    enum Stte {
        Null,
        Ready,
    };
    Q_ENUM(Stte)
    Q_INVOKABLE void create();
    QString getDropArray()const{return m_drops;}
    Q_INVOKABLE void setDropArray(QString drops);
    Q_INVOKABLE void restart(void);
    Stte state()const{return state_;}
    void setState(Stte state_m){if(state_m!=state_){state_=state_m;emit stateChanged();}};

    QJsonObject funds(void)const{return funds_json;}
    void setFunds(quint64 funds_m);
    BoxModel* model()const{return model_;}


signals:
    void fundsChanged();
    void created(void);
    void newBlock(QString);
    void notEnought(QJsonObject);
    void stateChanged();
    void dropArrayChanged();

private:
    void checkOutputs(std::vector<qiota::Node_output>  outs);
    void createdBlock(qblocks::c_array);
    OutMonitor* monitor;
    QString m_drops;
    quint64 funds_;
    QJsonObject funds_json;
    QObject* receiver;
    QHash<QString,quint64> total_funds;
    BoxModel* model_;
    Stte state_;

};



