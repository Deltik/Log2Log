#define VERSION "1.0.0"
#ifndef UPDATE_H
#define UPDATE_H

#include <QDialog>

namespace Ui {
    class Update;
}

class Update : public QDialog
{
    Q_OBJECT

public:
    explicit Update(QWidget *parent = 0);
    ~Update();

private slots:
    void goBack();

private:
    Ui::Update *ui;
};

#endif // UPDATE_H
