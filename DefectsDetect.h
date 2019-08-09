#ifndef DEFECTSDETECT_H
#define DEFECTSDETECT_H

#include <Halcon.h>
#include <HalconCpp.h>
#include <QImage>
#include <QJsonObject>
using namespace HalconCpp;

extern int ThresholdBlack;
extern int ThresholdWhite;

class DefectsDetect:public QObject
{
    Q_OBJECT

public:
	DefectsDetect();
    ~DefectsDetect();
    void set_Params(int ThresholdBlack =33, int ThresholdWhite =26,float P1areaSelect_t_low =18.0,float P1areaSelect_t_high =28.0,float P1deviation_t_big  =6.0, int P1DefectsNum_small_t=30
                    ,float P2areaSelect_t_low =18.0,float P2areaSelect_t_high =45.0,float P2deviation_t_big =6.0,float P2height_t = 12.518,float P2width_t = 12.016,int P2DefectsNum_small_t = 30);

    void run(HObject ho_Image,HObject deal_image, const int width, const int height, const Hlong &winid, int x, int y);
    int get_result();
    int get_defectsType();

signals:
    void tell_window_check_result(int,int,HObject deal_image);

public slots:
    void accept_run(HObject &object,HObject &deal_object, const int width, const int height,const Hlong &winid,QJsonObject params);

private:
    int _iResult;
    int _iType=0xF0;

    // Local iconic variables
    HObject  ho_Image, ho_RegionUnion1, ho_RegionUnion2;
    HObject  ho_Rectangle, ho_ImageMedian, ho_ImageEdgeAmp1;
    HObject  ho_Regions1, ho_ConnectedRegions1, ho_SelectedRegions;
    HObject  ho_P1SelectedRegions_small, ho_P1RegionDilation1;
    HObject  ho_P1RegionUnion1_small, ho_P1SelectedRegions_big0;
    HObject  ho_P1SelectedRegions_big, ho_P1RegionDilation2;
    HObject  ho_P1RegionUnion2_big, ho_ImageSmooth, ho_ImageEdgeAmp2;
    HObject  ho_Regions2, ho_preConnectedRegions2, ho_P2Selectedarea_holes;
    HObject  ho_P2SelectedRegions_small, ho_RegionDilation2_small;
    HObject  ho_P2SelectedRegions_big1, ho_RegionDilation2_big1;
    HObject  ho_P2SelectedRegions_big2, ho_RegionDilation2_big2;
    HObject  ho_RegionUnion2_small, ho_RegionUnion2_big;

    // Local control variables
    HTuple  hv_NumberNg, hv_NumberGood, hv_NumberOkNotGood;
    HTuple  hv_Width, hv_Height, hv_WindowHandle, hv_ScratchExist;
    HTuple  hv_WhitePointExist, hv_BlackPointExist, hv_ReturnIsOK;
    HTuple  hv_ReturnIsGood, hv_medianDegree, hv_thresholdBlack;
    HTuple  hv_smoothDegree, hv_thresholdWhite, hv_P1areaSelect_t_low;
    HTuple  hv_P1areaSelect_t_high, hv_P1deviation_t_big, hv_P1DefectsNum_small_t;
    HTuple  hv_P2areaSelect_t_low, hv_P2areaSelect_t_high, hv_P2deviation_t_big;
    HTuple  hv_P2height_t, hv_P2width_t, hv_P2DefectsNum_small_t;
    HTuple  hv_Mean, hv_Deviation, hv_P1DefectsNum_small, hv_P1DefectsNum_big;
    HTuple  hv_P2NumberArea_holes, hv_P2DefectsNum_small, hv_P2DefectsNum2_big1;
    HTuple  hv_P2DefectsNum2_big2, hv_P2DefectsNum2_big;

};


#endif
