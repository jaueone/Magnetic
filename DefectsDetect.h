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
	bool get_result();
    int get_defectsType();

private:
    bool _bResult;
    int _iType=0xF0;

    // Local iconic variables
    HObject  ho_ImageEdgeAmp2, ho_Regions2, ho_preConnectedRegions2;
    HObject  ho_SelectedRegions2, ho_RegionDilation2, ho_Image;
    HObject  ho_Rectangle, ho_ImageMedian, ho_ImageEdgeAmp1;
    HObject  ho_Regions1, ho_RegionDilation1, ho_ConnectedRegions1;
    HObject  ho_Part1SelectedRegions, ho_RegionUnion1, ho_ImageSmooth;
    HObject  ho_Part2ConnectedRegions, ho_RegionUnion2, ho_RegionUnion;

    // Local control variables
    HTuple  hv_thresholdWhite, hv_Width, hv_Height;
    HTuple  hv_mainWindowHandle, hv_ScratchExist, hv_WhitePointExist;
    HTuple  hv_BlackPointExist, hv_ReturnIsOK, hv_Mean, hv_Deviation;
    HTuple  hv_medianDegree, hv_thresholdBlack, hv_Part1blackDefectsNum;
    HTuple  hv_smoothDegree, hv_defectsNum1, hv_defectsNum2;
    HTuple  hv_defectsNum, hv_ScratchNum, hv_WhitePointNum;
    HTuple  hv_BlackPointNum, hv_Rectangularity, hv_Area, hv_Row;
    HTuple  hv_Column, hv_Grayval, hv_RecThreshold, hv_GrayvalThreshold;
    HTuple  hv_Index, hv_ResultMessage, hv_Color;

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
