#ifndef LOGSMANAGER_H
#define LOGSMANAGER_H

#include <QObject>
#include <QSharedPointer>

#include "log.h"


class LogsManager : public QObject
{
  Q_OBJECT

public:
  explicit LogsManager(QObject *parent = nullptr);

  QStringList GetBatch();


public slots:
  void SL_AddLog(const Log::TYPE &type, const QByteArray &content, const qint64 &timestamp);


private:
  QList<Log*> _logs;
};

#endif // LOGSMANAGER_H
