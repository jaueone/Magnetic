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


    int setParams(DType type,const char *params, QVariant value);
    bool isOpened();
    bool isCollecting();
    static void camera_message_warn();
    static void camera_message_done();
    void __rgb3_to_interleaved (HObject ho_ImageRGB, HObject *ho_ImageInterleaved)
    {
      HObject  ho_ImageAffineTrans, ho_ImageRed, ho_ImageGreen;
      HObject  ho_ImageBlue, ho_RegionGrid, ho_RegionMoved, ho_RegionClipped;


      HTuple  hv_PointerRed, hv_PointerGreen, hv_PointerBlue;
      HTuple  hv_Type, hv_Width, hv_Height, hv_HomMat2DIdentity;
      HTuple  hv_HomMat2DScale;

      GetImagePointer3(ho_ImageRGB, &hv_PointerRed, &hv_PointerGreen, &hv_PointerBlue,
          &hv_Type, &hv_Width, &hv_Height);
      GenImageConst(&(*ho_ImageInterleaved), "byte", hv_Width*3, hv_Height);
      //
      HomMat2dIdentity(&hv_HomMat2DIdentity);
      HomMat2dScale(hv_HomMat2DIdentity, 1, 3, 0, 0, &hv_HomMat2DScale);
      AffineTransImageSize(ho_ImageRGB, &ho_ImageAffineTrans, hv_HomMat2DScale, "constant",
          hv_Width*3, hv_Height);
      //
      Decompose3(ho_ImageAffineTrans, &ho_ImageRed, &ho_ImageGreen, &ho_ImageBlue);
      GenGridRegion(&ho_RegionGrid, 2*hv_Height, 3, "lines", hv_Width*3, hv_Height+1);
      MoveRegion(ho_RegionGrid, &ho_RegionMoved, -1, 0);
      ClipRegion(ho_RegionMoved, &ho_RegionClipped, 0, 0, hv_Height-1, (3*hv_Width)-1);

      ReduceDomain(ho_ImageRed, ho_RegionClipped, &ho_ImageRed);
      MoveRegion(ho_RegionGrid, &ho_RegionMoved, -1, 1);
      ClipRegion(ho_RegionMoved, &ho_RegionClipped, 0, 0, hv_Height-1, (3*hv_Width)-1);
      ReduceDomain(ho_ImageGreen, ho_RegionClipped, &ho_ImageGreen);
      MoveRegion(ho_RegionGrid, &ho_RegionMoved, -1, 2);
      ClipRegion(ho_RegionMoved, &ho_RegionClipped, 0, 0, hv_Height-1, (3*hv_Width)-1);
      ReduceDomain(ho_ImageBlue, ho_RegionClipped, &ho_ImageBlue);
      OverpaintGray((*ho_ImageInterleaved), ho_ImageRed);
      OverpaintGray((*ho_ImageInterleaved), ho_ImageGreen);
      OverpaintGray((*ho_ImageInterleaved), ho_ImageBlue);
      return;
    }
    void HObjectToQImage(HObject himage,QImage **qimage)
    {
        HTuple hChannels;
        HTuple   width,height;
        width=height=0;
        HTuple htype;
        HTuple hpointer;

        ConvertImageType(himage,&himage,"byte");//将图片转化成byte类型
        CountChannels(himage,&hChannels);       //判断图像通道数

        if(hChannels[0].I()==1)//单通道图
        {
            unsigned char *ptr;

            GetImagePointer1(himage,&hpointer,&htype,&width,&height);

            ptr=(unsigned char *)hpointer[0].L();
            *(*qimage)=QImage(ptr,width,height,width,QImage::Format_Indexed8);//不知道是否已自动4字节对齐
        }
        else if(hChannels[0].I()==3)//三通道图
        {
            unsigned char *ptr3;
            HObject ho_ImageInterleaved;
            __rgb3_to_interleaved(himage, &ho_ImageInterleaved);

            GetImagePointer1(ho_ImageInterleaved, &hpointer, &htype, &width, &height);

            ptr3=(unsigned char *)hpointer[0].L();
            *(*qimage)=QImage(ptr3,width/3,height,width,QImage::Format_RGB888);
        }
    }
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


#endif // CAMERA_H


