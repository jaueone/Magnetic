#include "screenresult.h"
#include "ui_screenresult.h"
#include "my_control.h"
#include <QStringLiteral>
#include <QValueAxis>
#include <QFont>


ScreenResult::ScreenResult(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScreenResult),
    check_date(QDate::currentDate()),
    start_time(0,0,0,0),
    end_time(QTime::currentTime())
{
    ui->setupUi(this);

    chart_pie = new QChart();
    chart_bar = new QChart();
    chart_pie_view = new QChartView(chart_pie, this);
//    chart_bar_view = new QChartView(chart_bar, this);
    series_pie = new QPieSeries();
    series_bar = new QBarSeries();

    time_select_form_ = new timeSelect;
    this->men_model = new QStandardItemModel;
    this->ui->listView->setModel(men_model);
    this->timer = new QTimer(this);
    this->ui->verticalLayout->addWidget(chart_pie_view);
    this->paint_pie();
//    this->paint_bar();
    this->ui->calendarWidget->hide();
    this->ui->label_16->hide();
    this->ui->label_8->setText(QDate::currentDate().toString("yyyy\345\271\264"));
    this->ui->label_10->setText(QDate::currentDate().toString("MM\346\234\210dd\346\227\245"));
    this->ui->label_12->setText(QTime::currentTime().toString("00:00-hh:mm"));
    this->connect(this->timer,&QTimer::timeout,this, &ScreenResult::update_data);
    this->connect(this->time_select_form_,&timeSelect::tell_window_time_quantum,this,&ScreenResult::accept_time_quantum);
    this->installEventFilter(this);
    this->ui->listView->hide();
}

ScreenResult::~ScreenResult()
{
    timer->stop();
    delete time_select_form_;
    delete ui;
}

void ScreenResult::setMen(const Meninfo &info)
{
    this->men = info;
    this->ui->label_14->setText(info.name);
    this->check_name = men.name;
    this->timer->setInterval(600);
    this->timer->start();
}

void ScreenResult::paint_pie()
{
    this->ui->label_17->setParent(chart_pie_view);
    this->ui->label_18->setParent(chart_pie_view);
    this->ui->label_17->setGeometry(350,10,100,20);
    this->ui->label_18->setGeometry(10,174,100,100);
    this->ui->label_17->show();
    this->ui->label_18->show();
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
    QFont font;
    font.setFamily("微软雅黑");
    font.setPixelSize(15);


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
    chart_bar->axisX()->setLabelsFont(font);
    chart_bar->axisY()->setLabelsFont(font);
    chart_bar->setFont(font);

    chart_bar_view->setRenderHint(QPainter::Antialiasing);
    this->ui->verticalLayout_2->addWidget(chart_bar_view);
}

void ScreenResult::update_data()
{
    QMap<QString,int> map = this->select_result();  //\345\271\264  年   \346\234\210 月 \346\227\245 日
    if (map.isEmpty())
        return;
    int all = map["all"];

    int qualified = map["qualified"];
    int unqualified = map["unqualified"];
    this->ui->label_5->setText(QString("%1").arg(all));
    this->ui->label_6->setText(QString("%1").arg(qualified));
    this->ui->label_7->setText(QString("%1").arg(unqualified));     //\344\270\215\345\220\210\346\240\274\347\216\20710% 不合格率
    double q;
    if (all !=0){
        q = qualified/all;
    }
    else {
        q = 1;
    }
    double unq = 1 - q;
    this->ui->label_18->setText("\345\220\210\346\240\274\347\216\207" + QString::number(q*100,'f',2) + "%");
    this->ui->label_17->setText("\344\270\215\345\220\210\346\240\274\347\216\207"+ QString::number(unq*100,'f',2) + "%");
    QPieSlice *slice_red = series_pie->slices().at(0);
    QPieSlice *slice_green = series_pie->slices().at(1);
    slice_red->setValue(unq*10);
    slice_green->setValue(q*10);
}

QMap<QString, int> ScreenResult::select_result()
{
    QMap<QString,int> map;
    QSqlDatabase *db = DB::getInterface();
    if (!db->open()){
        qDebug() << "Error: Failed to connect database." << db->lastError();
        return map;
    }
    QSqlQuery query(*db);
    QString start = this->check_date.toString("yyyy-MM-dd ") + start_time.toString("hh:mm:ss");
    QString end = this->check_date.toString("yyyy-MM-dd ") + end_time.toString("hh:mm:ss");
    QString str = QString("select * from checked_result where time>=\"%1\" and time<=\"%2\" and men_name= \"%3\"").arg(start).arg(end).arg(check_name);
    if (!query.exec(str)){
          qDebug() << "Error: Failed to query database." << db->lastError();
          db->close();
          return map;
    }
    int all, unqualified,qualified;
    all = 0;
    qualified = 0;
    unqualified = 0;
    for (all;query.next();all++){
        QSqlRecord rec = query.record();
        if (rec.value("result").toString() == "true")
            qualified +=1;
        else if(rec.value("result").toString() == "false")
            unqualified +=1;
    }
    map["all"] = all;
    map["qualified"] = qualified;
    map["unqualified"] = unqualified;
    db->close();
    return map;
}

bool ScreenResult::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this){
        if (event->type() == QEvent::MouseButtonPress){
            this->ui->calendarWidget->hide();
            this->ui->listView->hide();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void ScreenResult::on_pushButton_clicked()
{
    if (this->ui->calendarWidget->isVisible()){
        this->ui->calendarWidget->hide();
    }
    else
    {
    this->ui->calendarWidget->show();
    this->ui->calendarWidget->setGeometry(20,53,300,197);
    }
}

void ScreenResult::on_pushButton_2_clicked()
{
    this->on_pushButton_clicked();
}


void ScreenResult::on_pushButton_3_clicked()
{
    this->time_select_form_->show();
}

void ScreenResult::accept_time_quantum(const QTime &start, const QTime &end)
{
    this->start_time = start;
    this->end_time = end;
    this->ui->label_12->setText(QTime::currentTime().toString("%1-%2").arg(start_time.toString("hh:mm")).arg(end_time.toString("hh:mm")));
}

void ScreenResult::on_calendarWidget_clicked(const QDate &date)
{
    this->check_date = date;
    this->ui->label_8->setText(date.toString("yyyy\345\271\264"));
    this->ui->label_10->setText(date.toString("MM\346\234\210dd\346\227\245"));
}

void ScreenResult::on_pushButton_4_clicked()
{
    if(this->men.isengineer != "yes")
        return;
    QSqlDatabase *db = DB::getInterface();
    if (!db->open()){
        qDebug() << "Error: Failed to connect database." << db->lastError();
        return;
    }
    QSqlQuery query(*db);
    QString str = QString("select * from men");
    if (!query.exec(str)){
          qDebug() << "Error: Failed to query database." << db->lastError();
          db->close();
          return;
    }
    int i = 1;
    men_model->clear();
    while (query.next()) {
        i += 1;
        QSqlRecord rec = query.record();
        QString name = rec.value("name").toString();
        QStandardItem *item = new QStandardItem(name);
        men_model->appendRow(item);
    }
    if (i >12) i = 12;
    this->ui->listView->setGeometry(462, 50, 120,22*i);
    this->ui->listView->show();
}

void ScreenResult::on_listView_clicked(const QModelIndex &index)
{
    QStandardItem * item = this->men_model->itemFromIndex(index);
    this->check_name = item->text();
    this->ui->label_14->setText(this->check_name);
    this->ui->listView->hide();
}
