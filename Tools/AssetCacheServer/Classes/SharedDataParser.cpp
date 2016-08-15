#include "SharedDataParser.h"

#include "Logger/Logger.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace SharedDataParser
{
DAVA::List<SharedPoolParams> ParsePoolsReply(const QByteArray& data)
{
    DAVA::List<SharedPoolParams> pools;

    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        DAVA::Logger::Error("Not a valid JSON document");
        return DAVA::List<SharedPoolParams>();
    }
    QJsonObject rootObj = document.object();
    QJsonValue rootValue = rootObj["pools"];

    if (rootValue.isUndefined())
    {
        DAVA::Logger::Error("'pools' key is not found");
        return DAVA::List<SharedPoolParams>();
    }

    if (!rootValue.isArray())
    {
        DAVA::Logger::Error("Array type is expected for key 'pools'");
        return DAVA::List<SharedPoolParams>();
    }

    QJsonArray rootArray = rootValue.toArray();
    for (QJsonValue val : rootArray)
    {
        if (!val.isObject())
        {
            DAVA::Logger::Error("Object type is expected");
            return DAVA::List<SharedPoolParams>();
        }
        QJsonObject poolObject = val.toObject();

        SharedPoolParams pool;
        pool.poolID = poolObject["key"].toString().toInt();
        pool.name = poolObject["name"].toString().toStdString();
        pool.description = poolObject["description"].toString().toStdString();
        pools.push_back(std::move(pool));
    }

    return pools;
}

DAVA::List<SharedServerParams> ParseServersReply(const QByteArray& data)
{
    DAVA::List<SharedServerParams> servers;

    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        DAVA::Logger::Error("Not a valid JSON document '%s'", data.data());
        return DAVA::List<SharedServerParams>();
    }
    QJsonObject rootObj = document.object();
    QJsonValue rootValue = rootObj["shared servers"];

    if (rootValue.isUndefined())
    {
        DAVA::Logger::Error("'shared servers' key is not found");
        return DAVA::List<SharedServerParams>();
    }

    if (!rootValue.isArray())
    {
        DAVA::Logger::Error("Array type is expected for key 'shared servers'");
        return DAVA::List<SharedServerParams>();
    }

    QJsonArray rootArray = rootValue.toArray();
    for (QJsonValue val : rootArray)
    {
        if (!val.isObject())
        {
            DAVA::Logger::Error("Object type is expected");
            return DAVA::List<SharedServerParams>();
        }
        QJsonObject poolObject = val.toObject();

        SharedServerParams server;
        server.serverID = poolObject["key"].toString().toInt();
        server.poolID = poolObject["poolKey"].toString().toInt();
        server.name = poolObject["name"].toString().toStdString();
        server.ip = poolObject["ip"].toString().toStdString();
        server.port = poolObject["port"].toInt();
        servers.push_back(std::move(server));
    }

    return servers;
}

ServerID ParseAddReply(const QByteArray& data)
{
    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        DAVA::Logger::Error("Not a valid JSON document '%s'", data.data());
        return 0;
    }
    QJsonObject rootObj = document.object();
    return rootObj["key"].toString().toInt();
}
}
