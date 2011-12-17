#include "meeboconnectkeepalive.h"

MeeboConnectKeepAlive::MeeboConnectKeepAlive(MeeboConnect *mommy)
{
    // Link the parent locally.
    parent = mommy;
    // Create a QTimer object.
    updateCycler = new QTimer(this);
    // Create an API object.
    updateApi = new Api();
    // Store the revision number link.
    revision = parent->getRevision();
    // Move to thread
    moveToThread(this);
}

void MeeboConnectKeepAlive::run()
{
    connect(updateCycler, SIGNAL(timeout()), this, SLOT(updateCycle()));
    updateCycler->start(0);
    exec();
}

void MeeboConnectKeepAlive::updateCycle()
{
    // This is where it gets difficult.
    // Capture data.

    // Trickery and deception
    if (*revision == 3)
    {
        parent->gwidAPI();
        parent->dbgAPI();
        parent->prefgetAPI();
    }
    // Get next update.
    QMap<QString, QVariant> temp = parent->updateAPI();
    QString result = temp.value("raw").toString();

    /* DEBUG */qDebug()<<"BLATANT UPDATE CYCLE!!!"<<result;

    // Detect incorrect authentication.
    if (result.contains("\"protocol\":\"meebo\",\"data\":{\"type\":1,\"description\":\"Username does not exist\"}}}"))
    {
        emit updateAPIError("Incorrect username");
    }
    else if (result.contains("\"protocol\":\"meebo\",\"data\":{\"type\":2,\"description\":\"Incorrect password\"}}}"))
    {
        emit updateAPIError("Incorrect password");
    }
    else if (result.contains("\"protocol\":\"meebo\",\"data\":{\"type\":2,\"description\":\"Incorrect username or password.\"}}}"))
    {
        emit updateAPIError("Incorrect username or password");
    }
    else if (result.contains("\"data\":{\"stat\":\"fail\",\"msg\":\"Invalid request\",\"errorcode\":3}"))
    {
        emit updateAPIError("Bad login information");
    }

    // Detect update failures.
    else if (result.contains("<title>400 Bad Request</title>") ||
             result.contains("<title>502 Bad Gateway</title>"))
    {
        emit updateAPIError("Connection broke");
    }
    else if (temp.value("events").toList().size() == 0)
    {
        emit updateAPIError("Looks like you're IP-banned... :(");
    }

    // Broadcast the Meebo "events" data.
    emit updateAPIReply(temp);
}
