#ifndef CONSOLEPANEL_H
#define CONSOLEPANEL_H

#include <QWidget>
#include <QVariant>

namespace Ui { class ConsolePanel; }

class ConsolePanel : public QWidget
{
    Q_OBJECT

public:
    explicit ConsolePanel(QWidget *parent = nullptr);
    ~ConsolePanel();

    void appendOutput(const QString &html);
    void setCommandCount(int count);
    void setInfoText(const QString &text);
    void clearOutput();
    QString inputText() const;
    void clearInput();

public slots:
    void onResponse(const QVariant &parsed, const QString &formatted);

signals:
    void commandRequested(const QString &commandLine);

private slots:
    void onSendClicked();
    void onCommandEntered();
    void onClearClicked();

private:
    void setupUI();
    void setupConnections();
    bool eventFilter(QObject *obj, QEvent *event) override;

    Ui::ConsolePanel *ui;

    QStringList m_history;
    int m_historyIndex;
    int m_cmdCount;
};

#endif // CONSOLEPANEL_H
