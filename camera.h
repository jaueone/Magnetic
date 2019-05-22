#ifndef CAMERA_H
#define CAMERA_H

#include "MvCameraControl.h"
#include "CameraParams.h"
#include "MvErrorDefine.h"
#include "DefectsDetect.h"

#include <QThread>
#include <thread>
#include <Halcon.h>
#include <HalconCpp.h>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QDebug>
#include <QLabel>



using namespace HalconCpp;

enum DType{
    Bool = 0,
    Int,
    Float,
    String,
};

enum GainAuto{
    Off  = 0,
    Once,
    Continuous
};
enum GammaSelector{
    User = 1,
    sRGB
};
enum ShadingSelector{
    FPNCCorrection = 0,
    PRNUCCorrection
};
struct CameraSetting{
    unsigned int thresholdValue_whiteDetect;
    unsigned int thresholdValue_blackDetect;

    unsigned int width;
    unsigned int height;
    unsigned int offsetX;
    unsigned int offsetY;

    unsigned int acquisitionLineRate;
    bool acquisitionLineRateEnable;

    float gain;
    GainAuto gainAuto;

    float gamma;
    bool gammaEnable;
    GammaSelector gammaSelector;

    float exposureTime;
    bool nucEnable;
};

class HKCamera
{
public:
    HKCamera();
    ~HKCamera();
    int enumDevices();
    int openDevice(const int &index);

    int startCollect();
    int collectFrame(QLabel *label);
    int stopCollect();

    int closeDevice();

    HObject getImage();
    CameraSetting get_camera_setting();
    static QByteArray get_camera_bin(CameraSetting setting);


    int setParams(DType type, char * params, QVariant value);
    bool isOpened();
    bool isCollecting();
private:
    int destroyHandle();

    bool is_start_collected = false;
    int nRet = -1;
    void *handle = NULL;

    MV_CC_DEVICE_INFO m_stDevInfo;
    MV_CC_DEVICE_INFO_LIST m_stDevList;

    HObject ho_Image;

    unsigned char*  m_pBufForSaveImage;         // 用于保存图像的缓存
    unsigned int    m_nBufSizeForSaveImage;
};


class PreviewThread:public QThread
{   Q_OBJECT
public:
    PreviewThread(HKCamera *camera, QLabel *label):
    QThread(){
        this->camera = camera;
        this->label = label;
    }
    ~PreviewThread(){qDebug() << "preview end";}
    void run(){
        if (MV_OK != camera->startCollect())
            return;
        if (MV_OK != camera->collectFrame(label))
            return;

    }
private:
    HKCamera *camera;
    QLabel *label;
};

class CollectThread:public QThread
{   Q_OBJECT
public:
    CollectThread(HKCamera *camera, QLabel *label) :
        QThread(){
        this->camera = camera;
        this->label = label;
    }
    void run(){
        if (MV_OK != camera->startCollect())
            return;
        if (MV_OK != camera->collectFrame(label))
            return;
        if (MV_OK != camera->stopCollect())
            return;
    }
private:
    HKCamera *camera;
    QLabel *label;
};

#endif // CAMERA_H


