#ifndef IDENTITYUI_H
#define IDENTITYUI_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>

#include "identity.h"


class IdentityUI : public QWidget
{
  Q_OBJECT

public:
  enum class FORM_REFS {NONE = 0, CREATE, SAVE};

  explicit IdentityUI(QWidget *parent = nullptr);

  void BuildUI();
  void SetCurrentForm(const IdentityUI::FORM_REFS &ref);

signals:
  void SI_ReqCreateIdentity(Identity *ident);
  void SI_ReqSaveIdentity(Identity *ident);

public slots:
  void SL_AddIdentityToList(Identity *ident);

private slots:
  void SL_OnBT_Create();
  void SL_OnBT_Save();

private:
  IdentityUI::FORM_REFS _current_form = IdentityUI::FORM_REFS::NONE;
  QSharedPointer<Identity*> _current_ident;
  QGridLayout *_lay = nullptr;

  struct {
    QLabel *lb = nullptr;
    QLineEdit *fd = nullptr;
  } _cred_id, _cred_pwd, _api_key;

  struct {
    QLabel *lb = nullptr;
    QListWidget *fd = nullptr;
  } _idents_list;

  struct {
    QHBoxLayout *lay = nullptr;
    QPushButton *create = nullptr, *save = nullptr;
  } _buttons;
};

#endif // IDENTITYUI_H
