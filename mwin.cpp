#include "mwin.h"
#include "ui_mwin.h"

#include <QDebug>


MWin::MWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MWin)
{
  ui->setupUi(this);


  // create logs manager:
  _logs_mg = new LogsManager(this);
  _logs_timer = new QTimer(this);

  connect(_logs_timer, &QTimer::timeout, this, &MWin::SL_ProcessLogsBatch);

  _logs_timer->setInterval(5);
  _logs_timer->start();


  // create networkAccessManager object and session manager:
  _netmg = new QNetworkAccessManager(this);
  _session_mg = new SessionManager(QSharedPointer<QNetworkAccessManager*>::create(_netmg), this);

  connect(_session_mg, &SessionManager::SI_AddLog, _logs_mg, &LogsManager::SL_AddLog);

  QTimer::singleShot(0, _session_mg, &SessionManager::SL_Init);

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
  if (_logs_timer && _logs_timer->isActive()) {
    _logs_timer->stop();
  }

  delete ui;
}



void MWin::SL_ProcessLogsBatch()
{
  if (_logs_mg && _logs_ui) {
    QStringList batch = _logs_mg->GetBatch();

    while (!batch.isEmpty())
      _logs_ui->append(batch.takeFirst());
  }
}
