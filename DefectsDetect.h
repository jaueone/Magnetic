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
    int get_result();
    int get_defectsType();

private:
    int _iResult;
    int _iType=0xF0;


    // Local iconic variables
    HObject  ho_ConnectedRegions1, ho_Image, ho_Part1SelectedRegion1;
    HObject  ho_Part1SelectedRegion2, ho_Rectangle, ho_ImageMedian;
    HObject  ho_ImageEdgeAmp1, ho_Regions1, ho_RegionDilation1;
    HObject  ho_Part1SelectedRegion0, ho_RegionUnion1, ho_Part1SelectedRegions;
    HObject  ho_ImageSmooth, ho_ImageEdgeAmp2, ho_Regions2, ho_preConnectedRegions2;
    HObject  ho_SelectedRegions_good, ho_RegionDilation2_good;
    HObject  ho_SelectedRegions_notGood1, ho_SelectedRegions_notGood2;
    HObject  ho_RegionDilation2_notGood1, ho_RegionDilation2_notGood2;
    HObject  ho_UnionSelectedRegions_notGood, ho_SelectedRegions_notGood;
    HObject  ho_RegionUnion2_good, ho_RegionUnion2_notGood, ho_RegionUnion2;

    // Local control variables
    HTuple  hv_NumberNg, hv_NumberGood, hv_NumberOkNotGood;
    HTuple  hv_Width, hv_Height, hv_mainWindowHandle, hv_ScratchExist;
    HTuple  hv_WhitePointExist, hv_BlackPointExist, hv_ReturnIsOK;
    HTuple  hv_ReturnIsGood, hv_medianDegree, hv_thresholdBlack;
    HTuple  hv_smoothDegree, hv_thresholdWhite, hv_areaSelect_good;
    HTuple  hv_areaSelect_notGood, hv_Mean, hv_Deviation, hv_Part1DefectsNum;
    HTuple  hv_Part2DefectsNum2_good, hv_Part2DefectsNum2_notGood;


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
