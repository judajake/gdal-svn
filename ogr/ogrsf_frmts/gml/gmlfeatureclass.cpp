/**********************************************************************
 * $Id$
 *
 * Project:  GML Reader
 * Purpose:  Implementation of GMLFeatureClass.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 **********************************************************************
 * Copyright (c) 2002, Frank Warmerdam
 * Copyright (c) 2010-2013, Even Rouault <even dot rouault at mines-paris dot org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#include "gmlreader.h"
#include "cpl_conv.h"
#include "ogr_core.h"

/************************************************************************/
/*                          GMLFeatureClass()                           */
/************************************************************************/

GMLFeatureClass::GMLFeatureClass( const char *pszName )

{
    m_pszName = CPLStrdup( pszName );
    n_nNameLen = strlen( m_pszName );
    m_pszElementName = NULL;
    n_nElementNameLen = 0;
    m_nPropertyCount = 0;
    m_papoProperty = NULL;
    m_nGeometryPropertyCount = 0;
    m_papoGeometryProperty = NULL;
    m_bSchemaLocked = FALSE;

    m_pszExtraInfo = NULL;
    m_bHaveExtents = FALSE;
    m_nFeatureCount = -1; // unknown

    m_pszSRSName = NULL;
    m_bSRSNameConsistant = TRUE;
}

/************************************************************************/
/*                          ~GMLFeatureClass()                          */
/************************************************************************/

GMLFeatureClass::~GMLFeatureClass()

{
    CPLFree( m_pszName );
    CPLFree( m_pszElementName );

    for( int i = 0; i < m_nPropertyCount; i++ )
        delete m_papoProperty[i];
    CPLFree( m_papoProperty );

    ClearGeometryProperties();

    CPLFree( m_pszSRSName );
}

/************************************************************************/
/*                            SetName()                                 */
/************************************************************************/

void GMLFeatureClass::SetName(const char* pszNewName)
{
    CPLFree( m_pszName );
    m_pszName = CPLStrdup( pszNewName );
}

/************************************************************************/
/*                           GetProperty(int)                           */
/************************************************************************/

GMLPropertyDefn *GMLFeatureClass::GetProperty( int iIndex ) const

{
    if( iIndex < 0 || iIndex >= m_nPropertyCount )
        return NULL;
    else
        return m_papoProperty[iIndex];
}

/************************************************************************/
/*                          GetPropertyIndex()                          */
/************************************************************************/

int GMLFeatureClass::GetPropertyIndex( const char *pszName ) const

{
    for( int i = 0; i < m_nPropertyCount; i++ )
        if( EQUAL(pszName,m_papoProperty[i]->GetName()) )
            return i;

    return -1;
}

/************************************************************************/
/*                        GetPropertyIndexBySrcElement()                */
/************************************************************************/

int GMLFeatureClass::GetPropertyIndexBySrcElement( const char *pszElement, int nLen ) const

{
    for( int i = 0; i < m_nPropertyCount; i++ )
        if( nLen == (int)m_papoProperty[i]->GetSrcElementLen() &&
            memcmp(pszElement,m_papoProperty[i]->GetSrcElement(), nLen) == 0)
            return i;

    return -1;
}

/************************************************************************/
/*                            AddProperty()                             */
/************************************************************************/

int GMLFeatureClass::AddProperty( GMLPropertyDefn *poDefn )

{
    if( GetProperty(poDefn->GetName()) != NULL )
    {
        CPLError(CE_Warning, CPLE_AppDefined,
                 "Field with same name (%s) already exists. Skipping newer ones",
                 poDefn->GetName());
        return -1;
    }

    m_nPropertyCount++;
    m_papoProperty = (GMLPropertyDefn **)
        CPLRealloc( m_papoProperty, sizeof(void*) * m_nPropertyCount );

    m_papoProperty[m_nPropertyCount-1] = poDefn;

    return m_nPropertyCount-1;
}

/************************************************************************/
/*                         GetGeometryProperty(int)                      */
/************************************************************************/

GMLGeometryPropertyDefn *GMLFeatureClass::GetGeometryProperty( int iIndex ) const
{
    if( iIndex < 0 || iIndex >= m_nGeometryPropertyCount )
        return NULL;
    else
        return m_papoGeometryProperty[iIndex];
}

/************************************************************************/
/*                   GetGeometryPropertyIndexBySrcElement()             */
/************************************************************************/

int GMLFeatureClass::GetGeometryPropertyIndexBySrcElement( const char *pszElement ) const

{
    for( int i = 0; i < m_nGeometryPropertyCount; i++ )
        if( strcmp(pszElement,m_papoGeometryProperty[i]->GetSrcElement()) == 0)
            return i;

    return -1;
}

/************************************************************************/
/*                         AddGeometryProperty()                        */
/************************************************************************/

int GMLFeatureClass::AddGeometryProperty( GMLGeometryPropertyDefn *poDefn )

{
    if( GetGeometryPropertyIndexBySrcElement(poDefn->GetSrcElement()) >= 0 )
    {
        CPLError(CE_Warning, CPLE_AppDefined,
                 "Field with same name (%s) already exists. Skipping newer ones",
                 poDefn->GetSrcElement());
        return -1;
    }

    m_nGeometryPropertyCount++;
    m_papoGeometryProperty = (GMLGeometryPropertyDefn **)
        CPLRealloc( m_papoGeometryProperty, sizeof(void*) * m_nGeometryPropertyCount );

    m_papoGeometryProperty[m_nGeometryPropertyCount-1] = poDefn;

    return m_nGeometryPropertyCount-1;
}

/************************************************************************/
/*                       ClearGeometryProperties()                      */
/************************************************************************/

void GMLFeatureClass::ClearGeometryProperties()
{
    for( int i = 0; i < m_nGeometryPropertyCount; i++ )
        delete m_papoGeometryProperty[i];
    CPLFree( m_papoGeometryProperty );
    m_nGeometryPropertyCount = 0;
    m_papoGeometryProperty = NULL;
}

/************************************************************************/
/*                         HasFeatureProperties()                       */
/************************************************************************/

int GMLFeatureClass::HasFeatureProperties()
{
    for( int i = 0; i < m_nPropertyCount; i++ )
    {
        if( m_papoProperty[i]->GetType() == GMLPT_FeatureProperty ||
            m_papoProperty[i]->GetType() == GMLPT_FeaturePropertyList )
            return TRUE;
    }
    return FALSE;
}

/************************************************************************/
/*                           SetElementName()                           */
/************************************************************************/

void GMLFeatureClass::SetElementName( const char *pszElementName )

{
    CPLFree( m_pszElementName );
    m_pszElementName = CPLStrdup( pszElementName );
    n_nElementNameLen = strlen(pszElementName);
}

/************************************************************************/
/*                           GetElementName()                           */
/************************************************************************/

const char *GMLFeatureClass::GetElementName() const

{
    if( m_pszElementName == NULL )
        return m_pszName;
    else
        return m_pszElementName;
}

/************************************************************************/
/*                           GetElementName()                           */
/************************************************************************/

size_t GMLFeatureClass::GetElementNameLen() const

{
    if( m_pszElementName == NULL )
        return n_nNameLen;
    else
        return n_nElementNameLen;
}

/************************************************************************/
/*                          GetFeatureCount()                           */
/************************************************************************/

int GMLFeatureClass::GetFeatureCount()

{
    return m_nFeatureCount;
}

/************************************************************************/
/*                          SetFeatureCount()                           */
/************************************************************************/

void GMLFeatureClass::SetFeatureCount( int nNewCount )

{
    m_nFeatureCount = nNewCount;
}

/************************************************************************/
/*                            GetExtraInfo()                            */
/************************************************************************/

const char *GMLFeatureClass::GetExtraInfo()

{
    return m_pszExtraInfo;
}

/************************************************************************/
/*                            SetExtraInfo()                            */
/************************************************************************/

void GMLFeatureClass::SetExtraInfo( const char *pszExtraInfo )

{
    CPLFree( m_pszExtraInfo );
    m_pszExtraInfo = NULL;

    if( pszExtraInfo != NULL )
        m_pszExtraInfo = CPLStrdup( pszExtraInfo );
}

/************************************************************************/
/*                             SetExtents()                             */
/************************************************************************/

void GMLFeatureClass::SetExtents( double dfXMin, double dfXMax, 
                                  double dfYMin, double dfYMax )

{
    m_dfXMin = dfXMin;
    m_dfXMax = dfXMax;
    m_dfYMin = dfYMin;
    m_dfYMax = dfYMax;

    m_bHaveExtents = TRUE;
}

/************************************************************************/
/*                             GetExtents()                             */
/************************************************************************/

int GMLFeatureClass::GetExtents( double *pdfXMin, double *pdfXMax, 
                                 double *pdfYMin, double *pdfYMax )

{
    if( m_bHaveExtents )
    {
        *pdfXMin = m_dfXMin;
        *pdfXMax = m_dfXMax;
        *pdfYMin = m_dfYMin;
        *pdfYMax = m_dfYMax;
    }

    return m_bHaveExtents;
}

/************************************************************************/
/*                            SetSRSName()                              */
/************************************************************************/

void GMLFeatureClass::SetSRSName( const char* pszSRSName )

{
    m_bSRSNameConsistant = TRUE;
    CPLFree(m_pszSRSName);
    m_pszSRSName = (pszSRSName) ? CPLStrdup(pszSRSName) : NULL;
}

/************************************************************************/
/*                           MergeSRSName()                             */
/************************************************************************/

void GMLFeatureClass::MergeSRSName( const char* pszSRSName )

{
    if(!m_bSRSNameConsistant)
        return;

    if( m_pszSRSName == NULL )
    {
        if (pszSRSName)
            m_pszSRSName = CPLStrdup(pszSRSName);
    }
    else
    {
        m_bSRSNameConsistant = pszSRSName != NULL &&
                                  strcmp(m_pszSRSName, pszSRSName) == 0;
        if (!m_bSRSNameConsistant)
        {
            CPLFree(m_pszSRSName);
            m_pszSRSName = NULL;
        }
    }
}

/************************************************************************/
/*                         InitializeFromXML()                          */
/************************************************************************/

int GMLFeatureClass::InitializeFromXML( CPLXMLNode *psRoot )

{
/* -------------------------------------------------------------------- */
/*      Do some rudimentary checking that this is a well formed         */
/*      node.                                                           */
/* -------------------------------------------------------------------- */
    if( psRoot == NULL 
        || psRoot->eType != CXT_Element 
        || !EQUAL(psRoot->pszValue,"GMLFeatureClass") )
    {
        CPLError( CE_Failure, CPLE_AppDefined, 
                  "GMLFeatureClass::InitializeFromXML() called on %s node!",
                  psRoot->pszValue );
        return FALSE;
    }

    if( CPLGetXMLValue( psRoot, "Name", NULL ) == NULL )
    {
        CPLError( CE_Failure, CPLE_AppDefined, 
                  "GMLFeatureClass has no <Name> element." );
        return FALSE;
    }

/* -------------------------------------------------------------------- */
/*      Collect base info.                                              */
/* -------------------------------------------------------------------- */
    CPLFree( m_pszName );
    m_pszName = CPLStrdup( CPLGetXMLValue( psRoot, "Name", NULL ) );
    n_nNameLen = strlen(m_pszName);

    SetElementName( CPLGetXMLValue( psRoot, "ElementPath", m_pszName ) );

/* -------------------------------------------------------------------- */
/*      Collect geometry properties.                                    */
/* -------------------------------------------------------------------- */
    CPLXMLNode *psThis;

    int bHasValidGeometryElementPath = FALSE;
    int bHasFoundGeomType = FALSE;
    int bHasFoundGeomElements = FALSE;
    const char* pszGPath = "";
    int nGeomType = wkbUnknown;

    for( psThis = psRoot->psChild; psThis != NULL; psThis = psThis->psNext )
    {
        if( psThis->eType == CXT_Element &&
            strcmp(psThis->pszValue, "GeometryElementPath") == 0 )
        {
            bHasFoundGeomElements = TRUE;

            if( bHasValidGeometryElementPath )
            {
                AddGeometryProperty( new GMLGeometryPropertyDefn( pszGPath, nGeomType ) );
                bHasValidGeometryElementPath = FALSE;
                bHasFoundGeomType = FALSE;
                pszGPath = "";
                nGeomType = wkbUnknown;
            }
            pszGPath = CPLGetXMLValue( psThis, NULL, "" );
            bHasValidGeometryElementPath = TRUE;
        }
        else if( psThis->eType == CXT_Element &&
                 strcmp(psThis->pszValue, "GeometryType") == 0 )
        {
            bHasFoundGeomElements = TRUE;

            if( bHasFoundGeomType )
            {
                AddGeometryProperty( new GMLGeometryPropertyDefn( pszGPath, nGeomType ) );
                bHasValidGeometryElementPath = FALSE;
                bHasFoundGeomType = FALSE;
                pszGPath = "";
                nGeomType = wkbUnknown;
            }
            const char* pszGeometryType = CPLGetXMLValue( psThis, NULL, NULL );
            if( pszGeometryType != NULL )
            {
                nGeomType = atoi(pszGeometryType);
                int nFlattenGeomType = nGeomType & (~wkb25DBit);
                if( nGeomType == 100 )
                {
                    bHasValidGeometryElementPath = FALSE;
                    bHasFoundGeomType = FALSE;
                    break;
                }
                else if( !(nFlattenGeomType >= 0 && nFlattenGeomType <= 7) )
                {
                    nGeomType = wkbUnknown;
                    CPLError(CE_Warning, CPLE_AppDefined, "Unrecognised geometry type : %s",
                            pszGeometryType);
                }
            }
            bHasFoundGeomType = TRUE;
        }
    }

    /* If there was a dangling <GeometryElementPath> or <GeometryType> or */
    /* that no explicit geometry information has been found, then add */
    /* a geometry field */
    if( bHasValidGeometryElementPath || bHasFoundGeomType || !bHasFoundGeomElements )
    {
        AddGeometryProperty( new GMLGeometryPropertyDefn( pszGPath, nGeomType ) );
    }

    SetSRSName( CPLGetXMLValue( psRoot, "SRSName", NULL ) );

/* -------------------------------------------------------------------- */
/*      Collect dataset specific info.                                  */
/* -------------------------------------------------------------------- */
    CPLXMLNode *psDSI = CPLGetXMLNode( psRoot, "DatasetSpecificInfo" );
    if( psDSI != NULL )
    {
        const char *pszValue;

        pszValue = CPLGetXMLValue( psDSI, "FeatureCount", NULL );
        if( pszValue != NULL )
            SetFeatureCount( atoi(pszValue) );

        // Eventually we should support XML subtrees.
        pszValue = CPLGetXMLValue( psDSI, "ExtraInfo", NULL );
        if( pszValue != NULL )
            SetExtraInfo( pszValue );

        if( CPLGetXMLValue( psDSI, "ExtentXMin", NULL ) != NULL 
            && CPLGetXMLValue( psDSI, "ExtentXMax", NULL ) != NULL
            && CPLGetXMLValue( psDSI, "ExtentYMin", NULL ) != NULL
            && CPLGetXMLValue( psDSI, "ExtentYMax", NULL ) != NULL )
        {
            SetExtents( CPLAtof(CPLGetXMLValue( psDSI, "ExtentXMin", "0.0" )),
                        CPLAtof(CPLGetXMLValue( psDSI, "ExtentXMax", "0.0" )),
                        CPLAtof(CPLGetXMLValue( psDSI, "ExtentYMin", "0.0" )),
                        CPLAtof(CPLGetXMLValue( psDSI, "ExtentYMax", "0.0" )) );
        }
    }
    
/* -------------------------------------------------------------------- */
/*      Collect property definitions.                                   */
/* -------------------------------------------------------------------- */
    for( psThis = psRoot->psChild; psThis != NULL; psThis = psThis->psNext )
    {
        if( psThis->eType == CXT_Element &&
            EQUAL(psThis->pszValue, "PropertyDefn") )
        {
            const char *pszName = CPLGetXMLValue( psThis, "Name", NULL );
            const char *pszType = CPLGetXMLValue( psThis, "Type", "Untyped" );
            const char *pszCondition = CPLGetXMLValue( psThis, "Condition", NULL );
            GMLPropertyDefn *poPDefn;

            if( pszName == NULL )
            {
                CPLError( CE_Failure, CPLE_AppDefined, 
                          "GMLFeatureClass %s has a PropertyDefn without a <Name>..",
                          m_pszName );
                return FALSE;
            }

            poPDefn = new GMLPropertyDefn( 
                pszName, CPLGetXMLValue( psThis, "ElementPath", NULL ) );
            
            if( EQUAL(pszType,"Untyped") )
                poPDefn->SetType( GMLPT_Untyped );
            else if( EQUAL(pszType,"String") ) 
            {
                poPDefn->SetType( GMLPT_String );
                poPDefn->SetWidth( atoi( CPLGetXMLValue( psThis, "Width", "0" ) ) );
            }
            else if( EQUAL(pszType,"Integer") )
            {
                poPDefn->SetType( GMLPT_Integer );
                poPDefn->SetWidth( atoi( CPLGetXMLValue( psThis, "Width", "0" ) ) );
            }
            else if( EQUAL(pszType,"Real") )
            {
                poPDefn->SetType( GMLPT_Real );
                poPDefn->SetWidth( atoi( CPLGetXMLValue( psThis, "Width", "0" ) ) );
                poPDefn->SetPrecision( atoi( CPLGetXMLValue( psThis, "Precision", "0" ) ) );
            }
            else if( EQUAL(pszType,"StringList") ) 
                poPDefn->SetType( GMLPT_StringList );
            else if( EQUAL(pszType,"IntegerList") )
                poPDefn->SetType( GMLPT_IntegerList );
            else if( EQUAL(pszType,"RealList") )
                poPDefn->SetType( GMLPT_RealList );
            else if( EQUAL(pszType,"Complex") )
                poPDefn->SetType( GMLPT_Complex );
            else if( EQUAL(pszType,"FeatureProperty") )
                poPDefn->SetType( GMLPT_FeatureProperty );
            else if( EQUAL(pszType,"FeaturePropertyList") )
                poPDefn->SetType( GMLPT_FeaturePropertyList );
            else
            {
                CPLError( CE_Failure, CPLE_AppDefined, 
                          "Unrecognised property type %s.", 
                          pszType );
                delete poPDefn;
                return FALSE;
            }
            if( pszCondition != NULL )
                poPDefn->SetCondition(pszCondition);

            AddProperty( poPDefn );
        }
    }

    return TRUE;
}

/************************************************************************/
/*                           SerializeToXML()                           */
/************************************************************************/

CPLXMLNode *GMLFeatureClass::SerializeToXML()

{
    CPLXMLNode  *psRoot;
    int         iProperty;

/* -------------------------------------------------------------------- */
/*      Set feature class and core information.                         */
/* -------------------------------------------------------------------- */
    psRoot = CPLCreateXMLNode( NULL, CXT_Element, "GMLFeatureClass" );

    CPLCreateXMLElementAndValue( psRoot, "Name", GetName() );
    CPLCreateXMLElementAndValue( psRoot, "ElementPath", GetElementName() );
    
    if( m_nGeometryPropertyCount > 0 )
    {
        GMLGeometryPropertyDefn* poGeomFDefn = m_papoGeometryProperty[0];
        
        if( poGeomFDefn->GetSrcElement() != NULL && strlen(poGeomFDefn->GetSrcElement()) > 0 )
            CPLCreateXMLElementAndValue( psRoot, "GeometryElementPath", 
                                         poGeomFDefn->GetSrcElement() );
        
        if( poGeomFDefn->GetType() != 0 /* wkbUnknown */ )
        {
            char szValue[128];

            sprintf( szValue, "%d", poGeomFDefn->GetType() );
            CPLCreateXMLElementAndValue( psRoot, "GeometryType", szValue );
        }
    }
    else
    {
        CPLCreateXMLElementAndValue( psRoot, "GeometryType", "100" );
    }

    const char* pszSRSName = GetSRSName();
    if( pszSRSName )
    {
        CPLCreateXMLElementAndValue( psRoot, "SRSName", pszSRSName );
    }

/* -------------------------------------------------------------------- */
/*      Write out dataset specific information.                         */
/* -------------------------------------------------------------------- */
    CPLXMLNode *psDSI;

    if( m_bHaveExtents || m_nFeatureCount != -1 || m_pszExtraInfo != NULL )
    {
        psDSI = CPLCreateXMLNode( psRoot, CXT_Element, "DatasetSpecificInfo" );

        if( m_nFeatureCount != -1 )
        {
            char szValue[128];

            sprintf( szValue, "%d", m_nFeatureCount );
            CPLCreateXMLElementAndValue( psDSI, "FeatureCount", szValue );
        }

        if( m_bHaveExtents &&
            fabs(m_dfXMin) < 1e100 &&
            fabs(m_dfXMax) < 1e100 &&
            fabs(m_dfYMin) < 1e100 &&
            fabs(m_dfYMax) < 1e100 )
        {
            char szValue[128];

            snprintf( szValue, sizeof(szValue), "%.5f", m_dfXMin );
            CPLCreateXMLElementAndValue( psDSI, "ExtentXMin", szValue );

            snprintf( szValue, sizeof(szValue), "%.5f", m_dfXMax );
            CPLCreateXMLElementAndValue( psDSI, "ExtentXMax", szValue );

            snprintf( szValue, sizeof(szValue), "%.5f", m_dfYMin );
            CPLCreateXMLElementAndValue( psDSI, "ExtentYMin", szValue );

            snprintf( szValue, sizeof(szValue), "%.5f", m_dfYMax );
            CPLCreateXMLElementAndValue( psDSI, "ExtentYMax", szValue );
        }

        if( m_pszExtraInfo )
            CPLCreateXMLElementAndValue( psDSI, "ExtraInfo", m_pszExtraInfo );
    }
    
/* -------------------------------------------------------------------- */
/*      emit property information.                                      */
/* -------------------------------------------------------------------- */
    for( iProperty = 0; iProperty < GetPropertyCount(); iProperty++ )
    {
        GMLPropertyDefn *poPDefn = GetProperty( iProperty );
        CPLXMLNode *psPDefnNode;
        const char *pszTypeName = "Unknown";

        psPDefnNode = CPLCreateXMLNode( psRoot, CXT_Element, "PropertyDefn" );
        CPLCreateXMLElementAndValue( psPDefnNode, "Name", 
                                     poPDefn->GetName() );
        CPLCreateXMLElementAndValue( psPDefnNode, "ElementPath", 
                                     poPDefn->GetSrcElement() );
        switch( poPDefn->GetType() )
        {
          case GMLPT_Untyped:
            pszTypeName = "Untyped";
            break;
            
          case GMLPT_String:
            pszTypeName = "String";
            break;
            
          case GMLPT_Integer:
            pszTypeName = "Integer";
            break;
            
          case GMLPT_Real:
            pszTypeName = "Real";
            break;
            
          case GMLPT_Complex:
            pszTypeName = "Complex";
            break;

          case GMLPT_IntegerList:
            pszTypeName = "IntegerList";
            break;

          case GMLPT_RealList:
            pszTypeName = "RealList";
            break;

          case GMLPT_StringList:
            pszTypeName = "StringList";
            break;

          /* should not happen in practise for now because this is not */
          /* autodetected */
          case GMLPT_FeatureProperty:
            pszTypeName = "FeatureProperty";
            break;

          /* should not happen in practise for now because this is not */
          /* autodetected */
          case GMLPT_FeaturePropertyList:
            pszTypeName = "FeaturePropertyList";
            break;
        }
        CPLCreateXMLElementAndValue( psPDefnNode, "Type", pszTypeName );

        if( EQUAL(pszTypeName,"String") )
        {
            char szMaxLength[48];
            sprintf(szMaxLength, "%d", poPDefn->GetWidth());
            CPLCreateXMLElementAndValue ( psPDefnNode, "Width", szMaxLength );
        }
        if( poPDefn->GetWidth() > 0 && EQUAL(pszTypeName,"Integer") )
        {
            char szLength[48];
            sprintf(szLength, "%d", poPDefn->GetWidth());
            CPLCreateXMLElementAndValue ( psPDefnNode, "Width", szLength );
        }
        if( poPDefn->GetWidth() > 0 && EQUAL(pszTypeName,"Real") )
        {
            char szLength[48];
            sprintf(szLength, "%d", poPDefn->GetWidth());
            CPLCreateXMLElementAndValue ( psPDefnNode, "Width", szLength );
            char szPrecision[48];
            sprintf(szPrecision, "%d", poPDefn->GetPrecision());
            CPLCreateXMLElementAndValue ( psPDefnNode, "Precision", szPrecision );
        }
    }

    return psRoot;
}

