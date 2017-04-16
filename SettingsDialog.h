#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QMap>

class QAbstractButton;
class QDialogButtonBox;
class QListWidget;
class QStackedWidget;

class SettingsPage : public QWidget {
public:
    SettingsPage(QWidget* parent = 0) : QWidget(parent) {}
    virtual bool apply() = 0;
};

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    SettingsDialog(QWidget* parent = 0);

    void addPage(const QString& name, SettingsPage* widget);

private:
    // model
    QMap<QString, SettingsPage*> pages_;

    // view
    QListWidget* listPage_;
    QStackedWidget* stackedWidget_;
    QDialogButtonBox* btns_;

private:
    void setupUi();
    void btnClicked(QAbstractButton* button);
    bool tryApply();

    void selectPage();
};

#endif // SETTINGSWIDGET_H
