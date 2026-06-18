#ifndef MONITORPANEL_H
#define MONITORPANEL_H

#include <QWidget>
#include <QVariant>

namespace Ui { class MonitorPanel; }

class MonitorPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MonitorPanel(QWidget *parent = nullptr);

public slots:
    void onResponse(const QVariant &parsed, const QString &formatted);
    void onCommandSent(const QString &commandLine);

signals:
    void commandRequested(const QString &commandLine);

private slots:
    void onToggleMonitor();
    void onClearClicked();

private:
    void setupUI();
    void setupConnections();

    Ui::MonitorPanel *ui;

    bool m_monitoring;
    int m_cmdCount;
};

#endif // MONITORPANEL_H
