#include "keybrowserpanel.h"
#include "ui_keybrowserpanel.h"
#include "resparser.h"
#include <QFont>
#include <QDateTime>
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>

KeyBrowserPanel::KeyBrowserPanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::KeyBrowserPanel)
    , m_awaitingKeys(false)
    , m_awaitingDetail(false)
    , m_debounceTimer(new QTimer(this))
{
    ui->setupUi(this);
    setupUI();
    setupConnections();

    m_debounceTimer->setSingleShot(true);
    m_debounceTimer->setInterval(300);
    connect(m_debounceTimer, &QTimer::timeout, this, &KeyBrowserPanel::onRefreshClicked);
}

void KeyBrowserPanel::setupUI()
{
    QFont titleFont = ui->title->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    ui->title->setFont(titleFont);
    ui->title->setStyleSheet("color: #89b4fa;");

    ui->m_keyCountLabel->setStyleSheet(
        "color: #a6adc8; font-size: 11px; padding: 2px 8px; background: #313244; border-radius: 8px;");

    ui->m_refreshBtn->setStyleSheet(R"(
        QPushButton {
            background: #89b4fa; color: #1e1e2e; border: none;
            border-radius: 4px; padding: 4px 12px; font-weight: bold; font-size: 11px;
        }
        QPushButton:hover { background: #b4d0fb; }
    )");

    ui->m_searchInput->setStyleSheet(R"(
        QLineEdit {
            background-color: #313244; color: #cdd6f4;
            border: 1px solid #45475a; border-radius: 6px;
            padding: 6px 10px; font-size: 12px;
        }
        QLineEdit:focus { border-color: #89b4fa; }
    )");

    ui->m_keysList->setStyleSheet(R"(
        QListWidget {
            background-color: #11111b; border: 1px solid #313244;
            border-radius: 6px; padding: 4px; font-size: 12px;
        }
        QListWidget::item {
            padding: 6px 8px; border-radius: 4px; margin: 1px 0;
        }
        QListWidget::item:selected {
            background-color: #313244; color: #89b4fa;
        }
        QListWidget::item:hover {
            background-color: #252538;
        }
    )");
    ui->m_keysList->setMinimumWidth(200);
    ui->m_keysList->setMaximumWidth(350);

    ui->m_detailKeyName->setStyleSheet("font-weight: bold; font-size: 14px; color: #cdd6f4;");

    ui->m_copyBtn->setStyleSheet(R"(
        QPushButton {
            background: #45475a; color: #cdd6f4; border: none;
            border-radius: 4px; padding: 4px 8px; font-size: 11px;
        }
        QPushButton:hover { background: #585b70; }
    )");

    ui->m_deleteBtn->setStyleSheet(R"(
        QPushButton {
            background: #f38ba8; color: #1e1e2e; border: none;
            border-radius: 4px; padding: 4px 8px; font-size: 11px; font-weight: bold;
        }
        QPushButton:hover { background: #fab387; }
    )");

    ui->m_detailTtlValue->setStyleSheet("color: #a6adc8; font-size: 11px;");
    ui->m_detailSizeValue->setStyleSheet("color: #a6adc8; font-size: 11px;");

    ui->m_detailValue->setStyleSheet(R"(
        QTextEdit {
            background-color: #1e1e2e; color: #a6e3a1;
            border: 1px solid #313244; border-radius: 4px;
            padding: 8px; font-family: "Consolas", monospace;
            font-size: 12px;
        }
    )");
}

void KeyBrowserPanel::setupConnections()
{
    connect(ui->m_refreshBtn, &QPushButton::clicked, this, &KeyBrowserPanel::onRefreshClicked);
    connect(ui->m_keysList, &QListWidget::itemClicked, this, &KeyBrowserPanel::onKeyClicked);
    connect(ui->m_deleteBtn, &QPushButton::clicked, this, &KeyBrowserPanel::onDeleteClicked);
    connect(ui->m_copyBtn, &QPushButton::clicked, this, &KeyBrowserPanel::onCopyClicked);
    connect(ui->m_searchInput, &QLineEdit::textChanged, this, &KeyBrowserPanel::onSearchChanged);
}

void KeyBrowserPanel::sendCommand(const QString &cmd)
{
    emit commandRequested(cmd);
}

void KeyBrowserPanel::onRefreshClicked()
{
    m_awaitingKeys = true;
    m_awaitingDetail = false;
    m_allKeys.clear();
    ui->m_keysList->clear();
    sendCommand("KEYS *");
}

void KeyBrowserPanel::onSearchChanged(const QString &)
{
    m_debounceTimer->start();
}

void KeyBrowserPanel::onKeyClicked(QListWidgetItem *item)
{
    if (!item) return;
    // The item text is "🔑  keyname" — extract the key
    QString text = item->text();
    m_selectedKey = text.section("  ", -1).trimmed();
    m_awaitingDetail = true;

    ui->m_detailKeyName->setText(m_selectedKey);
    ui->m_detailKeyType->setStyleSheet(
        "font-size: 11px; padding: 2px 8px; border-radius: 8px;"
        "background-color: #45475a; color: #cdd6f4;");
    ui->m_detailKeyType->setText("...");
    ui->m_detailValue->clear();
    ui->m_detailValue->setPlaceholderText("Loading...");
    ui->m_detailTtlValue->setText("TTL: ...");
    ui->m_detailSizeValue->setText("Size: ...");

    sendCommand("TYPE " + m_selectedKey);
    sendCommand("TTL " + m_selectedKey);
}

void KeyBrowserPanel::onResponse(const QVariant &parsed, const QString &)
{
    if (m_awaitingKeys && parsed.canConvert<QVariantList>()) {
        QVariantList list = parsed.toList();
        m_allKeys.clear();
        ui->m_keysList->clear();
        for (const QVariant &v : list) {
            QString key = v.toString();
            m_allKeys.append(key);

            QString searchText = ui->m_searchInput->text();
            if (!searchText.isEmpty()) {
                if (!key.contains(searchText.replace('*', ""), Qt::CaseInsensitive))
                    continue;
            }
            ui->m_keysList->addItem(new QListWidgetItem("🔑  " + key));
        }
        ui->m_keyCountLabel->setText(QString::number(m_allKeys.size()) + " keys");
        m_awaitingKeys = false;
        return;
    }

    if (m_awaitingDetail && !m_selectedKey.isEmpty()) {
        if (parsed.type() == QVariant::String) {
            QString typeStr = parsed.toString().toUpper();
            QString bg;
            if (typeStr == "STRING") bg = "#a6e3a1";
            else if (typeStr == "LIST") bg = "#f9e2af";
            else if (typeStr == "SET") bg = "#fab387";
            else if (typeStr == "HASH") bg = "#89b4fa";
            else if (typeStr == "ZSET") bg = "#cba6f7";
            else bg = "#a6adc8";

            ui->m_detailKeyType->setStyleSheet(
                QString("font-size: 11px; padding: 2px 8px; border-radius: 8px;"
                        "background-color: %1; color: #1e1e2e; font-weight: bold;").arg(bg));
            ui->m_detailKeyType->setText(typeStr);

            if (typeStr == "STRING") sendCommand("GET " + m_selectedKey);
            else if (typeStr == "LIST") sendCommand("LRANGE " + m_selectedKey + " 0 -1");
            else if (typeStr == "SET") sendCommand("SMEMBERS " + m_selectedKey);
            else if (typeStr == "HASH") sendCommand("HGETALL " + m_selectedKey);
            else if (typeStr == "ZSET") sendCommand("ZRANGE " + m_selectedKey + " 0 -1 WITHSCORES");
            return;
        }

        if (parsed.type() == QVariant::LongLong || parsed.type() == QVariant::Int) {
            qint64 ttl = parsed.toLongLong();
            if (ttl == -1) ui->m_detailTtlValue->setText("TTL: ∞ (no expiry)");
            else if (ttl == -2) ui->m_detailTtlValue->setText("TTL: expired / not found");
            else ui->m_detailTtlValue->setText(QString("TTL: %1s").arg(ttl));
            return;
        }

        if (parsed.canConvert<QVariantList>() || parsed.type() == QVariant::String) {
            QString val = RespParser::formatValue(parsed);
            ui->m_detailValue->setText(val);
            ui->m_detailSizeValue->setText(QString("Size: %1").arg(val.length()));
            return;
        }
    }
}

void KeyBrowserPanel::onDeleteClicked()
{
    if (m_selectedKey.isEmpty()) return;

    auto reply = QMessageBox::question(
        this, "Delete Key",
        QString("Are you sure you want to delete key '%1'?").arg(m_selectedKey),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        sendCommand("DEL " + m_selectedKey);
        QTimer::singleShot(200, this, &KeyBrowserPanel::onRefreshClicked);
    }
}

void KeyBrowserPanel::onCopyClicked()
{
    if (ui->m_detailValue->toPlainText().isEmpty()) return;
    QApplication::clipboard()->setText(ui->m_detailValue->toPlainText());
}
