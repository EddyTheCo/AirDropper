#include"CreateDrop.hpp"
#include<QDataStream>
#include<QJsonDocument>
#include<QTimer>
#include"mydesigns.hpp"

using namespace qblocks;
DropBox::DropBox(QObject *parent):QObject(parent),m_validAddr(false),m_sent(false),out(nullptr),m_amount(1)

{
    /*m_amount_json({{"default",1},
                  {"largeValue",QJsonObject({{"value",1},{"unit","glow"}})},
                  {"shortValue",QJsonObject({{"value",0.000001},{"unit","rms"}})}}),
    m_deposit_json({{"default",1},
                  {"largeValue",QJsonObject({{"value",0},{"unit","glow"}})},
                  {"shortValue",QJsonObject({{"value",0.000},{"unit","rms"}})}})*/

    fillout();
    connect(this,&DropBox::addressChanged,this,&DropBox::checkAddress);
    connect(this,&DropBox::amountChanged,this,&DropBox::fillout);
    connect(this,&DropBox::metdataChanged,this,&DropBox::fillout);

}
DropBox::DropBox(const QJsonValue &prop, QObject *parent):QObject(parent),m_validAddr(false),m_sent(false),out(nullptr),
    m_alias((prop["alias"].isUndefined())?"":prop["alias"].toString()),
    m_address((prop["address"].isUndefined())?"":prop["address"].toString()),
    m_amount((prop["amount"].isUndefined())?1:prop["amount"].toInteger())

{
    checkAddress();
    connect(this,&DropBox::addressChanged,this,&DropBox::checkAddress);
    connect(this,&DropBox::amountChanged,this,&DropBox::fillout);
    connect(this,&DropBox::metdataChanged,this,&DropBox::fillout);
    if(!prop["message"].isUndefined())setMetdata(prop["message"].toString());
}
void DropBox::fillout()
{
    auto info=Node_Conection::rest_client->get_api_core_v2_info();
    QObject::connect(info,&Node_info::finished,this,[=]( ){
        auto used=0;
        if(m_validAddr)
        {
            auto addr_pair=qencoding::qbech32::Iota::decode(m_address);
            const auto addrr=Address::from_array(addr_pair.second);

            auto addr=Account::get_addr({0,0,0}).get_address();
            const auto now=QDateTime::currentDateTime().addDays(1).toSecsSinceEpoch();
            auto expir=Unlock_Condition::Expiration(now,addr);

            auto unlock=Unlock_Condition::Address(addrr);
            pvector<const Feature> features;
            if(!data_array().isNull())
            {
                const auto metadata=Feature::Metadata(data_array());
                features.push_back(metadata);
            }
            auto Pout=Output::Basic(m_amount,{unlock,expir},{},features);
            used=m_amount;
            auto mindepo=Client::get_deposit(Pout,info);


            if(mindepo>m_amount)
            {

                auto minBasic=Client::get_deposit(Output::Basic(0,{unlock}),info);

                auto retu=Unlock_Condition::Storage_Deposit_Return(addr,0);
                Pout=Output::Basic(mindepo+minBasic,{unlock,retu,expir},{},features);
                mindepo=Client::get_deposit(Pout,info);
                retu=Unlock_Condition::Storage_Deposit_Return(addr,
                                                              (mindepo-m_amount>minBasic)?(mindepo-m_amount):minBasic);
                used=(mindepo-m_amount>minBasic)?(mindepo):(minBasic+m_amount);
                Pout=Output::Basic(used,{unlock,retu,expir},{},features);
            }

            out=Pout;

        }
        m_amount_json=info->amount_json(m_amount);
        m_deposit_json=info->amount_json(used);
        emit amountJsonChanged();
        emit depositJsonChanged();
        info->deleteLater();
    });

}

void DropBox::checkAddress()
{
    auto info=Node_Conection::rest_client->get_api_core_v2_info();
    QObject::connect(info,&Node_info::finished,this,[=]( ){

        const auto addr_pair=qencoding::qbech32::Iota::decode(m_address);
        if(addr_pair.second.size()&&addr_pair.first==info->bech32Hrp)
        {
            m_validAddr=true;
            emit validAddrChanged();
            fillout();
        }
        else
        {
            m_validAddr=false;
            emit validAddrChanged();
        }
        info->deleteLater();
    });



}
void DropBox::setMetdata(QString data_m)
{
    auto var=QJsonDocument::fromJson(data_m.toUtf8());
    c_array vararr;
    if(!var.isNull())
    {
        vararr=var.toJson();
    }
    else
    {
        vararr=c_array(data_m.toUtf8());
    }
    if(vararr!=m_data)
    {
        m_data=vararr;
        emit metdataChanged();
    }

};
BoxModel::BoxModel(QObject *parent):QAbstractListModel(parent),amount(0),m_count(0),m_countNotSent(0)
{
    connect(this,&BoxModel::amountChanged,this,&BoxModel::checkAmount);
}
void BoxModel::checkAmount()
{
    auto info=Node_Conection::rest_client->get_api_core_v2_info();
    QObject::connect(info,&Node_info::finished,this,[=]( ){
        m_amount_json=info->amount_json(amount);
        emit amountJsonChanged();
        info->deleteLater();
    });
}

int BoxModel::count() const
{
    return boxes.size();
}
void BoxModel::clearBoxes()
{
    while(boxes.size())
    {
        rmBox(0);
    }
}

int BoxModel::rowCount(const QModelIndex &p) const
{
    Q_UNUSED(p)
    return boxes.size();
}
QHash<int, QByteArray> BoxModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[aliasRole] = "alias";
    roles[metdataRole] = "metdata";
    roles[addressRole] = "address";
    roles[validAddrRole] = "validAddr";
    roles[amountRole] = "amount";
    roles[sentRole] = "sent";
    roles[amountJsonRole] = "amountJson";
    roles[depositJsonRole] = "depositJson";
    return roles;
}
QVariant BoxModel::data(const QModelIndex &index, int role) const
{
    return boxes[index.row()]->property(roleNames().value(role));
}
bool BoxModel::setData(const QModelIndex &index, const QVariant &value, int role )
{
    const auto re=boxes[index.row()]->setProperty(roleNames().value(role),value);

    if(re)
    {
        emit dataChanged(index,index,QList<int>{role});
        return true;
    }
    return false;
}
bool BoxModel::setProperty(int i,QString role,const QVariant value)
{
    const auto ind=index(i);
    const auto rol=roleNames().keys(role.toUtf8());
    return setData(ind,value,rol.front());
}
QModelIndex BoxModel::index(int row, int column , const QModelIndex &parent ) const
{
    return createIndex(row,column);
}
void BoxModel::addBox(DropBox* o)
{
    connect(o,&DropBox::amountChanged,this,&BoxModel::getTotal);
    int i = boxes.size();

    beginInsertRows(QModelIndex(), i, i);
    boxes.append(o);
    emit countChanged(count());
    endInsertRows();

    auto ind=QPersistentModelIndex(index(i));
    connect(o,&DropBox::amountJsonChanged,this,[=](){
        emit dataChanged(ind,ind,QList<int>{roleNames().key("amountJson")});
    });
    connect(o,&DropBox::depositJsonChanged,this,[=](){
        emit dataChanged(ind,ind,QList<int>{roleNames().key("depositJson")});
    });
    connect(o,&DropBox::validAddrChanged,this,[=](){
        if(o->isValidAddr())
        {
            m_countNotSent++;
        }
        else
        {
            m_countNotSent--;
        }
        emit countNotSentChanged();
        emit dataChanged(ind,ind,QList<int>{roleNames().key("validAddr")});
    });
}
void BoxModel::getTotal()
{
    amount=0;
    for(const auto& v:boxes)amount+=v->m_amount;
}
void BoxModel::rmBox(int i) {
    if(!boxes[i]->isSent()&&boxes[i]->isValidAddr())
    {
        m_countNotSent--;
        emit countNotSentChanged();
    }
    amount-=boxes[i]->m_amount;
    beginRemoveRows(QModelIndex(),i,i);
    boxes[i]->deleteLater();
    boxes.remove(i);
    emit countChanged(count());
    endRemoveRows();
}


DropCreator::DropCreator(QObject *parent):QObject(parent),funds_(0),receiver(nullptr),
    monitor(new OutMonitor(this)),model_(new BoxModel(this)),state_(Null)
{
    auto foo=fooDesign::fooPrint();//unused function to link qml lib
}
void DropCreator::restart (void)
{
    if(receiver)receiver->deleteLater();
    receiver=new QObject(this);
    monitor->restart();
    model_->clearBoxes();
    setFunds(0);
    if(Node_Conection::state()==Node_Conection::Connected)
    {
        auto info=Node_Conection::rest_client->get_api_core_v2_info();
        connect(info,&Node_info::finished,this,[=]( ){
            setState(Ready);
            connect(monitor,&OutMonitor::finished,receiver,[=](auto  outs,auto jsonOuts)
            {
                checkOutputs(outs);
            });
            connect(monitor,&OutMonitor::gotNewOutsMqtt,receiver,[=](auto  outs,auto jsonOuts)
            {
                checkOutputs(outs);
            });
            const auto address=Account::addr_bech32({0,0,0},info->bech32Hrp);
            monitor->setRestCalls(2);
            monitor->getRestBasicOuts("address="+address);

            monitor->getRestBasicOuts("expirationReturnAddress="+address);
            monitor->subscribe("outputs/unlock/address/"+address);
            info->deleteLater();
        });

    }
}
pvector<const Output> BoxModel::newDrops(Node_info* info)
{
    pvector<const Output> var;
    for (const auto& v:boxes)
    {
        if(v->m_validAddr&&!v->isSent())
        {
            var.push_back(v->out);
        }
    }
    return var;
}
void BoxModel::setSent()
{
    for(auto i=0;i<boxes.size();i++)
    {

        if(!boxes[i]->isSent()&&boxes[i]->isValidAddr())
        {
            setProperty(i,"sent",true);
            m_countNotSent--;
            emit countNotSentChanged();
        }
    }
}

void DropCreator::create()
{
    setState(Null);
    QTimer::singleShot(15000,this,[=](){setState(Ready);});
    auto info=Node_Conection::rest_client->get_api_core_v2_info();
    QObject::connect(info,&Node_info::finished,this,[=]( ){

        auto mintmonitor=new OutMonitor(receiver);
        connect(mintmonitor,&OutMonitor::finished,receiver,[=](auto  outs,auto jsonOuts)
        {
            auto bundle=Account::get_addr({0,0,0});

            auto nouts=model_->newDrops(info);

            if(nouts.size())
            {
                auto outAmount=0;
                for(const auto &v:nouts)outAmount+=v->amount_;

                bundle.consume_outputs(outs,outAmount);

                if(bundle.amount>=outAmount)
                {
                    if(bundle.amount>outAmount)
                    {
                        const auto eddaddr=bundle.get_address();
                        const auto addunloc=Unlock_Condition::Address(eddaddr);
                        auto BaOut=Output::Basic(0,{addunloc});
                        auto min_deposit=Client::get_deposit(BaOut,info);
                        bundle.consume_outputs(outs,min_deposit);
                        if(bundle.amount>=outAmount+min_deposit)
                        {
                            BaOut->amount_=bundle.amount-outAmount;
                            nouts.push_back(BaOut);
                        }
                        else
                        {
                            setState(Ready);
                            emit notEnought(info->amount_json(outAmount+min_deposit-bundle.amount));
                            info->deleteLater();
                            mintmonitor->deleteLater();
                            return;
                        }


                    }
                    nouts.insert(nouts.end(), bundle.ret_outputs.begin(), bundle.ret_outputs.end());

                    auto Inputs_Commitment=Block::get_inputs_Commitment(bundle.Inputs_hash);

                    auto essence=Essence::Transaction(info->network_id_,bundle.inputs,Inputs_Commitment,nouts);

                    bundle.create_unlocks(essence->get_hash());

                    auto trpay=Payload::Transaction(essence,bundle.unlocks);

                    auto resp=Node_Conection::mqtt_client->get_subscription("transactions/"+trpay->get_id().toHexString() +"/included-block");
                    connect(resp,&ResponseMqtt::returned,this,[=](auto var){
                        model_->setSent();
                        setState(Ready);
                        resp->deleteLater();
                    });

                    auto block_=Block(trpay);
                    Node_Conection::rest_client->send_block(block_);
                }
                else
                {
                    setState(Ready);
                    emit notEnought(info->amount_json(outAmount-bundle.amount));
                }

            }
            info->deleteLater();
            mintmonitor->deleteLater();

        });
        const auto address=Account::addr_bech32({0,0,0},info->bech32Hrp);
        mintmonitor->setRestCalls(2);
        mintmonitor->getRestBasicOuts("address="+address+"&hasNativeTokens=false");
        mintmonitor->getRestBasicOuts("expirationReturnAddress="+address+"&hasNativeTokens=false");

    });


}
void DropCreator::setFunds(quint64 funds_m){

    if(funds_!=funds_m||funds_m==0)
    {
        funds_=funds_m;
        auto info=Node_Conection::rest_client->get_api_core_v2_info();
        QObject::connect(info,&Node_info::finished,receiver,[=]( ){
            funds_json=info->amount_json(funds_);
            emit fundsChanged();
            info->deleteLater();
        });
    }
}
void DropCreator::checkOutputs(std::vector<qiota::Node_output>  outs)
{

    quint64 total=0;
    if(outs.size()&&(outs.front().output()->type()==Output::Basic_typ))
    {

        for(const auto& v:outs)
        {
            std::vector<Node_output> var{v};
            auto bundle= Account::get_addr({0,0,0});
            bundle.consume_outputs(var);

            if(bundle.amount)
            {
                total+=bundle.amount;
                total_funds.insert(v.metadata().outputid_.toHexString(),bundle.amount);
                auto resp=Node_Conection::mqtt_client->get_outputs_outputId(v.metadata().outputid_.toHexString());
                connect(resp,&ResponseMqtt::returned,receiver,[=](QJsonValue data){
                    const auto node_output=Node_output(data);


                    if(node_output.metadata().is_spent_)
                    {
                        auto it=total_funds.constFind(node_output.metadata().outputid_.toHexString());
                        if(it!=total_funds.cend())
                        {
                            setFunds(funds_-it.value());
                            total_funds.erase(it);
                        }

                        resp->deleteLater();
                    }
                });

            }
            if(bundle.to_unlock.size())
            {
                const auto unixtime=bundle.to_unlock.front();
                const auto triger=(unixtime-QDateTime::currentDateTime().toSecsSinceEpoch())*1000;
                QTimer::singleShot(triger+5000,receiver,[=](){
                    auto resp=Node_Conection::mqtt_client->get_outputs_outputId(v.metadata().outputid_.toHexString());
                    connect(resp,&ResponseMqtt::returned,receiver,[=](QJsonValue data){
                        const auto node_output=Node_output(data);
                        checkOutputs({node_output});
                        resp->deleteLater();
                    });
                });
            }
            if(bundle.to_expire.size())
            {
                const auto unixtime=bundle.to_expire.front();
                const auto triger=(unixtime-QDateTime::currentDateTime().toSecsSinceEpoch())*1000;
                QTimer::singleShot(triger,receiver,[=](){
                    auto it=total_funds.find(v.metadata().outputid_.toHexString());
                    if(it!=total_funds.end())
                    {
                        setFunds(funds_-it.value());
                        total_funds.erase(it);
                    }
                });
            }


        }
        setFunds(funds_+total);
    }

}
void DropCreator::setDropArray(QString drops)
{
    auto var=QJsonDocument::fromJson(drops.toUtf8());
    if(!var.isNull())
    {
        auto arr=var.array();
        model_->newBoxFromArray(arr);
        m_drops=drops;
        emit dropArrayChanged();
    }
};
