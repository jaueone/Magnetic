#ifndef SCREENRESULT_H
#define SCREENRESULT_H

#include <QWidget>

namespace Ui {
class ScreenResult;
}

class ScreenResult : public QWidget
{
    Q_OBJECT

public:
    explicit ScreenResult(QWidget *parent = nullptr);
    ~ScreenResult();

private:
    Ui::ScreenResult *ui;
};

#endif // SCREENRESULT_H
