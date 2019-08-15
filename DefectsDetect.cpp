#include "DefectsDetect.h"
#include <QDebug>


void estimate_background_illumination (HObject ho_Image, HObject *ho_IlluminationImage)
{

    // Local iconic variables
    HObject  ho_ImageFFT, ho_ImageGauss, ho_ImageConvol;

    // Local control variables
    HTuple  hv_Width, hv_Height;

    GetImageSize(ho_Image, &hv_Width, &hv_Height);
    RftGeneric(ho_Image, &ho_ImageFFT, "to_freq", "none", "complex", hv_Width);
    GenGaussFilter(&ho_ImageGauss, 50, 50, 0, "n", "rft", hv_Width, hv_Height);
    ConvolFft(ho_ImageFFT, ho_ImageGauss, &ho_ImageConvol);
    RftGeneric(ho_ImageConvol, &(*ho_IlluminationImage), "from_freq", "none", "byte",
      hv_Width);
    return;
}
void preprocess_dl_classifier_images(HObject ho_Images, HObject *ho_ImagesPreprocessed,HTuple hv_GenParamName, HTuple hv_GenParamValue, HTuple hv_DLClassifierHandle)
{

    // Local iconic variables
    HObject  ho_ObjectSelected, ho_ThreeChannelImage;
    HObject  ho_SingleChannelImage;

    // Local control variables
    HTuple  hv_ContrastNormalization, hv_DomainHandling;
    HTuple  hv_GenParamIndex, hv_ImageWidth, hv_ImageHeight;
    HTuple  hv_ImageRangeMin, hv_ImageRangeMax, hv_ImageNumChannels;
    HTuple  hv_ImageWidthInput, hv_ImageHeightInput, hv_EqualWidth;
    HTuple  hv_EqualHeight, hv_Type, hv_NumMatches, hv_NumImages;
    HTuple  hv_EqualByte, hv_RescaleRange, hv_ImageIndex, hv_NumChannels;

    //This procedure preprocesses the provided images given by Image
    //so that they can be handled by
    //train_dl_classifier_batch and apply_dl_classifier_batch.
    //Note that depending on the images,
    //additional preprocessing steps might be beneficial.
    //
    //Set defaults.
    hv_ContrastNormalization = "false";
    hv_DomainHandling = "full_domain";
    //Set generic parameters.
    {
        HTuple end_val10 = (hv_GenParamName.TupleLength()) - 1;
        HTuple step_val10 = 1;
        for (hv_GenParamIndex = 0; hv_GenParamIndex.Continue(end_val10, step_val10); hv_GenParamIndex += step_val10)
        {
            if (0 != (HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("contrast_normalization")))
            {
                //Set 'contrast_normalization'
                hv_ContrastNormalization = HTuple(hv_GenParamValue[hv_GenParamIndex]);
            }
            else if (0 != (HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("domain_handling")))
            {
                //Set 'domain_handling'
                hv_DomainHandling = HTuple(hv_GenParamValue[hv_GenParamIndex]);
            }
            else
            {
                throw HException(("Unknown generic parameter: '" + HTuple(hv_GenParamName[hv_GenParamIndex])) + "'");
            }
        }
    }
    //
    //Get the network's image requirements
    //from the handle of the classifier
    //and use them as preprocessing parameters.
    //
    //Expected input image size:
    GetDlClassifierParam(hv_DLClassifierHandle, "image_width", &hv_ImageWidth);
    GetDlClassifierParam(hv_DLClassifierHandle, "image_height", &hv_ImageHeight);
    //Expected gray value range:
    GetDlClassifierParam(hv_DLClassifierHandle, "image_range_min", &hv_ImageRangeMin);
    GetDlClassifierParam(hv_DLClassifierHandle, "image_range_max", &hv_ImageRangeMax);
    //Expected number of channels:
    GetDlClassifierParam(hv_DLClassifierHandle, "image_num_channels", &hv_ImageNumChannels);
    //
    //Preprocess the images.
    //
    if (0 != (hv_DomainHandling == HTuple("full_domain")))
    {
        FullDomain(ho_Images, &ho_Images);
    }
    else if (0 != (hv_DomainHandling == HTuple("crop_domain")))
    {
        CropDomain(ho_Images, &ho_Images);
    }
    else
    {
        throw HException("Unsupported parameter value for 'domain_handling'");
    }
    //
    //Zoom images only if they have a different size than the specified size
    GetImageSize(ho_Images, &hv_ImageWidthInput, &hv_ImageHeightInput);
    hv_EqualWidth = hv_ImageWidth.TupleEqualElem(hv_ImageWidthInput);
    hv_EqualHeight = hv_ImageHeight.TupleEqualElem(hv_ImageHeightInput);
    if (0 != (HTuple((hv_EqualWidth.TupleMin()) == 0).TupleOr((hv_EqualHeight.TupleMin()) == 0)))
    {
        ZoomImageSize(ho_Images, &ho_Images, hv_ImageWidth, hv_ImageHeight, "constant");
    }
    if (0 != (hv_ContrastNormalization == HTuple("true")))
    {
        //Scale the gray values to [0-255].
        //Note that this converts the image to 'byte'.
        ScaleImageMax(ho_Images, &ho_Images);
    }
    else if (0 != (hv_ContrastNormalization != HTuple("false")))
    {
        throw HException("Unsupported parameter value for 'contrast_normalization'");
    }
    //Check the type of the input images.
    //If the type is not 'byte',
    //the gray value scaling does not work correctly.
    GetImageType(ho_Images, &hv_Type);
    TupleRegexpTest(hv_Type, "byte|real", &hv_NumMatches);
    CountObj(ho_Images, &hv_NumImages);
    if (0 != (hv_NumMatches != hv_NumImages))
    {
        throw HException("Please provide only images of type 'byte' or 'real'.");
    }
    hv_EqualByte = hv_Type.TupleEqualElem("byte");
    if (0 != ((hv_EqualByte.TupleMax()) == 1))
    {
        if (0 != ((hv_EqualByte.TupleMin()) == 0))
        {
            throw HException("Passing mixed type images is not supported.");
        }
        //Convert the image type from byte to real,
        //because the classifier expects 'real' images.
        ConvertImageType(ho_Images, &ho_Images, "real");
        //Scale/Shift the gray values from [0-255] to the expected range.
        hv_RescaleRange = (hv_ImageRangeMax - hv_ImageRangeMin) / 255.0;
        ScaleImage(ho_Images, &ho_Images, hv_RescaleRange, hv_ImageRangeMin);
    }
    else
    {
        //For real images it is assumed that the range is already correct
    }

    //Check the number of channels.
    CountObj(ho_Images, &hv_NumImages);
    {
        HTuple end_val85 = hv_NumImages;
        HTuple step_val85 = 1;
        for (hv_ImageIndex = 1; hv_ImageIndex.Continue(end_val85, step_val85); hv_ImageIndex += step_val85)
        {
            SelectObj(ho_Images, &ho_ObjectSelected, hv_ImageIndex);
            CountChannels(ho_ObjectSelected, &hv_NumChannels);
            if (0 != (hv_NumChannels != hv_ImageNumChannels))
            {
                //
                if (0 != (HTuple(hv_NumChannels == 1).TupleAnd(hv_ImageNumChannels == 3)))
                {
                    //If the image is a grayscale image, but the classifier expects a color image:
                    //convert it to an image with three channels.
                    Compose3(ho_ObjectSelected, ho_ObjectSelected, ho_ObjectSelected, &ho_ThreeChannelImage
                             );
                    ReplaceObj(ho_Images, ho_ThreeChannelImage, &ho_Images, hv_ImageIndex);
                }
                else if (0 != (HTuple(hv_NumChannels == 3).TupleAnd(hv_ImageNumChannels == 1)))
                {
                    //If the image is a color image, but the classifier expects a grayscale image:
                    //convert it to an image with only one channel.
                    Rgb1ToGray(ho_ObjectSelected, &ho_SingleChannelImage);
                    ReplaceObj(ho_Images, ho_SingleChannelImage, &ho_Images, hv_ImageIndex);
                }
                else
                {
                    throw HException("Number of channels not supported. Please provide a grayscale or an RGB image.");
                }
                //
            }
        }
    }
    (*ho_ImagesPreprocessed) = ho_Images;
    return;
}

// Chapter: Graphics / Text
// Short Description: Set font independent of OS
void set_display_font(HTuple hv_WindowHandle, HTuple hv_Size, HTuple hv_Font, HTuple hv_Bold,HTuple hv_Slant)
{

    // Local iconic variables

    // Local control variables
    HTuple  hv_OS, hv_Fonts, hv_Style, hv_Exception;
    HTuple  hv_AvailableFonts, hv_Fdx, hv_Indices;

    //This procedure sets the text font of the current window with
    //the specified attributes.
    //
    //Input parameters:
    //WindowHandle: The graphics window for which the font will be set
    //Size: The font size. If Size=-1, the default of 16 is used.
    //Bold: If set to 'true', a bold font is used
    //Slant: If set to 'true', a slanted font is used
    //
    GetSystem("operating_system", &hv_OS);
    if (0 != (HTuple(hv_Size == HTuple()).TupleOr(hv_Size == -1)))
    {
        hv_Size = 16;
    }
    if (0 != ((hv_OS.TupleSubstr(0, 2)) == HTuple("Win")))
    {
        //Restore previous behaviour
        hv_Size = (1.13677*hv_Size).TupleInt();
    }
    else
    {
        hv_Size = hv_Size.TupleInt();
    }
    if (0 != (hv_Font == HTuple("Courier")))
    {
        hv_Fonts.Clear();
        hv_Fonts[0] = "Courier";
        hv_Fonts[1] = "Courier 10 Pitch";
        hv_Fonts[2] = "Courier New";
        hv_Fonts[3] = "CourierNew";
        hv_Fonts[4] = "Liberation Mono";
    }
    else if (0 != (hv_Font == HTuple("mono")))
    {
        hv_Fonts.Clear();
        hv_Fonts[0] = "Consolas";
        hv_Fonts[1] = "Menlo";
        hv_Fonts[2] = "Courier";
        hv_Fonts[3] = "Courier 10 Pitch";
        hv_Fonts[4] = "FreeMono";
        hv_Fonts[5] = "Liberation Mono";
    }
    else if (0 != (hv_Font == HTuple("sans")))
    {
        hv_Fonts.Clear();
        hv_Fonts[0] = "Luxi Sans";
        hv_Fonts[1] = "DejaVu Sans";
        hv_Fonts[2] = "FreeSans";
        hv_Fonts[3] = "Arial";
        hv_Fonts[4] = "Liberation Sans";
    }
    else if (0 != (hv_Font == HTuple("serif")))
    {
        hv_Fonts.Clear();
        hv_Fonts[0] = "Times New Roman";
        hv_Fonts[1] = "Luxi Serif";
        hv_Fonts[2] = "DejaVu Serif";
        hv_Fonts[3] = "FreeSerif";
        hv_Fonts[4] = "Utopia";
        hv_Fonts[5] = "Liberation Serif";
    }
    else
    {
        hv_Fonts = hv_Font;
    }
    hv_Style = "";
    if (0 != (hv_Bold == HTuple("true")))
    {
        hv_Style += HTuple("Bold");
    }
    else if (0 != (hv_Bold != HTuple("false")))
    {
        hv_Exception = "Wrong value of control parameter Bold";
        throw HException(hv_Exception);
    }
    if (0 != (hv_Slant == HTuple("true")))
    {
        hv_Style += HTuple("Italic");
    }
    else if (0 != (hv_Slant != HTuple("false")))
    {
        hv_Exception = "Wrong value of control parameter Slant";
        throw HException(hv_Exception);
    }
    if (0 != (hv_Style == HTuple("")))
    {
        hv_Style = "Normal";
    }
    QueryFont(hv_WindowHandle, &hv_AvailableFonts);
    hv_Font = "";
    {
        HTuple end_val48 = (hv_Fonts.TupleLength()) - 1;
        HTuple step_val48 = 1;
        for (hv_Fdx = 0; hv_Fdx.Continue(end_val48, step_val48); hv_Fdx += step_val48)
        {
            hv_Indices = hv_AvailableFonts.TupleFind(HTuple(hv_Fonts[hv_Fdx]));
            if (0 != ((hv_Indices.TupleLength())>0))
            {
                if (0 != (HTuple(hv_Indices[0]) >= 0))
                {
                    hv_Font = HTuple(hv_Fonts[hv_Fdx]);
                    break;
                }
            }
        }
    }
    if (0 != (hv_Font == HTuple("")))
    {
        throw HException("Wrong value of control parameter Font");
    }
    hv_Font = (((hv_Font + "-") + hv_Style) + "-") + hv_Size;
    SetFont(hv_WindowHandle, hv_Font);
    return;
}

// Local procedures
void preprocess_dl_waterMark_example(HObject ho_Image, HObject *ho_ImagePreprocessed,HTuple hv_DLClassifierHandle)
{	// Local iconic variables
    HObject  ho_ImagePart, ho_ImageIlluminate, ho_ImageEquHisto;
    HObject  ho_ImageMedian;

    // Local control variables
    HTuple  hv_Width, hv_Height;

    //for DL preprocess image
    GetImageSize(ho_Image, &hv_Width, &hv_Height);
    CropDomainRel(ho_Image, &ho_ImagePart, hv_Height / 4, 100, hv_Height / 4, 100);
    Illuminate(ho_ImagePart, &ho_ImageIlluminate, 1000, 1000, 0.9);
    EquHistoImage(ho_ImageIlluminate, &ho_ImageEquHisto);
    MedianImage(ho_ImageEquHisto, &ho_ImageMedian, "circle", 1, "mirrored");

    preprocess_dl_classifier_images(ho_ImageMedian, &(*ho_ImagePreprocessed), "contrast_normalization",
                                    "true", hv_DLClassifierHandle);
    return;
}


DefectsDetect::DefectsDetect()
{

}

DefectsDetect::~DefectsDetect()
{

}

void DefectsDetect::set_Params(int ThresholdBlack , int ThresholdWhite ,float P1areaSelect_t_low ,float P1areaSelect_t_high ,float P1deviation_t_big  , int P1DefectsNum_small_t,
                               float P2areaSelect_t_low ,float P2areaSelect_t_high ,float P2deviation_t_big ,float P2height_t ,float P2width_t ,int P2DefectsNum_small_t ,float confidence_Contain,
                               float confidence_notContain )
{

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
    hv_confidence_Contain = confidence_Contain;
    hv_confidence_notContain = confidence_notContain ;
}




void DefectsDetect::run(HObject ho_Image, HObject deal_image, const int width, const int height, const Hlong &winid, int x, int y)
{

    //filename: StickDetect-3class--new--2019年8月14.hdev



    //release CONVENTION *
    //1. continue to return ()
    //2. display to
    //dev_display (Image)
    //dev_display (RegionUnion1)
    //dev_display (RegionUnion2)
    //3.
    //currentFiledir := ImageFiles[Index]
    //disp_message (mainWindowHandle, ImageFiles[Index], 'window', 60, 12, 'black', 'true')

    //4.
    //initualization
    //dev_update_off ()
    //dev_close_window ()

    //***上位机注意事项：****
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




    //initualization
    //dev_update_off ()
    //dev_close_window ()
    SetWindowAttr("background_color","black");
    OpenWindow(0,0,width,height,winid,"visible","",&hv_WindowHandle);
    HDevWindowStack::Push(hv_WindowHandle);
    set_display_font(hv_WindowHandle, 20, "sans", "false", "false");



    //*********** statistic parameters***********
    hv_NumberNg = 0;
    hv_NumberGood = 0;
    hv_NumberOkNotGood = 0;
    //*********** statistic parameters***********




    //*********** init deep learning ***********
    ReadDlClassifier("classifier_waterMark--2019-08-13_172906.hdl", &hv_DLClassifierHandle);
    SetDlClassifierParam(hv_DLClassifierHandle, "batch_size", 1);
    SetDlClassifierParam(hv_DLClassifierHandle, "runtime", "cpu");
    //*********** init deep learning ***********


    //ReadImage(&ho_Image, //'E:/WorkSpace/ImgData/磁锟样本/02--20190802低端/171305_NG.bmp'
    // "NG_152028.bmp");
    //list_files ('E:/WorkSpace/ImgData/磁锟样本/手印样本-20190715', ['files','follow_links'], ImageFiles)

    //list_files ('E:/WorkSpace/ImgData/磁锟样本/51--总和--良品', ['files','follow_links'], ImageFiles)
    //list_files ('E:/WorkSpace/ImgData/磁锟样本/52--总和--次品', ['files','follow_links'], ImageFiles)
    //list_files ('E:/WorkSpace/ImgData/磁锟样本/53--总和--废品', ['files','follow_links'], ImageFiles)
    //tuple_regexp_select (ImageFiles, ['\\.(tif|tiff|gif|bmp|jpg|jpeg|jp2|png|pcx|pgm|ppm|pbm|xwd|ima|hobj)$','ignore_case'], ImageFiles)
    //* for Index := 0 to |ImageFiles| - 1 by 1
    //*     read_image (Image, ImageFiles[Index])



    GetImageSize(ho_Image, &hv_Width, &hv_Height);
    //set_display_font (mainWindowHandle, 14, 'mono', 'true', 'false')
    if (HDevWindowStack::IsOpen())
        SetDraw(HDevWindowStack::GetActive(),"margin");
    if (HDevWindowStack::IsOpen())
        SetLineWidth(HDevWindowStack::GetActive(),3);
    if (HDevWindowStack::IsOpen())
        SetColored(HDevWindowStack::GetActive(),12);


    //* currentFiledir:=ImageFiles[Index]
    //* disp_message (WindowHandle, ImageFiles[Index], 'window', 60, 12, 'black', 'true')
  //dstFiledir := 'E:/WorkSpace/ImgData/磁锟样本/漏检B--2019年8月14日/'

    //*********** return parameters setting  ***********
    hv_ScratchExist = 0;
    hv_WhitePointExist = 0;
    hv_BlackPointExist = 0;
    hv_ReturnIsOK = 0;
    hv_ReturnIsGood = 0;
    //*********** return parameters setting ***********
    //*********** global parameters ***********
    hv_medianDegree = 1;
    hv_smoothDegree = 1;

    //P1
    //hv_thresholdBlack = 36;
    //hv_P1areaSelect_t_low = 18;
    //hv_P1areaSelect_t_high = 22;
    //hv_P1deviation_t_big = 0.0;
    //hv_P1DefectsNum_small_t = 50;


    //P2
    //hv_thresholdWhite = 29;
    //P2areaSelect_t_low := 34.5
    //P2areaSelect_t_high := 54
    //hv_P2areaSelect_t_low = 16;
    //hv_P2areaSelect_t_high = 53;
    //hv_P2deviation_t_big = 6;
    //hv_P2height_t = 13.518;
    //hv_P2width_t = 13.016;
    //hv_P2DefectsNum_small_t = 50;


    //DL
    //hv_confidence_Contain = 0.9998;
    //hv_confidence_notContain = 0.83;

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
        0, 63.601);
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
        "and", hv_P1deviation_t_big, 100);
        DilationCircle(ho_P1SelectedRegions_big, &ho_P1RegionDilation2, 12.5);
        CountObj(ho_P1SelectedRegions_big, &hv_P1DefectsNum_big);
        Union1(ho_P1RegionDilation2, &ho_P1RegionUnion2_big);

        //union2 (P1RegionUnion1_small, P1RegionUnion2_big, RegionUnion1)
        Union1(ho_P1RegionUnion2_big, &ho_RegionUnion1);



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
        "and", hv_P2deviation_t_big, 100);
        DilationCircle(ho_P2SelectedRegions_big1, &ho_RegionDilation2_big1, 12.5);
        CountObj(ho_P2SelectedRegions_big1, &hv_P2DefectsNum2_big1);
        if (0 != (hv_P2DefectsNum2_big1<1))
        {
            SelectShape(ho_preConnectedRegions2, &ho_P2SelectedRegions_big2_pre, "height",
          "and", hv_P2height_t, 50);
            SelectGray(ho_P2SelectedRegions_big2_pre, ho_Image, &ho_P2SelectedRegions_big2,
          "deviation", "and", 4.7, 50);
            DilationCircle(ho_P2SelectedRegions_big2, &ho_RegionDilation2_big2, 12.5);
            CountObj(ho_P2SelectedRegions_big2, &hv_P2DefectsNum2_big2);

            SelectShape(ho_preConnectedRegions2, &ho_P2SelectedRegions_big3_pre, "width",
          "and", hv_P2width_t, 50);
            SelectGray(ho_P2SelectedRegions_big3_pre, ho_Image, &ho_P2SelectedRegions_big3,
          "deviation", "and", 5.9, 50);
            DilationCircle(ho_P2SelectedRegions_big3, &ho_RegionDilation2_big3, 12.5);
            CountObj(ho_P2SelectedRegions_big3, &hv_P2DefectsNum2_big3);
            hv_P2DefectsNum2_big = (hv_P2DefectsNum2_big1+hv_P2DefectsNum2_big2)+hv_P2DefectsNum2_big3;
            //Union good and notgood defects for displaying
            Union2(ho_RegionDilation2_big1, ho_RegionDilation2_big2, &ho_RegionUnion2_pre
                   );
            Union2(ho_RegionUnion2_pre, ho_RegionDilation2_big3, &ho_RegionUnion2);
        }
        else
        {
            hv_P2DefectsNum2_big = hv_P2DefectsNum2_big1;
            //Union good and notgood defects for displaying
            Union1(ho_RegionDilation2_small, &ho_RegionUnion2_small);
            Union1(ho_RegionDilation2_big1, &ho_RegionUnion2_big);
            Union1(ho_RegionUnion2_big, &ho_RegionUnion2);
        }




        //display
        if (HDevWindowStack::IsOpen())
            DispObj(ho_Image, HDevWindowStack::GetActive());
        if (HDevWindowStack::IsOpen())
            DispObj(ho_RegionUnion1, HDevWindowStack::GetActive());
        if (HDevWindowStack::IsOpen())
            DispObj(ho_RegionUnion2, HDevWindowStack::GetActive());

        DumpWindowImage(&deal_image,hv_WindowHandle);
        //dumpWindows



        //**P3-- return result  ***
        //if black BIG defects contain
        if (0 != (hv_P1DefectsNum_big>0))
        {
            hv_ReturnIsOK = 0;
            hv_NumberNg += 1;
            //copy_file (currentFiledir, dstFiledir+Index+'.bmp')

            return;
        }
        //if white BIG defects contain
        if (0 != (hv_P2DefectsNum2_big>0))
        {
            hv_ReturnIsOK = 0;
            hv_NumberNg += 1;
            //copy_file (currentFiledir, dstFiledir+Index+'.bmp')
            return;
        }



        //apply deeplearning classifier *
        preprocess_dl_waterMark_example(ho_Image, &ho_ImagePreprocessed, hv_DLClassifierHandle);
        ApplyDlClassifier(ho_ImagePreprocessed, hv_DLClassifierHandle, &hv_DLClassifierResultHandle);
        GetDlClassifierResult(hv_DLClassifierResultHandle, "all", "predicted_classes",
        &hv_PredictedClass);
        GetDlClassifierResult(hv_DLClassifierResultHandle, "all", "confidences", &hv_confidence);
        ClearDlClassifierResult(hv_DLClassifierResultHandle);



        if (0 != (HTuple(hv_PredictedClass==HTuple("WMContain")).TupleAnd(hv_confidence>hv_confidence_Contain)))
        {
            hv_Text = //'存在白印、黑印'
                "WaterMarkContain";
            if (HDevWindowStack::IsOpen())
                DispText(HDevWindowStack::GetActive(),hv_Text, "window", "top", "left", "red",
            "box", "true");
            hv_ReturnIsOK = 0;
            hv_NumberNg += 1;
            DumpWindowImage(&deal_image,hv_WindowHandle);
            //dumpWindows
            return;
        }
        else if (0 != (HTuple(hv_PredictedClass==HTuple("WMnotContain")).TupleAnd(hv_confidence>hv_confidence_notContain)))
        {

            //Text := 'Predicted class: ' + PredictedClass+'  '  +confidence
            //dev_disp_text (Text, 'window', 'top', 'left', 'white', 'box', 'false')
            hv_ReturnIsOK = 1;
            hv_ReturnIsGood = 1;
            hv_NumberGood += 1;

            return;
        }
        else
        {
            //Text := 'Predicted class: ' + PredictedClass+'  '  +confidence
            //dev_disp_text (Text, 'window', 'top', 'left', 'white', 'box', 'false')
            hv_ReturnIsOK = 1;
            hv_ReturnIsGood = 0;
            hv_NumberOkNotGood += 1;
            //copy_file (currentFiledir, dstFiledir+Index+'.bmp')
            return;
        }


    }


    //* endfor
}

int DefectsDetect::run_rujie(HObject ho_Image, HObject deal_image, const int width, const int height, const Hlong &winid)
{
    // Local iconic variables
    HObject  ho_Image0, ho_GrayImage, ho_Rectangle0;
    HObject  ho_ImageReduced0, ho_ImageFFT1, ho_ImageSub, ho_Image1;
    HObject  ho_Rectangle1, ho_ImageReduced1, ho_Image2, ho_Rectangle2;
    HObject  ho_ImageReduced2;

    // Local control variables
    HTuple  hv_saveimage, hv_saveFile0, hv_studyFile0;
    HTuple  hv_studyFile1, hv_studyFile2, hv_train, hv_writemlp;
    HTuple  hv_readmlp, hv_test, hv_numbleOK, hv_numbleNG, hv_numbleLightOK;
    HTuple  hv_numbleLightNG, hv_redlight, hv_greenlight, hv_Index112;
    HTuple  hv_name, hv_datanumber, hv_Classes, hv_inspectOnLine;
    HTuple  hv_medianDegree, hv_MLPHandle, hv_ImageFiles, hv_Index;
    HTuple  hv_Width0, hv_Height0, hv_WindowHandle0, hv_Energy;
    HTuple  hv_Correlation, hv_Homogeneity, hv_Contrast, hv_AbsoluteHisto;
    HTuple  hv_FeaturesExtended, hv_NumFeatures, hv_FeatureVector;
    HTuple  hv_Result, hv_ImageFiles1, hv_Index1, hv_Width1;
    HTuple  hv_Height1, hv_WindowHandle1, hv_Energy1, hv_Correlation1;
    HTuple  hv_Homogeneity1, hv_Contrast1, hv_AbsoluteHisto1;
    HTuple  hv_FeaturesExtended1, hv_NumFeatures1, hv_FeatureVector1;
    HTuple  hv_Result1, hv_ImageFiles2, hv_Width2, hv_Height2;
    HTuple  hv_Result2, hv_Error, hv_ErrorLog, hv_FileHandle;
    HTuple  hv_WindowHandle2, hv_Energy2, hv_Correlation2, hv_Homogeneity2;
    HTuple  hv_Contrast2, hv_AbsoluteHisto2, hv_FeaturesExtended112;
    HTuple  hv_NumFeatures112, hv_FeatureVector112, hv_Class112;
    HTuple  hv_Confidence112;

    //Image Acquisition 01: Code generated by Image Acquisition 01
    //Image Acquisition 01: be set in a specific order (e.g., image resolution vs. offset).


    //保存图片使能
    hv_saveimage = 0;
    //保存图片到文件0
    hv_saveFile0 = 0;
    //学习文件0内样本
    hv_studyFile0 = 1;
    //学习文件1内样本
    hv_studyFile1 = 1;
    //学习文件2内样本
    hv_studyFile2 = 1;
    hv_train = 1;
    //保存训练结果
    hv_writemlp = 0;
    //读取训练结果
    hv_readmlp = 0;
    //报告表编号
    hv_test = 1;

    hv_numbleOK = 0;
    hv_numbleNG = 0;
    hv_numbleLightOK = 0;
    hv_numbleLightNG = 0;
    hv_redlight = 0;
    hv_greenlight = 0;
    hv_Index112 = 0;

    hv_name = 0;
    hv_datanumber = 0;
    hv_Classes.Clear();
    hv_Classes[0] = "OK";
    hv_Classes[1] = "NG";
    hv_Classes[2] = "notOK";
    hv_inspectOnLine = 0;


    hv_medianDegree = 1.5;


    if (0 != hv_test)
    {
            qDebug() << "start";
        ReadClassMlp("D:/StickDetect.gmc", &hv_MLPHandle);
        {
//            ReadImage(&ho_Image2, HTuple(hv_ImageFiles[hv_Index]));
            ho_Image2 = ho_Image;
            //Image Acquisition 01: Do something
            GetImageSize(ho_Image2, &hv_Width2, &hv_Height2);
            if (HDevWindowStack::IsOpen())
                CloseWindow(HDevWindowStack::Pop());
            SetWindowAttr("background_color","black");
            OpenWindow(0,0,width,height,winid,"","",&hv_WindowHandle2);
            HDevWindowStack::Push(hv_WindowHandle2);
            if (HDevWindowStack::IsOpen())
                SetPart(HDevWindowStack::GetActive(),0, 0, hv_Height2, hv_Width2);
            if (HDevWindowStack::IsOpen())
                DispObj(ho_Image2, HDevWindowStack::GetActive());
            //disp_message (WindowHandle2, '带分辨', 'window', 10, 10, 'black', 'true')
            //draw_rectangle1 (WindowHandle2, Row2, Column2, Row21, Column21)
            //gen_rectangle1 (Rectangle2, Row2, Column2, Row21, Column21)

            GenRectangle1(&ho_Rectangle2, 1, 1, hv_Height2-2, hv_Width2-2);
            ReduceDomain(ho_Image2, ho_Rectangle2, &ho_ImageReduced2);


        qDebug() << "end1";
            CoocFeatureImage(ho_Rectangle2, ho_ImageReduced2, 6, 90, &hv_Energy2, &hv_Correlation2,
        &hv_Homogeneity2, &hv_Contrast2);
            estimate_background_illumination(ho_ImageReduced2, &ho_ImageFFT1);
            SubImage(ho_ImageReduced2, ho_ImageFFT1, &ho_ImageSub, 2, 100);
            GrayHistoAbs(ho_Rectangle2, ho_ImageSub, 8, &hv_AbsoluteHisto2);
            //***************************
            hv_FeaturesExtended112.Clear();
            hv_FeaturesExtended112.Append(hv_Energy2);
            hv_FeaturesExtended112.Append(hv_Correlation2);
            hv_FeaturesExtended112.Append(hv_Homogeneity2);
            hv_FeaturesExtended112.Append(hv_Contrast2);
            hv_FeaturesExtended112.Append(hv_AbsoluteHisto2);
            hv_NumFeatures112 = hv_FeaturesExtended112.TupleLength();
            hv_FeatureVector112 = hv_FeaturesExtended112.TupleReal();
            ClassifyClassMlp(hv_MLPHandle, hv_FeatureVector112, 1, &hv_Class112, &hv_Confidence112);
            if (0 != (hv_Class112==0))
            {
                //                        disp_message(hv_WindowHandle2, "良品"+hv_Class112, "window", 10, 10, "black",
//              "true");
            }
            else if (0 != (hv_Class112==1))
            {

                //                        disp_message(hv_WindowHandle2, "废品"+hv_Class112, "window", 10, 10, "black",
//              "true");
            }
            else if (0 != (hv_Class112==2))
            {

                //                        disp_message(hv_WindowHandle2, "次品"+hv_Class112, "window", 10, 10, "black",
//              "true");
            }
            //FwriteString(hv_FileHandle, (((hv_Index+1)+":")+hv_Class112)+"\n");


        }
        // stop(); only in hdevelop
    }
    qDebug() << "end";
    //**********************************************
    int i = hv_Class112.I() - 1;
    if(i < 0 )
        return 2;
    else
        return i;

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
               params["defect_length_part2"].toDouble(), params["defect_width_part2"].toDouble(),params["num_part2"].toInt(),
               params["confidence_Contain"].toDouble(), params["confidence_notContain"].toDouble());

    run(object,deal_object,width,height,winid,0,0);
//    int result = run_rujie(object,deal_object,width,height,winid);
//    qDebug() << result;
    emit tell_window_check_result(get_result(),get_defectsType(),deal_object);
}
