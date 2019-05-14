#include "screenresult.h"
#include "ui_screenresult.h"
#include <QStringLiteral>
#include <QValueAxis>

ScreenResult::ScreenResult(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScreenResult)
{
    ui->setupUi(this);

    chart_pie = new QChart();
    chart_bar = new QChart();
    chart_pie_view = new QChartView(chart_pie, this);
    chart_bar_view = new QChartView(chart_bar, this);
    series_pie = new QPieSeries();
    series_bar = new QBarSeries();


    this->ui->verticalLayout->addWidget(chart_pie_view);
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

    chart_pie->addSeries(this->series_pie);
    chart_pie->setTitle("");
    chart_pie->legend()->hide();
    chart_pie->setBackgroundVisible(false);

}

void ScreenResult::paint_bar()
{

    QBarSet *set0 = new QBarSet("Jane");
    *set0  << 900 << 800 << 200 << 180 << 210 << 280;

    QBarSeries *series = new QBarSeries();
    series->append(set0);
    series->setLabelsPosition(QAbstractBarSeries::LabelsInsideEnd); // 设置数据系列标签的位置于数据柱内测上方
    series->setLabelsVisible(true); // 设置显示数据系列标签

    //QChart *chart = new QChart();
    chart_bar->addSeries(series);
    chart_bar->setBackgroundVisible(false);
    chart_bar->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    categories << "坏点" << "坏线" << "漏光" << "划痕" << "条纹" <<"其他";
    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chart_bar->createDefaultAxes();
    chart_bar->setAxisX(axis, series);

    chart_bar->legend()->setVisible(false);
    chart_bar->legend()->setAlignment(Qt::AlignBottom);
    chart_bar->axisY()->setGridLineVisible(false);
    chart_bar->axisX()->setGridLineVisible(false);

    chart_bar_view->setRenderHint(QPainter::Antialiasing);
    this->ui->verticalLayout_2->addWidget(chart_bar_view);
}

void ScreenResult::on_pushButton_released()
{

}
