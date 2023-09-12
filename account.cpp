#include "account.h"

Account::Account(QObject *parent)
    : QObject{parent}
{
}


Account::Account(const QByteArray &id, const QByteArray &name, QObject *parent)
    : QObject{parent},
    _id(id), _name(name)
{
}




void Account::SetID(const QByteArray &id) { _id = id; }
void Account::SetName(const QByteArray &name) { _name = name; }
void Account::SetPrefered(const bool &pref) { _prefered = pref; }


const QByteArray & Account::GetID() const { return _id; }
const QByteArray & Account::GetName() const { return _name; }
const bool & Account::GetPrefered() const { return _prefered; }
