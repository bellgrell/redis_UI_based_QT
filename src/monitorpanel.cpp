#include "monitorpanel.h"
#include "ui_monitorpanel.h"
#include <QFont>
#include <QScrollBar>
#include <QDateTime>
#include <QVariantMap>

MonitorPanel::MonitorPanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MonitorPanel)
    , m_monitoring(false)
    , m_cmdCount(0)
{
    ui->setupUi(this);
    setupUI();
    setupConnections();
}

void MonitorPanel::setupUI()
{
    QFont titleFont = ui->title->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    ui->title->setFont(titleFont);
    ui->title->setStyleSheet("color: #89b4fa;");

    ui->m_statusLabel->setStyleSheet(
        "color: #a6adc8; font-size: 11px; padding: 2px 8px; background: #45475a; border-radius: 8px;");
    ui->m_countLabel->setStyleSheet("color: #a6adc8; font-size: 11px;");

    ui->m_toggleBtn->setStyleSheet(R"(
        QPushButton {
            background: #89b4fa; color: #1e1e2e; border: none;
            border-radius: 4px; padding: 4px 12px; font-weight: bold; font-size: 11px;
        }
        QPushButton:hover { background: #b4d0fb; }
    )");

    ui->m_clearBtn->setStyleSheet(R"(
        QPushButton {
            background: #45475a; color: #cdd6f4; border: none;
            border-radius: 4px; padding: 4px 10px; font-size: 11px;
        }
        QPushButton:hover { background: #585b70; }
    )");

    ui->infoBar->setStyleSheet("color: #585b70; font-size: 11px; padding: 2px 4px;");

    ui->m_output->setStyleSheet(R"(
        QPlainTextEdit {
            background-color: #11111b; color: #cdd6f4;
            border: 1px solid #313244; border-radius: 6px;
            padding: 8px; font-family: "Consolas", monospace;
            font-size: 12px;
        }
    )");
}

void MonitorPanel::setupConnections()
{
    connect(ui->m_toggleBtn, &QPushButton::clicked, this, &MonitorPanel::onToggleMonitor);
    connect(ui->m_clearBtn, &QPushButton::clicked, this, &MonitorPanel::onClearClicked);
}

void MonitorPanel::onToggleMonitor()
{
    m_monitoring = !m_monitoring;
    if (m_monitoring) {
        ui->m_statusLabel->setText("🔴 Monitoring");
        ui->m_statusLabel->setStyleSheet(
            "color: #a6adc8; font-size: 11px; padding: 2px 8px;"
            "background: #f38ba8; border-radius: 8px; font-weight: bold;");
        ui->m_toggleBtn->setText("⏹️ Stop Monitor");
        ui->m_output->appendHtml(
            "<span style='color:#a6e3a1;font-weight:bold;'>--- Monitoring started ---</span>");
    } else {
        ui->m_statusLabel->setText("Inactive");
        ui->m_statusLabel->setStyleSheet(
            "color: #a6adc8; font-size: 11px; padding: 2px 8px; background: #45475a; border-radius: 8px;");
        ui->m_toggleBtn->setText("▶️ Start Monitor");
        ui->m_output->appendHtml(
            "<span style='color:#fab387;font-weight:bold;'>--- Monitoring stopped ---</span>");
    }
}

void MonitorPanel::onClearClicked()
{
    ui->m_output->clear();
    m_cmdCount = 0;
    ui->m_countLabel->setText("0 commands");
}

void MonitorPanel::onCommandSent(const QString &commandLine)
{
    if (!m_monitoring) return;

    m_cmdCount++;
    ui->m_countLabel->setText(QString::number(m_cmdCount) + " commands");

    QString ts = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    ui->m_output->appendHtml(
        QString("<span style='color:#585b70;'>[%1]</span> "
                "<span style='color:#cdd6f4;'>❯</span> "
                "<span style='color:#a6e3a1;'>%2</span>")
            .arg(ts, commandLine.toHtmlEscaped()));

    QScrollBar *sb = ui->m_output->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void MonitorPanel::onResponse(const QVariant &parsed, const QString &formatted)
{
    if (!m_monitoring) return;

    bool isError = parsed.type() == QVariant::Map
                   && parsed.toMap().contains("error");
    QString color = isError ? "#f38ba8" : "#a6e3a1";
    ui->m_output->appendHtml(
        QString("<span style='color:%1;'>  %2</span>").arg(color, formatted.toHtmlEscaped()));
}
