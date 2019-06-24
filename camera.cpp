#include "camera.h"
#include <QDebug>
#include <QLabel>
//#define MAX_BUF_SIZE    (6576*4384)


HKCamera::HKCamera() {

}

HKCamera::~HKCamera()
{
    this->stopCollect();
    this->closeDevice();
    this->destroyHandle();
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
        qDebug() << "information camera opened";
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
    qDebug() << "information camera opened";
    MV_CC_SetIntValue(this->handle,"GevSCPD",7000);
    MV_CC_SetIntValue(this->handle,"PreDivider",1);
    MV_CC_SetIntValue(this->handle,"Multiplier",2);
    MV_CC_SetBoolValue(this->handle,"GevPAUSEFrameReception",true);
    MV_CC_SetEnumValue(this->handle,"TriggerSelector",9);
    MV_CC_SetEnumValue(this->handle,"TriggerMode",1);
    MV_CC_SetEnumValue(this->handle,"TriggerSource",8);
    MV_CC_SetEnumValue(this->handle,"LineSelector",0);
    MV_CC_SetIntValue(this->handle,"Width",3800);
    MV_CC_SetIntValue(this->handle,"Height",3700);
    MV_CC_SetIntValue(this->handle,"OffsetX",150);
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
    if (this->is_start_collected){
        qDebug() << "start collect";
        return 0;
    }
    nRet = MV_CC_StartGrabbing(handle);
    if (MV_OK != nRet)
    {
        qDebug("error: StartGrabbing fail [%x]\n", nRet);
        return -1;
    }
    //设置相机图像的像素格式
   //unsigned int enValue = PixelType_Gvsp_BayerRG8;
    //unsigned int enValue = PixelType_Gvsp_RGB8_Packed;
//    nRet = MV_CC_SetPixelFormat(handle, enValue);
//    if (MV_OK != nRet)
//    {
//        qDebug("error: SetPixelFormat fail [%x]\n", nRet);
//        return -1;
//    }
    this->is_start_collected = true;
    qDebug() << "start collect";
    return 0;
}

int HKCamera::collectFrame(QLabel *label)
{

    /************************************************************************/
    /* 5.获取一帧并保存成图像  MV_CC_GetOneFrameTimeout  MV_CC_SaveImage    */
    /************************************************************************/
//    unsigned int    nTestFrameSize = 0;
//    unsigned char*  pFrameBuf = nullptr;
//    pFrameBuf = (unsigned char*)malloc(MAX_BUF_SIZE+2048);

//    MV_FRAME_OUT_INFO stInfo;
//    memset(&stInfo, 0, sizeof(MV_FRAME_OUT_INFO));

//    MV_NETTRANS_INFO stNetTransInfo = {0};
    //获取一帧数据的大小
        MVCC_INTVALUE stIntvalue = {0};
        nRet = MV_CC_GetIntValue(handle, "PayloadSize", &stIntvalue);
        if (nRet != MV_OK)
        {
            qDebug("Get PayloadSize failed! nRet [%x]\n", nRet);
            return -1;
        }
        int nBufSize = stIntvalue.nCurValue; //一帧数据大小
        qDebug() << " size of image:"<< nBufSize;
       // unsigned int    nTestFrameSize = 0;
        unsigned char*  pFrameBuf = NULL;
        pFrameBuf = (unsigned char*)malloc(nBufSize);//显示数据
        unsigned char*  data = NULL;
        data = (unsigned char*)malloc(nBufSize);//转换到halcon数据使用
        MV_FRAME_OUT_INFO stInfo;
        memset(&stInfo, 0, sizeof(MV_FRAME_OUT_INFO));

        //上层应用程序需要根据帧率，控制好调用该接口的频率
        //此次代码仅供参考，实际应用建议另建线程进行图像帧采集和处理

        //pFrameBuf是相机采集到的一帧原始图像数据
        nRet = MV_CC_GetOneFrame(handle, pFrameBuf, nBufSize, &stInfo);
        MVCC_INTVALUE width,height;
        MV_CC_GetIntValue(handle, "Width",&width);
        MV_CC_GetIntValue(handle, "Height",&height);

        int hgt = height.nCurValue;
        int wid = width.nCurValue;
        qDebug() << "hgt" << hgt << "wid" << wid;
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
        memcpy(data, pFrameBuf, (size_t)nBufSize);
        GenImage1(&ho_Image, "byte", wid, hgt, (Hlong)(data));

//        WriteImage(ho_Image, "bmp", -1, HTuple("D:/photo/") + i);
        //Sleep(500);
        free(data);
        /**********************************************************************************/
    free(pFrameBuf);
    qDebug() << "colledted";
    i+= 1;
    return 0;
}

int HKCamera::stopCollect()
{
    /************************************************************************/
    /* 6. 停止抓图  MV_CC_StopGrabbing                                      */
    /************************************************************************/
    //停止采集图像
    if (!is_start_collected)
        return 0;
    nRet = MV_CC_StopGrabbing(handle);
    if (MV_OK != nRet)
    {
        qDebug("error: StopGrabbing fail [%x]\n", nRet);
        return -1;
    }
    this->is_start_collected = false;
    return 0;
}

int HKCamera::closeDevice()
{
    /************************************************************************/
    /* 7. 关闭设备  MV_CC_CloseDevice                                       */
    /************************************************************************/
    //关闭设备，释放资源
    if (!this->isOpened()){
        return 0;
    }
    nRet = MV_CC_CloseDevice(handle);
    if (MV_OK != nRet)
    {
        qDebug("error: CloseDevice fail [%x]\n", nRet);
        return -1;
    }
    return 0;
}
int HKCamera::destroyHandle()
{
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
HObject HKCamera::getImage()
{
    return ho_Image;

}

CameraSetting HKCamera::get_camera_setting()
{
    CameraSetting setting;
    if (!MV_CC_IsDeviceConnected(handle)){
        qDebug() << "camera no connected, get params fail";
        return setting;
    }
    MVCC_INTVALUE width,height,offsetX,offsetY,acquisitionLineRate;
    MVCC_FLOATVALUE gain, gamma, exposureTime;
    MVCC_ENUMVALUE gainAuto, gammaSelector, triggerSelector, triggerMode, triggerSource, lineSelector;

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
    MV_CC_GetEnumValue(handle, "TriggerSelector", &triggerSelector);
    MV_CC_GetEnumValue(handle, "TriggerMode", &triggerMode);
    MV_CC_GetEnumValue(handle, "TriggerSource", &triggerSource);
    MV_CC_GetEnumValue(handle, "LineSelector", &lineSelector);

    setting.width = width.nCurValue;
    setting.height = height.nCurValue;
    setting.offsetX = offsetX.nCurValue;
    setting.offsetY = offsetY.nCurValue;
    setting.acquisitionLineRate = acquisitionLineRate.nCurValue;

    setting.gain = gain.fCurValue;
    setting.gamma = gamma.fCurValue;
    setting.exposureTime = exposureTime.fCurValue;

    setting.gainAuto = static_cast<GainAuto>(gainAuto.nCurValue);
    setting.gammaSelector = static_cast<GammaSelector>(gammaSelector.nCurValue);
    setting.triggerSelector = static_cast<TriggerSelector>(triggerSelector.nCurValue);
    setting.triggerMode = static_cast<TriggerMode>(triggerMode.nCurValue);
    setting.triggerSource = static_cast<TriggerSource>(triggerSource.nCurValue);
    setting.lineSelector = static_cast<LineSelector>(lineSelector.nCurValue);
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
    QMap<int,QString> lineSelector_map {
        {0,"Line0"},
        {1,"Line1"},
        {2,"Line2"},
        {3,"Line3"},
        {4,"Line4"},
    };
    QMap<int,QString> triggerSource_map {
        {0,"Line0"},
        {1,"Line1"},
        {2,"Line2"},
        {3,"Line3"},
        {4,"Counter0"},
        {7,"Software"},
        {8,"FrequencyConverter"},
    };
    QMap<int,QString> triggerSelector_map {
        {6,"FrameBurstStart"},
        {9,"LineStart"},
    };
    QMap<int,QString> triggerMode_map {
        {0,"Off"},
        {1,"On"},
    };
    QJsonObject camera_obj
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
        {"triggerMode",triggerMode_map[setting.triggerMode]},
        {"triggerSelector",triggerSelector_map[setting.triggerSelector]},
        {"triggerSource",triggerSource_map[setting.triggerSource]},
        {"lineSelector",lineSelector_map[setting.lineSelector]},
    };

    QJsonDocument camera_doc = QJsonDocument(camera_obj);
    QByteArray camera_data = camera_doc.toJson();
//    qDebug() << "saved camera" <<  camera_obj;
    return camera_data;
}

bool HKCamera::isOpened()
{
    if (MV_CC_IsDeviceConnected(handle)){
        return true;
    }
    return false;
}

bool HKCamera::isCollecting()
{
    return this->is_start_collected;
}

void HKCamera::camera_message_done()
{
    QMessageBox messageBox;
    messageBox.setWindowTitle("信息");
    messageBox.setIcon(QMessageBox::Information);
    QPushButton button("确定");
    messageBox.setText("设置相机参数完成!");
    messageBox.exec();
}

void HKCamera::camera_message_warn()
{
      QMessageBox messageBox;
      messageBox.setWindowTitle("警告");
      messageBox.setIcon(QMessageBox::Warning);
      QPushButton button("确定");
      messageBox.setText("相机连接失败,\n请先打开相机!");
      messageBox.exec();
}


int HKCamera::setParams(DType type,const char *params, QVariant value)
{
    if (type == DType::Bool){
        return  MV_CC_SetBoolValue(handle, params,value.toBool());
    }
    else if (type == DType::Int){
        return MV_CC_SetIntValue(handle, params,value.toUInt());
    }
    else if (type == DType::Float){
        return MV_CC_SetFloatValue(handle, params,value.toFloat());
    }
    else if (type == DType::Enum){
        return MV_CC_SetEnumValue(handle, params,value.toUInt());
    }
    else if (type == DType::String){
    }
    return 0;
}


