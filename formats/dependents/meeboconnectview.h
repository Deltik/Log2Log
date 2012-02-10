#ifndef MEEBOCONNECTVIEW_H
#define MEEBOCONNECTVIEW_H

#include <QDialog>

namespace Ui {
    class MeeboConnectView;
}

class MeeboConnectView : public QDialog
{
    Q_OBJECT

public:
    explicit MeeboConnectView(QWidget *parent = 0);
    ~MeeboConnectView();

private:
    Ui::MeeboConnectView *ui;
};

#endif // MEEBOCONNECTVIEW_H
