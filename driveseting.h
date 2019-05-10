#ifndef DRIVESETING_H
#define DRIVESETING_H

#include <QWidget>

namespace Ui {
class DriveSeting;
}

class DriveSeting : public QWidget
{
    Q_OBJECT

public:
    explicit DriveSeting(QWidget *parent = nullptr);
    ~DriveSeting();

private:
    Ui::DriveSeting *ui;
};

#endif // DRIVESETING_H
