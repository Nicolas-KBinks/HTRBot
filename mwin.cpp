#include "mwin.h"
#include "ui_mwin.h"

#include <QDebug>


MWin::MWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MWin)
{
  ui->setupUi(this);

  // create networkAccessManager object and session manager:
  _netmg = new QNetworkAccessManager(this);
  _session_mg = new SessionManager(QSharedPointer<QNetworkAccessManager*>::create(_netmg), this);

  QTimer::singleShot(0, _session_mg, &SessionManager::SL_Init);

  // create MWin UI:
  _lay = new QGridLayout();
  this->centralWidget()->setLayout(_lay);


  // test session create:
  QTimer::singleShot(5000, _session_mg, std::bind(&SessionManager::SL_Create, _session_mg,
                                                  "nicolascrpt@gmail.com", "c3qP7YtCEed4K8H!", false));

  // test session logout:
  QTimer::singleShot(10000, _session_mg, &SessionManager::SL_Logout);
}


MWin::~MWin()
{
  delete ui;
}
