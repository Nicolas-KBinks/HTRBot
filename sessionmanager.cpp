#include "sessionmanager.h"
#include <QDebug>


// default constructor:
SessionManager::SessionManager(QObject *parent)
    : QObject{parent}
{
}


// custom constructor:
SessionManager::SessionManager(const QSharedPointer<QNetworkAccessManager*> &ptr_netmg, QObject *parent)
    : QObject{parent}, _ptr_netmg(ptr_netmg)
{
}



/* ================ public slots ================ */

// initialize session manager:
void SessionManager::SL_Init()
{
  if (_initialized)
    return;

  qDebug().noquote() << QString("[SessionManager 0x%1] : initializing..")
                            .arg((qint64)(this), 8, 16, QChar('0'));

  if (!_ptr_netmg.isNull())
    connect((*_ptr_netmg), &QNetworkAccessManager::finished, this, &SessionManager::SL_OnNetMG_Finished);

  _initialized = true;

  qDebug().noquote() << QString("[SessionManager 0x%1] : initialized")
                            .arg((qint64)(this), 8, 16, QChar('0'));
}


// create a session:
void SessionManager::SL_Create(const QByteArray &identifier, const QByteArray &pwd, const bool &isEncrypted)
{
  if (_ptr_netmg.isNull() || identifier.isEmpty() || pwd.isEmpty() || this->IsCreated())
    return;

  _current_req = REQUEST_CODE::CREATE;

  qDebug().noquote() << QString("[SessionManager 0x%1] : request session create")
                            .arg((qint64)(this), 8, 16, QChar('0'));

  // create request and set rawHeaders:
  QNetworkRequest req(QString("%1/api/v1/session").arg(DEMO_URL_API));

  req.setRawHeader("X-CAP-API-KEY", API_KEY);
  req.setRawHeader("Content-Type", "application/json");

  // create request body:
  QJsonObject obj;

  obj.insert("identifier", QString::fromUtf8(identifier));
  obj.insert("password", QString::fromUtf8(pwd));
  obj.insert("encryptedPassword", isEncrypted);

  // connect to host and execute request:
  QJsonDocument doc(obj);

  (*_ptr_netmg)->connectToHost(DEMO_URL_API);
  (*_ptr_netmg)->post(req, doc.toJson(QJsonDocument::Compact));
}


// get details about a session:
void SessionManager::SL_Details()
{
  if (_security_token.isEmpty() || _cst.isEmpty() || _ptr_netmg.isNull() || !this->IsCreated())
    return;

  _current_req = REQUEST_CODE::DETAILS;

  qDebug().noquote() << QString("[SessionManager 0x%1] : request session details")
                            .arg((qint64)(this), 8, 16, QChar('0'));

  // create request and set rawHeaders:
  QNetworkRequest req(QString("%1/api/v1/session").arg(DEMO_URL_API));

  req.setRawHeader("X-SECURITY-TOKEN", this->GetSecurityToken());
  req.setRawHeader("CST", this->GetCST());

  // execute request:
  (*_ptr_netmg)->get(req);
}


// switch active account:
void SessionManager::SL_Switch(const QSharedPointer<Account*> &account)
{
  if (_security_token.isEmpty() || _cst.isEmpty() || account.isNull() || !this->IsCreated())
    return;

  _current_req = REQUEST_CODE::SWITCH_ACCOUNT;

  qDebug().noquote() << QString("[SessionManager 0x%1] : request session switch account")
                            .arg((qint64)(this), 8, 16, QChar('0'));

  // create request and set rawHeaders:
  QNetworkRequest req(QString("%1/api/v1/session").arg(DEMO_URL_API));

  req.setRawHeader("X-SECURITY-TOKEN", this->GetSecurityToken());
  req.setRawHeader("CST", this->GetCST());

  // create request body:
  QJsonObject obj;

  obj.insert("accountId", QString::fromUtf8((*account)->GetID()) );

  // execute request:
  QJsonDocument doc(obj);

  (*_ptr_netmg)->put(req, doc.toJson(QJsonDocument::Compact));
}


// session logout:
void SessionManager::SL_Logout()
{
  if (_security_token.isEmpty() || _cst.isEmpty() || _ptr_netmg.isNull() || !this->IsCreated())
    return;

  _current_req = REQUEST_CODE::LOGOUT;

  qDebug().noquote() << QString("[SessionManager 0x%1] : request session logout")
                            .arg((qint64)(this), 8, 16, QChar('0'));

  QNetworkRequest req(QString("%1/api/v1/session").arg(DEMO_URL_API));

  req.setRawHeader("X-SECURITY-TOKEN", this->GetSecurityToken());
  req.setRawHeader("CST", this->GetCST());

  (*_ptr_netmg)->deleteResource(req);
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


  // handles reply from server upon session created:

  if (!this->IsCreated() && _current_req == REQUEST_CODE::CREATE) {

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

    qDebug().noquote() << QString("[SessionManager 0x%1] : session created")
                              .arg((qint64)(this), 8, 16, QChar('0'));
  } // end of session created


  // session destroyed:
  if (this->IsCreated() && _current_req == REQUEST_CODE::LOGOUT) {
    this->SetSecurityToken("");
    this->SetCST("");

    _created = false;
    _current_req = REQUEST_CODE::NONE;

    qDebug().noquote() << QString("[SessionManager 0x%1] : session destroyed")
                              .arg((qint64)(this), 8, 16, QChar('0'));
  }
}

/* ================ end of private slots ================ */



// mutators:
void SessionManager::SetSecurityToken(const QByteArray &token) { _security_token = token; }
void SessionManager::SetCST(const QByteArray &cst) { _cst = cst; }



// accessors:
const QByteArray & SessionManager::GetSecurityToken() const { return _security_token; }
const QByteArray & SessionManager::GetCST() const { return _cst; }
const bool & SessionManager::IsCreated() const { return _created; }
