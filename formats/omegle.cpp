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
        qlonglong $time_base;

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

            // If entering a log item (_line)
            if (xml.attributes().value("class").toString() == "logitem")
            {
                xml.readNext();
            }

            // If element is a status log (_evt)
            if (xml.attributes().value("class").toString() == "statuslog")
            {
                xml.readNext();
                QString $_evt = xml.text().toString();
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

                final->setContent(xml.text().toString().mid(1));
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

                final->setContent(xml.text().toString().mid(1));
                final->setTime($time_base);
                final->setSpecificity(6);
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
    QMap<QString, QVariant> $log_new;
    // Counter
    int $i = 1;
    // Browser
    $log->resetPointer();

    // If no entries, quit.
    if ($log->gotoEntry(0) == false)
        return false;

    do
    {
        // Put the longer variables into something more readily accessible.
        QString $protocol      = $log->getProtocol();
        QString $account       = $log->getSelf();
        QString $self_alias    = $log->getSelfAlias();
        QString $with          = $log->getWith();
        QString $with_alias    = $log->getWithAlias();
        qlonglong $time_base   = $log->getTime();
        QString $timezone_base = $log->getTimezone();

        // Chat log skeleton
        QString $HEADER = "\n<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n<html>\n    <head>\n        <title>Omegle conversation log</title>\n        <meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n        <style type=\"text/css\">\n        body {\n            margin: 0;\n            padding: 0;\n            font-family: sans-serif;\n        }\n        #header {\n            background: #EEE;\n            border-bottom: 1px solid #DDD;\n            padding: 1em;\n        }\n        h1 {\n            display: inline;\n            margin: 0;\n            padding: 0;\n            font-weight: normal;\n            font-size: 2em;\n            color: #000;\n        }\n        h2 {\n            display: inline;\n            font-size: 1.25em;\n            margin-left: .5em;\n            font-weight: bold;\n            color: #555;\n            letter-spacing: 0.05em;\n        }\n        #header a {\n            color: #3F9FFF;\n            font-weight: bold;\n            text-decoration: none;\n        }\n        #header a:hover {\n            text-decoration: underline;\n        }\n        #log {\n            padding: 1em;\n        }\n        .youmsg, .strangermsg {\n            margin: 0;\n            padding: 0;\n        }\n        .youmsg .msgsource {\n            color: blue;\n            font-weight: bold;\n        }\n        .strangermsg .msgsource {\n            color: red;\n            font-weight: bold;\n        }\n        .statuslog {\n            color: #555;\n            font-size: 0.9em;\n            font-weight: bold;\n            margin: 0;\n            padding: 0;\n        }\n        .logitem {\n            padding-bottom: 0.5em;\n        }\n        </style>\n    </head>\n    <body>\n        <div id=\"header\">\n            <h1><a href=\"http://omegle.com/\">Omegle</a> conversation log</h1>\n            <h2>";
        QString $DATER  = "";
        QString $INNER  = "</h2>\n        </div>\n        <div id=\"log\">\n            ";
        QString $LOGGER = "";
        QString $FOOTER = "\n        </div>\n    </body>\n</html>\n        ";

        // Base time (goes between $HEADER and $INNER)
        QDateTime $time_base_proc;
        $time_base_proc.setMSecsSinceEpoch($time_base);
        qDebug() << $time_base_proc.toTime_t();
        qDebug() << $time_base;
        $DATER = $time_base_proc.toString("yyyy-MM-dd");

        // Go through each chat line.
        while ($log->hasNextLine())
        {
            // Make array items more readily accessible.
            qlonglong $time_cur  = $log->getTime();
            int     $code        = $log->getCode();
            QString $sender      = $log->getSender();
            QString $alias       = $log->getAlias();
            QString $message     = $log->getContent();
            int     $specificity = $log->getSpecificity();
            int     $accuracy    = $log->getAccuracy();
            int     $nice        = $log->getNice();

            // If we're looking at a system message...
            if ($code >= 1)
            {
                // Exception: Omegle Spy Mode Indicator
                if ($message.startsWith("Question to discuss: "))
                {
                    $LOGGER = $LOGGER + "<div class=\"logitem\"><div class=\"question\"><div class=\"questionHeading\">Question to discuss:</div><div class=\"questionText\">" + $message.mid(21) + "</div></div></div>";
                }
                // System messages, whoo!
                else if ($sender.startsWith("_evt"))
                {
                    // Conversation open (_evt_open)
                    if ($sender == "_evt_open")
                    {
                        $message = "You're now chatting with a random stranger. Say hi!";
                    }
                    // Conversation closed (_evt_close)
                    if ($sender == "_evt_close")
                    {
                        QString $message_proto = "Conversation has disconnected";
                        if ($message == "_with")
                            $message_proto = "Your conversational partner has disconnected.";
                        else if ($message == "_self")
                            $message_proto = "You have disconnected.";
                        else if ($message.startsWith("_group"))
                            $message_proto = $message.split(" ").value(1) + " has disconnected";
                        $message = $message_proto;
                    }
                    $LOGGER = $LOGGER + "<div class=\"logitem\"><p class=\"statuslog\">" + $message + "</p></div>";
                }
            }
            // Otherwise, it's a normal message...
            else
            {
                // Common Replacements
                if ($sender == "_with")
                    $sender = $with;
                if ($sender == "_self")
                    $sender = $account;
                if ($alias == "_with")
                    $alias = $with_alias;
                if ($alias == "_self")
                    $alias = $self_alias;

                // Determine the message sender indicator
                QString $sender_color;
                if ($sender == $account)
                    $sender_color = "youmsg";
                else
                    $sender_color = "strangermsg";

                $LOGGER = $LOGGER + "<div class=\"logitem\"><p class=\"" + $sender_color + "\"><strong class=\"msgsource\">" + $alias + ":</strong> " + $message + "</p></div>";
            }

            // Next!
            $log->nextLine();
        }

        // Add to the generated log.
        QString $appender = "";
        if ($i > 1)
        {
            $appender.setNum($i);
            $appender = " " + $appender;
        }
        $log_new["Omegle conversation log"+$appender+".html"] = $HEADER + $DATER + $INNER + $LOGGER + $FOOTER;

        /*DEBUG*/
        qDebug() << "DEBUG TO: " + $HEADER + $DATER + $INNER + $LOGGER + $FOOTER;

        // Increment the entry key.
        $log->nextEntry();
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
void Omegle::to(StdFormat* $log)
{
    data = this->generate($log);
}
