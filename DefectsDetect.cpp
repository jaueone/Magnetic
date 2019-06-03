#include "DefectsDetect.h"


DefectsDetect::DefectsDetect()
{
	
}

DefectsDetect::~DefectsDetect()
{
	
}

void DefectsDetect::run(HObject &ho_Image)
{
    GetImageSize(ho_Image, &hv_Width, &hv_Height);

  if (HDevWindowStack::IsOpen())
    ClearWindow(HDevWindowStack::GetActive());
  SetWindowAttr("background_color","black");
  OpenWindow(0,0,1314,876,0,"visible","",&hv_WindowHandle);
  HDevWindowStack::Push(hv_WindowHandle);
  if (HDevWindowStack::IsOpen())
    SetDraw(HDevWindowStack::GetActive(),"margin");
  if (HDevWindowStack::IsOpen())
    SetColored(HDevWindowStack::GetActive(),12);
  // dev_update_window(...); only in hdevelop
  if (HDevWindowStack::IsOpen())
    DispObj(ho_Image, HDevWindowStack::GetActive());

  hv_Row1 = 975;
  hv_Column1 = 1055;
  hv_Row2 = 3340;
  hv_Column2 = 5360;
  GenRectangle1(&ho_ROI_0, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
  ReduceDomain(ho_Image, ho_ROI_0, &ho_ImageReduced);
  GenRectangle1(&ho_ROI_0, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
  GenRectangle1(&ho_ROI_L, hv_Row1, hv_Column1, hv_Row2, 0.4*(hv_Column1+hv_Column2));
  GenRectangle1(&ho_ROI_R, hv_Row1, 0.6*(hv_Column1+hv_Column2), hv_Row2, hv_Column2);
  ReduceDomain(ho_Image, ho_ROI_0, &ho_ImageReduced);

  Rgb1ToGray(ho_ImageReduced, &ho_GrayImage);
  GaussFilter(ho_GrayImage, &ho_ImageGauss, 7);




  //***找圆点
  Roberts(ho_ImageGauss, &ho_ImageRoberts1, "gradient_sum");
  Intensity(ho_ROI_0, ho_ImageRoberts1, &hv_M, &hv_D);
  Threshold(ho_ImageRoberts1, &ho_RegionsRoberts1, 8, 75);
  AreaCenter(ho_RegionsRoberts1, &hv_are1, &hv_r1, &hv_c1);
  Connection(ho_RegionsRoberts1, &ho_coRegionsRoberts1);
  AreaCenter(ho_coRegionsRoberts1, &hv_are2, &hv_r2, &hv_c2);
  if (0 != (hv_Row1<hv_r1))
  {
    if (0 != (hv_Column1<hv_c1))
    {
      //dilation_circle (RegionsRoberts1, RegionsRoberts1, 50)
      DilationCircle(ho_RegionsRoberts1, &ho_RegionsRoberts11, 50);
      //dilation_circle (RegionsRoberts2, RegionsRoberts2, 50)
      //dilation_circle (RegionsRoberts2, RegionsRoberts2, 50)
      //erosion_circle (RegionsRoberts2, ErosionRoberts2, 50)
      Connection(ho_RegionsRoberts11, &ho_ConnectedRegions1);
      GenContourRegionXld(ho_ConnectedRegions1, &ho_Contours1, "border");
      SelectShapeXld(ho_Contours1, &ho_SelectedContours1, (((((HTuple("row1").Append("column1")).Append("row2")).Append("column2")).Append("circularity")).Append("contlength")), 
          "and", (((((HTuple(2).Append(2)).Append(2)).Append(2)).Append(0.7)).Append(2)), 
          ((((hv_Height-2).TupleConcat(hv_Width-2)).TupleConcat(hv_Height-2)).TupleConcat(hv_Width-2)).TupleConcat((HTuple(1).Append(999999))));
      //smallest_rectangle1_xld (SelectedContours1, Row11, Column11, Row21, Column21)
      //gen_rectangle1 (Rectangle, Row11-5, Column11-5, Row21+5, Column21+5)

      GenRegionContourXld(ho_SelectedContours1, &ho_SelectedContours1, "margin");

      CountObj(ho_SelectedContours1, &hv_CircleNum);

      Union1(ho_SelectedContours1, &ho_SelectedContours11);
      if (HDevWindowStack::IsOpen())
        DispObj(ho_SelectedContours11, HDevWindowStack::GetActive());

      DumpWindowImage(&ho_detectedImage, hv_WindowHandle);

      if (HDevWindowStack::IsOpen())
        CloseWindow(HDevWindowStack::Pop());

      if (0 != (hv_CircleNum>0))
      {
        hv_ReturnIsOK = 0;
      }
      else
      {
        hv_ReturnIsOK = 1;
      }



    }
  }


	
}

/************************************************************************
@return 1: is OK

@return 0: is not OK 
 
 ************************************************************************/
bool DefectsDetect::get_result()
{

	if (hv_ReturnIsOK==1)
	_bResult = 1;
	else
	_bResult = 0;

	return _bResult;
}



