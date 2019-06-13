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


    void run(HObject &ho_Image,HObject &deal_image, const int width, const int height, const Hlong &winid, int x, int y);
	bool get_result();
    int get_defectsType();

private:
    bool _bResult;
    int _iType=0xF0;


    // Local iconic variables
    HObject  ho_Image, ho_ImageMedian, ho_ImageEdgeAmp1;
    HObject  ho_Regions1, ho_RegionDilation1, ho_ConnectedRegions1;
    HObject  ho_SelectedRegions, ho_RegionUnion1, ho_ImageSmooth;
    HObject  ho_ImageEdgeAmp2, ho_Regions2, ho_RegionDilation2;
    HObject  ho_ConnectedRegions2, ho_RegionUnion2, ho_RegionUnion;

    // Local control variables
    HTuple  hv_Width, hv_Height, hv_mainWindowHandle;
    HTuple  hv_medianDegree, hv_thresholdBlack, hv_GraySelectedNum;
    HTuple  hv_smoothDegree, hv_thresholdWhite, hv_defectsNum1;
    HTuple  hv_defectsNum2, hv_defectsNum, hv_Rectangularity;
    HTuple  hv_Area, hv_Row, hv_Column, hv_Grayval, hv_ScratchNum;
    HTuple  hv_WhitePointNum, hv_BlackPointNum, hv_RecThreshold;
    HTuple  hv_GrayvalThreshold, hv_ScratchExist, hv_WhitePointExist;
    HTuple  hv_BlackPointExist, hv_ReturnIsOK, hv_regionNum;
    HTuple  hv_Index, hv_ResultMessage, hv_Color;


};

#endif
