#include "mwin.h"
#include "ui_mwin.h"

#include <QDebug>


MWin::MWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MWin)
{
  ui->setupUi(this);

  _netmg = new QNetworkAccessManager(this);

  this->Create_LogsManager();
  this->Create_SessionManager();

  // create MWin UI:
  _lay = new QGridLayout();
  this->centralWidget()->setLayout(_lay);

  _logs_ui = new QTextEdit();
  _logs_ui->setReadOnly(true);
  _lay->addWidget(_logs_ui, 0, 0, 1, 1);


  // test session create:
  QTimer::singleShot(5000, _session_mg, std::bind(&SessionManager::SL_Create, _session_mg,
                                                  "", "", false));

  // test session logout:
  QTimer::singleShot(10000, _session_mg, &SessionManager::SL_Logout);
}


MWin::~MWin()
{
  this->Destroy_SessionManager();
  this->Destroy_LogsManager();

  delete ui;
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

void MWin::Destroy_LogsManager()
{
  if (_logs.th) {
    if (_logs.th->isRunning()) {
      _logs.th->quit();
      _logs.th->wait();
    }

    delete _logs.th;
    _logs.th = nullptr;
  }
}



void MWin::Create_SessionManager()
{
  if (_session.th && _session.mg)
    return;

  _session.th = new QThread();
  _session.mg = new SessionManager(QSharedPointer<QNetworkAccessManager*>::create(_netmg), nullptr);

  connect(_session.th, &QThread::finished, _session.mg, &SessionManager::deleteLater);
  connect(_session.mg, &SessionManager::SI_AddLog, _logs.mg, &LogsManager::SL_AddLog);

  _session.mg->moveToThread(_session.th);
  _session.th->start();

  QTimer::singleShot(0, _session.mg, &SessionManager::SL_Init);
}

void MWin::Destroy_SessionManager()
{
  if (_session.th) {
    if (_session.th->isRunning()) {
      _session.th->quit();
      _session.th->wait();
    }

    delete _session.th;
    _session.th = nullptr;
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
