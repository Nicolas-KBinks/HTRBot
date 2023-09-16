#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlError>

#include "Logs/log.h"
#include "Encryption/encryptionmanager.h"


enum class QUERIES_REFS {
  NONE = 0,

  CREATE_TABLE_USER,
  INSERT_USER,
  INSERT_MULTI_USERS,
  SAVE_USER,
  DELETE_USER,
  DELETE_MULTI_USERS,
  LOADALL_USERS,
};



class DatabaseManager : public QObject
{
  Q_OBJECT

public:
  typedef struct S_UserDatas {
    qint64 id = 0;
    QByteArray cred_id = "", cred_pwd = "";
  } S_UserDatas;

  explicit DatabaseManager(QObject *parent = nullptr);
  ~DatabaseManager();

  void User_Insert(const QByteArray &cred_id_encrypted, const QByteArray &cred_pwd_encypted);
  QList<DatabaseManager::S_UserDatas> User_LoadAll();


signals:
  void SI_AddLog(const Log::TYPE &type, const QByteArray &content, const qint64 &timestamp);


public slots:
  void SL_Init(const QSharedPointer<EncryptionManager*> &ptr_enc);


private:
  // -- functions -- //
  void CreateQueries();
  void CreateTables();

  // -- variables -- //
  bool _initialized = false;
  QSharedPointer<EncryptionManager*> _ptr_enc;
  QHash<QUERIES_REFS, QByteArray> _queries;

  QSqlDatabase  _db = QSqlDatabase::addDatabase("QSQLITE");
  QSqlQuery     _query = QSqlQuery(_db);
};

#endif // DATABASEMANAGER_H
