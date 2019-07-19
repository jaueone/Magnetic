#ifndef DEFECTSDETECT_H
#define DEFECTSDETECT_H

#include <Halcon.h>
#include <HalconCpp.h>
#include <QImage>

using namespace HalconCpp;

class DefectsDetect
{
public:
	DefectsDetect();
	~DefectsDetect();

    void set_threshold_param(int ThresholdBlack=34,int ThresholdWhite=50);
    int threshold_param();

    void run(HObject &ho_Image,HObject &deal_image, const int width, const int height, const Hlong &winid, int x, int y);
    bool get_isok();
    bool get_isgood();
    int get_defectsType();

private:
    bool _bResult;
    bool _isGood;
    int _iType=0xF0;


    // Local iconic variables
    HObject  ho_Image, ho_Rectangle, ho_ImageMedian;
    HObject  ho_ImageEdgeAmp1, ho_Regions1, ho_RegionDilation1;
    HObject  ho_ConnectedRegions1, ho_Part1SelectedRegions, ho_RegionUnion1;
    HObject  ho_ImageSmooth, ho_ImageEdgeAmp2, ho_Regions2, ho_preConnectedRegions2;
    HObject  ho_SelectedRegions2, ho_RegionDilation2, ho_Part2ConnectedRegions;
    HObject  ho_RegionUnion2, ho_RegionUnion;

    // Local control variables
    HTuple  hv_Width, hv_Height, hv_mainWindowHandle;
    HTuple  hv_ScratchExist, hv_WhitePointExist, hv_BlackPointExist;
    HTuple  hv_ReturnIsOK, hv_medianDegree, hv_thresholdBlack;
    HTuple  hv_smoothDegree, hv_thresholdWhite, hv_areaSelect;
    HTuple  hv_Mean, hv_Deviation, hv_Part1blackDefectsNum;
    HTuple  hv_defectsNum1, hv_defectsNum2, hv_defectsNum, hv_ScratchNum;
    HTuple  hv_WhitePointNum, hv_BlackPointNum, hv_Rectangularity;
    HTuple  hv_Area, hv_Row, hv_Column, hv_Grayval, hv_RecThreshold;
    HTuple  hv_GrayvalThreshold, hv_Index, hv_ResultMessage;
    HTuple  hv_Color;

};
static DefectsDetect __defects__;
class ImageAlgorithm
{
public:
    static DefectsDetect *getInterface();
private:
    ImageAlgorithm();
    ImageAlgorithm(const ImageAlgorithm& other);
    ~ImageAlgorithm();
};
#endif
