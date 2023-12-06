#include "databasemanager.h"

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject{parent}
{
}

DatabaseManager::~DatabaseManager()
{
  if (_db.isOpen())
    _db.close();
}


void DatabaseManager::SL_Init(const QSharedPointer<EncryptionManager*> &ptr_enc)
{
  if (_initialized)
    return;

  _ptr_enc = ptr_enc;

  this->CreateQueries();
  _db.setDatabaseName("datas");

  if (!_db.open())
    return;

  this->CreateTables();
  _initialized = true;

  emit SI_AddLog(Log::TYPE::INFO, "Database Initialized", QDateTime::currentMSecsSinceEpoch());
}


void DatabaseManager::CreateQueries()
{
  // users queries:
  _queries.insert(QUERIES_REFS::CREATE_TABLE_USER,
                  QString("CREATE TABLE IF NOT EXISTS users("
                          "id INTEGER PRIMARY KEY,"
                          "cred_id VARCHAR(256),"
                          "cred_pwd TEXT);").toUtf8());

  _queries.insert(QUERIES_REFS::INSERT_USER,
                  QString("INSERT INTO users(id, cred_id, cred_pwd) "
                          "VALUES(:cred_id, :cred_pwd);").toUtf8());

  _queries.insert(QUERIES_REFS::LOADALL_USERS,
                  QString("SELECT id, cred_id, cred_pwd FROM users ORDER BY id DESC").toUtf8());

  _queries.insert(QUERIES_REFS::SAVE_USER,
                  QString().toUtf8());
}

void DatabaseManager::CreateTables()
{
  QStringList tables = _db.tables();

  if (!tables.contains("users", Qt::CaseSensitive)) {
    _query.clear();

    if (_query.prepare(_queries.value(QUERIES_REFS::CREATE_TABLE_USER)))
      _query.exec();
  }
}

void DatabaseManager::CreateIdentity(Identity *ident)
{
  _query.clear();

  if (!ident)
    return;

  if (_query.prepare(_queries.value(QUERIES_REFS::INSERT_USER))) {
    _query.bindValue(":cred_id", ident->GetCredentials().first);
    _query.bindValue(":cred_pwd", ident->GetCredentials().second);
    _query.bindValue(":api_key", ident->GetKeys().first);

    if (_query.exec()) {
      ident->SetID(_query.lastInsertId().toLongLong());
      emit SI_IdentityCreated(ident);
    } else {
      emit SI_AddLog(Log::TYPE::ERROR,
                     QString("DB_ERROR : failed to create identity <strong>%1</strong>").arg(ident->GetCredentials().first).toUtf8(),
                     QDateTime::currentMSecsSinceEpoch());
    }
  }
}

void DatabaseManager::SaveIdentity(Identity *ident)
{
  _query.clear();

  if (!ident)
    return;

  if (_query.prepare(_queries.value(QUERIES_REFS::SAVE_USER))) {
    _query.bindValue(":id", ident->GetID());
    _query.bindValue(":cred_id", ident->GetCredentials().first);
    _query.bindValue(":cred_pwd", ident->GetCredentials().second);
    _query.bindValue(":api_key", ident->GetKeys().first);

    if (_query.exec()) {
      emit SI_IdentitySaved(ident);
    } else {
      emit SI_AddLog(Log::TYPE::ERROR,
                     QString("DB_ERROR : failed to save identity <strong>%1</strong>").arg(ident->GetCredentials().first).toUtf8(),
                     QDateTime::currentMSecsSinceEpoch());
    }
  }
}
