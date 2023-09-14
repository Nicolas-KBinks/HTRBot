#ifndef MWIN_H
#define MWIN_H

#include <QMainWindow>

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>

#include <QNetworkAccessManager>

#include <QThread>
#include <QTimer>

#include "Logs/logsmanager.h"
#include "Session/sessionmanager.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MWin; }
QT_END_NAMESPACE


class MWin : public QMainWindow
{
  Q_OBJECT

public:
  MWin(QWidget *parent = nullptr);
  ~MWin();


private slots:
  void SL_ProcessLogsBatch();


private:
  Ui::MWin *ui;
  QGridLayout *_lay = nullptr;
  QTextEdit *_logs_ui = nullptr;

  LogsManager *_logs_mg = nullptr;
  QTimer *_logs_timer = nullptr;

  QNetworkAccessManager *_netmg = nullptr;
  SessionManager *_session_mg = nullptr;
};
#endif // MWIN_H
