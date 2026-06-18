#ifndef INFOPANEL_H
#define INFOPANEL_H

#include <QWidget>
#include <QVariant>

namespace Ui { class InfoPanel; }

class InfoPanel : public QWidget
{
    Q_OBJECT

public:
    explicit InfoPanel(QWidget *parent = nullptr);

public slots:
    void onResponse(const QVariant &parsed, const QString &formatted);

signals:
    void commandRequested(const QString &commandLine);

private slots:
    void onRefreshClicked();

private:
    void setupUI();
    void setupConnections();

    Ui::InfoPanel *ui;
    bool m_awaitingInfo;
};

#endif // INFOPANEL_H
