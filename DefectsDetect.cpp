#include "DefectsDetect.h"


DefectsDetect::DefectsDetect()
{

}

DefectsDetect::~DefectsDetect()
{

}

void DefectsDetect::run(HObject &ho_Image, HObject &deal_image, const int width, const int height, const Hlong &winid, int x, int y)
{
    //initualization
  if (HDevWindowStack::IsOpen())
    CloseWindow(HDevWindowStack::Pop());

  GetImageSize(ho_Image, &hv_Width, &hv_Height);
  SetWindowAttr("background_color","black");
  OpenWindow(x,y,width,height,winid,"visible","",&hv_mainWindowHandle);
  HDevWindowStack::Push(hv_mainWindowHandle);

  if (HDevWindowStack::IsOpen())
    SetDraw(HDevWindowStack::GetActive(),"margin");
  if (HDevWindowStack::IsOpen())
    SetLineWidth(HDevWindowStack::GetActive(),3);
  if (HDevWindowStack::IsOpen())
    SetColored(HDevWindowStack::GetActive(),12);

  //Image process procedure-1 ,especially for black or bigger size defects
  hv_medianDegree = 5;
  hv_thresholdBlack = 34;
  MedianImage(ho_Image, &ho_ImageMedian, "circle", hv_medianDegree, "mirrored");
  //invert_image (ImageMedian, ImageInvert)
  KirschAmp(ho_ImageMedian, &ho_ImageEdgeAmp1);
  Threshold(ho_ImageEdgeAmp1, &ho_Regions1, hv_thresholdBlack, 255);
  DilationCircle(ho_Regions1, &ho_RegionDilation1, 12.5);
  Connection(ho_RegionDilation1, &ho_ConnectedRegions1);
  SelectGray(ho_ConnectedRegions1, ho_Image, &ho_SelectedRegions, "mean", "and",
      0, 55);
  CountObj(ho_SelectedRegions, &hv_GraySelectedNum);
  Union1(ho_SelectedRegions, &ho_RegionUnion1);



  //Image process  procedure-2 ,especially for white or small size defects
  hv_smoothDegree = 1.7;
  hv_thresholdWhite = 50;

  SmoothImage(ho_Image, &ho_ImageSmooth, "gauss", hv_smoothDegree);
  KirschAmp(ho_ImageSmooth, &ho_ImageEdgeAmp2);
  Threshold(ho_ImageEdgeAmp2, &ho_Regions2, hv_thresholdWhite, 255);
  DilationCircle(ho_Regions2, &ho_RegionDilation2, 12.5);
  Connection(ho_RegionDilation2, &ho_ConnectedRegions2);
  Union1(ho_ConnectedRegions2, &ho_RegionUnion2);

  //****************
  Union2(ho_RegionUnion1, ho_RegionUnion2, &ho_RegionUnion);



  //result feedback
  CountObj(ho_SelectedRegions, &hv_defectsNum1);
  CountObj(ho_ConnectedRegions2, &hv_defectsNum2);
  hv_defectsNum = hv_defectsNum1+hv_defectsNum2;

  //if is not ok ,then...
  if (0 != (hv_defectsNum>2))
  {
    //defects statistic
    Rectangularity(ho_ConnectedRegions2, &hv_Rectangularity);
    AreaCenter(ho_ConnectedRegions2, &hv_Area, &hv_Row, &hv_Column);
    GetGrayval(ho_Image, hv_Row, hv_Column, &hv_Grayval);

    hv_ScratchNum = 0;
    hv_WhitePointNum = 0;
    hv_BlackPointNum = 0;

    hv_RecThreshold = 0.85;
    hv_GrayvalThreshold = 20;


    //*********** return parameters  ***********
    hv_ScratchExist = 0;
    hv_WhitePointExist = 0;
    hv_BlackPointExist = 0;
    hv_ReturnIsOK = 0;
    //*********** return parameters  ***********


    hv_regionNum = hv_Rectangularity.TupleLength();

    {
    HTuple end_val74 = hv_regionNum;
    HTuple step_val74 = 1;
    for (hv_Index=1; hv_Index.Continue(end_val74, step_val74); hv_Index += step_val74)
    {
      if (0 != (HTuple(hv_Rectangularity[hv_Index-1])>hv_RecThreshold))
      {
        hv_ScratchNum += 1;
        continue;
      }
      else if (0 != (HTuple(hv_Grayval[hv_Index-1])>hv_GrayvalThreshold))
      {
        hv_WhitePointNum += 1;
        continue;
      }
      else
      {
        hv_BlackPointNum += 1;
      }
    }
    }

    hv_BlackPointNum += hv_GraySelectedNum;
    hv_ResultMessage.Clear();
    hv_ResultMessage[0] = " Not OK";
    hv_ResultMessage.Append(//' 划痕: '
      " \273\256\272\333: "+hv_ScratchNum);
    hv_ResultMessage.Append(//' 白色缺陷: '
      " \260\327\311\253\310\261\317\335: "+hv_WhitePointNum);
    hv_ResultMessage.Append(//' 黑色缺陷: '
      " \272\332\311\253\310\261\317\335: "+hv_BlackPointNum);
    hv_Color.Clear();
    hv_Color[0] = "red";
    hv_Color[1] = "black";
    hv_Color[2] = "black";
    hv_Color[3] = "black";
    hv_ReturnIsOK = 0;
  }
  else
  {
    hv_ResultMessage = "OK";
    hv_Color = "forest green";
    hv_ReturnIsOK = 1;
  }

  //*********** return parameters  ***********
  if (0 != (hv_ScratchNum!=0))
  {
    hv_ScratchExist = 1;
  }
  if (0 != (hv_WhitePointNum!=0))
  {
    hv_WhitePointExist = 1;
  }
  if (0 != (hv_BlackPointNum!=0))
  {
    hv_BlackPointExist = 1;
  }
  //*********** return parameters  ***********


  //Display the results
  if (HDevWindowStack::IsOpen())
    DispObj(ho_Image, HDevWindowStack::GetActive());
  if (HDevWindowStack::IsOpen())
    DispObj(ho_RegionUnion, HDevWindowStack::GetActive());
  DumpWindowImage(&deal_image,hv_mainWindowHandle);
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



int DefectsDetect::get_defectsType()
{
    if(hv_ScratchExist==1)
        _iType=0xF1;
    if (hv_WhitePointExist == 1)
        _iType = (0xF1 || 0xF2);
    if (hv_BlackPointExist==1)
        _iType = (0xF1 || 0xF4);


    return _iType;

}
