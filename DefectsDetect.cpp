#include "DefectsDetect.h"
#include <QDebug>

int ThresholdBlack = 34;
int ThresholdWhite = 16;

DefectsDetect::DefectsDetect()
{
    hv_thresholdBlack = 34;
    hv_thresholdWhite = 50;
}

DefectsDetect::~DefectsDetect()
{

}

void DefectsDetect::run(HObject &ho_Image, HObject &deal_image, const int width, const int height, const Hlong &winid, int x, int y)
{
    //filename: StickDetect-3classV3.5-withoutWaterMarkDetectV4.5.hdev
    //date:2019.8.1
    //Algorithm procedure see: 3classification.draw.io
    //inherit file: StickDetectV6

    //modify:

    //1. add condition for height selectshape in part2detect



    //上位机注意事项：
    //好良品:
    //returnIsOK =1;
    //returnIsGood=1;

    //一般良品：
    //returnIsOK=1；
    //returnIsGood=0;

    //不良品：
    //returnIsOK=0;

    //去掉了函数： get defectsType ();
    //getResult() 返回值bool改为int ;

    //窗口显示需要加在，每个//display defects之后,return之前

    //display 对象：
    //
    //RegionUnion1
    //RegionUnion2

    //initualization
//    dev_update_off();
    if (HDevWindowStack::IsOpen())
        CloseWindow(HDevWindowStack::Pop());

    //*********** statistic parameters***********
    hv_NumberNg = 0;
    hv_NumberGood = 0;
    hv_NumberOkNotGood = 0;

    //*********** statistic parameters***********


//    ReadImage(&ho_Image, "E:/WorkSpace/ImgData/磁锟样本/报废20190801/120054_NG.bmp");
    //list_files ('E:/WorkSpace/ImgData/磁锟样本/03--不良批次检测结果20190715', ['files','follow_links'], ImageFiles)
    //tuple_regexp_select (ImageFiles, ['\\.(tif|tiff|gif|bmp|jpg|jpeg|jp2|png|pcx|pgm|ppm|pbm|xwd|ima|hobj)$','ignore_case'], ImageFiles)
    //* for Index := 0 to |ImageFiles| - 1 by 1
    //read_image (Image, ImageFiles[Index])



    GetImageSize(ho_Image, &hv_Width, &hv_Height);
    SetWindowAttr("background_color","black");
    OpenWindow(0,0,width,height,winid,"visible","",&hv_mainWindowHandle);
    HDevWindowStack::Push(hv_mainWindowHandle);
    //set_display_font (mainWindowHandle, 14, 'mono', 'true', 'false')
    if (HDevWindowStack::IsOpen())
        SetDraw(HDevWindowStack::GetActive(),"margin");
    if (HDevWindowStack::IsOpen())
        SetLineWidth(HDevWindowStack::GetActive(),3);
    if (HDevWindowStack::IsOpen())
        SetColored(HDevWindowStack::GetActive(),12);


    //currentFiledir := ImageFiles[Index]
    //disp_message (mainWindowHandle, ImageFiles[Index], 'window', 60, 12, 'black', 'true')


    //*********** return parameters setting  ***********
    hv_ScratchExist = 0;
    hv_WhitePointExist = 0;
    hv_BlackPointExist = 0;
    hv_ReturnIsOK = 0;
    hv_ReturnIsGood = 0;
    //*********** return parameters setting ***********
    //*********** global parameters ***********
    hv_medianDegree = 5;
    hv_thresholdBlack = 14;
    hv_smoothDegree = 1.5;
    hv_thresholdWhite = 30;
    //default=36.047
    hv_areaSelect_good = 54.5;
    hv_areaSelect_notGood = 85;
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



        //Image process procedure-1 ,especially for black or bigger size defects
        MedianImage(ho_Image, &ho_ImageMedian, "circle", hv_medianDegree, "mirrored");
        //invert_image (ImageMedian, ImageInvert)
        KirschAmp(ho_ImageMedian, &ho_ImageEdgeAmp1);
        Threshold(ho_ImageEdgeAmp1, &ho_Regions1, hv_thresholdBlack, 255);
        DilationCircle(ho_Regions1, &ho_RegionDilation1, 12.5);
        Connection(ho_RegionDilation1, &ho_ConnectedRegions1);
        SelectGray(ho_ConnectedRegions1, ho_Image, &ho_Part1SelectedRegion0, "mean",
        "and", 0, 54.9);
        SelectGray(ho_ConnectedRegions1, ho_Image, &ho_Part1SelectedRegion1, "deviation",
        "and", 3.95, 5);
        SelectShape(ho_Part1SelectedRegion1, &ho_Part1SelectedRegion2, "area", "and",
        1100, 5000);
        Union2(ho_Part1SelectedRegion0, ho_Part1SelectedRegion2, &ho_RegionUnionPre);
        Union1(ho_RegionUnionPre, &ho_RegionUnion1);
        Connection(ho_RegionUnion1, &ho_Part1SelectedRegions);
        CountObj(ho_Part1SelectedRegions, &hv_Part1DefectsNum);

        //if black defects contain
        if (0 != (hv_Part1DefectsNum>0))
        {
            hv_ReturnIsOK = 0;
            hv_NumberNg += 1;
            //stop ()
            //display defects
            if (HDevWindowStack::IsOpen())
                DispObj(ho_Image, HDevWindowStack::GetActive());
            if (HDevWindowStack::IsOpen())
                DispObj(ho_RegionUnion1, HDevWindowStack::GetActive());
            DumpWindowImage(&deal_image,hv_mainWindowHandle);
            return;
        }


        //Image process  procedure-2 ,especially for white or small size defects
        SmoothImage(ho_Image, &ho_ImageSmooth, "gauss", hv_smoothDegree);
        KirschAmp(ho_ImageSmooth, &ho_ImageEdgeAmp2);
        Threshold(ho_ImageEdgeAmp2, &ho_Regions2, hv_thresholdWhite, 255);
        Connection(ho_Regions2, &ho_preConnectedRegions2);
        //good condition Area select
        SelectShape(ho_preConnectedRegions2, &ho_SelectedRegions_good, "area", "and",
        hv_areaSelect_good, 9999.615);
        DilationCircle(ho_SelectedRegions_good, &ho_RegionDilation2_good, 12.5);
        CountObj(ho_SelectedRegions_good, &hv_Part2DefectsNum2_good);
        //notGood condition Area select
        SelectShape(ho_preConnectedRegions2, &ho_SelectedRegions_notGood1, "area", "and",
        hv_areaSelect_notGood, 9999.615);
        DilationCircle(ho_SelectedRegions_notGood1, &ho_RegionDilation2_notGood1, 12.5);
        CountObj(ho_SelectedRegions_notGood1, &hv_Part2DefectsNum2_notGood1);
        if (0 != (hv_Part2DefectsNum2_notGood1<1))
        {
            SelectShape(ho_preConnectedRegions2, &ho_SelectedRegions_notGood2, "height",
          "and", 10.518, 50);
            DilationCircle(ho_SelectedRegions_notGood2, &ho_RegionDilation2_notGood2, 12.5);
            //union2 (SelectedRegions_notGood1, SelectedRegions_notGood2, UnionSelectedRegions_notGood)
            //connection (UnionSelectedRegions_notGood, SelectedRegions_notGood)
            CountObj(ho_SelectedRegions_notGood2, &hv_Part2DefectsNum2_notGood2);
            hv_Part2DefectsNum2_notGood = hv_Part2DefectsNum2_notGood1+hv_Part2DefectsNum2_notGood2;
            //Union good and notgood defects for displaying
            Union1(ho_RegionDilation2_good, &ho_RegionUnion2_good);
            Union2(ho_RegionDilation2_notGood1, ho_RegionDilation2_notGood2, &ho_RegionUnion2_notGood
                   );
            Union2(ho_RegionUnion2_good, ho_RegionUnion2_notGood, &ho_RegionUnion2);
        }
        else
        {
            hv_Part2DefectsNum2_notGood = hv_Part2DefectsNum2_notGood1;

            //Union good and notgood defects for displaying
            Union1(ho_RegionDilation2_good, &ho_RegionUnion2_good);
            Union1(ho_RegionDilation2_notGood1, &ho_RegionUnion2_notGood);
            Union2(ho_RegionUnion2_good, ho_RegionUnion2_notGood, &ho_RegionUnion2);

        }






        //if white defects not contain
        if (0 != (hv_Part2DefectsNum2_good<=2))
        {
            //start :water mark detect
            //gen_rectangle2 (waterRectangle, Height/2, Width/2, 0, Width-2000, Height/5-50)
            //reduce_domain (Image, waterRectangle, waterImageReduced)
            //crop_domain (waterImageReduced, waterImagePart)
            //intensity (waterRectangle, waterImageReduced, Mean_Value, deviation_Value)
            //if water mark contain
            //if (deviation_Value>4.97)
            //NumberNg := NumberNg+1
            //ReturnIsOK := 0
            //*             continue
            //water mark not contain
            //else
            hv_NumberGood += 1;
            hv_ReturnIsOK = 1;
            hv_ReturnIsGood = 1;
            //display defects
            if (HDevWindowStack::IsOpen())
                DispObj(ho_Image, HDevWindowStack::GetActive());
            DumpWindowImage(&deal_image,hv_mainWindowHandle);
            return;
            //endif
            //if white defects contain
        }
        else if (0 != (hv_Part2DefectsNum2_good>2))
        {
            //if Big Defects contain
            if (0 != (hv_Part2DefectsNum2_notGood>=1))
            {
                hv_ReturnIsOK = 0;
                hv_NumberNg += 1;
                //stop ()
                //display defects
                if (HDevWindowStack::IsOpen())
                    DispObj(ho_Image, HDevWindowStack::GetActive());
                if (HDevWindowStack::IsOpen())
                    DispObj(ho_RegionUnion1, HDevWindowStack::GetActive());
                if (HDevWindowStack::IsOpen())
                    DispObj(ho_RegionUnion2, HDevWindowStack::GetActive());
                DumpWindowImage(&deal_image,hv_mainWindowHandle);
                return;
                //if Big Defects not contain
            }
            else
            {
                //gen_rectangle2 (waterRectangle, Height/2, Width/2, 0, Width-2000, Height/5-50)
                //intensity (waterRectangle, ImageMedian, Mean_Value, deviation_Value)
                //if water mark contain
                //if (deviation_Value>4.92)
                //ReturnIsOK := 0
                //NumberNg := NumberNg+1
                //stop ()
                //*                 continue
                //if water mark not contain
                //else
                hv_ReturnIsOK = 1;
                hv_ReturnIsGood = 0;
                hv_NumberOkNotGood += 1;
                if (HDevWindowStack::IsOpen())
                    DispObj(ho_Image, HDevWindowStack::GetActive());
                if (HDevWindowStack::IsOpen())
                    DispObj(ho_RegionUnion1, HDevWindowStack::GetActive());
                if (HDevWindowStack::IsOpen())
                    DispObj(ho_RegionUnion2, HDevWindowStack::GetActive());
                DumpWindowImage(&deal_image,hv_mainWindowHandle);
                //endif
            }

        }
    }
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

void DefectsDetect::accept_run(HObject &object,HObject &deal_object, const int width, const int height, const Hlong &winid)
{
    qDebug() << "run";
    hv_thresholdBlack=ThresholdBlack;
    hv_thresholdWhite=ThresholdWhite;
    run(object,deal_object,width,height,winid,0,0);
    emit tell_window_check_result(get_result(),get_defectsType(),deal_object);
}
