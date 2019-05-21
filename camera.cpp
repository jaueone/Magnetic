#include "camera.h"
#include <QDebug>
#include <QLabel>

#define MAX_BUF_SIZE    (4096*3700)

HKCamera::HKCamera() {

}

HKCamera::~HKCamera()
{
    this->closeDevice();
    MV_CC_DestroyHandle(handle);
    handle= NULL;
}

int HKCamera::enumDevices()
{
    nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &m_stDevList);
    if (MV_OK != nRet)
    {
        qDebug() << "error: EnumDevices fail" << nRet;
        return -1;
    }
    if (m_stDevList.nDeviceNum == 0)
    {
        qDebug() << "no camera found!" << nRet;
        return 2;
    }
    return 0;
}

int HKCamera::openDevice(const int &index)
{
    if (this->isOpened()){
        qDebug() << "information opened";
        return 0;
    }
    memcpy(&m_stDevInfo, m_stDevList.pDeviceInfo[index], sizeof(MV_CC_DEVICE_INFO));
    nRet = MV_CC_CreateHandle(&handle, &m_stDevInfo);
    if (MV_OK != nRet)
    {
        qDebug("error: CreateHandle fail[%x]", nRet);
        return -1;
    }
    nRet = MV_CC_OpenDevice(handle);
    if (nRet  != MV_OK)
    {
        qDebug("error: OpenDevice fail [%x]", nRet);
        return -1;
    }
    qDebug() << "information opened";
    return 0;
}

int HKCamera::startCollect()
{
    if (!this->isOpened()){
        qDebug() << "camera not opened";
        return -1;
    }
    /************************************************************************/
    /* 4.开启抓图     MV_CC_StartGrabbing                                   */
    /************************************************************************/
    //开始采集图像
    nRet = MV_CC_StartGrabbing(handle);
    if (MV_OK != nRet)
    {
        qDebug("error: StartGrabbing fail [%x]\n", nRet);
        return -1;
    }
    //设置相机图像的像素格式
    unsigned int enValue = PixelType_Gvsp_Undefined;
    //unsigned int enValue = PixelType_Gvsp_RGB8_Packed;
    nRet = MV_CC_SetPixelFormat(handle, enValue);
    if (MV_OK != nRet)
    {
        qDebug("error: SetPixelFormat fail [%x]\n", nRet);
        return -1;
    }
    qDebug() << "start collect";
    return 0;

}

int HKCamera::collectFrame(QLabel *label)
{
    /************************************************************************/
    /* 5.获取一帧并保存成图像  MV_CC_GetOneFrameTimeout  MV_CC_SaveImage    */
    /************************************************************************/
    unsigned int    nTestFrameSize = 0;
    unsigned char*  pFrameBuf = nullptr;
    pFrameBuf = (unsigned char*)malloc(MAX_BUF_SIZE+2048);

    MV_FRAME_OUT_INFO stInfo;
    memset(&stInfo, 0, sizeof(MV_FRAME_OUT_INFO));

    MV_NETTRANS_INFO stNetTransInfo = {0};

    //上层应用程序需要根据帧率，控制好调用该接口的频率
    //此次代码仅供参考，实际应用建议另建线程进行图像帧采集和处理
    for (int in = 0; in <10 ; in++)
    {
        //pFrameBuf是相机采集到的一帧原始图像数据
        nRet = MV_CC_GetOneFrame(handle, pFrameBuf, MAX_BUF_SIZE+2048, &stInfo);
        /*************************************显示图像**************************************/
        //获取窗口句柄
        HWND MainWndID = (HWND)label->winId();

        //显示图像
        //该接口需要在MV_CC_StartGrabbing之后调用，显示采集到的图像。如果相机当前采集图像是JPEG压缩的格式，则不支持调用该接口进行显示。
        nRet = MV_CC_Display(handle, MainWndID);
        if (MV_OK != nRet)
        {
            qDebug("error: Display fail [%x]\n", nRet);
            return -1;
        }
        /**************************unsigned char* 图像转换为HObject************************/
        int hgt = stInfo.nHeight;
        int wid = stInfo.nWidth;
        unsigned char *  data      = new unsigned char[wid * hgt];
        memcpy(data, pFrameBuf, wid * hgt);
        /*for (int i = 0; i <wid * hgt; i++)
        {
                data[i] = (data[i]);

        }*/
        GenImage1(&ho_Image, "byte", wid, hgt, (Hlong)(data));
        WriteImage(ho_Image, "bmp", 0, HTuple("E:/photo/") + 1);
        Sleep(500);
        delete[]  data;
        /**********************************************************************************/
    }
    free(pFrameBuf);
    qDebug() << "colledted";
    return 0;
}

int HKCamera::stopCollect()
{
    /************************************************************************/
    /* 6. 停止抓图  MV_CC_StopGrabbing                                      */
    /************************************************************************/
    //停止采集图像
    nRet = MV_CC_StopGrabbing(handle);
    if (MV_OK != nRet)
    {
        qDebug("error: StopGrabbing fail [%x]\n", nRet);
        return -1;
    }
    return 0;
}

int HKCamera::closeDevice()
{
    /************************************************************************/
    /* 7. 关闭设备  MV_CC_CloseDevice                                       */
    /************************************************************************/
    //关闭设备，释放资源
    nRet = MV_CC_CloseDevice(handle);
    if (MV_OK != nRet)
    {
        qDebug("error: CloseDevice fail [%x]\n", nRet);
        return -1;
    }

    /************************************************************************/
    /* 8. 销毁句柄  MV_CC_DestroyHandle                                     */
    /************************************************************************/
    //销毁句柄，释放资源
    nRet = MV_CC_DestroyHandle(handle);
    if (MV_OK != nRet)
    {
        qDebug("error: DestroyHandle fail [%x]\n", nRet);
        return -1;
    }
    qDebug() << "stop collect";
    return 0;
}

CameraSetting HKCamera::get_camera_setting()
{
    CameraSetting setting;
    if (!MV_CC_IsDeviceConnected(handle)){
        qDebug() << "camera no connected, get params fail";
        return setting;
    }
    MVCC_INTVALUE thresholdValue_whiteDetect, thresholdValue_blackDetect,width,height,offsetX,offsetY,acquisitionLineRate;
    MVCC_FLOATVALUE gain, gamma, exposureTime;
    MVCC_ENUMVALUE gainAuto, gammaSelector;
    qDebug() << acquisitionLineRate.nCurValue << exposureTime.fCurValue << thresholdValue_whiteDetect.nCurValue;
    MV_CC_GetIntValue(handle, "ThresholdValue_whiteDetect",&thresholdValue_whiteDetect);
    MV_CC_GetIntValue(handle, "ThresholdValue_blackDetect",&thresholdValue_blackDetect);
    MV_CC_GetIntValue(handle, "Width",&width);
    MV_CC_GetIntValue(handle, "Height",&height);
    MV_CC_GetIntValue(handle, "OffsetX",&offsetX);
    MV_CC_GetIntValue(handle, "OffsetY",&offsetY);
    MV_CC_GetIntValue(handle, "AcquisitionLineRate",&acquisitionLineRate);

    MV_CC_GetFloatValue(handle, "Gain", &gain);
    MV_CC_GetFloatValue(handle, "Gamma", &gamma);
    MV_CC_GetFloatValue(handle, "ExposureTime", &exposureTime);

    MV_CC_GetBoolValue(handle, "AcquisitionLineRateEnable", &setting.acquisitionLineRateEnable);
    MV_CC_GetBoolValue(handle, "GammaEnable", &setting.gammaEnable);
    MV_CC_GetBoolValue(handle, "NUCEnable", &setting.nucEnable);

    MV_CC_GetEnumValue(handle, "GainAuto", &gainAuto);
    MV_CC_GetEnumValue(handle, "GammaSelector", &gammaSelector);

    setting.thresholdValue_whiteDetect = thresholdValue_whiteDetect.nCurValue;
    setting.thresholdValue_blackDetect = thresholdValue_blackDetect.nCurValue;
    setting.width = width.nCurValue;
    setting.height = height.nCurValue;
    setting.offsetX = offsetX.nCurValue;
    setting.offsetY = offsetY.nCurValue;
    setting.acquisitionLineRate = acquisitionLineRate.nCurValue;

    setting.gain = gain.fCurValue;
    setting.gamma = gamma.fCurValue;
    setting.exposureTime = exposureTime.fCurValue;

    setting.gainAuto = static_cast<GainAuto>(gainAuto.nCurValue);
    qDebug() << setting.gainAuto;
    setting.gammaSelector = static_cast<GammaSelector>(gammaSelector.nCurValue);
    return setting;
}

QByteArray HKCamera::get_camera_bin(CameraSetting setting)
{
    QMap<int,QString> gain_map {
        {0,"Off"},
        {1,"Once"},
        {2,"Continuous"},
    };
    QMap<int,QString> gamma_map {
        {1,"User"},
        {2,"sRGB"},
    };

    QJsonObject camera
    {
        {"thresholdValue_whiteDetect",(int)setting.thresholdValue_whiteDetect},
        {"thresholdValue_blackDetect",(int)setting.thresholdValue_blackDetect},
        {"width",(int)setting.width},
        {"height",(int)setting.height},
        {"offsetX",(int)setting.offsetX},
        {"offsetY",(int)setting.offsetY},
        {"acquisitionLineRate",(int)setting.acquisitionLineRate},
        {"acquisitionLineRateEnable",setting.acquisitionLineRateEnable},
        {"gain",(double)setting.gain},
        {"gainAuto",gain_map[setting.gainAuto]},
        {"gamma",(double)setting.gamma},
        {"gammaEnable",setting.gammaEnable},
        {"gammaSelector",gamma_map[setting.gammaSelector]},
        {"exposureTime",(double)setting.exposureTime},
        {"nucEnable",setting.nucEnable},
    };

    QJsonDocument camera_doc = QJsonDocument(camera);
    QByteArray camera_data = camera_doc.toJson();
    return camera_data;
}

bool HKCamera::isOpened()
{
    if (MV_CC_IsDeviceConnected(handle)){
        qDebug() << "information opened";
        return true;
    }
    return false;
}

int HKCamera::setParams(DType type, char *params, QVariant value)
{
    if (type == DType::Bool){
        if (MV_OK != MV_CC_SetBoolValue(handle, params,value.toBool()))
            return -1;
    }
    else if (type == DType::Int){
        if (MV_OK != MV_CC_SetIntValue(handle, params, value.toUInt()))
            return -1;
    }
    else if (type == DType::Float){
        if (MV_OK != MV_CC_SetFloatValue(handle, params, value.toFloat()))
            return -1;
    }
    else if (type == DType::String){

    }
    return 0;
}


