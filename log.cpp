#include "log.h"

// default constructor:
Log::Log(QObject *parent)
    : QObject{parent}
{
}


// custom constructor:
Log::Log(const TYPE &type, const QByteArray &content, const qint64 &timestamp, QObject *parent)
    : QObject{parent},
    _timestamp(timestamp), _type(type), _content(content)
{
}



qint64 Log::GetTimestamp() const { return _timestamp; }
Log::TYPE Log::GetType() const { return _type; }
const QByteArray & Log::GetContent() const { return _content; }
