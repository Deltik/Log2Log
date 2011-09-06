#include "update.h"
#include "ui_update.h"
#include "about.h"
#include "api.h"

Update::Update(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Update)
{
    ui->setupUi(this);
    connect(ui->backButton, SIGNAL(clicked()), this, SLOT(goBack()));
    connect(ui->changelogButton, SIGNAL(clicked()), this, SLOT(loadChangelog()));

    this->setWindowTitle("Check for Updates");

    ui->infoBrowser->setHtml("<center><h1>Checking for updates...</h1></center>");
    ui->updateButton->setDisabled(true);

    Api* api = new Api();

    connect(api, SIGNAL(requestComplete(QString)), this, SLOT(interpretReply(QString)));

    api->setURL((QString)DL_SOURCE);
    api->start();
}

Update::~Update()
{
    delete ui;
}

/**
 * Go back / Cancel button
 */
void Update::goBack()
{
    this->close();
}

/**
 * Load Changelog
 */
void Update::loadChangelog()
{
    ui->infoBrowser->setHtml("<center><h1>Loading changelog...</h1></center>");

    Api* api = new Api();

    connect(api, SIGNAL(requestComplete(QString)), this, SLOT(displayChangelog(QString)));

    api->setURL((QString)DL_SOURCE+"?action=changelog");
    api->start();
}

/**
 * Display Changelog
 */
void Update::displayChangelog(QString reply)
{
    ui->infoBrowser->setStyleSheet("");
    ui->infoBrowser->setHtml("<code>" + reply.replace("\n", "<br />") + "</code>");
}

/**
 * Interpret API reply
 */
void Update::interpretReply(QString reply)
{
    // Create XML stream reader from reply
    QXmlStreamReader xml(reply);

    // Loop through the XML data
    while(!xml.atEnd())
    {
        // Read next item
        QXmlStreamReader::TokenType token = xml.readNext();

        // Skip beginning of file
        if (token == QXmlStreamReader::StartDocument) continue;

        // If token is an element...
        if(token == QXmlStreamReader::StartElement)
        {
            // Bingo. If we found the format we're looking for...
            if (xml.attributes().value("id").toString() == "log2log" &&
                xml.qualifiedName().toString() == "product")
            {
                dig_the_information_out(xml);
                break;
            }
        }
    }

    // If update info couldn't be found...
    if (updateInfo.isEmpty())
    {
        ui->updateButton->setText("Go to the Deltik Website");
        $url = QUrl("http://www.deltik.org/");
        connect(ui->updateButton, SIGNAL(clicked()), this, SLOT(loadURL()));
        ui->updateButton->adjustSize();
        ui->updateButton->setFixedHeight(27);
        ui->updateButton->setDisabled(false);
        ui->infoBrowser->setHtml((QString)"<center>" +
                                 "<h1 style=\"margin: 0\">Could not check for updates</h1>" +
                                 "<hr />" +
                                 "Log2Log could not connect to the Deltik Update System at:<br />" +
                                 "<code>" + DL_SOURCE + "</code>" +
                                 "<hr />" +
                                 "<table width=\"100%\" style=\"text-align: left;\"><tr><td><ul>" +
                                 "<li>Are you connected to the Internet? Check your network connection and DNS server settings.</li>" +
                                 "<li>Is your computer or network protected by a firewall or proxy? Incorrect settings can interfere with Log2Log's attempts to connect.</li>" +
                                 "</ul></td></tr></table>" +
                                 "</center>" +
                                 "<hr />" +
                                 "If the update site is actually down (which is rare), you can try to check for information by going to the Deltik Website.");
    }
    // Otherwise:
    else
    {
        bool buttonSet = false;
        // Header
        if (version_compare(VERSION, updateInfo["version"].toString(), "==").toBool())
        {
            ui->updateButton->setText("No update available");
            $url = QUrl("http://log2log.sf.net/");
            connect(ui->updateButton, SIGNAL(clicked()), this, SLOT(loadURL()));
            ui->infoBrowser->setHtml((QString)"<center>" +
                                     "<h1 style=\"margin: 0\">You are running the latest version of Log2Log.</h1>"
                                     "</center>" +
                                     "<hr />");
        }
        else if (version_compare(VERSION, updateInfo["version"].toString(), "<").toBool())
        {
            ui->updateButton->setText("Get Update");
            $url = QUrl("http://log2log.sf.net/");
            connect(ui->updateButton, SIGNAL(clicked()), this, SLOT(loadURL()));
            ui->updateButton->setDisabled(false);
            buttonSet = true;
            ui->infoBrowser->setHtml((QString)"<center>" +
                                     "<h1 style=\"margin: 0\">There is an update for Log2Log!</h1>"
                                     "</center>" +
                                     "<hr />");
        }
        else if (version_compare(VERSION, updateInfo["version"].toString(), ">").toBool() &&
                 version_compare(VERSION, updateInfo["version-beta"].toString(), ">").toBool() &&
                 version_compare(VERSION, updateInfo["version-dev"].toString(), ">").toBool())
        {
            ui->updateButton->setText("Downgrade? What's going on?");
            $url = QUrl("http://www.deltik.org/");
            connect(ui->updateButton, SIGNAL(clicked()), this, SLOT(loadURL()));
            ui->updateButton->setDisabled(false);
            buttonSet = true;
            ui->infoBrowser->setHtml((QString)"<center>" +
                                     "<h1 style=\"margin: 0\">It appears to be that your version of Log2Log is newer than the update server's.</h1>"
                                     "</center>" +
                                     "<hr />" +
                                     "<p>Log2Log <b>v" + VERSION + "</b> is completely clueless as to why this running copy is newer than <b>v" +
                                     updateInfo["version-dev"].toString() +
                                     "</b> on the server.</p>" +
                                     "<ul><li>Is this copy not the official Deltik release of Log2Log?</li>" +
                                     "<li>Did you modify and compile this program by yourself?</li></ul>" +
                                     "Anyway, answers might be located on the Deltik Website. Click on the left button below to go there.");
        }
        else
        {
            ui->infoBrowser->setHtml((QString)"<center>" +
                                     "<h1 style=\"margin: 0\">You are running a Log2Log preview release.</h1>"
                                     "</center>" +
                                     "<hr />");
        }
        // Body
        if (version_compare(VERSION, updateInfo["version"].toString(), "<").toBool())
        {
            ui->infoBrowser->setHtml(ui->infoBrowser->toHtml() +
                                     "<p><small><b>There is an update for Log2Log: <strong>v" +
                                     updateInfo["version-beta"].toString() +
                                     "</strong>. This new version has been tested to work very well, and it is recommended that you update.</b> " +
                                     "For a list of changes, see the changelog.</small></p>");
        }
        if (version_compare(VERSION, updateInfo["version-beta"].toString(), "<").toBool())
        {
            ui->infoBrowser->setHtml(ui->infoBrowser->toHtml() +
                                     "<p><small>There is a beta release of Log2Log available: <strong>v" +
                                     updateInfo["version-beta"].toString() +
                                     "</strong>. Log2Log Beta contains previews of new features but may have bugs and issues that Log2Log Stable does not have. " +
                                     "These may include new format converters that aren't perfect yet and experimental enhancements.</small></p>");
            if (!buttonSet)
            {
                ui->updateButton->setText("Get Beta");
                $url = QUrl("http://log2log.sf.net/");
                connect(ui->updateButton, SIGNAL(clicked()), this, SLOT(loadURL()));
                ui->updateButton->setDisabled(false);
                buttonSet = true;
            }
        }
        if (version_compare(VERSION, updateInfo["version-dev"].toString(), "<").toBool())
        {
            ui->infoBrowser->setHtml(ui->infoBrowser->toHtml() +
                                     "<p><small>There is a development preview of Log2Log available: <strong>v" +
                                     updateInfo["version-dev"].toString() +
                                     "</strong>. Log2Log Dev contains the bleeding-edge software that is still undergoing heavy development. " +
                                     "These may include new format converters that don't even work yet and new features that only have a dummy outline.</small></p>");
            if (!buttonSet)
            {
                ui->updateButton->setText("Get Development Preview");
                $url = QUrl("http://log2log.sf.net/");
                connect(ui->updateButton, SIGNAL(clicked()), this, SLOT(loadURL()));
                ui->updateButton->setDisabled(false);
                buttonSet = true;
            }
        }
        if (!buttonSet)
        {
            ui->infoBrowser->setHtml(ui->infoBrowser->toHtml() +
                                     "<br /><br /><center><img src=\":/images/etc/Deltik.png\" /></center>");
        }
    }

    /*DEBUG*/
    QString symbol = "<";
    QString v1 = "0.9.9";
    QString v2 = updateInfo["version"].toString();
    qDebug() << "Comparing: "+v1+" "+symbol+" "+v2;
    qDebug() << version_compare(v1, v2, symbol);
}

/**
 * Dig the Information Out
 * @precondition reader is currently pointing at a <product>
 * @param QXmlStreamReader reader The reader to pick up from
 * @returns bool true if successful
 */
bool Update::dig_the_information_out(QXmlStreamReader &reader)
{
    // Check if reader is pointing at an opening <product> element
    if(reader.tokenType() != QXmlStreamReader::StartElement && reader.name() == "product") return false;

    // Keep going until </product>
    while(!(reader.tokenType() == QXmlStreamReader::EndElement && reader.name() == "product"))
    {
        QString currentElement = "product";
        QXmlStreamAttributes currentAttributes;

        if (reader.tokenType() == QXmlStreamReader::StartElement)
        {
            currentElement = reader.name().toString();
            currentAttributes = reader.attributes();
            reader.readNext();
            if (reader.tokenType() != QXmlStreamReader::Characters)
            {
                continue;
            }
        }

        if (currentElement == "name")
            updateInfo["name"] = reader.text().toString();
        if (currentElement == "version" && (currentAttributes.value("type") == "stable" || !currentAttributes.hasAttribute("type")))
            updateInfo["version"] = reader.text().toString();
        if (currentElement == "version" && currentAttributes.value("type") == "stable")
            updateInfo["version-beta"] = reader.text().toString();
        if (currentElement == "version" && currentAttributes.value("type") == "stable")
            updateInfo["version-dev"] = reader.text().toString();
        if (currentElement == "summary")
            updateInfo["summary"] = reader.text().toString();
        if (currentElement == "description")
            updateInfo["description"] = reader.text().toString();

        // Next!
        reader.readNext();
    }
}

/**
 * Invoke Web Browser with URL
 */
void Update::loadURL()
{
    QDesktopServices::openUrl($url);
}

/* ############################### PHP Port ############################### */
#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define sign(n) ((n)<0?-1:((n)>0?1:0))

/**
 * php_canonicalize_version()
 */
char* php_canonicalize_version(const char *version)
{
    int len = strlen(version);
    char *buf = (char*)malloc(len*2+1), *q, lp, lq;
    const char *p;

    if (len == 0) {
        *buf = '\0';
        return buf;
    }

    p = version;
    q = buf;
    *q++ = lp = *p++;

    while (*p) {
/*  s/[-_+]/./g;
 *  s/([^\d\.])([^\D\.])/$1.$2/g;
 *  s/([^\D\.])([^\d\.])/$1.$2/g;
 */
#define isdig(x) (isdigit(x)&&(x)!='.')
#define isndig(x) (!isdigit(x)&&(x)!='.')
#define isspecialver(x) ((x)=='-'||(x)=='_'||(x)=='+')

                lq = *(q - 1);
                if (isspecialver(*p)) {
                        if (lq != '.') {
                                *q++ = '.';
                        }
                } else if ((isndig(lp) && isdig(*p)) || (isdig(lp) && isndig(*p))) {
                        if (lq != '.') {
                                *q++ = '.';
                        }
                        *q++ = *p;
                } else if (!isalnum(*p)) {
                        if (lq != '.') {
                                *q++ = '.';
                        }
                } else {
                        *q++ = *p;
                }
                lp = *p++;
    }
    *q++ = '\0';
    return buf;
}

/**
 * compare_special_version_forms()
 */
typedef struct {
        const char *name;
        int order;
} special_forms_t;

static int
compare_special_version_forms(char *form1, char *form2)
{
        int found1 = -1, found2 = -1;
        special_forms_t special_forms[11] = {
                {"dev", 0},
                {"alpha", 1},
                {"a", 1},
                {"beta", 2},
                {"b", 2},
                {"RC", 3},
                {"rc", 3},
                {"#", 4},
                {"pl", 5},
                {"p", 5},
                {NULL, 0},
        };
        special_forms_t *pp;

        for (pp = special_forms; pp && pp->name; pp++) {
                if (strncmp(form1, pp->name, strlen(pp->name)) == 0) {
                        found1 = pp->order;
                        break;
                }
        }
        for (pp = special_forms; pp && pp->name; pp++) {
                if (strncmp(form2, pp->name, strlen(pp->name)) == 0) {
                        found2 = pp->order;
                        break;
                }
        }
        return sign(found1 - found2);
}

/**
 * php_version_compare()
 */
int php_version_compare(const char *orig_ver1, const char *orig_ver2)
{
    char *ver1;
    char *ver2;
    char *p1, *p2, *n1, *n2;
    long l1, l2;
    int compare = 0;

    if (!*orig_ver1 || !*orig_ver2) {
            if (!*orig_ver1 && !*orig_ver2) {
                    return 0;
            } else {
                    return *orig_ver1 ? 1 : -1;
            }
    }
    if (orig_ver1[0] == '#') {
            ver1 = strdup(orig_ver1);
    } else {
            ver1 = php_canonicalize_version(orig_ver1);
    }
    if (orig_ver2[0] == '#') {
            ver2 = strdup(orig_ver2);
    } else {
            ver2 = php_canonicalize_version(orig_ver2);
    }
    p1 = n1 = ver1;
    p2 = n2 = ver2;
    while (*p1 && *p2 && n1 && n2) {
            if ((n1 = strchr(p1, '.')) != NULL) {
                    *n1 = '\0';
            }
            if ((n2 = strchr(p2, '.')) != NULL) {
                    *n2 = '\0';
            }
            if (isdigit(*p1) && isdigit(*p2)) {
                    /* compare element numerically */
                    l1 = strtol(p1, NULL, 10);
                    l2 = strtol(p2, NULL, 10);
                    compare = sign(l1 - l2);
            } else if (!isdigit(*p1) && !isdigit(*p2)) {
                    /* compare element names */
                    compare = compare_special_version_forms(p1, p2);
            } else {
                    /* mix of names and numbers */
                    if (isdigit(*p1)) {
                            compare = compare_special_version_forms("#N#", p2);
                    } else {
                            compare = compare_special_version_forms(p1, "#N#");
                    }
            }
            if (compare != 0) {
                    break;
            }
            if (n1 != NULL) {
                    p1 = n1 + 1;
            }
            if (n2 != NULL) {
                    p2 = n2 + 1;
            }
    }
    if (compare == 0) {
            if (n1 != NULL) {
                    if (isdigit(*p1)) {
                            compare = 1;
                    } else {
                            compare = php_version_compare(p1, "#N#");
                    }
            } else if (n2 != NULL) {
                    if (isdigit(*p2)) {
                            compare = -1;
                    } else {
                            compare = php_version_compare("#N#", p2);
                    }
            }
    }
    free(ver1);
    free(ver2);
    return compare;
}

/**
 * Compares two "PHP-standardized" version number strings
 * @param QString version1 First version number.
 * @param QString version2 Second version number.
 * @param QString operater If you specify the third optional operator argument,
 *                  you can test for a particular relationship. The possible
 *                  operators are:
 *                  <, lt, <=, le, >, gt, >=, ge, ==, =, eq, !=, <>, ne
 *                  respectively. This parameter is case-sensitive, so values
 *                  should be lowercase.
 */
QVariant version_compare(QString version1, QString version2, QString operater)
{
    char *v1, *v2, *op = NULL;
    int v1_len, v2_len, op_len = 0;
    int compare;

    // Qt to Standard C & PHP
    QByteArray proto_v1;
    QByteArray proto_v2;
    QByteArray proto_op;
    proto_v1 = version1.toAscii();
    v1 = proto_v1.data();
    proto_v2 = version2.toAscii();
    v2 = proto_v2.data();
    proto_op = operater.toAscii();
    op = proto_op.data();
    v1_len = version1.length();
    v2_len = version2.length();
    op_len = operater.length();

    compare = php_version_compare(v1, v2);
    if (operater.isNull()) {
            return QVariant(compare);
    }
    if (!strncmp(op, "<", op_len) || !strncmp(op, "lt", op_len)) {
            return QVariant(compare == -1);
    }
    if (!strncmp(op, "<=", op_len) || !strncmp(op, "le", op_len)) {
            return QVariant(compare != 1);
    }
    if (!strncmp(op, ">", op_len) || !strncmp(op, "gt", op_len)) {
            return QVariant(compare == 1);
    }
    if (!strncmp(op, ">=", op_len) || !strncmp(op, "ge", op_len)) {
            return QVariant(compare != -1);
    }
    if (!strncmp(op, "==", op_len) || !strncmp(op, "=", op_len) || !strncmp(op, "eq", op_len)) {
            return QVariant(compare == 0);
    }
    if (!strncmp(op, "!=", op_len) || !strncmp(op, "<>", op_len) || !strncmp(op, "ne", op_len)) {
            return QVariant(compare != 0);
    }
    return QVariant();
}
