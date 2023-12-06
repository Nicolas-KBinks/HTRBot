#include "mwin.h"
#include "ui_mwin.h"

#include <QDebug>


MWin::MWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MWin)
{
  ui->setupUi(this);

  this->Create_LogsManager();
  this->Create_EncryptionManager();
  this->Create_DatabaseManager();
  this->Create_NetworkAccessManager();
  this->Create_SessionManager();

  this->BuildUI();

  this->ConnectObjects();
}


MWin::~MWin()
{
  this->Destroy_Manager(_session.th);
  this->Destroy_Manager(_netmg.th);
  this->Destroy_Manager(_db.th);
  this->Destroy_Manager(_encryption.th);
  this->Destroy_Manager(_logs.th);

  delete ui;
}


void MWin::BuildUI()
{
  // create MWin UI:
  _lay = new QGridLayout();
  this->centralWidget()->setLayout(_lay);

  // user credentials UI:
  _ident_ui = new IdentityUI();
  _ident_ui->BuildUI();
  _ident_ui->SetCurrentForm(IdentityUI::FORM_REFS::CREATE);

  connect(_ident_ui, &IdentityUI::SI_ReqCreateIdentity, this, &MWin::SL_OnReqCreateIdentity);
  connect(_ident_ui, &IdentityUI::SI_ReqSaveIdentity, this, &MWin::SL_OnReqSaveIdentity);

  _lay->addWidget(_ident_ui, 0, 0, 1, 1);

  // logs UI:
  _logs_ui = new QTextEdit();
  _logs_ui->setReadOnly(true);
  _lay->addWidget(_logs_ui, 1, 0, 1, 1);
}

void MWin::ConnectObjects()
{
  // IdentityUI -> DatabaseManager:
  connect(_ident_ui, &IdentityUI::SI_ReqCreateIdentity, _db.mg, &DatabaseManager::CreateIdentity);
  connect(_ident_ui, &IdentityUI::SI_ReqSaveIdentity, _db.mg, &DatabaseManager::SaveIdentity);

  // DatabaseManager -> SessionManager:
  connect(_db.mg, &DatabaseManager::SI_IdentityCreated, _session.mg, &SessionManager::SL_OnIdentityCreated);

  // DatabaseManager -> IdentityUI:
  connect(_db.mg, &DatabaseManager::SI_IdentitySaved, _ident_ui, &IdentityUI::SL_OnIdentitySaved);
}


void MWin::Create_LogsManager()
{
  if (_logs.th && _logs.tm && _logs.mg)
    return;

  _logs.th = new QThread();
  _logs.tm = new QTimer();
  _logs.mg = new LogsManager();

  connect(_logs.th, &QThread::finished, _logs.tm, &QTimer::deleteLater);
  connect(_logs.th, &QThread::finished, _logs.mg, &LogsManager::deleteLater);

  connect(_logs.tm, &QTimer::timeout, this, &MWin::SL_ProcessLogsBatch);

  _logs.tm->setInterval(5);
  _logs.mg->moveToThread(_logs.th);

  _logs.th->start();
  _logs.tm->start();
}

void MWin::Create_SessionManager()
{
  if (_session.th && _session.mg)
    return;

  _session.th = new QThread();
  _session.mg = new SessionManager(nullptr);

  connect(_session.th, &QThread::finished, _session.mg, &SessionManager::deleteLater);
  connect(_session.mg, &SessionManager::SI_AddLog, _logs.mg, &LogsManager::SL_AddLog);

  _session.mg->moveToThread(_session.th);
  _session.th->start();

  QTimer::singleShot(0, _session.mg, &SessionManager::SL_Init);
}

void MWin::Create_NetworkAccessManager()
{
  _netmg.th = new QThread();
  _netmg.mg = new QNetworkAccessManager();

  connect(_netmg.th, &QThread::finished, _netmg.mg, &QNetworkAccessManager::deleteLater);

  _netmg.mg->moveToThread(_netmg.th);
  _netmg.th->start();
}

void MWin::Create_DatabaseManager()
{
  _db.th = new QThread();
  _db.mg = new DatabaseManager();

  connect(_db.th, &QThread::finished, _db.mg, &DatabaseManager::deleteLater);
  connect(_db.mg, &DatabaseManager::SI_AddLog, _logs.mg, &LogsManager::SL_AddLog);

  _db.mg->moveToThread(_db.th);
  _db.th->start();

  /*QTimer::singleShot(0, _db.mg,
                     std::bind(&DatabaseManager::SL_Init, _db.mg, QSharedPointer<EncryptionManager*>::create(_encryption.mg))
                     );*/
}

void MWin::Create_EncryptionManager()
{
  _encryption.th = new QThread();
  _encryption.mg = new EncryptionManager();

  connect(_encryption.th, &QThread::finished, _encryption.mg, &EncryptionManager::deleteLater);
  connect(_encryption.mg, &EncryptionManager::SI_AddLog, _logs.mg, &LogsManager::SL_AddLog);

  _encryption.mg->moveToThread(_encryption.th);
  _encryption.th->start();

  QTimer::singleShot(0, _encryption.mg, &EncryptionManager::SL_Init);
}


void MWin::Destroy_Manager(QThread *th)
{
  if (th) {
    if (th->isRunning()) {
      th->quit();
      th->wait();
    }

    delete th;
    th = nullptr;
  }
}


void MWin::SL_ProcessLogsBatch()
{
  if (_logs.mg && _logs_ui) {
    QStringList batch = _logs.mg->GetBatch();

    while (!batch.isEmpty())
      _logs_ui->append(batch.takeFirst());
  }
}

/* handles signal SI_ReqCreateUser from IdentityUI */
void MWin::SL_OnReqCreateIdentity(Identity *ident)
{
  if (!ident)
    return;

  // send signal directly to database manager
  // db manager send reply to session manager to request encryption_key
  // session manager send reply with key to IdentityUI
  // IdentityUI add created identity to its list

  QTimer::singleShot(0, _logs.mg, std::bind(&LogsManager::SL_AddLog, _logs.mg,
                                            Log::TYPE::DBG,
                                            QString("<p><em>create identity</em><br />"
                                                    "ident.id = %1<br />ident.cre_id = %2<br />ident.cred_pwd = %3<br />"
                                                    "ident.api_key = %4<br />ident.encryption_key = %5</p>")
                                                .arg(ident->GetID())
                                                .arg(ident->GetCredentials().first,
                                                     ident->GetCredentials().second,
                                                     ident->GetKeys().first,
                                                     ident->GetKeys().second)
                                                .toUtf8(), QDateTime::currentMSecsSinceEpoch()) );

  QTimer::singleShot(0, _ident_ui, std::bind(&IdentityUI::SL_AddIdentityToList, _ident_ui, ident));
}

/* handles signal SI_ReqSaveUser from IdentityUI */
void MWin::SL_OnReqSaveIdentity(Identity *ident)
{
  if (!ident)
    return;

  // send signal directly to database manager
  // if identity datas have changed:
  //   db send reply to session manager to request a new encryption key
  //   session manager send reply with key to IdentityUI
  //   IdentityUI updates identity datas
  // if identity datas didn't changed:
  //   db send reply to IdentityUI

  QTimer::singleShot(0, _logs.mg, std::bind(&LogsManager::SL_AddLog, _logs.mg,
                                            Log::TYPE::DBG,
                                            QString("<p><em>save identity</em><br />"
                                                    "ident.id = %1<br />ident.cre_id = %2<br />ident.cred_pwd = %3<br />"
                                                    "ident.api_key = %4<br />ident.encryption_key = %5</p>")
                                                .arg(ident->GetID())
                                                .arg(ident->GetCredentials().first,
                                                     ident->GetCredentials().second,
                                                     ident->GetKeys().first,
                                                     ident->GetKeys().second)
                                                .toUtf8(), QDateTime::currentMSecsSinceEpoch()) );
}
