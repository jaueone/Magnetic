#ifndef CHOOSE_H
#define CHOOSE_H

#include <QWidget>

namespace Ui {
class Choose;
}

class Choose : public QWidget
{
    Q_OBJECT

public:
    explicit Choose(QWidget *parent = nullptr);
    ~Choose();

private slots:
    void on_pushButton_2_released();

    void on_pushButton_released();

signals:
    void tell_window_step_page3(int page);

private:
    Ui::Choose *ui;
};

#endif // CHOOSE_H
