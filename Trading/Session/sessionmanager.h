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

#include "Logs/log.h"
#include "Trading/Accounts/account.h"
#include "Encryption/encryptionmanager.h"
#include "Identity/identity.h"


#define DEMO_URL_API "https://demo-api-capital.backend-capital.com/"


class SessionManager : public QObject
{
  Q_OBJECT

public:
  explicit SessionManager(QObject *parent = nullptr);
  SessionManager(QNetworkAccessManager *netmg, EncryptionManager *encryption_mg, QObject *parent = nullptr);


  enum class REQUEST_CODE {
    NONE = 0,
    ENCRYPTION_KEY,
    CREATE,
    DETAILS,
    SWITCH_ACCOUNT,
    LOGOUT
  };


  // mutators:
  void SetSecurityToken(const QByteArray &token);
  void SetCST(const QByteArray &cst);
  void SetAPIKey(const QByteArray &key, const bool &isEncrypted);

  // accessors:
  const QByteArray & GetSecurityToken() const;
  const QByteArray & GetCST() const;
  const QByteArray & GetAPIKey() const;
  const QByteArray & GetEncryptionKey() const;
  const bool & IsCreated() const;


signals:
  void SI_AddLog(const Log::TYPE &type, const QByteArray &content, const qint64 &timestamp);


public slots:
  void SL_Init();
  void SL_OnIdentityCreated(Identity *ident);
  void SL_Create(const QByteArray &identifier, const QByteArray &pwd, const bool &isEncrypted);
  void SL_Details();
  void SL_Switch(const QSharedPointer<Account*> &account);
  void SL_Logout();

  void RequestEncryptionKey();


private slots:
  void SL_OnNetMG_Finished(QNetworkReply *rep);


private:
  // functions:
  void HandleNetReply_EncryptionKey(QNetworkReply *rep);
  void HandleNetReply_SessionCreated(QNetworkReply *rep);
  void HandleNetReply_SessionLogout(QNetworkReply *rep);
  void HandleNetReply_AccountSwitched(QNetworkReply *rep);
  void HandleNetReply_SessionDetails(QNetworkReply *rep);


  // network access manager:
  QNetworkAccessManager *_netmg = nullptr;
  QSharedPointer<EncryptionManager*> _encryption_mg;
  bool _initialized = false;

  // session infos:
  QByteArray
      _security_token = "",
      _cst = "",
      _api_key = "",
      _encryption_key = "";
  bool _created = false;
  REQUEST_CODE _current_req = REQUEST_CODE::NONE;

  /* TO DO :
   *   add encryption features within SessionManager
   *   use encryption with requested key from server to encode user password
   */

  // accounts list and active account:
  QList<Account*>           _accounts;
  QSharedPointer<Account*>  _active_account;
};

#endif // SESSIONMANAGER_H
