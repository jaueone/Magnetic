#ifndef DEFECTSDETECT_H
#define DEFECTSDETECT_H

#include <Halcon.h>
#include <HalconCpp.h>
#include <QImage>

using namespace HalconCpp;

extern int ThresholdBlack;
extern int ThresholdWhite;

class DefectsDetect:public QObject
{
    Q_OBJECT

public:
	DefectsDetect();
	~DefectsDetect();

    void run(HObject &ho_Image,HObject &deal_image, const int width, const int height, const Hlong &winid, int x, int y);
    int get_result();
    int get_defectsType();

signals:
    void tell_window_check_result(int,int);

public slots:
    void accept_run(HObject &object,HObject &deal_object, const int width, const int height,const Hlong &winid);

private:
    int _iResult;
    int _iType=0xF0;

    // Local iconic variables
    HObject  ho_ConnectedRegions1, ho_Image, ho_Part1SelectedRegion1;
    HObject  ho_Part1SelectedRegion2, ho_Rectangle, ho_ImageMedian;
    HObject  ho_ImageEdgeAmp1, ho_Regions1, ho_RegionDilation1;
    HObject  ho_Part1SelectedRegion0, ho_RegionUnionPre, ho_RegionUnion1;
    HObject  ho_Part1SelectedRegions, ho_ImageSmooth, ho_ImageEdgeAmp2;
    HObject  ho_Regions2, ho_preConnectedRegions2, ho_SelectedRegions_good;
    HObject  ho_RegionDilation2_good, ho_SelectedRegions_notGood1;
    HObject  ho_RegionDilation2_notGood1, ho_SelectedRegions_notGood2;
    HObject  ho_RegionDilation2_notGood2, ho_RegionUnion2_good;
    HObject  ho_RegionUnion2_notGood, ho_RegionUnion2;

    // Local control variables
    HTuple  hv_NumberNg, hv_NumberGood, hv_NumberOkNotGood;
    HTuple  hv_Width, hv_Height, hv_mainWindowHandle, hv_ScratchExist;
    HTuple  hv_WhitePointExist, hv_BlackPointExist, hv_ReturnIsOK;
    HTuple  hv_ReturnIsGood, hv_medianDegree, hv_thresholdBlack;
    HTuple  hv_smoothDegree, hv_thresholdWhite, hv_areaSelect_good;
    HTuple  hv_areaSelect_notGood, hv_Mean, hv_Deviation, hv_Part1DefectsNum;
    HTuple  hv_Part2DefectsNum2_good, hv_Part2DefectsNum2_notGood1;
    HTuple  hv_Part2DefectsNum2_notGood2, hv_Part2DefectsNum2_notGood;
};


#endif
