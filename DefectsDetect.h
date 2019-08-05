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

    void run(HObject ho_Image,HObject deal_image, const int width, const int height, const Hlong &winid, int x, int y);
    int get_result();
    int get_defectsType();

signals:
    void tell_window_check_result(int,int,HObject deal_image);

public slots:
    void accept_run(HObject &object,HObject &deal_object, const int width, const int height,const Hlong &winid);

private:
    int _iResult;
    int _iType=0xF0;

    // Local iconic variables
    HObject  ho_Image, ho_RegionUnion1, ho_RegionUnion2;
    HObject  ho_Rectangle, ho_ImageMedian, ho_ImageEdgeAmp1;
    HObject  ho_Regions1, ho_ConnectedRegions1, ho_SelectedRegions;
    HObject  ho_RegionDilation1, ho_ImageSmooth, ho_ImageEdgeAmp2;
    HObject  ho_Regions2, ho_preConnectedRegions2, ho_SelectedRegions_good;
    HObject  ho_RegionDilation2_good, ho_SelectedRegions_notGood1;
    HObject  ho_RegionDilation2_notGood1, ho_SelectedRegions_notGood2;
    HObject  ho_RegionDilation2_notGood2, ho_RegionUnion2_good;
    HObject  ho_RegionUnion2_notGood;

    // Local control variables
    HTuple  hv_ImageFiles, hv_Index, hv_mainWindowHandle;
    HTuple  hv_NumberNg, hv_NumberGood, hv_NumberOkNotGood;
    HTuple  hv_Width, hv_Height, hv_ScratchExist, hv_WhitePointExist;
    HTuple  hv_BlackPointExist, hv_ReturnIsOK, hv_ReturnIsGood;
    HTuple  hv_medianDegree, hv_thresholdBlack, hv_smoothDegree;
    HTuple  hv_thresholdWhite, hv_areaSelect_good, hv_areaSelect_notGood;
    HTuple  hv_Mean, hv_Deviation, hv_Part1DefectsNum, hv_Part2DefectsNum2_good;
    HTuple  hv_Part2DefectsNum2_notGood1, hv_Part2DefectsNum2_notGood2;
    HTuple  hv_Part2DefectsNum2_notGood;
};


#endif
