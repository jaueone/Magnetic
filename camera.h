#ifndef CAMERA_H
#define CAMERA_H

#include "MvCameraControl.h"
#include "CameraParams.h"
#include "MvErrorDefine.h"

#include <QThread>
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

    CameraSetting get_camera_setting();
    static QByteArray get_camera_bin(CameraSetting setting);

    bool isOpened();
    int setParams(DType type, char * params, QVariant value);
private:
    int nRet = -1;
    void *handle = NULL;

    MV_CC_DEVICE_INFO m_stDevInfo;
    MV_CC_DEVICE_INFO_LIST m_stDevList;

    HObject   ho_Image;

    unsigned char*  m_pBufForSaveImage;         // 用于保存图像的缓存
    unsigned int    m_nBufSizeForSaveImage;
};

class Collect: public QThread
{
public:
    Collect(HKCamera *camera, QLabel *label) {
        camera_ = camera;
        label_ = label;
    }
    void run(){
        if (MV_OK != this->camera_->startCollect())
            return;
        if (MV_OK != this->camera_->collectFrame(this->label_))
            return;
        if (MV_OK != this->camera_->stopCollect())
            return;
    }
protected:
    HKCamera *camera_;
    QLabel *label_;
};
#endif // CAMERA_H
