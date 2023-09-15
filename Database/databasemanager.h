#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlError>

#include "Logs/log.h"


enum class QUERIES_REFS {
  NONE = 0,

  CREATE_TABLE_USER,
  INSERT_USER,
  INSERT_MULTI_USERS,
  SAVE_USER,
  DELETE_USER,
  DELETE_MULTI_USERS,
};



class DatabaseManager : public QObject
{
  Q_OBJECT

public:
  explicit DatabaseManager(QObject *parent = nullptr);
  ~DatabaseManager();

signals:
  void SI_AddLog(const Log::TYPE &type, const QByteArray &content, const qint64 &timestamp);

public slots:
  void SL_Init();

private:
  // -- functions -- //
  void CreateQueries();
  void CreateTables();

  // -- variables -- //
  bool _initialized = false;
  QHash<QUERIES_REFS, QByteArray> _queries;

  QSqlDatabase  _db = QSqlDatabase::addDatabase("QSQLITE");
  QSqlQuery     _query = QSqlQuery(_db);
};

#endif // DATABASEMANAGER_H
