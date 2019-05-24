#ifndef SCREENRESULT_H
#define SCREENRESULT_H

#include <QWidget>
#include <QtCharts/QPieSlice>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QtCharts>
#include <QtCharts/QChartView>
#include <QTimer>
#include <QTime>
#include <QDate>

#include "login.h"

namespace Ui {
class ScreenResult;
}

class ScreenResult : public QWidget
{
    Q_OBJECT

public:
    explicit ScreenResult(QWidget *parent = nullptr);
    ~ScreenResult();

    void setMen(const Meninfo &info);
    void paint_pie();
    void paint_bar();

    QMap<QString,int> select_result();
signals:
    void tell_window_step_page(int page);


private slots:

    void update_data();
private:
    Ui::ScreenResult *ui;
    QChartView *chart_pie_view;
    QChartView *chart_bar_view;
    QChart *chart_pie;
    QChart *chart_bar;
    QPieSeries *series_pie;
    QBarSeries *series_bar;

    QTimer *timer;
};

#endif // SCREENRESULT_H
