#include "account.h"


// default constructor:
Account::Account(QObject *parent)
    : QObject{parent}
{
}


// custom constructor:
Account::Account(const QByteArray &id, const QByteArray &name, const QByteArray &type, QObject *parent)
    : QObject{parent},
    _id(id), _name(name), _type(type)
{
}



// operators:
bool Account::operator == (const Account &other)
{
  if ( (!this->GetID().isEmpty() && other.GetID().isEmpty()) &&
      this->GetID().compare(other.GetID(), Qt::CaseSensitive) == 0)
    return true;

  return false;
}


// mutators:
void Account::SetID(const QByteArray &id) { _id = id; }
void Account::SetName(const QByteArray &name) { _name = name; }
void Account::SetType(const QByteArray &type) { _type = type; }
void Account::SetPrefered(const bool &pref) { _prefered = pref; }


// accessors:
const QByteArray & Account::GetID() const { return _id; }
const QByteArray & Account::GetName() const { return _name; }
const QByteArray & Account::GetType() const { return _type; }
const bool & Account::GetPrefered() const { return _prefered; }
