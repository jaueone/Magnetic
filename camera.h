#ifndef CAMERA_H
#define CAMERA_H

#include "MvCameraControl.h"
#include "CameraParams.h"
#include "MvErrorDefine.h"
#include "DefectsDetect.h"

#include <QImage>
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
#include <QPushButton>
#include <QMessageBox>

using namespace HalconCpp;

enum DType{
    Bool = 0,
    Int,
    Float,
    Enum,
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

enum TriggerSelector{
    FrameBurstStart = 6,
    LineStart = 9,
};

enum TriggerMode{
    TriggerModeOff  = 0,
    TriggerModeOn = 1,
};

enum TriggerSource{
    TriggerSourceLine0 = 0,
    TriggerSourceLine1 = 1,
    TriggerSourceLine2 = 2,
    TriggerSourceLine3 = 3,
    Counter0 = 4,
    Software = 7,
    FrequencyConverter = 8,
};

enum LineSelector{
    Line0 = 0,
    Line1 = 1,
    Line2 = 2,
    Line3 = 3,
    Line4 = 4,
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

    TriggerSelector triggerSelector;
    TriggerMode triggerMode;
    TriggerSource triggerSource;
    LineSelector lineSelector;
};


class HKCamera
{
public:
    static HKCamera *getInterface();
    static QByteArray get_camera_bin(CameraSetting setting);
    static void camera_message_warn();
    static void camera_message_done();
    static void HObjectToQImage(HObject himage,QImage **qimage);


    int enumDevices();
    int openDevice(const int &index);

    int startCollect();
    int collectFrame(QLabel *label);
    int stopCollect();
    int closeDevice();

    bool isOpened();
    bool isCollecting();

    HObject getImage();

    int setParams(DType type,const char *params, QVariant value);
    CameraSetting get_camera_setting();
    ~HKCamera();

private:
    static void rgb3_to_interleaved(HObject ho_ImageRGB, HObject *ho_ImageInterleaved);
    HKCamera();

    int destroyHandle();

    bool is_start_collected = false;
    int nRet = -1;
    void *handle = NULL;

    MV_CC_DEVICE_INFO m_stDevInfo;
    MV_CC_DEVICE_INFO_LIST m_stDevList;

    HObject ho_Image;
    unsigned char*  m_pBufForSaveImage;         // 用于保存图像的缓存
    unsigned int    m_nBufSizeForSaveImage;
    int i = 0;
};

#endif // CAMERA_H


