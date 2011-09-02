/**
 * Log2Log Online Chat Log Converter
 *  Formats
 *   Omegle
 *
 * @author Deltik
 */

#include "omegle.h"
#include "helper.h"
#include <QtCore>
#include <QtXml>
#include <QDateTime>

// QDebug
#include <QDebug>

/**
 * Constructor
 */
Omegle::Omegle()
{
    final = new StdFormat();
    final->setClient("Omegle");
}

/**
 * Destructor
 */
Omegle::~Omegle()
{
}

/**
 * Load a Chat Log
 */
void Omegle::load(QVariant $log_raw)
{
    qDebug() << "Omegle::load() triggered!";

    // If File Not Omegle Chat Log
    if ($log_raw.toString().indexOf("<h1><a href=\"http://omegle.com/\">Omegle</a> conversation log</h1>") == -1)
    {
        qDebug() << "Not Omegle Chat Log";
        return;
    }

    // File is Omegle Chat Log. Create new entry.
    final->newEntry();
    final->setProtocol("omegle");
    final->setSelf("you@omegle.com");
    final->setSelfAlias("You");
    final->setWith("stranger@omegle.com");
    final->setWithAlias("Stranger");

    // Fix Invalid XML for QXmlStreamReader
    $log_raw = $log_raw.toString().replace(QRegExp("(<meta[^>]+>)"),
                                           "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />");

    // Create HTML reader
    QXmlStreamReader xml($log_raw.toString());

    // Read the HTML file
    while (!xml.atEnd())
    {
        // Read next item
        QXmlStreamReader::TokenType token = xml.readNext();

        // Just some items that might be used in this scope
        qint64 $time_base;

        // Looking at element beginnings...
        if (token == QXmlStreamReader::StartElement)
        {
            // If token is the time indicator (_time)
            if (xml.qualifiedName().toString() == "h2")
            {
                xml.readNext();
                QString $date_cur = xml.text().toString();
                QDateTime $date_proc = QDateTime::fromString($date_cur, Qt::ISODate);
                $time_base = $date_proc.toMSecsSinceEpoch();
                final->setTime($time_base);
            }

            // If element is a status log (_evt)
            if (xml.attributes().value("class").toString() == "statuslog")
            {
                xml.readNext();
                QString $_evt = xml.text().toString();
                qDebug() << "_evt: " + $_evt;
                // If conversation initialized (_evt_open)
                if ($_evt == "You're now chatting with a random stranger. Say hi!")
                {
                    final->newLine();
                    final->setCode(1);
                    final->setSender("_evt_open");
                    final->setContent($_evt);
                    final->setTime($time_base);
                    final->setSpecificity(6);
                }

                // If conversation closed (_evt_close) by _self
                if ($_evt == "You have disconnected.")
                {
                    final->newLine();
                    final->setCode(1);
                    final->setSender("_evt_close");
                    final->setContent("_self");
                    final->setTime($time_base);
                    final->setSpecificity(6);
                }

                // If conversation closed (_evt_close)
                if ($_evt.indexOf("disconnected") != -1)
                {
                    final->newLine();
                    final->setCode(1);
                    final->setSender("_evt_close");
                    // If _evt_close by _with
                    if ($_evt == "Your conversational partner has disconnected.")
                        final->setContent("_with");
                    // If _evt_close by _self
                    else if ($_evt == "You have disconnected.")
                        final->setContent("_self");
                    // If _evt_close by _group
                    else if ($_evt.indexOf(" has disconnected") != -1)
                    {
                        $_evt.chop(17);
                        final->setContent("_group_byalias " + $_evt);
                    }
                    // Otherwise? O_o
                    else
                        final->setContent("_unknown");
                    final->setTime($time_base);
                    final->setSpecificity(6);
                }
            }

            // If the element is an Omegle Question (_evt)
            if (xml.attributes().value("class").toString() == "questionText")
            {
                xml.readNext();
                final->newLine();
                final->setCode(1);
                final->setSender("_evt");
                final->setContent("Question to discuss: " + xml.text().toString());
                final->setTime($time_base);
                final->setSpecificity(6);
            }

            // If the element is a chat line (_msg)
            if (xml.attributes().value("class").toString() == "logitem")
            {
                xml.readNext();
                // If the element was sent by _self (_msg_self) {There is an exception.}
                if (xml.attributes().value("class").toString() == "youmsg")
                {
                    // Entering StartElement for <strong class="msgsource">
                    xml.readNext();
                    // Entering message source
                    xml.readNext();

                    final->newLine();
                    final->setCode(0);
                    QString $_alias = xml.text().toString();
                    $_alias.chop(1);
                    // Exception: If element was actually being sent by Stranger 1
                    if (xml.text().toString() == "Stranger 1:")
                    {
                        final->setSender("stranger1@omegle.com");
                        final->setAlias($_alias);
                    }
                    else
                    {
                        final->setSender("_self");
                        final->setAlias($_alias);
                    }

                    // Entering EndElement for </strong>
                    xml.readNext();
                    // Entering message contents
                    xml.readNext();

                    final->setContent(xml.text().toString());
                    final->setTime($time_base);
                    final->setSpecificity(6);
                }
                // If the element was sent by _with (_msg_with) {There is an exception.}
                if (xml.attributes().value("class").toString() == "strangermsg")
                {
                    // Entering StartElement for <strong class="msgsource">
                    xml.readNext();
                    // Entering message source
                    xml.readNext();

                    final->newLine();
                    final->setCode(0);
                    QString $_alias = xml.text().toString();
                    $_alias.chop(1);
                    // Exception: If element was actually being sent by Stranger 2
                    if (xml.text().toString() == "Stranger 2:")
                    {
                        final->setSender("stranger2@omegle.com");
                        final->setAlias($_alias);
                    }
                    else
                    {
                        final->setSender("_with");
                        final->setAlias($_alias);
                    }

                    // Entering EndElement for </strong>
                    xml.readNext();
                    // Entering message contents
                    xml.readNext();

                    final->setContent(xml.text().toString());
                    final->setTime($time_base);
                    final->setSpecificity(6);
                }
            }
        }
    }
    /*DEBUG*/
    qDebug()<<final->final;
}

/**
 * Generate Log from Standardized Log
 */
QVariant Omegle::generate(StdFormat *$log)
{
    // Generated Log Container
    QVariant $log_generated;
    // Counter
    int $i = 1;

    // If no entries, quit.
    if ($log->gotoEntry(0) == false)
        return false;

    do
    {
        // Put the longer variables into something more readily accessible.
        QString $protocol      = $log->getProtocol();
        QString $account       = $log->getSelf();
        QString $with          = $log->getWith();
        QString $with_alias    = $log->getWithAlias();
        qint64  $time_base     = $log->getTime();
        QString $timezone_base = $log->getTimezone();
    }
    while ($log->hasNextEntry());

    return $log_generated;
}

/**
 * Process "From" Request
 */
StdFormat* Omegle::from(QHash<QString, QVariant> data)
{
    // Step 1/3: Fetch the data.
    QMap<QString, QVariant> list;
    if (data["files"].isNull())
        list = Helper::files_get_contents(data["path"].toString());
    else
        list = data["files"].toMap();
    QMap<QString, QVariant>::const_iterator i = list.constBegin();

    // Step 2/3: Process the data.
    /*foreach ($raw as $raw_item)
      {
      $this->load($raw_item);
      }*/
    int c = 0;
    while (i != list.constEnd())
    {
        QVariant $raw_item = (i.value());
        qDebug() << "Encountered file: " + i.key();
        this->load($raw_item);
        c++;
        msleep(1);
        updateProgress((40 * c / list.count()) + 10, "Interpreted " + QVariant(c).toString() + "/" + QVariant(list.count()).toString() + " files...");
        i ++;
    }

    // Step 3/3: Submit the Log2Log-standardized chat log array.
    emit finished();
    return this->final;
}

/**
 * Process "To" Request
 */
void Omegle::to()
{
}
