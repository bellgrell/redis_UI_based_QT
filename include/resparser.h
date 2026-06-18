#ifndef RESPPARSER_H
#define RESPPARSER_H

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QByteArray>
#include <QList>

/**
 * Redis RESP protocol parser.
 * Handles parsing of RESP arrays (commands) and formatting of responses.
 */
class RespParser
{
public:
    RespParser();

    /// Encode a command into RESP array format (*N\r\n$L\r\nDATA\r\n...)
    static QByteArray encodeCommand(const QStringList &parts);

    /// Parse raw RESP data into a list of results (handles pipelining)
    static QList<QVariant> parseResponse(const QByteArray &data, int &consumed);

    /// Format a parsed RESP value for human-readable display
    static QString formatValue(const QVariant &value, int indent = 0);

    /// Parse a command line string into tokens (supports quoted strings)
    static QStringList parseCommandLine(const QString &line);
};

#endif // RESPPARSER_H
