#include "SettingsDialog.h"

#include <QtWidgets>

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent)
{
    setupUi();
}

void SettingsDialog::setupUi()
{
    auto l = new QVBoxLayout();

    auto hl = new QHBoxLayout();
    {
        hl->addWidget(listPage_ = new QListWidget());
        connect(listPage_, &QListWidget::currentItemChanged, this, &SettingsDialog::selectPage);
        listPage_->setMinimumWidth(80);
        listPage_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

        auto vl = new QVBoxLayout();
        vl->addWidget(stackedWidget_ = new QStackedWidget());
        stackedWidget_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        stackedWidget_->setMinimumWidth(640);
        stackedWidget_->setMinimumHeight(320);
        vl->addStretch();

        hl->addLayout(vl);
    }
    l->addLayout(hl);

    btns_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
    connect(btns_, &QDialogButtonBox::clicked, this, &SettingsDialog::btnClicked);
    l->addWidget(btns_);

    setLayout(l);
}

void SettingsDialog::addPage(const QString& name, SettingsPage* widget)
{
    pages_[name] = widget;

    listPage_->addItem(name);
    stackedWidget_->addWidget(widget);
}

void SettingsDialog::btnClicked(QAbstractButton* button)
{
    switch (btns_->buttonRole(button)) {
    case QDialogButtonBox::AcceptRole:
        if (tryApply()) {
            accept();
        }
        break;
    case QDialogButtonBox::ApplyRole:
        tryApply();
        // do nothing regardless of error
        break;
    case QDialogButtonBox::RejectRole:
        reject();
        break;
    default:
        Q_ASSERT(false);
        break;
    }
}

bool SettingsDialog::tryApply()
{
    bool allOk = true;
    foreach (auto page, pages_) {
        bool b = page->apply();
        allOk &= b;
    }
    return allOk;
}

void SettingsDialog::selectPage()
{
    const QString name = listPage_->currentItem()->text();

    auto w = pages_.value(name, nullptr);
    Q_ASSERT(w);
    stackedWidget_->setCurrentWidget(w);
}
