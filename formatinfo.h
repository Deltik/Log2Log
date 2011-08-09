/**
 * Log2Log
 *  Classes
 *   Format Information
 *    Header File
 *
 * @author Deltik
 */

#ifndef FORMATINFO_H
#define FORMATINFO_H

#include <QString>
#include <QFile>
#include <QIcon>
#include <QXmlStreamReader>
#include <string>
#include <stdio.h>
#include <QtDebug>

class FormatInfo
{

public:
    FormatInfo();
    FormatInfo(char *unixname);
    ~FormatInfo();
    /* Browser */
    void pointerReset();
    bool pointerNext();
    bool pointerPrevious();
    bool pointerDig();
    /* Getters (Setters N/A) */
    QString getName(QString type) { if (type == "display") return fName_display; else if (type == "client") return fName_client; else return fName_unix; }
    QIcon getIcon() { return fIcon; }
    QString getIconPath() { return fIcon_path; }
    bool getTo() { return fTo; }
    bool getFrom() { return fFrom; }
    int getSpecificity() { return fSpecificity; }
    bool getAccuracy() { return fAccuracy; }
    int getTime() { return fTime; }
    int getTimezone() { return fTimezone; }
    bool getStatus() { return fStatus; }
    bool getSystem() { return fSystem; }
    bool getAliases() { return fAliases; }
    bool getGroup() { return fGroup; }
    QString getFromInfo() { return fFromInfo; }
    QString getToInfo() { return fToInfo; }

    QString xmlsrc;

private slots:
    //internals();

private:
    bool dig_the_information_out(QXmlStreamReader& reader);

    /***********\
    | VARIABLES |
    \***********/
    QString fName_unix;
    QString fName_display;
    QString fName_client;
    QString fIcon_path;
    QIcon fIcon;
    bool fTo;
    bool fFrom;
    int fSpecificity;
    bool fAccuracy;
    int fTime;
    int fTimezone;
    bool fStatus;
    bool fSystem;
    bool fAliases;
    bool fGroup;
    QString fFromInfo;
    QString fToInfo;
    // Since QXmlStreamReader is stupid, I need to keep track of the pointer
    // by using a simple int that counts the number of readNext()s I run.
    int num_of_readNexts;

};

#endif // FORMATINFO_H