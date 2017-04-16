#ifndef LOGVIEWERSETTINGSDIALOG_H
#define LOGVIEWERSETTINGSDIALOG_H

#include "SettingsDialog.h"

class QLineEdit;
class QTableWidget;
class QSettings;

class LogViewer;
class SettingsWidget;

class LogViewerSettingsDialog : public SettingsDialog {
    Q_OBJECT
public:
    explicit LogViewerSettingsDialog(QSettings* s, LogViewer *viewer, QWidget* parent = 0);

    void applyOn(QSettings& s, LogViewer* viewer) const;

private:
    LogViewer* viewer_;

};

#endif // LOGVIEWERSETTINGSWIDGET_H
