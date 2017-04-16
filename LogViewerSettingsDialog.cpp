#include "LogViewerSettingsDialog.h"

#include <QSettings>
#include <QtWidgets>

#include "LogViewer.h"
#include "LogViewerSettings.h"
#include "RegExpParserSettingsWidget.h"

namespace {
enum { C_SEVERITY = 0,
    C_COLOR,
    C_NUM_COLUMN };

class ParserSettingsPage : public SettingsPage {
public:
    ParserSettingsPage(QWidget* parent = 0)
        : SettingsPage(parent)
    {
        auto l = new QFormLayout();
        l->addRow(tr("Name"), cmbSettings_ = new QComboBox());

        auto g = new QGroupBox(tr("Settings"));
        {
            auto vl = new QVBoxLayout();
            vl->addWidget(stack_ = new QStackedWidget());
            g->setLayout(vl);
        }
        l->addRow(g);

        l->addRow(tr("Separator"), lineSeparator_ = new QLineEdit());

        setLayout(l);

        addParsers();
        lineSeparator_->setText(LogViewerSettings::get()->getSeparator().replace("\r", "\\r").replace("\n", "\\n"));

        connect(cmbSettings_, &QComboBox::currentTextChanged, this, &ParserSettingsPage::selectWidget);
        cmbSettings_->setCurrentText(LogViewerSettings::get()->getActiveParser());
    }

    bool apply() override
    {
        LogViewerSettings::get()->clearParserSettings();
        foreach (auto name, widgets_.keys()) {
            LogViewerSettings::get()->addParserSettings(name, widgets_[name]->getSettings());
        }
        LogViewerSettings::get()->setActiveParser(cmbSettings_->currentText());

        LogViewerSettings::get()->setSeparator(lineSeparator_->text().replace("\\r", "\r").replace("\\n", "\n"));

        return true;
    }

private:
    QComboBox* cmbSettings_;
    QStackedWidget* stack_;
    QMap<QString, RegExpParserSettingsWidget*> widgets_;
    QLineEdit* lineSeparator_;

private:
    void addParsers()
    {
        auto names = LogViewerSettings::get()->settingNames();
        foreach (auto name, names) {
            cmbSettings_->addItem(name);

            auto s = LogViewerSettings::get()->getParserSettings(name);
            auto w = new RegExpParserSettingsWidget(s);

            stack_->addWidget(w);
            widgets_[name] = w;
        }
    }

    void selectWidget(const QString& name)
    {
        auto w = widgets_[name];
        stack_->setCurrentWidget(w);
    }
};

class SourceCodeMappingsPage : public SettingsPage {
public:
    SourceCodeMappingsPage(QSettings* settings, LogViewer* viewer, QWidget* parent)
        : SettingsPage(parent)
        , settings_(settings)
        , viewer_(viewer)
    {
        auto l = new QVBoxLayout();

        auto g = new QGridLayout();
        g->addWidget(new QLabel(tr("Source")), 0, 0);
        g->addWidget(lineSource_ = new QLineEdit(), 0, 1);

        g->addWidget(new QLabel(tr("Local")), 1, 0);
        g->addWidget(lineLocal_ = new QLineEdit(), 1, 1);

        lineSource_->setText(settings_->value("source").toString());
        lineLocal_->setText(settings_->value("local").toString());

        l->addLayout(g);
        l->addStretch();

        setLayout(l);
    }

    bool apply() override
    {
        settings_->setValue("source", lineSource_->text());
        settings_->setValue("local", lineLocal_->text());

        viewer_->setMapping(lineSource_->text(), lineLocal_->text());
        return true;
    }

private:
    QSettings* settings_;
    LogViewer* viewer_;

    QLineEdit* lineSource_;
    QLineEdit* lineLocal_;
};
}

LogViewerSettingsDialog::LogViewerSettingsDialog(QSettings* settings, LogViewer* viewer, QWidget* parent)
    : SettingsDialog(parent)
{
    Q_ASSERT(settings);

    addPage(tr("Parser settings"), new ParserSettingsPage());
    addPage(tr("Source code mapping"), new SourceCodeMappingsPage(settings, viewer, this));

    //toTable( viewer );
}
