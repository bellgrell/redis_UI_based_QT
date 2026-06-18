#include "infopanel.h"
#include "ui_infopanel.h"
#include <QFont>

InfoPanel::InfoPanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InfoPanel)
    , m_awaitingInfo(false)
{
    ui->setupUi(this);
    setupUI();
    setupConnections();
}

void InfoPanel::setupUI()
{
    QFont titleFont = ui->m_title->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    ui->m_title->setFont(titleFont);
    ui->m_title->setStyleSheet("color: #89b4fa;");

    ui->m_refreshBtn->setStyleSheet(R"(
        QPushButton {
            background: #89b4fa; color: #1e1e2e; border: none;
            border-radius: 4px; padding: 4px 12px; font-weight: bold; font-size: 11px;
        }
        QPushButton:hover { background: #b4d0fb; }
    )");

    ui->m_output->setStyleSheet(R"(
        QPlainTextEdit {
            background-color: #11111b; color: #cdd6f4;
            border: 1px solid #313244; border-radius: 6px;
            padding: 12px; font-family: "Consolas", monospace;
            font-size: 12px;
        }
    )");
}

void InfoPanel::setupConnections()
{
    connect(ui->m_refreshBtn, &QPushButton::clicked, this, &InfoPanel::onRefreshClicked);
}

void InfoPanel::onRefreshClicked()
{
    m_awaitingInfo = true;
    ui->m_output->clear();
    ui->m_output->appendPlainText("Fetching server info...");
    emit commandRequested("INFO");
}

void InfoPanel::onResponse(const QVariant &parsed, const QString &)
{
    if (!m_awaitingInfo) return;
    if (parsed.type() != QVariant::String) return;

    QString info = parsed.toString();
    ui->m_output->clear();

    QStringList lines = info.split("\r\n");
    for (const QString &line : lines) {
        if (line.startsWith('#')) {
            QString section = line.mid(2).trimmed();
            ui->m_output->appendHtml(
                QString("<span style='color:#89b4fa;font-weight:bold;font-size:13px;'>"
                        "--- %1 ---------------------------------------</span>")
                    .arg(section.toHtmlEscaped()));
        } else if (line.contains(':')) {
            int colon = line.indexOf(':');
            QString key = line.left(colon);
            QString value = line.mid(colon + 1);
            ui->m_output->appendHtml(
                QString("<span style='color:#a6adc8;'>%1:</span> "
                        "<span style='color:#a6e3a1;'>%2</span>")
                    .arg(key.toHtmlEscaped(), value.toHtmlEscaped()));
        }
    }
    m_awaitingInfo = false;
}
