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


void DatabaseManager::SL_Init()
{
  if (_initialized)
    return;

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

  _queries.insert(QUERIES_REFS::SAVE_USER,
                  QString().toUtf8());
}


void DatabaseManager::CreateTables()
{
  QStringList tables = _db.tables();

  if (!tables.contains("users", Qt::CaseSensitive)) {
    if (_query.prepare(_queries.value(QUERIES_REFS::CREATE_TABLE_USER)))
      _query.exec();

    _query.clear();
  }
}
