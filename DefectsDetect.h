#ifndef DEFECTSDETECT_H
#define DEFECTSDETECT_H

#include <Halcon.h>
#include <HalconCpp.h>

using namespace HalconCpp;

struct Result
{
    HObject image;
    bool is_ok;
};

class DefectsDetect
{
public:
	DefectsDetect();
	~DefectsDetect();


	void run(HObject &ho_Image);
	bool get_result();
	//int get_defectsType();
    Result getResult(HObject &ho_Image);
private:
	bool _bResult;
	//int _iType=0xF0;


	
  // Local iconic variables
  HObject  ho_Image, ho_ROI_0, ho_ImageReduced;
  HObject  ho_ROI_L, ho_ROI_R, ho_ConnectedRegions1, ho_GrayImage;
  HObject  ho_ImageGauss, ho_ImageRoberts1, ho_RegionsRoberts1;
  HObject  ho_coRegionsRoberts1, ho_RegionsRoberts11, ho_Contours1;
  HObject  ho_SelectedContours1, ho_SelectedContours11;

  // Local control variables
  HTuple  hv_Width, hv_Height, hv_WindowHandle;
  HTuple  hv_Row1, hv_Column1, hv_Row2, hv_Column2, hv_M;
  HTuple  hv_D, hv_are1, hv_r1, hv_c1, hv_are2, hv_r2, hv_c2;
  HTuple  hv_CircleNum, hv_ReturnIsOK;



};

#endif
