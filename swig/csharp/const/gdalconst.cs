/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.24
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace GDAL {

using System;

public class gdalconst {
  public static readonly int GDT_Unknown = gdalconstPINVOKE.get_GDT_Unknown();
  public static readonly int GDT_Byte = gdalconstPINVOKE.get_GDT_Byte();
  public static readonly int GDT_UInt16 = gdalconstPINVOKE.get_GDT_UInt16();
  public static readonly int GDT_Int16 = gdalconstPINVOKE.get_GDT_Int16();
  public static readonly int GDT_UInt32 = gdalconstPINVOKE.get_GDT_UInt32();
  public static readonly int GDT_Int32 = gdalconstPINVOKE.get_GDT_Int32();
  public static readonly int GDT_Float32 = gdalconstPINVOKE.get_GDT_Float32();
  public static readonly int GDT_Float64 = gdalconstPINVOKE.get_GDT_Float64();
  public static readonly int GDT_CInt16 = gdalconstPINVOKE.get_GDT_CInt16();
  public static readonly int GDT_CInt32 = gdalconstPINVOKE.get_GDT_CInt32();
  public static readonly int GDT_CFloat32 = gdalconstPINVOKE.get_GDT_CFloat32();
  public static readonly int GDT_CFloat64 = gdalconstPINVOKE.get_GDT_CFloat64();
  public static readonly int GDT_TypeCount = gdalconstPINVOKE.get_GDT_TypeCount();
  public static readonly int GA_ReadOnly = gdalconstPINVOKE.get_GA_ReadOnly();
  public static readonly int GA_Update = gdalconstPINVOKE.get_GA_Update();
  public static readonly int GF_Read = gdalconstPINVOKE.get_GF_Read();
  public static readonly int GF_Write = gdalconstPINVOKE.get_GF_Write();
  public static readonly int GCI_Undefined = gdalconstPINVOKE.get_GCI_Undefined();
  public static readonly int GCI_GrayIndex = gdalconstPINVOKE.get_GCI_GrayIndex();
  public static readonly int GCI_PaletteIndex = gdalconstPINVOKE.get_GCI_PaletteIndex();
  public static readonly int GCI_RedBand = gdalconstPINVOKE.get_GCI_RedBand();
  public static readonly int GCI_GreenBand = gdalconstPINVOKE.get_GCI_GreenBand();
  public static readonly int GCI_BlueBand = gdalconstPINVOKE.get_GCI_BlueBand();
  public static readonly int GCI_AlphaBand = gdalconstPINVOKE.get_GCI_AlphaBand();
  public static readonly int GCI_HueBand = gdalconstPINVOKE.get_GCI_HueBand();
  public static readonly int GCI_SaturationBand = gdalconstPINVOKE.get_GCI_SaturationBand();
  public static readonly int GCI_LightnessBand = gdalconstPINVOKE.get_GCI_LightnessBand();
  public static readonly int GCI_CyanBand = gdalconstPINVOKE.get_GCI_CyanBand();
  public static readonly int GCI_MagentaBand = gdalconstPINVOKE.get_GCI_MagentaBand();
  public static readonly int GCI_YellowBand = gdalconstPINVOKE.get_GCI_YellowBand();
  public static readonly int GCI_BlackBand = gdalconstPINVOKE.get_GCI_BlackBand();
  public static readonly int GRA_NearestNeighbour = gdalconstPINVOKE.get_GRA_NearestNeighbour();
  public static readonly int GRA_Bilinear = gdalconstPINVOKE.get_GRA_Bilinear();
  public static readonly int GRA_Cubic = gdalconstPINVOKE.get_GRA_Cubic();
  public static readonly int GRA_CubicSpline = gdalconstPINVOKE.get_GRA_CubicSpline();
  public static readonly int GPI_Gray = gdalconstPINVOKE.get_GPI_Gray();
  public static readonly int GPI_RGB = gdalconstPINVOKE.get_GPI_RGB();
  public static readonly int GPI_CMYK = gdalconstPINVOKE.get_GPI_CMYK();
  public static readonly int GPI_HLS = gdalconstPINVOKE.get_GPI_HLS();
  public static readonly int CXT_Element = gdalconstPINVOKE.get_CXT_Element();
  public static readonly int CXT_Text = gdalconstPINVOKE.get_CXT_Text();
  public static readonly int CXT_Attribute = gdalconstPINVOKE.get_CXT_Attribute();
  public static readonly int CXT_Comment = gdalconstPINVOKE.get_CXT_Comment();
  public static readonly int CXT_Literal = gdalconstPINVOKE.get_CXT_Literal();
  public static readonly int CE_None = gdalconstPINVOKE.get_CE_None();
  public static readonly int CE_Debug = gdalconstPINVOKE.get_CE_Debug();
  public static readonly int CE_Warning = gdalconstPINVOKE.get_CE_Warning();
  public static readonly int CE_Failure = gdalconstPINVOKE.get_CE_Failure();
  public static readonly int CE_Fatal = gdalconstPINVOKE.get_CE_Fatal();
  public static readonly int CPLE_None = gdalconstPINVOKE.get_CPLE_None();
  public static readonly int CPLE_AppDefined = gdalconstPINVOKE.get_CPLE_AppDefined();
  public static readonly int CPLE_OutOfMemory = gdalconstPINVOKE.get_CPLE_OutOfMemory();
  public static readonly int CPLE_FileIO = gdalconstPINVOKE.get_CPLE_FileIO();
  public static readonly int CPLE_OpenFailed = gdalconstPINVOKE.get_CPLE_OpenFailed();
  public static readonly int CPLE_IllegalArg = gdalconstPINVOKE.get_CPLE_IllegalArg();
  public static readonly int CPLE_NotSupported = gdalconstPINVOKE.get_CPLE_NotSupported();
  public static readonly int CPLE_AssertionFailed = gdalconstPINVOKE.get_CPLE_AssertionFailed();
  public static readonly int CPLE_NoWriteAccess = gdalconstPINVOKE.get_CPLE_NoWriteAccess();
  public static readonly int CPLE_UserInterrupt = gdalconstPINVOKE.get_CPLE_UserInterrupt();
  public static readonly int DMD_LONGNAME = gdalconstPINVOKE.get_DMD_LONGNAME();
  public static readonly int DMD_HELPTOPIC = gdalconstPINVOKE.get_DMD_HELPTOPIC();
  public static readonly int DMD_MIMETYPE = gdalconstPINVOKE.get_DMD_MIMETYPE();
  public static readonly int DMD_EXTENSION = gdalconstPINVOKE.get_DMD_EXTENSION();
  public static readonly int DMD_CREATIONOPTIONLIST = gdalconstPINVOKE.get_DMD_CREATIONOPTIONLIST();
  public static readonly int DMD_CREATIONDATATYPES = gdalconstPINVOKE.get_DMD_CREATIONDATATYPES();
  public static readonly int DCAP_CREATE = gdalconstPINVOKE.get_DCAP_CREATE();
  public static readonly int DCAP_CREATECOPY = gdalconstPINVOKE.get_DCAP_CREATECOPY();
  public static readonly int CPLES_BackslashQuotable = gdalconstPINVOKE.get_CPLES_BackslashQuotable();
  public static readonly int CPLES_XML = gdalconstPINVOKE.get_CPLES_XML();
  public static readonly int CPLES_URL = gdalconstPINVOKE.get_CPLES_URL();
  public static readonly int CPLES_SQL = gdalconstPINVOKE.get_CPLES_SQL();
  public static readonly int CPLES_CSV = gdalconstPINVOKE.get_CPLES_CSV();
}

}
