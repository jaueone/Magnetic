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


	void run(HObject &ho_Image);
	bool get_result();
	HObject *get_detectedImage(){return &ho_detectedImage;}
	//int get_defectsType();

private:
	bool _bResult;
	//int _iType=0xF0;



// Local iconic variables
  HObject  ho_Image, ho_ROI_0, ho_ImageReduced;
  HObject  ho_ROI_L, ho_ROI_R, ho_GrayImage, ho_ImageGauss;
  HObject  ho_ImageRoberts1, ho_RegionsRoberts1, ho_coRegionsRoberts1;
  HObject  ho_RegionsRoberts11, ho_ConnectedRegions1, ho_Contours1;
  HObject  ho_SelectedContours1, ho_SelectedContours11, ho_detectedImage;

  // Local control variables
  HTuple  hv_Width, hv_Height, hv_WindowHandle;
  HTuple  hv_Row1, hv_Column1, hv_Row2, hv_Column2, hv_M;
  HTuple  hv_D, hv_are1, hv_r1, hv_c1, hv_are2, hv_r2, hv_c2;
  HTuple  hv_CircleNum, hv_ReturnIsOK;


};

#endif
