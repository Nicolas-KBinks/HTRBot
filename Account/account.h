#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QObject>

class Account : public QObject
{
  Q_OBJECT

public:
  explicit Account(QObject *parent = nullptr);
  Account(const QByteArray &id, const QByteArray &name, const QByteArray &type, const bool &preferred = false,
          QObject *parent = nullptr);

  // operators:
  bool operator == (const Account &other);


  // mutators:
  void SetID(const QByteArray &id);
  void SetName(const QByteArray &name);
  void SetType(const QByteArray &type);
  void SetCurrency(const QByteArray &currency);
  void SetPreferred(const bool &pref);
  void SetBalance(const double &val);
  void SetDeposit(const double &val);
  void SetProfitLoss(const double &val);
  void SetAvailable(const double &val);

  // accessors:
  const QByteArray & GetID() const;
  const QByteArray & GetName() const;
  const QByteArray & GetType() const;
  const QByteArray & GetCurrency() const;
  const bool & GetPreferred() const;
  double GetBalance() const;
  double GetDeposit() const;
  double GetProfitLoss() const;
  double GetAvailable() const;


private:
  QByteArray
      _id       = "",
      _name     = "",
      _type     = "",
      _currency = "";

  bool _preferred = false;

  double
      _balance    = 0,
      _deposit    = 0,
      _profitLoss = 0,
      _available  = 0;
};

#endif // ACCOUNT_H
