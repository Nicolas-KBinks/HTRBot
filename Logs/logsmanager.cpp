#include "logsmanager.h"


QHash<Log::TYPE, QByteArray> logs_typeStrings = {
    QPair<Log::TYPE, QByteArray>(Log::TYPE::NONE, "NONE"),
    QPair<Log::TYPE, QByteArray>(Log::TYPE::INFO, "INFO"),
    QPair<Log::TYPE, QByteArray>(Log::TYPE::WARNING, "WARNING"),
    QPair<Log::TYPE, QByteArray>(Log::TYPE::ERROR, "ERROR"),
    QPair<Log::TYPE, QByteArray>(Log::TYPE::DBG, "DEBUG")
};



LogsManager::LogsManager(QObject *parent)
    : QObject{parent}
{
}



void LogsManager::SL_AddLog(const Log::TYPE &type, const QByteArray &content, const qint64 &timestamp)
{
  Log *lg = new Log(type, content, timestamp, this);

  if (lg) {
    if (_mtx.try_lock()) {
      _logs.append(lg);
      _mtx.unlock();
    } else {
      delete lg;
      lg = nullptr;
    }
  }
}


QStringList LogsManager::GetBatch()
{
  QStringList batch;

  for (int n = 0; n < 10; n++) {
    if (!_logs.isEmpty()) {

      if (_mtx.try_lock()) {
        Log *lg = _logs.takeFirst();

        if (!lg) {
          _mtx.unlock();
          continue;
        }

        batch.append( QString("[ %1 ][ %2 ] : %3")
                         .arg(QDateTime::fromMSecsSinceEpoch(lg->GetTimestamp()).toString("yyyy/MM/dd | hh:mm:ss.zzz"),
                              logs_typeStrings.value(lg->GetType()))
                         .arg(lg->GetContent()) );

        delete lg;
        lg = nullptr;
        _mtx.unlock();
      } // end of mutex try lock

    } // end of check logs not empty
  } // end of for n < 10 loop

  return batch;
}
