#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

/*
 * This class handles capital.com session.
 * Functions : create, logout, get details
*/

#include <QObject>
#include <QSharedPointer>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include "account.h"


#define DEMO_URL_API "https://demo-api-capital.backend-capital.com/"
#define API_KEY "CWL338744FMDHVhQ"


class SessionManager : public QObject
{
  Q_OBJECT

public:
  explicit SessionManager(QObject *parent = nullptr);
  SessionManager(const QSharedPointer<QNetworkAccessManager*> &ptr_netmg, QObject *parent = nullptr);


  enum class REQUEST_CODE {
    NONE = 0,
    CREATE,
    DETAILS,
    SWITCH_ACCOUNT,
    LOGOUT
  };


  // mutators:
  void SetSecurityToken(const QByteArray &token);
  void SetCST(const QByteArray &cst);

  // accessors:
  const QByteArray & GetSecurityToken() const;
  const QByteArray & GetCST() const;
  const bool & IsCreated() const;


public slots:
  void SL_Init();

  void SL_Create(const QByteArray &identifier, const QByteArray &pwd, const bool &isEncrypted);
  void SL_Details();
  void SL_Switch(const QSharedPointer<Account*> &account);
  void SL_Logout();


private slots:
  void SL_OnNetMG_Finished(QNetworkReply *rep);


private:
  // functions:
  void HandleNetReply_SessionCreated(QNetworkReply *rep);
  void HandleNetReply_SessionLogout(QNetworkReply *rep);
  void HandleNetReply_AccountSwitched(QNetworkReply *rep);
  void HandleNetReply_SessionDetails(QNetworkReply *rep);


  // network access manager:
  QSharedPointer<QNetworkAccessManager*> _ptr_netmg;
  bool _initialized = false;

  // session infos:
  QByteArray
      _security_token = "",
      _cst = "";
  bool _created = false;
  REQUEST_CODE _current_req = REQUEST_CODE::NONE;

  // accounts list and active account:
  QList<Account*>           _accounts;
  QSharedPointer<Account*>  _active_account;
};

#endif // SESSIONMANAGER_H
