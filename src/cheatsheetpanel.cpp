#include "cheatsheetpanel.h"
#include "ui_cheatsheetpanel.h"
#include <QFont>
#include <QHeaderView>

CheatSheetPanel::CheatSheetPanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CheatSheetPanel)
{
    ui->setupUi(this);
    setupUI();
    setupConnections();
    loadCommands();
}

void CheatSheetPanel::setupUI()
{
    QFont titleFont = ui->title->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    ui->title->setFont(titleFont);
    ui->title->setStyleSheet("color: #89b4fa;");

    ui->badge->setStyleSheet(
        "color: #a6adc8; font-size: 11px; padding: 2px 8px; background: #313244; border-radius: 8px;");

    ui->m_searchInput->setStyleSheet(R"(
        QLineEdit {
            background-color: #313244; color: #cdd6f4;
            border: 1px solid #45475a; border-radius: 6px;
            padding: 6px 10px; font-size: 12px;
        }
        QLineEdit:focus { border-color: #89b4fa; }
    )");

    ui->m_commandTree->setStyleSheet(R"(
        QTreeWidget {
            background-color: #11111b; border: 1px solid #313244;
            border-radius: 6px; padding: 4px; font-size: 12px;
        }
        QTreeWidget::item {
            padding: 4px 6px; border-radius: 4px;
        }
        QTreeWidget::item:selected {
            background-color: #313244; color: #89b4fa;
        }
        QTreeWidget::item:hover {
            background-color: #252538;
        }
    )");
    ui->m_commandTree->setMinimumWidth(220);
    ui->m_commandTree->setMaximumWidth(350);

    ui->m_detailName->setStyleSheet("font-weight: bold; font-size: 15px; color: #cdd6f4;");
    ui->m_detailSummary->setStyleSheet("color: #a6adc8; font-size: 12px;");
    ui->m_detailSyntax->setStyleSheet(
        "color: #a6e3a1; font-size: 12px; font-family: 'Consolas', monospace;"
        " padding: 6px; background: #1e1e2e; border-radius: 4px;");
    ui->m_detailGroup->setStyleSheet("color: #585b70; font-size: 11px;");

    ui->m_detailDesc->setStyleSheet(R"(
        QTextEdit {
            background-color: #1e1e2e; color: #cdd6f4;
            border: 1px solid #313244; border-radius: 4px;
            padding: 8px; font-size: 12px;
        }
    )");
}

void CheatSheetPanel::setupConnections()
{
    connect(ui->m_commandTree, &QTreeWidget::itemClicked, this, &CheatSheetPanel::onCommandClicked);
    connect(ui->m_searchInput, &QLineEdit::textChanged, this, &CheatSheetPanel::onSearchChanged);
    connect(ui->m_groupFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CheatSheetPanel::onGroupFilterChanged);
}

struct CmdDef {
    QString name;
    QString summary;
    QString syntax;
    QString group;
    QString description;
};

static QList<CmdDef> builtinCommands()
{
    return {
        {"PING",        "Check server connectivity",            "PING",                     "connection",   "Returns PONG if the server is alive."},
        {"ECHO",        "Echo back a message",                 "ECHO message",              "connection",   "Returns the given message."},
        {"QUIT",        "Close the connection",                "QUIT",                     "connection",   "Closes the connection to the server."},
        {"SET",         "Set a key's value",                   "SET key value [EX seconds] [PX ms] [NX|XX]", "strings", "Sets the value of a key."},
        {"GET",         "Get a key's value",                   "GET key",                  "strings",      "Returns the value of a key."},
        {"APPEND",      "Append to a key's value",             "APPEND key value",         "strings",      "Appends a value to a key."},
        {"STRLEN",      "Get the length of a key's value",     "STRLEN key",               "strings",      "Returns the length of the string stored at key."},
        {"INCR",        "Increment a key",                     "INCR key",                 "strings",      "Increments the number stored at key by 1."},
        {"DECR",        "Decrement a key",                     "DECR key",                 "strings",      "Decrements the number stored at key by 1."},
        {"DEL",         "Delete keys",                         "DEL key [key ...]",        "generic",      "Deletes the specified keys."},
        {"EXISTS",      "Check if keys exist",                 "EXISTS key [key ...]",     "generic",      "Returns 1 if the key exists, 0 otherwise."},
        {"EXPIRE",      "Set a key's time to live",            "EXPIRE key seconds",       "generic",      "Sets a timeout on a key."},
        {"TTL",         "Get a key's remaining TTL",           "TTL key",                  "generic",      "Returns the remaining time to live of a key."},
        {"KEYS",        "Find keys matching a pattern",        "KEYS pattern",             "generic",      "Returns all keys matching the given pattern."},
        {"TYPE",        "Get the type of a key",               "TYPE key",                 "generic",      "Returns the type of the value stored at key."},
        {"RENAME",      "Rename a key",                        "RENAME key newkey",        "generic",      "Renames a key."},
        {"FLUSHALL",    "Remove all keys from all databases",  "FLUSHALL",                 "server",       "Deletes all keys from all databases."},
        {"FLUSHDB",     "Remove all keys from current DB",     "FLUSHDB",                  "server",       "Deletes all keys from the current database."},
        {"DBSIZE",      "Get the number of keys",              "DBSIZE",                   "server",       "Returns the number of keys in the current database."},
        {"INFO",        "Get server information",              "INFO [section]",           "server",       "Returns information and statistics about the server."},
        {"SELECT",      "Switch to a database",                "SELECT index",             "server",       "Switches to the specified database index."},
        {"LPUSH",       "Prepend an element to a list",        "LPUSH key element [element ...]",  "lists", "Inserts elements at the head of a list."},
        {"RPUSH",       "Append an element to a list",         "RPUSH key element [element ...]",  "lists", "Inserts elements at the tail of a list."},
        {"LPOP",        "Remove and get the first element",    "LPOP key",                 "lists",        "Removes and returns the first element of a list."},
        {"RPOP",        "Remove and get the last element",     "RPOP key",                 "lists",        "Removes and returns the last element of a list."},
        {"LLEN",        "Get the length of a list",            "LLEN key",                 "lists",        "Returns the length of a list."},
        {"LRANGE",      "Get a range of elements from a list", "LRANGE key start stop",    "lists",        "Returns a range of elements from a list."},
        {"SADD",        "Add members to a set",                "SADD key member [member ...]", "sets",     "Adds elements to a set."},
        {"SREM",        "Remove members from a set",           "SREM key member [member ...]", "sets",     "Removes elements from a set."},
        {"SMEMBERS",    "Get all members in a set",            "SMEMBERS key",             "sets",         "Returns all elements in a set."},
        {"SCARD",       "Get the number of members in a set",  "SCARD key",                "sets",         "Returns the cardinality of a set."},
        {"SISMEMBER",   "Check if a value is in a set",        "SISMEMBER key member",     "sets",         "Returns 1 if the member exists in the set."},
        {"HSET",        "Set fields in a hash",                "HSET key field value [field value ...]", "hashes", "Sets the specified fields in a hash."},
        {"HGET",        "Get a field's value from a hash",     "HGET key field",           "hashes",       "Returns the value of a field in a hash."},
        {"HGETALL",     "Get all fields and values",           "HGETALL key",              "hashes",       "Returns all fields and values in a hash."},
        {"HDEL",        "Delete fields from a hash",           "HDEL key field [field ...]", "hashes",     "Deletes the specified fields from a hash."},
        {"HLEN",        "Get the number of fields in a hash",  "HLEN key",                 "hashes",       "Returns the number of fields in a hash."},
        {"HEXISTS",     "Check if a field exists in a hash",   "HEXISTS key field",        "hashes",       "Returns 1 if the field exists in the hash."},
        {"ZADD",        "Add members to a sorted set",         "ZADD key score member [score member ...]", "sorted-sets", "Adds elements with scores to a sorted set."},
        {"ZRANGE",      "Get a range of members in a sorted set", "ZRANGE key start stop [WITHSCORES]", "sorted-sets", "Returns a range of elements in a sorted set."},
        {"ZREM",        "Remove members from a sorted set",    "ZREM key member [member ...]", "sorted-sets", "Removes elements from a sorted set."},
        {"ZCARD",       "Get the size of a sorted set",        "ZCARD key",                "sorted-sets",  "Returns the cardinality of a sorted set."},
    };
}

void CheatSheetPanel::loadCommands()
{
    auto cmds = builtinCommands();

    QMap<QString, QTreeWidgetItem *> groupItems;
    QStringList groups = {"connection", "strings", "lists", "sets", "hashes", "sorted-sets", "generic", "server"};
    QStringList labels = {"Connection", "Strings", "Lists", "Sets", "Hashes", "Sorted Sets", "Generic", "Server"};

    for (int i = 0; i < groups.size(); i++) {
        auto *item = new QTreeWidgetItem(ui->m_commandTree);
        item->setText(0, labels[i]);
        item->setData(0, Qt::UserRole, groups[i]);
        QFont f = item->font(0);
        f.setBold(true);
        item->setFont(0, f);
        item->setForeground(0, QColor("#89b4fa"));
        groupItems[groups[i]] = item;
    }

    for (const auto &cmd : cmds) {
        auto *parent = groupItems.value(cmd.group);
        if (!parent) continue;
        auto *item = new QTreeWidgetItem(parent);
        item->setText(0, cmd.name);
        item->setData(0, Qt::UserRole + 1, cmd.summary);
        item->setData(0, Qt::UserRole + 2, cmd.syntax);
        item->setData(0, Qt::UserRole + 3, cmd.description);
        item->setData(0, Qt::UserRole + 4, cmd.group);
    }
}

void CheatSheetPanel::onCommandClicked(QTreeWidgetItem *item, int)
{
    QString syntax = item->data(0, Qt::UserRole + 2).toString();
    if (syntax.isEmpty()) return; // group header

    ui->m_detailName->setText(item->text(0));
    ui->m_detailSummary->setText(item->data(0, Qt::UserRole + 1).toString());
    ui->m_detailSyntax->setText(syntax);
    ui->m_detailGroup->setText("Group: " + item->data(0, Qt::UserRole + 4).toString());
    ui->m_detailDesc->clear();
    ui->m_detailDesc->setPlainText(item->data(0, Qt::UserRole + 3).toString());
}

void CheatSheetPanel::onSearchChanged(const QString &text)
{
    for (int i = 0; i < ui->m_commandTree->topLevelItemCount(); i++) {
        auto *groupItem = ui->m_commandTree->topLevelItem(i);
        bool visible = false;
        for (int j = 0; j < groupItem->childCount(); j++) {
            auto *cmdItem = groupItem->child(j);
            bool match = text.isEmpty()
                || cmdItem->text(0).contains(text, Qt::CaseInsensitive);
            cmdItem->setHidden(!match);
            if (match) visible = true;
        }
        groupItem->setHidden(!visible);
    }
}

void CheatSheetPanel::onGroupFilterChanged(int index)
{
    QString group = ui->m_groupFilter->itemData(index).toString();
    for (int i = 0; i < ui->m_commandTree->topLevelItemCount(); i++) {
        auto *groupItem = ui->m_commandTree->topLevelItem(i);
        QString g = groupItem->data(0, Qt::UserRole).toString();
        bool match = group.isEmpty() || g == group;
        groupItem->setHidden(!match);
        if (match) {
            for (int j = 0; j < groupItem->childCount(); j++)
                groupItem->child(j)->setHidden(false);
        }
    }
}

void CheatSheetPanel::onResponse(const QVariant &, const QString &)
{
    // not used
}
