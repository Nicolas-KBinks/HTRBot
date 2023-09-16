#ifndef ENCRYPTIONMANAGER_H
#define ENCRYPTIONMANAGER_H

#include <QObject>
#include <QFile>
#include <QSslConfiguration>
#include <QSslKey>

#include "QtAES/qaesencryption.h"
#include "Logs/log.h"


class EncryptionManager : public QObject
{
  Q_OBJECT

public:
  explicit EncryptionManager(QObject *parent = nullptr);

  QByteArray EncodeData(const QByteArray &data);
  QByteArray DecodeData(const QByteArray &data);

  // accessors:
  const QSslKey & GetSslKey() const;

signals:
  void SI_AddLog(const Log::TYPE &type, const QByteArray &content, const qint64 &timestamp);

public slots:
  void SL_Init();

private:
  void LoadCerts();
  void InitSsl();

  bool _initialized = false;
  QAESEncryption _enc;
  QSslConfiguration _ssl_config;
  QSslKey _ssl_key;
  QByteArray _client_key = "", _client_crt = "", _ca = "";
};

#endif // ENCRYPTIONMANAGER_H
