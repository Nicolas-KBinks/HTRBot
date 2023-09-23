#ifndef IDENTITY_H
#define IDENTITY_H

/* This class represents a unique user account [on capital.com]
 * It is use to authenticate user upon connection with the api server
 * After being authenticated, the user's encryption key returned by the server is store here
*/

#include <QObject>

class Identity : public QObject
{
  Q_OBJECT

public:
  explicit Identity(QObject *parent = nullptr);
  Identity(const Identity &other);
  Identity(const QByteArray &cred_id, const QByteArray &cred_pwd = "", const QByteArray &api_key = "",
           const QByteArray &encryption_key = "", const qint64 &id = 0, QObject *parent = nullptr);
  ~Identity();

  // operators:
  bool operator == (const Identity &other);
  Identity * operator = (const Identity &other);

  // mutators:
  void SetID(const qint64 &id);
  void SetCredentials(const QPair<QByteArray,QByteArray> &creds);
  void SetKeys(const QPair<QByteArray,QByteArray> &keys);

  // accessors:
  qint64 GetID() const;
  const QPair<QByteArray,QByteArray> & GetCredentials() const;
  const QPair<QByteArray,QByteArray> & GetKeys() const;

private:
  qint64 _id = 0;
  QPair<QByteArray,QByteArray>
      _creds = {"", ""},    // first = cred_id, second = pwd
      _keys = {"", ""};     // first = api_key, second = encryption_key
};

#endif // IDENTITY_H
