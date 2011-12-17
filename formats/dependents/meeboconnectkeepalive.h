#ifndef MEEBOCONNECTKEEPALIVE_H
#define MEEBOCONNECTKEEPALIVE_H

#include <QtCore>
#include "../meeboconnect.h"
#include "api.h"

// An object of this class is needed in MeeboConnect
class MeeboConnect;

class MeeboConnectKeepAlive : public QThread
{
    Q_OBJECT

public:
    explicit MeeboConnectKeepAlive(MeeboConnect *mommy);
    void run();
    
signals:
    void updateAPIReply(QMap<QString, QVariant>);
    void updateAPIError(QString);
    
public slots:
    void updateCycle();

private:
    MeeboConnect *parent;
    QTimer *updateCycler;
    Api *updateApi;
    int *revision;
    
};

#endif // MEEBOCONNECTKEEPALIVE_H
