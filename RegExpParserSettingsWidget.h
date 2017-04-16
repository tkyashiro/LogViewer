#ifndef REGEXPPARSERSETTINGSWIDGET_H
#define REGEXPPARSERSETTINGSWIDGET_H

#include <QWidget>

#include "Log4NetParser.h"

class QLineEdit;
class QTextEdit;
class QTableWidget;

class RegExpParserSettingsWidget : public QWidget {
public:
    RegExpParserSettingsWidget(const RegExpParser::Settings& settings, QWidget* parent = 0);

    RegExpParser::Settings getSettings() const { return model_; }

private:
    // model
    RegExpParser::Settings model_;

    // view
    QTextEdit* textPattern_;
    QTableWidget* tblMapping_;
    QTextEdit* textTestPattern_;

    QLineEdit* lineDateTimeFormat_;

private:
    void setupUi();
    void fromData();
};

#endif // REGEXPPARSERSETTINGSWIDGET_H
