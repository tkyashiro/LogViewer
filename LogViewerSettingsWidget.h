#ifndef LOGVIEWERSETTINGSWIDGET_H
#define LOGVIEWERSETTINGSWIDGET_H

#include <QWidget>

class LogViewer;
class QLineEdit;
class QTableWidget;
class QSettings;

class LogViewerSettingsWidget : public QWidget {
    Q_OBJECT
public:
    explicit LogViewerSettingsWidget(QSettings* s, QWidget* parent = 0);

    void applyOn(QSettings& s, LogViewer* viewer) const;

private:
    QTableWidget* table_;
    QLineEdit* lineSource_;
    QLineEdit* lineLocal_;

private:
    void setupUi();
    //void toTable(LogViewer *viewer);
};

#endif // LOGVIEWERSETTINGSWIDGET_H
