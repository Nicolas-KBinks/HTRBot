#include "encryptionmanager.h"

EncryptionManager::EncryptionManager(QObject *parent)
    : QObject{parent},
    _enc(QAESEncryption(QAESEncryption::AES_256, QAESEncryption::ECB))
{
}


QByteArray EncryptionManager::EncodeData(const QByteArray &data)
{
  return _enc.encode(data, _client_key);
}

QByteArray EncryptionManager::DecodeData(const QByteArray &data)
{
  return _enc.removePadding( _enc.decode(data, _client_key) );
}


void EncryptionManager::SL_Init()
{
  if (_initialized)
    return;

  this->LoadCerts();
  this->InitSsl();

  _initialized = true;

  emit SI_AddLog(Log::TYPE::INFO, "Encryption module initialized", QDateTime::currentMSecsSinceEpoch());
}


void EncryptionManager::LoadCerts()
{
  QFile file;

  // load client key:
  file.setFileName("cert/cl-key.pem");

  if (!file.open(QIODevice::ReadOnly)) {
    emit SI_AddLog(Log::TYPE::ERROR, "Cannot open cert/cl-key.pem", QDateTime::currentMSecsSinceEpoch());
    return;
  }

  _client_key = file.readAll();
  file.close();

  // load client certificate:
  file.setFileName("cert/cl-crt.pem");

  if (!file.open(QIODevice::ReadOnly)) {
    emit SI_AddLog(Log::TYPE::ERROR, "Cannot open cert/cl-crt.pem", QDateTime::currentMSecsSinceEpoch());
    return;
  }

  _client_crt = file.readAll();
  file.close();

  // load authority certificate:
  file.setFileName("cert/ca/ca.pem");

  if (!file.open(QIODevice::ReadOnly)) {
    emit SI_AddLog(Log::TYPE::ERROR, "Cannot open cert/ca/ca.pem", QDateTime::currentMSecsSinceEpoch());
    return;
  }

  _ca = file.readAll();
  file.close();
}

void EncryptionManager::InitSsl()
{
  if (_client_key.isEmpty() || _client_crt.isEmpty() || _ca.isEmpty() || _initialized)
    return;

  _ssl_config = QSslConfiguration::defaultConfiguration();

  _ssl_key = QSslKey(_client_key, QSsl::Rsa, QSsl::Pem);
  _ssl_config.setPrivateKey(_ssl_key);
  _ssl_config.addCaCertificate(QSslCertificate(_ca));
  _ssl_config.setLocalCertificate(QSslCertificate(_client_crt));
}


// accessors:
const QSslKey & EncryptionManager::GetSslKey() const { return _ssl_key; }
