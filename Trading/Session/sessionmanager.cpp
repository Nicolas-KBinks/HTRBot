#include "sessionmanager.h"
#include <QDebug>


// default constructor:
SessionManager::SessionManager(QObject *parent)
    : QObject{parent}
{
}


// custom constructor:
SessionManager::SessionManager(QNetworkAccessManager *netmg, EncryptionManager *encryption_mg, QObject *parent)
    : QObject{parent}, _netmg(netmg), _encryption_mg( QSharedPointer<EncryptionManager*>::create(encryption_mg) )
{
}



/* ================ public slots ================ */

// initialize session manager:
void SessionManager::SL_Init()
{
  if (_initialized)
    return;

  emit SI_AddLog(Log::TYPE::INFO, "intializing session manager..", QDateTime::currentMSecsSinceEpoch());

  if (_netmg) {
    connect(_netmg, &QNetworkAccessManager::finished, this, &SessionManager::SL_OnNetMG_Finished);
    _netmg->connectToHost(DEMO_URL_API);
  }

  _initialized = true;
  emit SI_AddLog(Log::TYPE::INFO, "session manager initialized", QDateTime::currentMSecsSinceEpoch());
}

// request encryption key:
void SessionManager::RequestEncryptionKey()
{
  if (!_netmg)
    return;

  _current_req = REQUEST_CODE::ENCRYPTION_KEY;

  QNetworkRequest req(QString("%1/api/v1/session").arg(DEMO_URL_API));

  req.setRawHeader("X-CAP-API-KEY", _api_key);
  _netmg->get(req);
}

// create a session:
void SessionManager::SL_Create(const QByteArray &identifier, const QByteArray &pwd, const bool &isEncrypted)
{
  if (_encryption_key.isEmpty()) {
    this->RequestEncryptionKey();
    return;
  }

  if (!_netmg || identifier.isEmpty() || pwd.isEmpty() || this->IsCreated())
    return;

  _current_req = REQUEST_CODE::CREATE;

  emit SI_AddLog(Log::TYPE::INFO, "request session create..", QDateTime::currentMSecsSinceEpoch());

  // create request and set rawHeaders:
  QNetworkRequest req(QString("%1/api/v1/session").arg(DEMO_URL_API));

  req.setRawHeader("X-CAP-API-KEY", _api_key);
  req.setRawHeader("Content-Type", "application/json");

  // create request body:
  QJsonObject obj;

  obj.insert("identifier", QString::fromUtf8(identifier));
  obj.insert("password", QString::fromUtf8(pwd));
  obj.insert("encryptedPassword", isEncrypted);

  // connect to host and execute request:
  QJsonDocument doc(obj);

  _netmg->post(req, doc.toJson(QJsonDocument::Compact));
}


// get details about a session:
void SessionManager::SL_Details()
{
  if (_security_token.isEmpty() || _cst.isEmpty() || !_netmg || !this->IsCreated())
    return;

  _current_req = REQUEST_CODE::DETAILS;

  emit SI_AddLog(Log::TYPE::INFO, "request session details..", QDateTime::currentMSecsSinceEpoch());

  // create request and set rawHeaders:
  QNetworkRequest req(QString("%1/api/v1/session").arg(DEMO_URL_API));

  req.setRawHeader("X-SECURITY-TOKEN", this->GetSecurityToken());
  req.setRawHeader("CST", this->GetCST());

  // execute request:
  _netmg->get(req);
}


// switch active account:
void SessionManager::SL_Switch(const QSharedPointer<Account*> &account)
{
  if (_security_token.isEmpty() || _cst.isEmpty() || account.isNull() || !this->IsCreated())
    return;

  _current_req = REQUEST_CODE::SWITCH_ACCOUNT;

  emit SI_AddLog(Log::TYPE::INFO, "request switch account..", QDateTime::currentMSecsSinceEpoch());

  // create request and set rawHeaders:
  QNetworkRequest req(QString("%1/api/v1/session").arg(DEMO_URL_API));

  req.setRawHeader("X-SECURITY-TOKEN", this->GetSecurityToken());
  req.setRawHeader("CST", this->GetCST());

  // create request body:
  QJsonObject obj;

  obj.insert("accountId", QString::fromUtf8((*account)->GetID()) );

  // execute request:
  QJsonDocument doc(obj);

  _netmg->put(req, doc.toJson(QJsonDocument::Compact));
}


// session logout:
void SessionManager::SL_Logout()
{
  if (_security_token.isEmpty() || _cst.isEmpty() || !_netmg || !this->IsCreated())
    return;

  _current_req = REQUEST_CODE::LOGOUT;

  emit SI_AddLog(Log::TYPE::INFO, "request session logout..", QDateTime::currentMSecsSinceEpoch());

  QNetworkRequest req(QString("%1/api/v1/session").arg(DEMO_URL_API));

  req.setRawHeader("X-SECURITY-TOKEN", this->GetSecurityToken());
  req.setRawHeader("CST", this->GetCST());

  _netmg->deleteResource(req);
}

/* ================ end of public slots ================ */



/* ================ private slots ================ */

// handles network manager replies:
void SessionManager::SL_OnNetMG_Finished(QNetworkReply *rep)
{
  if (!rep)
    return;

  // extract HTTP code:
  qint16 rep_code = rep->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

  emit SI_AddLog(Log::TYPE::DBG,
                 QString("SessionManager::reply : rep_code = %1").arg(rep_code).toUtf8(),
                 QDateTime::currentMSecsSinceEpoch());

  emit SI_AddLog(Log::TYPE::DBG,
                 QString("==== reply ====<br />%1<br />").arg(rep->readAll()).toUtf8(), QDateTime::currentMSecsSinceEpoch());

  switch (rep_code) {
    // 200 : OK
  case 200:
    break;

    // 404 : Not Found
  case 404:
    return;
    break;

  default:
    return;
    break;
  }

  if (!this->IsCreated() && _current_req == REQUEST_CODE::ENCRYPTION_KEY)
    this->HandleNetReply_EncryptionKey(rep);


  // handles reply from server upon session created:
  if (!this->IsCreated() && _current_req == REQUEST_CODE::CREATE)
    this->HandleNetReply_SessionCreated(rep);

  // session destroyed:
  if (this->IsCreated() && _current_req == REQUEST_CODE::LOGOUT)
    this->HandleNetReply_SessionLogout(rep);
}

/* ================ end of private slots ================ */


/* ================ private functions ================ */
void SessionManager::HandleNetReply_EncryptionKey(QNetworkReply *rep)
{
  if (!rep)
    return;

  QByteArray datas = rep->readAll();
  emit SI_AddLog(Log::TYPE::DBG, QString("SessionManager::reply :<br />%1<br />").arg(datas).toUtf8(),
                 QDateTime::currentMSecsSinceEpoch());

  QJsonParseError err;
  QJsonDocument doc = QJsonDocument::fromJson(datas, &err);

  if (doc.isObject()) {
    QJsonObject obj = doc.object();
    _encryption_key = obj.value("encryptionKey").toString().toUtf8();

    /*if (!_encryption_mg.isNull())
      (*_encryption_mg.data())->SetEncryptionKey(_encryption_key);*/
  }

  _current_req = REQUEST_CODE::NONE;
}

// handles reply to - create session:
void SessionManager::HandleNetReply_SessionCreated(QNetworkReply *rep)
{
  if (!rep)
    return;

  // extract security token and CST:
  if (rep->hasRawHeader("X-SECURITY-TOKEN"))
    this->SetSecurityToken(rep->rawHeader("X-SECURITY-TOKEN"));

  if (rep->hasRawHeader("CST"))
    this->SetCST(rep->rawHeader("CST"));


  // retrieves and extracts datas from reply:
  QByteArray datas = rep->readAll();
  QJsonParseError err;
  QJsonDocument doc = QJsonDocument::fromJson(datas, &err);

  if (doc.isObject()) {
    QJsonObject
        obj = doc.object(),
        obj_account_info = obj.value("accountInfo").toObject();

    // extract current account infos:
    double
        available = obj_account_info.value("available").toDouble(),
        balance = obj_account_info.value("balance").toDouble(),
        deposit = obj_account_info.value("deposit").toDouble(),
        profitLoss = obj_account_info.value("profitLoss").toDouble();

    QByteArray
        currency = obj.value("currencyIsoCode").toString().toUtf8(),
        currentAccountID = obj.value("currentAccountId").toString().toUtf8();

    // extract accounts list:
    QJsonArray ar_accounts = obj.value("accounts").toArray();

    for (int n = 0; n < ar_accounts.size(); n++) {
      QJsonObject o = ar_accounts.at(n).toObject();

      Account *ac = new Account(o.constFind("accountId").value().toString().toUtf8(),
                                o.constFind("accountName").value().toString().toUtf8(),
                                o.constFind("accountType").value().toString().toUtf8(),
                                o.constFind("preferred").value().toBool(), this);

      // check if account already presents in accounts list:
      if (_accounts.contains(ac)) {
        delete ac;
        ac = nullptr;
        continue;
      }

      if (ac->GetID().compare(currentAccountID, Qt::CaseSensitive) == 0) {
        ac->SetBalance(balance);
        ac->SetDeposit(deposit);
        ac->SetProfitLoss(profitLoss);
        ac->SetAvailable(available);
        ac->SetCurrency(currency);
      }

      _accounts.append(ac);
    } // end of iteration through accounts list

  } // end of doc.isObject


  _created = true;
  _current_req = REQUEST_CODE::NONE;

  emit SI_AddLog(Log::TYPE::INFO, "session created", QDateTime::currentMSecsSinceEpoch());


  qDebug().noquote() << QString("[SessionManager 0x%1] : session created")
                            .arg((qint64)(this), 8, 16, QChar('0'));
}


// handles reply to - get session details:
void SessionManager::HandleNetReply_SessionDetails(QNetworkReply */*rep*/)
{
}


// handles reply to - switch account:
void SessionManager::HandleNetReply_AccountSwitched(QNetworkReply */*rep*/)
{
}


// handles reply to - logout:
void SessionManager::HandleNetReply_SessionLogout(QNetworkReply *rep)
{
  // to be sure nothing stays in io-device buffer:
  rep->readAll();


  // cleaning up security token and cst and reset session status:
  this->SetSecurityToken("");
  this->SetCST("");

  _created = false;
  _current_req = REQUEST_CODE::NONE;

  emit SI_AddLog(Log::TYPE::INFO, "session logged out", QDateTime::currentMSecsSinceEpoch());
}

/* ================ end of private functions ================ */



// mutators:
void SessionManager::SetSecurityToken(const QByteArray &token) { _security_token = token; }
void SessionManager::SetCST(const QByteArray &cst) { _cst = cst; }
void SessionManager::SetAPIKey(const QByteArray &key, const bool &isEncrypted)
{
  _api_key = key;
  /*if (!_encryption.isNull()) {
    if (!isEncrypted)
      _api_key = (*_encryption.data())->EncodeData(key);
    else
      _api_key = key;
  }*/
}



// accessors:
const QByteArray & SessionManager::GetSecurityToken() const { return _security_token; }
const QByteArray & SessionManager::GetCST() const { return _cst; }
const QByteArray & SessionManager::GetAPIKey() const { return _api_key; }
const QByteArray & SessionManager::GetEncryptionKey() const { return _encryption_key; }
const bool & SessionManager::IsCreated() const { return _created; }
