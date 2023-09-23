#include "identity.h"

Identity::Identity(QObject *parent)
    : QObject{parent}
{
}

Identity::Identity(const Identity &other)
    : QObject{other.parent()}
{
}

Identity::Identity(const QByteArray &cred_id, const QByteArray &cred_pwd, const QByteArray &api_key,
                   const QByteArray &encryption_key, const qint64 &id, QObject *parent)
    : QObject{parent},
    _id(id), _creds({cred_id, cred_pwd}), _keys({api_key, encryption_key})
{
}

Identity::~Identity()
{
}


// operators:
bool Identity::operator == (const Identity &other)
{
  if ( (this->GetID() != 0 && other.GetID() != 0) &&
      (this->GetID() != other.GetID()))
    return true;
  else {
    if ( (!this->GetCredentials().first.isEmpty() && !other.GetCredentials().first.isEmpty()) &&
        (this->GetCredentials().first.compare(other.GetCredentials().first, Qt::CaseSensitive) == 0) )
      return true;
  }

  return false;
}

Identity * Identity::operator = (const Identity &other)
{
  this->SetCredentials(other.GetCredentials());
  this->SetKeys(other.GetKeys());

  return this;
}

// mutators:
void Identity::SetID(const qint64 &id) { _id = id; }
void Identity::SetCredentials(const QPair<QByteArray,QByteArray> &creds) { _creds = creds; }
void Identity::SetKeys(const QPair<QByteArray,QByteArray> &keys) { _keys = keys; }


// accessors:
qint64 Identity::GetID() const { return _id; }
const QPair<QByteArray, QByteArray> & Identity::GetCredentials() const { return _creds; }
const QPair<QByteArray, QByteArray> & Identity::GetKeys() const { return _keys; }
