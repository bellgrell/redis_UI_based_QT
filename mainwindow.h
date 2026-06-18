#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui { class MainWindow; }

#include "redisclient.h"
#include "consolepanel.h"
#include "keybrowserpanel.h"
#include "infopanel.h"
#include "monitorpanel.h"
#include "cheatsheetpanel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnectClicked();
    void onConnectionStatusChanged(RedisClient::State state);
    void onConnected(const QString &host, int port);
    void onDisconnected(const QString &reason);
    void onConnectionError(const QString &message);
    void onNavItemClicked(int index);

private:
    void setupUI();
    void setupConnections();
    void updateConnectionUI();
    void applyDarkTheme();

    Ui::MainWindow *ui;

    // Core
    RedisClient *m_client;

    // Panels
    ConsolePanel *m_consolePanel;
    KeyBrowserPanel *m_keyBrowserPanel;
    InfoPanel *m_infoPanel;
    MonitorPanel *m_monitorPanel;
    CheatSheetPanel *m_cheatSheetPanel;
};

#endif // MAINWINDOW_H
