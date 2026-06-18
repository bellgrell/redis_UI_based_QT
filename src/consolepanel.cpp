#include "consolepanel.h"
#include "ui_consolepanel.h"
#include <QKeyEvent>
#include <QFont>
#include <QScrollBar>
#include <QDateTime>
#include <QVariantMap>

static const char *commandSuggestions[] = {
    "SET", "GET", "DEL", "EXISTS", "EXPIRE", "TTL", "KEYS", "TYPE",
    "PING", "ECHO", "QUIT", "FLUSHALL", "FLUSHDB", "DBSIZE", "INFO",
    "SELECT", "RENAME", "APPEND", "STRLEN", "INCR", "DECR",
    "LPUSH", "RPUSH", "LPOP", "RPOP", "LLEN", "LRANGE",
    "SADD", "SREM", "SMEMBERS", "SCARD", "SISMEMBER",
    "HSET", "HGET", "HGETALL", "HDEL", "HLEN", "HEXISTS",
    "ZADD", "ZRANGE", "ZREM", "ZCARD", nullptr
};

ConsolePanel::ConsolePanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ConsolePanel)
    , m_historyIndex(-1)
    , m_cmdCount(0)
{
    ui->setupUi(this);
    setupUI();
    setupConnections();
}

ConsolePanel::~ConsolePanel()
{
    delete ui;
}

void ConsolePanel::setupUI()
{
    QFont titleFont = ui->title->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    ui->title->setFont(titleFont);
    ui->title->setStyleSheet("color: #89b4fa;");

    ui->m_cmdCountLabel->setStyleSheet(
        "color: #a6adc8; font-size: 11px; padding: 2px 8px; background: #313244; border-radius: 8px;");

    ui->m_clearBtn->setStyleSheet(R"(
        QPushButton {
            background: #45475a; color: #cdd6f4; border: none;
            border-radius: 4px; padding: 4px 10px; font-size: 11px;
        }
        QPushButton:hover { background: #585b70; }
    )");

    ui->m_output->setStyleSheet(R"(
        QPlainTextEdit {
            background-color: #11111b;
            color: #cdd6f4;
            border: 1px solid #313244;
            border-radius: 6px;
            padding: 8px;
            font-family: "Cascadia Code", "JetBrains Mono", "Consolas", monospace;
            font-size: 13px;
            selection-background-color: #45475a;
        }
    )");
    ui->m_output->appendHtml(
        "<span style='color:#a6adc8;'>mini-redis Console - Type a command and press Enter</span>");
    ui->m_output->appendHtml(
        "<span style='color:#585b70;'>─────────────────────────────────────</span>");

    ui->m_infoLabel->setStyleSheet("color: #585b70; font-size: 11px; padding: 2px 4px;");

    ui->prompt->setStyleSheet("color: #a6e3a1; font-weight: bold; font-size: 16px;");

    ui->m_input->setStyleSheet(R"(
        QLineEdit {
            background-color: #313244;
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 6px;
            padding: 8px 12px;
            font-family: "Cascadia Code", "JetBrains Mono", "Consolas", monospace;
            font-size: 13px;
            selection-background-color: #585b70;
        }
        QLineEdit:focus { border-color: #89b4fa; }
    )");

    ui->m_sendBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #89b4fa; color: #1e1e2e; border: none;
            border-radius: 6px; padding: 8px 16px; font-weight: bold;
        }
        QPushButton:hover { background-color: #b4d0fb; }
        QPushButton:pressed { background-color: #74c7ec; }
    )");

    ui->m_input->installEventFilter(this);
}

void ConsolePanel::setupConnections()
{
    connect(ui->m_sendBtn, &QPushButton::clicked, this, &ConsolePanel::onSendClicked);
    connect(ui->m_input, &QLineEdit::returnPressed, this, &ConsolePanel::onCommandEntered);
    connect(ui->m_clearBtn, &QPushButton::clicked, this, &ConsolePanel::onClearClicked);
}

bool ConsolePanel::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->m_input && event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Up) {
            if (!m_history.isEmpty() && m_historyIndex > 0) {
                m_historyIndex--;
                ui->m_input->setText(m_history[m_historyIndex]);
            }
            return true;
        }
        if (keyEvent->key() == Qt::Key_Down) {
            if (m_historyIndex < m_history.size() - 1) {
                m_historyIndex++;
                ui->m_input->setText(m_history[m_historyIndex]);
            } else {
                m_historyIndex = m_history.size();
                ui->m_input->clear();
            }
            return true;
        }
        if (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Backtab) {
            // Simple autocomplete: get current text and try to match
            QString text = ui->m_input->text().toUpper();
            for (int i = 0; commandSuggestions[i] != nullptr; i++) {
                if (QString(commandSuggestions[i]).startsWith(text)) {
                    ui->m_input->setText(commandSuggestions[i]);
                    break;
                }
            }
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void ConsolePanel::onSendClicked()
{
    onCommandEntered();
}

void ConsolePanel::onCommandEntered()
{
    QString cmd = ui->m_input->text().trimmed();
    if (cmd.isEmpty()) return;

    if (m_history.isEmpty() || m_history.last() != cmd)
        m_history.append(cmd);
    m_historyIndex = m_history.size();

    m_cmdCount++;
    ui->m_cmdCountLabel->setText(QString::number(m_cmdCount) + " commands");

    QString ts = QDateTime::currentDateTime().toString("HH:mm:ss");
    ui->m_output->appendHtml(
        QString("<span style='color:#585b70;'>[%1]</span> "
                "<span style='color:#a6e3a1;'>❯</span> "
                "<span style='color:#cdd6f4;font-weight:bold;'>%2</span>")
            .arg(ts, cmd.toHtmlEscaped()));

    ui->m_input->clear();
    ui->m_infoLabel->setText("Sent command");
    emit commandRequested(cmd);
}

void ConsolePanel::onClearClicked()
{
    ui->m_output->clear();
    ui->m_output->appendHtml("<span style='color:#a6adc8;'>Console cleared</span>");
    ui->m_output->appendHtml("<span style='color:#585b70;'>─────────────────────────────────────</span>");
    m_cmdCount = 0;
    ui->m_cmdCountLabel->setText("0 commands");
}

void ConsolePanel::onResponse(const QVariant &parsed, const QString &formatted)
{
    bool isError = parsed.type() == QVariant::Map
                   && parsed.toMap().contains("error");

    QString color = isError ? "#f38ba8" : "#a6e3a1";
    ui->m_output->appendHtml(
        QString("<span style='color:%1;'>%2</span>").arg(color, formatted.toHtmlEscaped()));

    QScrollBar *sb = ui->m_output->verticalScrollBar();
    sb->setValue(sb->maximum());

    ui->m_infoLabel->setText(isError ? "Error response" : "Response received");
}
