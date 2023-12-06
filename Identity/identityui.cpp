#include "identityui.h"

IdentityUI::IdentityUI(QWidget *parent)
    : QWidget{parent}
{
}

void IdentityUI::BuildUI()
{
  _lay = new QGridLayout();
  this->setLayout(_lay);

  // identities list:
  _idents_list = {new QLabel("Identities:"), new QListWidget()};
  _lay->addWidget(_idents_list.lb, 0, 0, 1, 1);
  _lay->addWidget(_idents_list.fd, 1, 0, 1, 2);

  // fields:
  _cred_id  = {new QLabel("ID:"), new QLineEdit()};
  _cred_pwd = {new QLabel("Password:"), new QLineEdit()};
  _api_key  = {new QLabel("API Key:"), new QLineEdit()};

  _lay->addWidget(_api_key.lb, 2, 0, 1, 1);
  _lay->addWidget(_api_key.fd, 2, 1, 1, 1);
  _lay->addWidget(_cred_id.lb, 3, 0, 1, 1);
  _lay->addWidget(_cred_id.fd, 3, 1, 1, 1);
  _lay->addWidget(_cred_pwd.lb, 3, 2, 1, 1);
  _lay->addWidget(_cred_pwd.fd, 3, 3, 1, 1);

  // buttons:
  _buttons = {new QHBoxLayout(), new QPushButton("Create"), new QPushButton("Save")};

  connect(_buttons.create, &QPushButton::clicked, this, &IdentityUI::SL_OnBT_Create);
  connect(_buttons.save, &QPushButton::clicked, this, &IdentityUI::SL_OnBT_Save);

  _buttons.lay->addWidget(_buttons.create);
  _buttons.lay->addWidget(_buttons.save);

  _buttons.create->setHidden(true);
  _buttons.save->setHidden(true);

  _lay->addLayout(_buttons.lay, 4, 0, 1, 1);
}

void IdentityUI::SetCurrentForm(const IdentityUI::FORM_REFS &ref)
{
  switch (ref) {
  case IdentityUI::FORM_REFS::CREATE:
    _current_form = ref;
    _buttons.create->setHidden(false);
    _buttons.save->setHidden(true);
    break;

  case IdentityUI::FORM_REFS::SAVE:
    _current_form = ref;
    _buttons.create->setHidden(true);
    _buttons.save->setHidden(false);
    break;

  default:
    _buttons.create->setHidden(true);
    _buttons.save->setHidden(true);
    break;
  }
}


void IdentityUI::SL_AddIdentityToList(Identity *ident)
{
  if (!ident)
    return;

  QListWidgetItem *itm = new QListWidgetItem(ident->GetCredentials().first);
  itm->setData(Qt::UserRole, QVariant::fromValue(ident));
  itm->setToolTip(QString("API Key : %1").arg(ident->GetKeys().first));

  _idents_list.fd->addItem(itm);
}

void IdentityUI::SL_OnIdentitySaved(Identity *ident)
{
  if (!ident)
    return;
}

void IdentityUI::SL_OnBT_Create()
{
  Identity *ident = new Identity(_cred_id.fd->text().toUtf8(),
                                 _cred_pwd.fd->text().toUtf8(),
                                 _api_key.fd->text().toUtf8(),
                                 "", 0, this);

  this->SetCurrentForm(IdentityUI::FORM_REFS::NONE);

  emit SI_ReqCreateIdentity(ident);
}

void IdentityUI::SL_OnBT_Save()
{
  if (!_current_ident.isNull()) {
    (*_current_ident.data())->SetCredentials({_cred_id.fd->text().toUtf8(), _cred_pwd.fd->text().toUtf8()});
    (*_current_ident.data())->SetKeys({_api_key.fd->text().toUtf8(), (*_current_ident.data())->GetKeys().second});

    this->SetCurrentForm(IdentityUI::FORM_REFS::NONE);

    emit SI_ReqSaveIdentity(*_current_ident.data());
  }
}
