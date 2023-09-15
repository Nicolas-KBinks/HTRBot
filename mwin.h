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
  // -- functions -- //
  void Create_LogsManager();
  void Create_SessionManager();
  void Create_NetworkAccessManager();

  void Destroy_Manager(QThread *th);



  // -- variables -- //
  Ui::MWin *ui;
  QGridLayout *_lay = nullptr;
  QTextEdit *_logs_ui = nullptr;

  struct {
    QThread     *th = nullptr;
    QTimer      *tm = nullptr;
    LogsManager *mg = nullptr;
  } _logs;

  struct {
    QThread         *th = nullptr;
    SessionManager  *mg = nullptr;
  } _session;

  struct {
    QThread *th = nullptr;
    QNetworkAccessManager *mg = nullptr;
  } _netmg;
};
#endif // MWIN_H
