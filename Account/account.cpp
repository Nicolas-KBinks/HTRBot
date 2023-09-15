#include "account.h"


// default constructor:
Account::Account(QObject *parent)
    : QObject{parent}
{
}


// custom constructor:
Account::Account(const QByteArray &id, const QByteArray &name, const QByteArray &type, const bool &preferred,
                 QObject *parent)
    : QObject{parent},
    _id(id), _name(name), _type(type), _preferred(preferred)
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
void Account::SetCurrency(const QByteArray &currency) { _currency = currency; }
void Account::SetPreferred(const bool &pref) { _preferred = pref; }
void Account::SetBalance(const double &val) { _balance = val; }
void Account::SetDeposit(const double &val) { _deposit = val; }
void Account::SetProfitLoss(const double &val) { _profitLoss = val; }
void Account::SetAvailable(const double &val) { _available = val; }


// accessors:
const QByteArray & Account::GetID() const { return _id; }
const QByteArray & Account::GetName() const { return _name; }
const QByteArray & Account::GetType() const { return _type; }
const QByteArray & Account::GetCurrency() const { return _currency; }
const bool & Account::GetPreferred() const { return _preferred; }
double Account::GetBalance() const { return _balance; }
double Account::GetDeposit() const { return _deposit; }
double Account::GetProfitLoss() const { return _profitLoss; }
double Account::GetAvailable() const { return _available; }
