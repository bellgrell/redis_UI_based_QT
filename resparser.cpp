#include "resparser.h"
#include <QDebug>

RespParser::RespParser()
{
}

QByteArray RespParser::encodeCommand(const QStringList &parts)
{
    QByteArray result;
    result.append('*');
    result.append(QByteArray::number(parts.size()));
    result.append("\r\n");

    for (const QString &part : parts) {
        QByteArray utf8 = part.toUtf8();
        result.append('$');
        result.append(QByteArray::number(utf8.size()));
        result.append("\r\n");
        result.append(utf8);
        result.append("\r\n");
    }
    return result;
}

QList<QVariant> RespParser::parseResponse(const QByteArray &data, int &consumed)
{
    QList<QVariant> results;
    int pos = 0;
    int len = data.size();

    while (pos < len) {
        char type = data.at(pos);

        if (type == '+') {
            // Simple string
            int end = data.indexOf("\r\n", pos);
            if (end == -1) break;
            QString str = QString::fromUtf8(data.mid(pos + 1, end - pos - 1));
            results.append(str);
            pos = end + 2;
        }
        else if (type == '-') {
            // Error
            int end = data.indexOf("\r\n", pos);
            if (end == -1) break;
            QString err = QString::fromUtf8(data.mid(pos + 1, end - pos - 1));
            QVariantMap errMap;
            errMap["error"] = err;
            results.append(errMap);
            pos = end + 2;
        }
        else if (type == ':') {
            // Integer
            int end = data.indexOf("\r\n", pos);
            if (end == -1) break;
            bool ok;
            qint64 val = QString::fromUtf8(data.mid(pos + 1, end - pos - 1)).toLongLong(&ok);
            results.append(ok ? val : QVariant());
            pos = end + 2;
        }
        else if (type == '$') {
            // Bulk string
            int end = data.indexOf("\r\n", pos);
            if (end == -1) break;
            int bulkLen = QString::fromUtf8(data.mid(pos + 1, end - pos - 1)).toInt();
            pos = end + 2;

            if (bulkLen == -1) {
                // Null bulk string
                results.append(QVariant());
            } else {
                if (pos + bulkLen + 2 > len) break; // incomplete
                QByteArray bulkData = data.mid(pos, bulkLen);
                results.append(QString::fromUtf8(bulkData));
                pos += bulkLen + 2;
            }
        }
        else if (type == '*') {
            // Array
            int end = data.indexOf("\r\n", pos);
            if (end == -1) break;
            int arrLen = QString::fromUtf8(data.mid(pos + 1, end - pos - 1)).toInt();
            pos = end + 2;

            if (arrLen == -1) {
                // Null array
                results.append(QVariant());
            } else {
                QVariantList arr;
                for (int i = 0; i < arrLen; i++) {
                    if (pos >= len) {
                        // Incomplete — unwind
                        return results;
                    }
                    int dummy = 0;
                    QList<QVariant> subResults = parseResponse(data.mid(pos), dummy);
                    if (subResults.isEmpty()) {
                        // Incomplete
                        return results;
                    }
                    arr.append(subResults.first());

                    // Advance pos by the number of bytes consumed for this sub-result
                    int subConsumed = 0;
                    parseResponse(data.mid(pos), subConsumed);
                    pos += subConsumed;
                }
                results.append(arr);
            }
        }
        else {
            // Unknown type, stop
            break;
        }
    }

    consumed = pos;
    return results;
}

QString RespParser::formatValue(const QVariant &value, int indent)
{
    QString pad(indent * 2, ' ');

    if (value.isNull() || !value.isValid()) {
        return pad + "(nil)";
    }

    if (value.type() == QVariant::Map) {
        QVariantMap map = value.toMap();
        if (map.contains("error")) {
            return pad + "(error) " + map["error"].toString();
        }
        return pad + "(error)";
    }

    if (value.type() == QVariant::List || value.canConvert<QVariantList>()) {
        QVariantList list = value.toList();
        if (list.isEmpty()) {
            return pad + "(empty array)";
        }
        QString result;
        for (int i = 0; i < list.size(); i++) {
            if (i > 0) result += "\n";
            result += pad + QString::number(i + 1) + ") ";
            // For nested arrays or complex types, sub-items indent by 1
            result += formatValue(list[i], 0).trimmed();
        }
        return result;
    }

    if (value.type() == QVariant::LongLong || value.type() == QVariant::Int) {
        return pad + "(integer) " + value.toString();
    }

    return pad + value.toString();
}

QStringList RespParser::parseCommandLine(const QString &line)
{
    QStringList parts;
    int i = 0;
    QString current;

    while (i < line.length()) {
        QChar c = line[i];

        if (c == '"' || c == '\'') {
            QChar quote = c;
            i++;
            current.clear();
            while (i < line.length() && line[i] != quote) {
                if (line[i] == '\\' && i + 1 < line.length()) {
                    i++;
                    current += line[i];
                } else {
                    current += line[i];
                }
                i++;
            }
            i++; // skip closing quote
            if (!current.isEmpty()) {
                parts.append(current);
                current.clear();
            }
        } else if (c == ' ') {
            if (!current.isEmpty()) {
                parts.append(current);
                current.clear();
            }
            i++;
        } else {
            current += c;
            i++;
        }
    }

    if (!current.isEmpty()) {
        parts.append(current);
    }

    return parts;
}
