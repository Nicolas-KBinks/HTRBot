#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QObject>

class Account : public QObject
{
  Q_OBJECT

public:
  explicit Account(QObject *parent = nullptr);
  Account(const QByteArray &id, const QByteArray &name, const QByteArray &type,
          QObject *parent = nullptr);

  // operators:
  bool operator == (const Account &other);


  // mutators:
  void SetID(const QByteArray &id);
  void SetName(const QByteArray &name);
  void SetType(const QByteArray &type);
  void SetPrefered(const bool &pref);

  // accessors:
  const QByteArray & GetID() const;
  const QByteArray & GetName() const;
  const QByteArray & GetType() const;
  const bool & GetPrefered() const;


private:
  QByteArray _id, _name, _type;
  bool _prefered = false;
};

#endif // ACCOUNT_H
