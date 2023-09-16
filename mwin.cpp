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

  // test session create:
  QTimer::singleShot(5000, _session.mg, std::bind(&SessionManager::SL_Create, _session.mg,
                                                  "", "", false));

  // test session logout:
  QTimer::singleShot(10000, _session.mg, &SessionManager::SL_Logout);
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
  _cred_fields = {
                  new QHBoxLayout(),
                  {new QLabel("ID:"), new QLineEdit()},
                  {new QLabel("Password:"), new QLineEdit()},
                  };

  _cred_fields.pwd.fd->setEchoMode(QLineEdit::Password);

  _cred_fields.lay->addWidget(_cred_fields.id.lb);
  _cred_fields.lay->addWidget(_cred_fields.id.fd);
  _cred_fields.lay->addWidget(_cred_fields.pwd.lb);
  _cred_fields.lay->addWidget(_cred_fields.pwd.fd);

  _lay->addLayout(_cred_fields.lay, 0, 0, 1, 1);

  // logs UI:
  _logs_ui = new QTextEdit();
  _logs_ui->setReadOnly(true);
  _lay->addWidget(_logs_ui, 1, 0, 1, 1);
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
  _session.mg = new SessionManager(QSharedPointer<QNetworkAccessManager*>::create(_netmg.mg), nullptr);

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

  QTimer::singleShot(0, _db.mg,
                     std::bind(&DatabaseManager::SL_Init, _db.mg, QSharedPointer<EncryptionManager*>::create(_encryption.mg))
                     );
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
