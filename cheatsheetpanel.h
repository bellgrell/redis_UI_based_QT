#ifndef CHEATSHEETPANEL_H
#define CHEATSHEETPANEL_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QVariant>

namespace Ui { class CheatSheetPanel; }

class CheatSheetPanel : public QWidget
{
    Q_OBJECT

public:
    explicit CheatSheetPanel(QWidget *parent = nullptr);

public slots:
    void onResponse(const QVariant &parsed, const QString &formatted);

private slots:
    void onCommandClicked(QTreeWidgetItem *item, int column);
    void onSearchChanged(const QString &text);
    void onGroupFilterChanged(int index);

private:
    void setupUI();
    void setupConnections();
    void loadCommands();

    Ui::CheatSheetPanel *ui;
};

#endif // CHEATSHEETPANEL_H
