#ifndef SCREENCHECK_H
#define SCREENCHECK_H

#include "my_control.h"
#include "camera.h"
#include "login.h"
#include "worker.h"
#include "pixitem.h"
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWidget>
#include <QMap>
#include <QString>
#include <QtSerialPort>

namespace Ui {
class ScreenCheck;
}

struct CurrentCheckResult
{
    bool isQualified;
    bool wrapedOver;
    QString isQualified_s;
    QString wrapedOver_s;
    QString scratch;
    QString whitePoint;
    QString blackPoint;
};

class ScreenCheck : public QWidget
{
    Q_OBJECT

public:
    explicit ScreenCheck(QWidget *parent = nullptr);
    ~ScreenCheck();

    void addlabel(const QString &name,const QString &content);
    void start_check();
    int getMaxID();
    Worker* workerObj(){return this->worker;}
    void setMen(const Meninfo &info);
    void setSerial(QSerialPort* serial_);
    void setCamera(HKCamera *camera_);
    void serial_send_start();
    void serial_send_end();
    void serial_send_change_image();
    void save_check_result(int product_id);
    QLabel * camera_label();
    void ImageCapture();
    bool workerSerialIsOpen();

    void analysis_FaultCode(QVariant data,QString &str);

public slots:
    void accept_stm_command(Command com,QVariant data);
    void accept_stm_respond_timeout();

    void accept_stm_status(Status status);
    void accept_serial_setting(SerialSetting serialsetting){this->serial_setting = serialsetting;}
    void accept_worker_serial_status(bool isopened){this->worker_thread_serial_status = isopened; qDebug() << "ask and get serial:"<< worker_thread_serial_status;}
    void accept_worker_step(int step);

signals:
    void tell_window_step_page(int page);
    void tell_window_start_check();
    void tell_window_stm_status(Status);

    void ask_serial_setting();
    void ask_worker_serial_status();

    void tell_worker_open_serial(SerialSetting);
    void tell_worker_start_work(SerialSetting);
    void tell_worker_stop_work();
    void tell_worker_stm_command(Command,int);
    void tell_result_update_data();

private slots:
    void on_pushButton_4_released();

    void on_start_check_released();

    void on_pushButton_17_released();

    void on_pushButton_18_released();

    void on_pushButton_8_released();

private:
    Ui::ScreenCheck *ui;
    QMap<QString ,Label *> object_label_map{};
    QSerialPort *serial;
    HKCamera *camera;
    Meninfo men_info;

    QThread *worker_thread;
    Worker *worker;
    QImage *qimage;

    CurrentCheckResult current_check_result;
    SerialSetting serial_setting;


    QPixmap *qpixmap;
    QGraphicsView *graph_view_preview;
    QGraphicsScene *graph_scene;
    PixItem *pixitem;

    QString filename;
    bool worker_thread_serial_status = false;
    unsigned char id= 1;

};

#endif // SCREENCHECK_H
