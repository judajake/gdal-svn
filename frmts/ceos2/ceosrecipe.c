/* Copyright (c) 1997
 * Atlantis Scientific Inc, 20 Colonnade, Suite 110
 * Nepean, Ontario, K2E 7M6, Canada
 *
 * All rights reserved.  Not to be used, reproduced
 * or disclosed without permission.
 */

/* +---------------------------------------------------------------------+
 * |@@@@@@@@@@    @@@| EASI/PACE V6.0, Copyright (c) 1997.               |
 * |@@@@@@ ***      @|                                                   |
 * |@@@  *******    @| PCI Inc., 50 West Wilmot Street,                  |
 * |@@  *********  @@| Richmond Hill, Ontario, L4B 1M5, Canada.          |
 * |@    *******  @@@|                                                   |
 * |@      *** @@@@@@| All rights reserved. Not to be used, reproduced   |
 * |@@@    @@@@@@@@@@| or disclosed without permission.                  |
 * +---------------------------------------------------------------------+
 */


#include "ceos.h"

/* Array of Datatypes and their names/values */

typedef struct { char *String;
                 int Type;
} CeosStringType_t;

typedef struct { int (*function)(CeosSARVolume_t *volume, void *token);
                 void *token;
} RecipeFunctionData_t;


CeosStringType_t CeosDataType[] = { { "IU1", __CEOS_TYP_UCHAR },
				    { "IU2", __CEOS_TYP_USHORT },
				    { "CI*2", __CEOS_TYP_COMPLEX_CHAR },
				    { "CI*4", __CEOS_TYP_COMPLEX_SHORT },
				    { "CI*8", __CEOS_TYP_COMPLEX_LONG },
				    { "C*8", __CEOS_TYP_COMPLEX_FLOAT },
				    { NULL, 0 } };

CeosStringType_t CeosInterleaveType[] = { { "BSQ", __CEOS_IL_BAND },
					  { "BIL", __CEOS_IL_LINE },
					  { NULL, 0 } };

#define IMAGE_OPT { 63, 192, 18, 18 }
#define PROC_DATA_REC { 50, 11, 18, 20 }
#define DATA_SET_SUMMARY { 18, 10, 18, 20 }

CeosRecipeType_t RadarSatRecipe[] =
{
    { __CEOS_REC_NUMCHANS, 1, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      233, 4, __CEOS_REC_TYP_I }, /* Number of channels */
    { __CEOS_REC_INTERLEAVE, 1, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      269, 4, __CEOS_REC_TYP_A }, /* Interleaving type */
    { __CEOS_REC_DATATYPE, 1, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      429, 4, __CEOS_REC_TYP_A }, /* Data type */
    { __CEOS_REC_BPR, 0, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      0, 0, __CEOS_REC_TYP_A }, /* For Defeault CEOS, this is done using other vals */
    { __CEOS_REC_LINES, 1, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      237, 8, __CEOS_REC_TYP_I }, /* How many lines */
    { __CEOS_REC_TBP, 0, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      261, 4, __CEOS_REC_TYP_I },
    { __CEOS_REC_BBP, 0, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      265, 4, __CEOS_REC_TYP_I }, /* Bottom border pixels */
    { __CEOS_REC_PPL, 1, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      249, 8, __CEOS_REC_TYP_I }, /* Pixels per line */
    { __CEOS_REC_LBP, 0, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      245, 4, __CEOS_REC_TYP_I }, /* Left Border Pixels */
    { __CEOS_REC_RBP, 0, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      257, 4, __CEOS_REC_TYP_I }, /* Isn't available for RadarSAT */
    { __CEOS_REC_BPP, 1, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      225, 4, __CEOS_REC_TYP_I }, /* Bytes Per Pixel */
    { __CEOS_REC_RPL, 1, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      273, 2, __CEOS_REC_TYP_I }, /* Records per line */
    { __CEOS_REC_PPR, 0, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      0, 0, __CEOS_REC_TYP_I }, /* Pixels Per Record -- need to fill record type */
    { __CEOS_REC_IDS, 1, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      277, 4, __CEOS_REC_TYP_I }, /* Prefix data per record */
    { __CEOS_REC_FDL, 1, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      9, 4, __CEOS_REC_TYP_B }, /* Length of Imagry Options Header */
    { __CEOS_REC_PIXORD, 0, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      0, 0, __CEOS_REC_TYP_I }, /* Must be calculated */
    { __CEOS_REC_LINORD, 0, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      0, 0, __CEOS_REC_TYP_I }, /* Must be calculated */
    { __CEOS_REC_PRODTYPE, 0, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      0, 0, __CEOS_REC_TYP_I },
    { __CEOS_REC_RECORDSIZE, 1, __CEOS_IMAGRY_OPT_FILE, PROC_DATA_REC,
      9, 4, __CEOS_REC_TYP_B }, /* The processed image record size */
    { __CEOS_REC_SUFFIX_SIZE, 1, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      289, 4, __CEOS_REC_TYP_I }, /* Suffix data per record */
    { 0, 0, 0, { 0, 0, 0, 0 }, 0, 0, 0 } /* Last record is Zero */
} ;

CeosRecipeType_t ScanSARRecipe[] =
{
    { __CEOS_REC_NUMCHANS, 1, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      233, 4, __CEOS_REC_TYP_I }, /* Number of channels */
    { __CEOS_REC_INTERLEAVE, 1, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      269, 4, __CEOS_REC_TYP_A }, /* Interleaving type */
    { __CEOS_REC_DATATYPE, 1, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      429, 4, __CEOS_REC_TYP_A }, /* Data type */
    { __CEOS_REC_LINES, 1, __CEOS_ANY_FILE, DATA_SET_SUMMARY,
      325, 8, __CEOS_REC_TYP_I }, /* How many lines */
    { __CEOS_REC_PPL, 1, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      249, 8, __CEOS_REC_TYP_I }, /* Pixels per line */
    { __CEOS_REC_BPP, 1, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      225, 4, __CEOS_REC_TYP_I }, /* Bytes Per Pixel */
    { __CEOS_REC_RPL, 1, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      273, 2, __CEOS_REC_TYP_I }, /* Records per line */
    { __CEOS_REC_IDS, 1, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      277, 4, __CEOS_REC_TYP_I }, /* Prefix data per record */
    { __CEOS_REC_FDL, 1, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      9, 4, __CEOS_REC_TYP_B }, /* Length of Imagry Options Header */
    { __CEOS_REC_RECORDSIZE, 1, __CEOS_IMAGRY_OPT_FILE, PROC_DATA_REC,
      9, 4, __CEOS_REC_TYP_B }, /* The processed image record size */
    { __CEOS_REC_SUFFIX_SIZE, 1, __CEOS_IMAGRY_OPT_FILE, IMAGE_OPT,
      289, 4, __CEOS_REC_TYP_I }, /* Suffix data per record */
    { 0, 0, 0, { 0, 0, 0, 0 }, 0, 0, 0 } /* Last record is Zero */
} ;

#undef IMAGE_OPT
#undef PROC_DATA_REC

static void ExtractInt( CeosRecord_t *record, int type, unsigned int offset, unsigned int length, int *value );

static char *ExtractString( CeosRecord_t *record, unsigned int offset, unsigned int length, char *string );

static int GetCeosStringType(const CeosStringType_t *CeosType, const char *string);

Link_t *RecipeFunctions = NULL;

void RegisterRecipes( void )
{

    AddRecipe( ScanSARRecipeFCN, ScanSARRecipe );
    AddRecipe( CeosDefaultRecipe, RadarSatRecipe );
    /*  AddRecipe( CeosDefaultRecipe, AtlantisRecipe ); */
}

void AddRecipe( int (*function)(CeosSARVolume_t *volume,
				void *token),
		void *token )
{

    RecipeFunctionData_t *TempData;

    Link_t *Link;

    TempData = HMalloc( sizeof( RecipeFunctionData_t ) );

    TempData->function = function;
    TempData->token = token;

    Link = CreateLink( TempData );

    if( RecipeFunctions == NULL)
    {
	RecipeFunctions = Link;
    } else {
	RecipeFunctions = InsertLink( RecipeFunctions, Link );
    }
}

int CeosDefaultRecipe( CeosSARVolume_t *volume, void *token )
{
    CeosSARImageDescRecipe_t *temp_imagerecipe;
    CeosRecipeType_t *recipe;
    CeosRecord_t *record;
    CeosTypeCode_t TypeCode;
    struct CeosSARImageDesc *ImageDesc = &(volume->ImageDesc);
    char temp_str[1024];
    int i, temp_int, e;
    
#define DoExtractInt(a) ExtractInt( record, recipe[i].Type, recipe[i].Offset, recipe[i].Length, &a)

    if(token == NULL)
    {
	return 0;
    }

    memset(ImageDesc, 0, sizeof( struct CeosSARImageDesc ) );

/*    temp_imagerecipe = (CeosSARImageDescRecipe_t *) token;
    recipe = temp_imagerecipe->Recipe; */
 
    recipe = token;

    for(i = 0; recipe[i].ImageDescValue != 0; i++ )
    {
	if(recipe[i].Override)
	{
	    TypeCode.UCharCode.Subtype1 = recipe[i].TypeCode.Subtype1;
	    TypeCode.UCharCode.Type = recipe[i].TypeCode.Type;
	    TypeCode.UCharCode.Subtype2 = recipe[i].TypeCode.Subtype2;
	    TypeCode.UCharCode.Subtype3 = recipe[i].TypeCode.Subtype3;

	    record = FindCeosRecord( volume->RecordList, TypeCode, recipe[i].FileId, -1, -1 );

	    if(record == NULL)
	    {
		temp_int = 0;
	    } else {

		switch( recipe[i].ImageDescValue )
		{
		case __CEOS_REC_NUMCHANS:
		   DoExtractInt( ImageDesc->NumChannels );
		   break;
		case __CEOS_REC_LINES:
		    DoExtractInt( ImageDesc->Lines );
		    break;
		case __CEOS_REC_BPP:
		    DoExtractInt( ImageDesc->BytesPerPixel );
		    break;
		case __CEOS_REC_RPL:
		    DoExtractInt( ImageDesc->RecordsPerLine );
		    break;
		case __CEOS_REC_FDL:
		    DoExtractInt( ImageDesc->FileDescriptorLength );
		    break;
		case __CEOS_REC_IDS:
		    DoExtractInt( ImageDesc->ImageDataStart );
		    if( ImageDesc->ImageDataStart == 180 )
		    {
			/* Probably 192, set manually */
			/* Issue warning */
			ImageDesc->ImageDataStart = 192;
		    }
		    break;
		case __CEOS_REC_SUFFIX_SIZE:
		    DoExtractInt( ImageDesc->ImageSuffixData );
		    break;
		case __CEOS_REC_RECORDSIZE:
		    DoExtractInt( ImageDesc->BytesPerRecord );
		    break;
		case __CEOS_REC_PPL:
		    DoExtractInt( ImageDesc->PixelsPerLine );
		    break;
		case __CEOS_REC_TBP:
		    DoExtractInt( ImageDesc->TopBorderPixels );
		    break;
		case __CEOS_REC_BBP:
		    DoExtractInt( ImageDesc->BottomBorderPixels );
		    break;
		case __CEOS_REC_LBP:
		    DoExtractInt( ImageDesc->LeftBorderPixels );
		    break;
		case __CEOS_REC_RBP:
		    DoExtractInt( ImageDesc->RightBorderPixels );
		    break;
		case __CEOS_REC_INTERLEAVE:
		    ExtractString( record, recipe[i].Offset, recipe[i].Length, temp_str );

		    ImageDesc->ChannelInterleaving = GetCeosStringType( CeosInterleaveType, temp_str );
		    break;
		case __CEOS_REC_DATATYPE:
		    ExtractString( record, recipe[i].Offset, recipe[i].Length, temp_str );

		    ImageDesc->DataType = GetCeosStringType( CeosDataType, temp_str );
		    break;
		}
		
	    }
	}
    }

    /* Now we find some values that aren't in the CEOS header, but we calculate */

    if( ImageDesc->PixelsPerRecord == 0 && ImageDesc->ImageDataStart != 0 &&
	ImageDesc->BytesPerRecord != 0 && ImageDesc->BytesPerPixel != 0 )
    {
	ImageDesc->PixelsPerRecord = ( ( ImageDesc->BytesPerRecord -
					 (ImageDesc->ImageSuffixData +
					  ImageDesc->ImageDataStart )) /
				       ImageDesc->BytesPerPixel );

	if(ImageDesc->PixelsPerRecord > ImageDesc->PixelsPerLine)
	    ImageDesc->PixelsPerRecord = ImageDesc->PixelsPerLine;
    }

    
    /* Sanity checking */
    
    if( ImageDesc->PixelsPerLine == 0 || ImageDesc->Lines == 0 ||
	ImageDesc->RecordsPerLine == 0 || ImageDesc->ImageDataStart == 0 ||
	ImageDesc->FileDescriptorLength == 0 || ImageDesc->DataType == 0 ||
	ImageDesc->NumChannels == 0 || ImageDesc->BytesPerPixel == 0 ||
	ImageDesc->ChannelInterleaving == 0 || ImageDesc->BytesPerRecord == 0)
    {
	return 0;
    } else {
	
	ImageDesc->ImageDescValid = TRUE;
	return 1;
    }
}

int ScanSARRecipeFCN( CeosSARVolume_t *volume, void *token )
{
    struct CeosSARImageDesc *ImageDesc = &(volume->ImageDesc);
    CeosRecipeType_t *recipe = token;

    memset( ImageDesc, 0, sizeof( struct CeosSARImageDesc ) );

    if( CeosDefaultRecipe( volume, token ) )
    {
	ImageDesc->Lines *= 2;
	return 1;
    }

    return 0;
}    

void GetCeosSARImageDesc( CeosSARVolume_t *volume )
{
    Link_t *link;
    RecipeFunctionData_t *rec_data;
    int (*function)(CeosSARVolume_t *volume, void *token);

    if( RecipeFunctions == NULL )
    {
	RegisterRecipes();
    }

    if(RecipeFunctions == NULL )
    {
	return ;
    }

    for(link = RecipeFunctions; link != NULL; link = link->next)
    {
	if(link->object)
	{
	    rec_data = link->object;
            function = rec_data->function;
	    if(( *function )( volume, rec_data->token ) )
		return ;
	}
    }

    return ;

}


static void ExtractInt(CeosRecord_t *record, int type, unsigned int offset, unsigned int length, int *value)
{
    void *buffer;
    char format[32];

    buffer = HMalloc( length + 1 );

    switch(type)
    {
    case __CEOS_REC_TYP_A:
	sprintf( format, "A%u", length );
	GetCeosField( record, offset, format,  buffer );
	*value = atoi( buffer );
	break;
    case __CEOS_REC_TYP_B:
	sprintf( format, "B%u", length );
	GetCeosField( record, offset, format, buffer );
	if( length <= 4 )
	    CeosToNative( value, buffer, length, length );
	else
	    *value = 0;
	break;
    case __CEOS_REC_TYP_I:
	sprintf( format, "I%u", length );
	GetCeosField( record, offset, format, value );
	break;
    }

    HFree( buffer );

}

static char *ExtractString( CeosRecord_t *record, unsigned int offset, unsigned int length, char *string )
{
    char format[12];

    if(string == NULL)
    {
	string = HMalloc( length + 1 );
    }

    sprintf( format, "A%u", length );

    GetCeosField( record, offset, format, string );

    return string;
}

static int GetCeosStringType(const CeosStringType_t *CeosStringType, const char *string)
{
    int i;

    for(i = 0;CeosStringType[i].String != NULL;i++)
    {
	if(strncmp(CeosStringType[i].String ,string, strlen( CeosStringType[i].String ) ) == 0 )
	{
	    return CeosStringType[i].Type;
	}
    }

    return 0;
}
