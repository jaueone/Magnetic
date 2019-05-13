#include "screenresult.h"
#include "ui_screenresult.h"
#include <QStringLiteral>

ScreenResult::ScreenResult(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScreenResult)
{
    ui->setupUi(this);

    chart_pie = new QChart();
    chart_bar = new QChart();
    chart_pie_view = new QChartView();
    chart_bar_view = new QChartView();
    series_pie = new QPieSeries();
    series_bar = new QBarSeries();
    this->chart_pie->addSeries(this->series_pie);
    this->chart_bar->addSeries(this->series_bar);
    this->ui->verticalLayout->addWidget(chart_pie_view);
    this->ui->verticalLayout_2->addWidget(chart_bar_view);
    this->chart_pie_view->setChart(this->chart_pie);
    this->chart_bar_view->setChart(this->chart_bar);
    this->paint_pie();
    this->paint_bar();

}

ScreenResult::~ScreenResult()
{
    delete ui;
}

void ScreenResult::setMen(const Meninfo &info)
{
    this->ui->label_14->setText(info.name);
}

void ScreenResult::paint_pie()
{
    series_pie->setPieSize(1.0);
    series_pie->append("不合格率10%", 1);
    series_pie->append("合格率90%" , 9);
    series_pie->setLabelsVisible();

    QPieSlice *slice_red = series_pie->slices().at(0);
    QPieSlice *slice_green = series_pie->slices().at(1);
    slice_red->setColor(QColor(255,0,0,255));
    slice_green->setColor(QColor(0,255,0,255));


    chart_pie->setTitle("");
    chart_pie->legend()->hide();
    chart_pie->setBackgroundVisible(false);

}

void ScreenResult::paint_bar()
{
    chart_bar->setBackgroundVisible(false);
}

void ScreenResult::on_pushButton_released()
{

}
