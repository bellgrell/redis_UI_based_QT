#ifndef KEYBROWSERPANEL_H
#define KEYBROWSERPANEL_H

#include <QWidget>
#include <QListWidgetItem>
#include <QTimer>
#include <QVariant>

namespace Ui { class KeyBrowserPanel; }

class KeyBrowserPanel : public QWidget
{
    Q_OBJECT

public:
    explicit KeyBrowserPanel(QWidget *parent = nullptr);

public slots:
    void onResponse(const QVariant &parsed, const QString &formatted);

signals:
    void commandRequested(const QString &commandLine);

private slots:
    void onRefreshClicked();
    void onKeyClicked(QListWidgetItem *item);
    void onDeleteClicked();
    void onCopyClicked();
    void onSearchChanged(const QString &text);

private:
    void setupUI();
    void setupConnections();
    void sendCommand(const QString &cmd);

    Ui::KeyBrowserPanel *ui;

    QStringList m_allKeys;
    QString m_selectedKey;
    bool m_awaitingKeys;
    bool m_awaitingDetail;
    QTimer *m_debounceTimer;
};

#endif // KEYBROWSERPANEL_H
