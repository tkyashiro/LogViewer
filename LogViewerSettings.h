#ifndef LOGVIEWERSETTINGS_H
#define LOGVIEWERSETTINGS_H

#include <QMap>
#include <QString>

#include "Log4NetParser.h"

class LogViewerSettings {
public:
    static LogViewerSettings* get();

    void setActiveParser(const QString& s)
    {
        Q_ASSERT(parserSettings_.contains(s) || active_.isNull());
        active_ = s;
    }
    QString getActiveParser() const { return active_; }
    QStringList settingNames() const { return parserSettings_.keys(); }
    RegExpParser::Settings getActiveParserSettings() const { return parserSettings_[active_]; }

    void clearParserSettings() { parserSettings_.clear(); }
    void addParserSettings(const QString& name, const RegExpParser::Settings& settings)
    {
        parserSettings_[name] = settings;
    }
    RegExpParser::Settings getParserSettings(const QString& name) const { return parserSettings_.value(name); }
    void removeParserSettings(const QString& name) { parserSettings_.take(name); }

    QString getSeparator() const { return separator_; }
    void setSeparator(const QString& s) { separator_ = s; }
private:
    LogViewerSettings();

    QString active_;
    QMap<QString, RegExpParser::Settings> parserSettings_;

    QString separator_;
};

#endif // LOGVIEWERSETTINGS_H
