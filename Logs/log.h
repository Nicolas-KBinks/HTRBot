#ifndef LOG_H
#define LOG_H

#include <QObject>
#include <QDateTime>
#include <QHash>

class Log : public QObject
{
  Q_OBJECT

public:

  enum class TYPE {
    NONE = 0,
    INFO,
    WARNING,
    ERROR,
    DBG
  };


  explicit Log(QObject *parent = nullptr);
  Log(const Log::TYPE &type, const QByteArray &content, const qint64 &timestamp = QDateTime::currentMSecsSinceEpoch(),
      QObject *parent = nullptr);


  qint64 GetTimestamp() const;
  Log::TYPE GetType() const;
  const QByteArray & GetContent() const;


private:
  qint64 _timestamp = 0;
  TYPE _type = TYPE::NONE;
  QByteArray _content = "";
};

extern QHash<Log::TYPE, QByteArray> logs_typeStrings;


#endif // LOG_H
