#ifndef MWIN_H
#define MWIN_H

#include <QMainWindow>

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

#include <QNetworkAccessManager>

#include <QThread>
#include <QTimer>

#include "sessionmanager.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MWin; }
QT_END_NAMESPACE


class MWin : public QMainWindow
{
  Q_OBJECT

public:
  MWin(QWidget *parent = nullptr);
  ~MWin();


private:
  Ui::MWin *ui;
  QGridLayout *_lay = nullptr;

  QNetworkAccessManager *_netmg = nullptr;
  SessionManager *_session_mg = nullptr;
};
#endif // MWIN_H
