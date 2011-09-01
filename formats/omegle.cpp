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
        qint64 $time_base = 0;

        // Looking at element beginnings...
        if (token == QXmlStreamReader::StartElement)
        {
            // If token is the time indicator (_time)
            if (xml.qualifiedName().toString() == "h2")
            {
                xml.readNext();
                QString $date_cur = xml.text().toString();
                QDateTime $date_proc = QDateTime::fromString($date_cur, Qt::ISODate);
                qint64 $time_cur = $date_proc.toMSecsSinceEpoch();
                final->setTime($time_cur);
                $time_base = $time_cur;
            }

            // If token is a status log (_evt)
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

                // If conversation closed (_evt_close) by _with
                if ($_evt == "Your conversational partner has disconnected.")
                {
                    final->newLine();
                    final->setCode(1);
                    final->setSender("_evt_close");
                    final->setContent("_with");
                    final->setTime($time_base);
                    final->setSpecificity(6);
                }
            }
        }
    }qDebug()<<final->final;
}

/**
 * Generate Log from Standardized Log
 */
void Omegle::generate()
{
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
    //return $this->log;
    StdFormat* dummy = new StdFormat();
    return dummy;
}

/**
 * Process "To" Request
 */
void Omegle::to()
{
}
