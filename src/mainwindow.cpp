#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_client(new RedisClient(this))
{
    ui->setupUi(this);
    setWindowTitle("mini-redis Manager");
    resize(1100, 720);
    setMinimumSize(800, 500);

    setupUI();
    setupConnections();
    updateConnectionUI();
    applyDarkTheme();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // Create panels
    m_consolePanel = new ConsolePanel;
    m_keyBrowserPanel = new KeyBrowserPanel;
    m_infoPanel = new InfoPanel;
    m_monitorPanel = new MonitorPanel;
    m_cheatSheetPanel = new CheatSheetPanel;

    ui->m_stackedWidget->addWidget(m_consolePanel);
    ui->m_stackedWidget->addWidget(m_keyBrowserPanel);
    ui->m_stackedWidget->addWidget(m_infoPanel);
    ui->m_stackedWidget->addWidget(m_monitorPanel);
    ui->m_stackedWidget->addWidget(m_cheatSheetPanel);
    ui->m_stackedWidget->setCurrentIndex(0);

    // Style logo
    QFont logoFont = ui->logoLabel->font();
    logoFont.setPointSize(14);
    logoFont.setBold(true);
    ui->logoLabel->setFont(logoFont);

    statusBar()->showMessage("Ready - Connect to your mini-redis server");
}

void MainWindow::setupConnections()
{
    connect(ui->m_connectBtn, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(m_client, &RedisClient::connected, this, &MainWindow::onConnected);
    connect(m_client, &RedisClient::disconnected, this, &MainWindow::onDisconnected);
    connect(m_client, &RedisClient::connectionError, this, &MainWindow::onConnectionError);
    connect(m_client, &RedisClient::stateChanged, this, &MainWindow::onConnectionStatusChanged);
    connect(ui->m_navList, &QListWidget::currentRowChanged, this, &MainWindow::onNavItemClicked);

    // Wire client to panels
    connect(m_client, &RedisClient::responseReceived, m_consolePanel, &ConsolePanel::onResponse);
    connect(m_client, &RedisClient::responseReceived, m_monitorPanel, &MonitorPanel::onResponse);
    connect(m_client, &RedisClient::responseReceived, m_keyBrowserPanel, &KeyBrowserPanel::onResponse);
    connect(m_client, &RedisClient::responseReceived, m_infoPanel, &InfoPanel::onResponse);

    // Wire console send to client + monitor
    connect(m_consolePanel, &ConsolePanel::commandRequested, m_client, &RedisClient::sendRaw);
    connect(m_consolePanel, &ConsolePanel::commandRequested, m_monitorPanel, &MonitorPanel::onCommandSent);

    // Wire panels
    connect(m_keyBrowserPanel, &KeyBrowserPanel::commandRequested, m_client, &RedisClient::sendRaw);
    connect(m_infoPanel, &InfoPanel::commandRequested, m_client, &RedisClient::sendRaw);
}

void MainWindow::onConnectClicked()
{
    if (m_client->state() == RedisClient::Connected) {
        m_client->disconnectFromServer();
    } else {
        ui->m_hostEdit->setEnabled(false);
        ui->m_portSpin->setEnabled(false);
        ui->m_connectBtn->setText("Connecting...");
        ui->m_connectBtn->setEnabled(false);
        m_client->connectToServer(ui->m_hostEdit->text(), ui->m_portSpin->value());
    }
}

void MainWindow::onConnectionStatusChanged(RedisClient::State)
{
    updateConnectionUI();
}

void MainWindow::onConnected(const QString &host, int port)
{
    statusBar()->showMessage(QString("Connected to %1:%2").arg(host).arg(port));
}

void MainWindow::onDisconnected(const QString &)
{
    statusBar()->showMessage("Disconnected from server");
}

void MainWindow::onConnectionError(const QString &message)
{
    ui->m_hostEdit->setEnabled(true);
    ui->m_portSpin->setEnabled(true);
    statusBar()->showMessage("Connection error: " + message);
}

void MainWindow::updateConnectionUI()
{
    switch (m_client->state()) {
    case RedisClient::Connected:
        ui->m_statusLabel->setText("🟢 Connected");
        ui->m_connectBtn->setText("Disconnect");
        ui->m_connectBtn->setEnabled(true);
        ui->m_hostEdit->setEnabled(false);
        ui->m_portSpin->setEnabled(false);
        break;
    case RedisClient::Connecting:
        ui->m_statusLabel->setText("🟡 Connecting...");
        ui->m_connectBtn->setText("Connecting...");
        ui->m_connectBtn->setEnabled(false);
        break;
    case RedisClient::Disconnected:
        ui->m_statusLabel->setText("🔴 Disconnected");
        ui->m_connectBtn->setText("Connect");
        ui->m_connectBtn->setEnabled(true);
        ui->m_hostEdit->setEnabled(true);
        ui->m_portSpin->setEnabled(true);
        break;
    }
}

void MainWindow::onNavItemClicked(int index)
{
    ui->m_stackedWidget->setCurrentIndex(index);
}

void MainWindow::applyDarkTheme()
{
    setStyleSheet(R"(
        QMainWindow, QWidget {
            background-color: #1e1e2e;
            color: #cdd6f4;
            font-family: "Segoe UI", sans-serif;
        }
        QFrame#sidebar {
            background-color: #181825;
            border: none;
        }
        QLabel#logoLabel {
            color: #89b4fa;
            padding: 4px 0;
        }
        QFrame#connFrame {
            background-color: #1e1e2e;
            border-radius: 6px;
            padding: 6px;
        }
        QLineEdit {
            background-color: #313244;
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 4px;
            padding: 4px 8px;
        }
        QSpinBox, QSpinBox#m_portSpin {
            background-color: #313244;
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 4px;
            padding: 4px;
            font-family: "Segoe UI", sans-serif;
            min-height: 18px;
        }
        QSpinBox::up-button, QSpinBox::down-button {
            background-color: #45475a;
            border: none;
            border-radius: 2px;
            width: 14px;
        }
        QSpinBox::up-arrow { image: none; border: none; }
        QSpinBox::down-arrow { image: none; border: none; }
        QComboBox {
            background-color: #313244;
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 4px;
            padding: 4px 8px;
        }
        QComboBox::drop-down { border: none; width: 20px; }
        QComboBox QAbstractItemView {
            background-color: #1e1e2e; color: #cdd6f4;
            selection-background-color: #313244;
            border: 1px solid #45475a;
        }
        QLabel#m_statusLabel {
            font-size: 11px;
            color: #a6adc8;
            padding: 2px 0;
        }
        QPushButton#m_connectBtn {
            background-color: #89b4fa;
            color: #1e1e2e;
            border: none;
            border-radius: 4px;
            padding: 6px 12px;
            font-weight: bold;
        }
        QPushButton#m_connectBtn:hover {
            background-color: #b4d0fb;
        }
        QPushButton#m_connectBtn:disabled {
            background-color: #45475a;
            color: #6c7086;
        }
        QListWidget#m_navList {
            background-color: transparent;
            color: #cdd6f4;
            font-size: 13px;
            border: none;
            outline: none;
        }
        QListWidget#m_navList::item {
            padding: 10px 12px;
            border-radius: 6px;
            margin: 2px 0;
        }
        QListWidget#m_navList::item:selected {
            background-color: #313244;
            color: #89b4fa;
        }
        QListWidget#m_navList::item:hover {
            background-color: #252538;
        }
        QFrame#separator {
            background-color: #313244;
            border: none;
            width: 1px;
        }
        QStatusBar {
            background-color: #181825;
            color: #a6adc8;
            font-size: 11px;
            border-top: 1px solid #313244;
        }
        QScrollBar:vertical {
            background-color: #1e1e2e;
            width: 8px;
            border: none;
        }
        QScrollBar::handle:vertical {
            background-color: #45475a;
            border-radius: 4px;
            min-height: 20px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
    )");
}
