#include "DefectsDetect.h"
#include <QDebug>



DefectsDetect::DefectsDetect()
{
    hv_thresholdBlack = 34;
    hv_thresholdWhite = 50;
}

DefectsDetect::~DefectsDetect()
{

}

void DefectsDetect::set_Params(int ThresholdBlack, int ThresholdWhite, float P1areaSelect_t_low, float P1areaSelect_t_high, float P1deviation_t_big, int P1DefectsNum_small_t, float P2areaSelect_t_low, float P2areaSelect_t_high, float P2deviation_t_big, float P2height_t, float P2width_t, int P2DefectsNum_small_t)
{
    qDebug() << "set_Params";

    hv_thresholdBlack=ThresholdBlack;
    hv_thresholdWhite=ThresholdWhite;

    hv_P1areaSelect_t_low = P1areaSelect_t_low;
    hv_P1areaSelect_t_high = P1areaSelect_t_high;
    hv_P1deviation_t_big = P1deviation_t_big;
    hv_P1DefectsNum_small_t = P1DefectsNum_small_t;

    //---------------
    hv_P2areaSelect_t_low = P2areaSelect_t_low;
    hv_P2areaSelect_t_high = P2areaSelect_t_high;
    hv_P2deviation_t_big = P2deviation_t_big;
    hv_P2height_t = P2height_t;
    hv_P2width_t = P2width_t;
    hv_P2DefectsNum_small_t = P2DefectsNum_small_t;
    qDebug() <<hv_thresholdBlack.I() << hv_thresholdWhite.I() << hv_P1areaSelect_t_low.D() << hv_P1areaSelect_t_high.D() << hv_P1deviation_t_big.D()
             <<hv_P1DefectsNum_small_t.I() << hv_P2areaSelect_t_low.D() << hv_P2areaSelect_t_high.D() << hv_P2deviation_t_big.D() << hv_P2height_t.D()
             << hv_P2width_t.D() << hv_P2DefectsNum_small_t.I();
}

void DefectsDetect::run(HObject ho_Image, HObject deal_image, const int width, const int height, const Hlong &winid, int x, int y)
{

    //filename: StickDetect-3class--new--2019年8月9日-release.hdev


    //release CONVENTION *
    //1. continue to return ()
    //2. display to
    //dev_display (Image)
    //dev_display (RegionUnion1)
    //dev_display (RegionUnion2)
    //3.
    //currentFiledir := ImageFiles[Index]
    //disp_message (mainWindowHandle, ImageFiles[Index], 'window', 60, 12, 'black', 'true')

    //***上位机注意事项：****
    //好良品:
    //returnIsOK =1;
    //returnIsGood=1;

    //一般良品：
    //returnIsOK=1；
    //returnIsGood=0;

    //不良品：
    //returnIsOK=0;

    //dumpWindow添加在(已经做标记*dumpWindow)



    //initualization
    //dev_update_off ()
    //dev_close_window ()




    //*********** statistic parameters***********
    hv_NumberNg = 0;
    hv_NumberGood = 0;
    hv_NumberOkNotGood = 0;
    //*********** statistic parameters***********


    //ReadImage(&ho_Image, "E:/WorkSpace/ImgData/磁锟样本/不良品检测结果20190716/182925_NG.bmp");
    //list_files ('E:/WorkSpace/ImgData/磁锟样本/20190806良品35', ['files','follow_links'], ImageFiles)

    //list_files ('E:/WorkSpace/ImgData/磁锟样本/51--总和--良品', ['files','follow_links'], ImageFiles)
    //list_files ('E:/WorkSpace/ImgData/磁锟样本/52--总和--次品', ['files','follow_links'], ImageFiles)
    //list_files ('E:/WorkSpace/ImgData/磁锟样本/53--总和--废品', ['files','follow_links'], ImageFiles)
    //tuple_regexp_select (ImageFiles, ['\\.(tif|tiff|gif|bmp|jpg|jpeg|jp2|png|pcx|pgm|ppm|pbm|xwd|ima|hobj)$','ignore_case'], ImageFiles)
    //* for Index := 0 to |ImageFiles| - 1 by 1
    //* read_image (Image, ImageFiles[Index])



    GetImageSize(ho_Image, &hv_Width, &hv_Height);
    SetWindowAttr("background_color","black");
    OpenWindow(0,0,width,height,winid,"visible","",&hv_WindowHandle);
    HDevWindowStack::Push(hv_WindowHandle);
    //set_display_font (mainWindowHandle, 14, 'mono', 'true', 'false')
    if (HDevWindowStack::IsOpen())
        SetDraw(HDevWindowStack::GetActive(),"margin");
    if (HDevWindowStack::IsOpen())
        SetLineWidth(HDevWindowStack::GetActive(),3);
    if (HDevWindowStack::IsOpen())
        SetColored(HDevWindowStack::GetActive(),12);


    //currentFiledir := ImageFiles[Index]
    //disp_message (WindowHandle, ImageFiles[Index], 'window', 60, 12, 'black', 'true')
    //dstFiledir := 'E:/WorkSpace/ImgData/磁锟样本/漏检废品--2019年8月4日/'

    //*********** return parameters setting  ***********
    hv_ScratchExist = 0;
    hv_WhitePointExist = 0;
    hv_BlackPointExist = 0;
    hv_ReturnIsOK = 0;
    hv_ReturnIsGood = 0;
    //*********** return parameters setting ***********
    //*********** global parameters ***********
    hv_medianDegree = 1;
   // hv_thresholdBlack = 33;
    hv_smoothDegree = 1;
  //  hv_thresholdWhite = 26;


//    hv_P1areaSelect_t_low = 18;
//    hv_P1areaSelect_t_high = 28;
//    hv_P1deviation_t_big = 6.0;
//    hv_P1DefectsNum_small_t = 30;

//    //---------------
//    hv_P2areaSelect_t_low = 18;
//    hv_P2areaSelect_t_high = 45;
//    hv_P2deviation_t_big = 6.0;
//    hv_P2height_t = 12.518;
//    hv_P2width_t = 12.016;
//    hv_P2DefectsNum_small_t = 30;


    //*********** global parameters***********


    //pre-judge : detect or not
    GenRectangle1(&ho_Rectangle, 0, 0, hv_Height, hv_Width);
    Intensity(ho_Rectangle, ho_Image, &hv_Mean, &hv_Deviation);
    if (0 != (hv_Mean<10))
    {
        hv_ReturnIsOK = 0;
    }
    else
    {



        //**P1-- for black defects***
        MedianImage(ho_Image, &ho_ImageMedian, "circle", hv_medianDegree, "mirrored");
        KirschAmp(ho_ImageMedian, &ho_ImageEdgeAmp1);
        Threshold(ho_ImageEdgeAmp1, &ho_Regions1, hv_thresholdBlack, 255);
        Connection(ho_Regions1, &ho_ConnectedRegions1);
        SelectGray(ho_ConnectedRegions1, ho_Image, &ho_SelectedRegions, "mean", "and",
        0, 70.601);
        //small--condition Area select
        SelectShape(ho_SelectedRegions, &ho_P1SelectedRegions_small, "area", "and", hv_P1areaSelect_t_low,
        hv_P1areaSelect_t_high);
        //select_gray (P1SelectedRegions_small0, Image, P1SelectedRegions_small, 'deviation', 'and', 4, 10)
        DilationCircle(ho_P1SelectedRegions_small, &ho_P1RegionDilation1, 12.5);
        CountObj(ho_P1SelectedRegions_small, &hv_P1DefectsNum_small);
        Union1(ho_P1RegionDilation1, &ho_P1RegionUnion1_small);

        //big--condition Area select
        SelectShape(ho_SelectedRegions, &ho_P1SelectedRegions_big0, "area", "and", hv_P1areaSelect_t_high,
        50000000);
        SelectGray(ho_P1SelectedRegions_big0, ho_Image, &ho_P1SelectedRegions_big, "deviation",
        "and", hv_P1deviation_t_big, 20);
        DilationCircle(ho_P1SelectedRegions_big, &ho_P1RegionDilation2, 12.5);
        CountObj(ho_P1SelectedRegions_big, &hv_P1DefectsNum_big);
        Union1(ho_P1RegionDilation2, &ho_P1RegionUnion2_big);

        Union2(ho_P1RegionUnion1_small, ho_P1RegionUnion2_big, &ho_RegionUnion1);



        //**P2-- for white defects ***
        SmoothImage(ho_Image, &ho_ImageSmooth, "gauss", hv_smoothDegree);
        KirschAmp(ho_ImageSmooth, &ho_ImageEdgeAmp2);
        Threshold(ho_ImageEdgeAmp2, &ho_Regions2, hv_thresholdWhite, 255);
        Connection(ho_Regions2, &ho_preConnectedRegions2);
        //select--'area_holes'
        SelectShape(ho_preConnectedRegions2, &ho_P2Selectedarea_holes, "area_holes",
        "and", 2.8975, 5);
        CountObj(ho_P2Selectedarea_holes, &hv_P2NumberArea_holes);
        //small--good condition Area select
        SelectShape(ho_preConnectedRegions2, &ho_P2SelectedRegions_small, "area", "and",
        hv_P2areaSelect_t_low, hv_P2areaSelect_t_high);
        DilationCircle(ho_P2SelectedRegions_small, &ho_RegionDilation2_small, 12.5);
        CountObj(ho_P2SelectedRegions_small, &hv_P2DefectsNum_small);
        //big--notGood condition Area select
        SelectShape(ho_preConnectedRegions2, &ho_P2SelectedRegions_big0, "area", "and",
        hv_P2areaSelect_t_high, 50000000);
        SelectGray(ho_P2SelectedRegions_big0, ho_Image, &ho_P2SelectedRegions_big1, "deviation",
        "and", hv_P2deviation_t_big, 20);
        DilationCircle(ho_P2SelectedRegions_big1, &ho_RegionDilation2_big1, 12.5);
        CountObj(ho_P2SelectedRegions_big1, &hv_P2DefectsNum2_big1);
        if (0 != (hv_P2DefectsNum2_big1<1))
        {
            SelectShape(ho_preConnectedRegions2, &ho_P2SelectedRegions_big2, "height",
          "and", hv_P2height_t, 50);
            SelectShape(ho_preConnectedRegions2, &ho_P2SelectedRegions_big2, "width", "and",
          hv_P2width_t, 20);
            DilationCircle(ho_P2SelectedRegions_big2, &ho_RegionDilation2_big2, 12.5);
            //union2 (SelectedRegions_notGood1, SelectedRegions_notGood2, UnionSelectedRegions_notGood)
            //connection (UnionSelectedRegions_notGood, SelectedRegions_notGood)
            CountObj(ho_P2SelectedRegions_big2, &hv_P2DefectsNum2_big2);
            hv_P2DefectsNum2_big = hv_P2DefectsNum2_big1+hv_P2DefectsNum2_big2;
            //Union good and notgood defects for displaying
            Union1(ho_RegionDilation2_small, &ho_RegionUnion2_small);
            Union2(ho_RegionDilation2_big1, ho_RegionDilation2_big2, &ho_RegionUnion2_big
                   );
            Union2(ho_RegionUnion2_small, ho_RegionUnion2_big, &ho_RegionUnion2);
        }
        else
        {
            hv_P2DefectsNum2_big = hv_P2DefectsNum2_big1;
            //Union good and notgood defects for displaying
            Union1(ho_RegionDilation2_small, &ho_RegionUnion2_small);
            Union1(ho_RegionDilation2_big1, &ho_RegionUnion2_big);
            Union2(ho_RegionUnion2_small, ho_RegionUnion2_big, &ho_RegionUnion2);
        }




        //display
        if (HDevWindowStack::IsOpen())
            DispObj(ho_Image, HDevWindowStack::GetActive());
        if (HDevWindowStack::IsOpen())
            DispObj(ho_RegionUnion1, HDevWindowStack::GetActive());
        if (HDevWindowStack::IsOpen())
            DispObj(ho_RegionUnion2, HDevWindowStack::GetActive());
        DumpWindowImage(&deal_image,hv_WindowHandle);
        //dumpWindow

        //**P3-- return result  ***
        //if black BIG defects contain
        if (0 != (hv_P1DefectsNum_big>0))
        {
            hv_ReturnIsOK = 0;
            hv_NumberNg += 1;
            return;
        }
        //if white BIG defects contain
        if (0 != (hv_P2DefectsNum2_big>0))
        {
            hv_ReturnIsOK = 0;
            hv_NumberNg += 1;
            return;
        }
        //if black SMALL defects contain
        if (0 != (hv_P1DefectsNum_small>hv_P1DefectsNum_small_t))
        {
            hv_ReturnIsOK = 1;
            hv_ReturnIsGood = 0;
            hv_NumberOkNotGood += 1;
            return;
        }
        //if white SMALL defects contain
        if (0 != (hv_P2DefectsNum_small>hv_P2DefectsNum_small_t))
        {
            hv_ReturnIsOK = 1;
            hv_ReturnIsGood = 0;
            hv_NumberOkNotGood += 1;
            return;
        }
        //if no : big and small defects
        hv_ReturnIsOK = 1;
        hv_ReturnIsGood = 1;
        hv_NumberGood += 1;



    }


    //* endfor
}


/************************************************************************
@return 1: is OK

@return 0: is not OK

 ************************************************************************/
int DefectsDetect::get_result()
{

    if((hv_ReturnIsOK==1)&&(hv_ReturnIsGood==1))
        _iResult = 2;

    if((hv_ReturnIsOK==1)&&(hv_ReturnIsGood==0))
        _iResult = 1;

    if (hv_ReturnIsOK==0)
        _iResult = 0;

    return _iResult;
}



int DefectsDetect::get_defectsType()
{
    if(hv_ScratchExist==1)
        _iType=0xF1;
    if (hv_WhitePointExist == 1)
        _iType = (0xF1 | 0xF2);
    if (hv_BlackPointExist==1)
        _iType = (0xF1 | 0xF4);

    return _iType;
}

void DefectsDetect::accept_run(HObject &object, HObject &deal_object, const int width, const int height, const Hlong &winid, QJsonObject params)
{
    set_Params(params["threshold_black"].toInt(), params["threshold_white"].toInt(), params["area_min_part1"].toDouble(),
               params["area_max_part1"].toDouble(), params["gradient_part1"].toDouble(),params["num_part1"].toInt(),
               params["area_min_part2"].toDouble(),params["area_max_part2"].toDouble(),params["gradient_part2"].toDouble(),
               params["defect_length_part2"].toDouble(), params["defect_width_part2"].toDouble(),params["num_part2"].toInt());

    run(object,deal_object,width,height,winid,0,0);
    emit tell_window_check_result(get_result(),get_defectsType(),deal_object);
}
