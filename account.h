#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QObject>

class Account : public QObject
{
  Q_OBJECT

public:
  explicit Account(QObject *parent = nullptr);
  Account(const QByteArray &id, const QByteArray &name, QObject *parent = nullptr);

  void SetID(const QByteArray &id);
  void SetName(const QByteArray &name);
  void SetPrefered(const bool &pref);

  const QByteArray & GetID() const;
  const QByteArray & GetName() const;
  const bool & GetPrefered() const;

private:
  QByteArray _id, _name, _type;
  bool _prefered = false;
};

#endif // ACCOUNT_H
