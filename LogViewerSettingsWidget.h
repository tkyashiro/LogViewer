#ifndef LOGVIEWERSETTINGSWIDGET_H
#define LOGVIEWERSETTINGSWIDGET_H

#include <QWidget>

class LogViewer;
class QTableWidget;

class LogViewerSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LogViewerSettingsWidget(LogViewer *viewer, QWidget *parent = 0);

    void applyOn( LogViewer *viewer ) const;

private:
    QTableWidget *table_;
private:
    void toTable(LogViewer *viewer);
};

#endif // LOGVIEWERSETTINGSWIDGET_H
