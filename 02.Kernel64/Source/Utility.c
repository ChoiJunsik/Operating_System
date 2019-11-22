/**
 *  file    Utility.h
 *  date    2009/01/17
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   OS占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙틸占쏙옙티 占쌉쇽옙占쏙옙 占쏙옙占시듸옙 占쏙옙占쏙옙
 */

#include "Utility.h"
#include "AssemblyUtility.h"
#include <stdarg.h>


// PIT 而⑦몃·�ш� 諛�� ��瑜� ���ν� 移댁댄�
volatile QWORD g_qwTickCount = 0;

/**
*  硫紐⑤━瑜� �뱀� 媛�쇰� 梨�
*/
void kMemSet( void* pvDestination, BYTE bData, int iSize )
{
    int i;

    for( i = 0 ; i < iSize ; i++ )
    {
        ( ( char* ) pvDestination )[ i ] = bData;
    }
}

/**
 *  占쌨몌옙占쏙옙 占쏙옙占쏙옙
 */
int kMemCpy( void* pvDestination, const void* pvSource, int iSize )
{
    int i;

    for( i = 0 ; i < iSize ; i++ )
    {
        ( ( char* ) pvDestination )[ i ] = ( ( char* ) pvSource )[ i ];
    }

    return iSize;
}

/**
 *  占쌨몌옙占쏙옙 占쏙옙占쏙옙
 */
int kMemCmp( const void* pvDestination, const void* pvSource, int iSize )
{
    int i;
    char cTemp;

    for( i = 0 ; i < iSize ; i++ )
    {
        cTemp = ( ( char* ) pvDestination )[ i ] - ( ( char* ) pvSource )[ i ];
        if( cTemp != 0 )
        {
            return ( int ) cTemp;
        }
    }
    return 0;
}

/**
 *  RFLAGS 占쏙옙占쏙옙占쏙옙占쏙옙占쏙옙 占쏙옙占싶뤄옙트 占시뤄옙占쌓몌옙 占쏙옙占쏙옙占싹곤옙 占쏙옙占쏙옙 占쏙옙占싶뤄옙트 占시뤄옙占쏙옙占쏙옙 占쏙옙占승몌옙 占쏙옙환
 */
BOOL kSetInterruptFlag( BOOL bEnableInterrupt )
{
    QWORD qwRFLAGS;

    // 占쏙옙占쏙옙占쏙옙 RFLAGS 占쏙옙占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占쌘울옙 占쏙옙占싶뤄옙트 占쏙옙占쏙옙/占쌀곤옙 처占쏙옙
    qwRFLAGS = kReadRFLAGS();
    if( bEnableInterrupt == TRUE )
    {
        kEnableInterrupt();
    }
    else
    {
        kDisableInterrupt();
    }

    // 占쏙옙占쏙옙 RFLAGS 占쏙옙占쏙옙占쏙옙占쏙옙占쏙옙 IF 占쏙옙트(占쏙옙트 9)占쏙옙 확占쏙옙占싹울옙 占쏙옙占쏙옙占쏙옙 占쏙옙占싶뤄옙트 占쏙옙占승몌옙 占쏙옙환
    if( qwRFLAGS & 0x0200 )
    {
        return TRUE;
    }
    return FALSE;
}

/**
 *  占쏙옙占쌘울옙占쏙옙 占쏙옙占싱몌옙 占쏙옙환
 */
int kStrLen( const char* pcBuffer )
{
    int i;

    for( i = 0 ; ; i++ )
    {
        if( pcBuffer[ i ] == '\0' )
        {
            break;
        }
    }
    return i;
}

// 占쏙옙占쏙옙 占쏙옙 크占쏙옙(Mbyte 占쏙옙占쏙옙)
static gs_qwTotalRAMMBSize = 0;

/**
 *  64Mbyte 占싱삼옙占쏙옙 占쏙옙치占쏙옙占쏙옙 占쏙옙 크占썩를 체크
 *      占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙占쏙옙 占싼뱄옙占쏙옙 호占쏙옙占쌔억옙 占쏙옙
 */
void kCheckTotalRAMSize( void )
{
    DWORD* pdwCurrentAddress;
    DWORD dwPreviousValue;

    // 64Mbyte(0x4000000)占쏙옙占쏙옙 4Mbyte占쏙옙占쏙옙占쏙옙 占싯삼옙 占쏙옙占쏙옙
    pdwCurrentAddress = ( DWORD* ) 0x4000000;
    while( 1 )
    {
        // 占쏙옙占쏙옙占쏙옙 占쌨모리울옙 占쌍댐옙 占쏙옙占쏙옙 占쏙옙占쏙옙
        dwPreviousValue = *pdwCurrentAddress;
        // 0x12345678占쏙옙 占써서 占싻억옙占쏙옙 占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙占쏙옙 占쏙옙효占쏙옙 占쌨몌옙占쏙옙
        // 占쏙옙占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙
        *pdwCurrentAddress = 0x12345678;
        if( *pdwCurrentAddress != 0x12345678 )
        {
            break;
        }
        // 占쏙옙占쏙옙 占쌨몌옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙
        *pdwCurrentAddress = dwPreviousValue;
        // 占쏙옙占쏙옙 4Mbyte 占쏙옙치占쏙옙 占싱듸옙
        pdwCurrentAddress += ( 0x400000 / 4 );
    }
    // 체크占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占썲레占쏙옙占쏙옙 1Mbyte占쏙옙 占쏙옙占쏙옙占쏙옙 Mbyte 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙
    gs_qwTotalRAMMBSize = ( QWORD ) pdwCurrentAddress / 0x100000;
}

/**
 *  RAM 크占썩를 占쏙옙환
 */
QWORD kGetTotalRAMSize( void )
{
    return gs_qwTotalRAMMBSize;
}

/**
 *  atoi() 占쌉쇽옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙
 */
long kAToI( const char* pcBuffer, int iRadix )
{
    long lReturn;

    switch( iRadix )
    {
        // 16占쏙옙占쏙옙
    case 16:
        lReturn = kHexStringToQword( pcBuffer );
        break;

        // 10占쏙옙占쏙옙 占실댐옙 占쏙옙타
    case 10:
    default:
        lReturn = kDecimalStringToLong( pcBuffer );
        break;
    }
    return lReturn;
}

/**
 *  16占쏙옙占쏙옙 占쏙옙占쌘울옙占쏙옙 QWORD占쏙옙 占쏙옙환
 */
QWORD kHexStringToQword( const char* pcBuffer )
{
    QWORD qwValue = 0;
    int i;

    // 占쏙옙占쌘울옙占쏙옙 占쏙옙占썽서 占쏙옙占십뤄옙 占쏙옙환
    for( i = 0 ; pcBuffer[ i ] != '\0' ; i++ )
    {
        qwValue *= 16;
        if( ( 'A' <= pcBuffer[ i ] )  && ( pcBuffer[ i ] <= 'Z' ) )
        {
            qwValue += ( pcBuffer[ i ] - 'A' ) + 10;
        }
        else if( ( 'a' <= pcBuffer[ i ] )  && ( pcBuffer[ i ] <= 'z' ) )
        {
            qwValue += ( pcBuffer[ i ] - 'a' ) + 10;
        }
        else
        {
            qwValue += pcBuffer[ i ] - '0';
        }
    }
    return qwValue;
}

/**
 *  10占쏙옙占쏙옙 占쏙옙占쌘울옙占쏙옙 long占쏙옙占쏙옙 占쏙옙환
 */
long kDecimalStringToLong( const char* pcBuffer )
{
    long lValue = 0;
    int i;

    // 占쏙옙占쏙옙占싱몌옙 -占쏙옙 占쏙옙占쏙옙占싹곤옙 占쏙옙占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙 long占쏙옙占쏙옙 占쏙옙환
    if( pcBuffer[ 0 ] == '-' )
    {
        i = 1;
    }
    else
    {
        i = 0;
    }

    // 占쏙옙占쌘울옙占쏙옙 占쏙옙占썽서 占쏙옙占십뤄옙 占쏙옙환
    for( ; pcBuffer[ i ] != '\0' ; i++ )
    {
        lValue *= 10;
        lValue += pcBuffer[ i ] - '0';
    }

    // 占쏙옙占쏙옙占싱몌옙 - 占쌩곤옙
    if( pcBuffer[ 0 ] == '-' )
    {
        lValue = -lValue;
    }
    return lValue;
}

/**
 *  itoa() 占쌉쇽옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙
 */
int kIToA( long lValue, char* pcBuffer, int iRadix )
{
    int iReturn;

    switch( iRadix )
    {
        // 16占쏙옙占쏙옙
    case 16:
        iReturn = kHexToString( lValue, pcBuffer );
        break;

        // 10占쏙옙占쏙옙 占실댐옙 占쏙옙타
    case 10:
    default:
        iReturn = kDecimalToString( lValue, pcBuffer );
        break;
    }

    return iReturn;
}

/**
 *  16占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쌘울옙占쏙옙 占쏙옙환
 */
int kHexToString( QWORD qwValue, char* pcBuffer )
{
    QWORD i;
    QWORD qwCurrentValue;

    // 0占쏙옙 占쏙옙占쏙옙占쏙옙占쏙옙 占쌕뤄옙 처占쏙옙
    if( qwValue == 0 )
    {
        pcBuffer[ 0 ] = '0';
        pcBuffer[ 1 ] = '\0';
        return 1;
    }

    // 占쏙옙占쌜울옙 1占쏙옙 占쌘몌옙占쏙옙占쏙옙 16, 256, ...占쏙옙 占쌘몌옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙
    for( i = 0 ; qwValue > 0 ; i++ )
    {
        qwCurrentValue = qwValue % 16;
        if( qwCurrentValue >= 10 )
        {
            pcBuffer[ i ] = 'A' + ( qwCurrentValue - 10 );
        }
        else
        {
            pcBuffer[ i ] = '0' + qwCurrentValue;
        }

        qwValue = qwValue / 16;
    }
    pcBuffer[ i ] = '\0';

    // 占쏙옙占쌜울옙 占쏙옙占쏙옙占쌍댐옙 占쏙옙占쌘울옙占쏙옙 占쏙옙占쏙옙占쏘서 ... 256, 16, 1占쏙옙 占쌘몌옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙
    kReverseString( pcBuffer );
    return i;
}

/**
 *  10占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쌘울옙占쏙옙 占쏙옙환
 */
int kDecimalToString( long lValue, char* pcBuffer )
{
    long i;

    // 0占쏙옙 占쏙옙占쏙옙占쏙옙占쏙옙 占쌕뤄옙 처占쏙옙
    if( lValue == 0 )
    {
        pcBuffer[ 0 ] = '0';
        pcBuffer[ 1 ] = '\0';
        return 1;
    }

    // 占쏙옙占쏙옙 占쏙옙占쏙옙占싱몌옙 占쏙옙占쏙옙 占쏙옙占쌜울옙 '-'占쏙옙 占쌩곤옙占싹곤옙 占쏙옙占쏙옙占쏙옙 占쏙옙환
    if( lValue < 0 )
    {
        i = 1;
        pcBuffer[ 0 ] = '-';
        lValue = -lValue;
    }
    else
    {
        i = 0;
    }

    // 占쏙옙占쌜울옙 1占쏙옙 占쌘몌옙占쏙옙占쏙옙 10, 100, 1000 ...占쏙옙 占쌘몌옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙
    for( ; lValue > 0 ; i++ )
    {
        pcBuffer[ i ] = '0' + lValue % 10;
        lValue = lValue / 10;
    }
    pcBuffer[ i ] = '\0';

    // 占쏙옙占쌜울옙 占쏙옙占쏙옙占쌍댐옙 占쏙옙占쌘울옙占쏙옙 占쏙옙占쏙옙占쏘서 ... 1000, 100, 10, 1占쏙옙 占쌘몌옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙
    if( pcBuffer[ 0 ] == '-' )
    {
        // 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙호占쏙옙 占쏙옙占쏙옙占싹곤옙 占쏙옙占쌘울옙占쏙옙 占쏙옙占쏙옙占쏙옙
        kReverseString( &( pcBuffer[ 1 ] ) );
    }
    else
    {
        kReverseString( pcBuffer );
    }

    return i;
}

/**
 *  占쏙옙占쌘울옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙
 */
void kReverseString( char* pcBuffer )
{
   int iLength;
   int i;
   char cTemp;


   // 占쏙옙占쌘울옙占쏙옙 占쏙옙占쏘데占쏙옙 占쌩쏙옙占쏙옙占쏙옙 占쏙옙/占쎌를 占쌕꿔서 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙
   iLength = kStrLen( pcBuffer );
   for( i = 0 ; i < iLength / 2 ; i++ )
   {
       cTemp = pcBuffer[ i ];
       pcBuffer[ i ] = pcBuffer[ iLength - 1 - i ];
       pcBuffer[ iLength - 1 - i ] = cTemp;
   }
}

/**
 *  sprintf() 占쌉쇽옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙
 */
int kSPrintf( char* pcBuffer, const char* pcFormatString, ... )
{
    va_list ap;
    int iReturn;

    // 占쏙옙占쏙옙 占쏙옙占쌘몌옙 占쏙옙占쏙옙占쏙옙 vsprintf() 占쌉쇽옙占쏙옙 占싼곤옙占쏙옙
    va_start( ap, pcFormatString );
    iReturn = kVSPrintf( pcBuffer, pcFormatString, ap );
    va_end( ap );

    return iReturn;
}

/**
 *  vsprintf() 占쌉쇽옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙
 *      占쏙옙占쌜울옙 占쏙옙占쏙옙 占쏙옙占쌘울옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙占싶몌옙 占쏙옙占쏙옙
 */
int kVSPrintf( char* pcBuffer, const char* pcFormatString, va_list ap )
{
    QWORD i, j;
    int iBufferIndex = 0;
    int iFormatLength, iCopyLength;
    char* pcCopyString;
    QWORD qwValue;
    int iValue;

    // 占쏙옙占쏙옙 占쏙옙占쌘울옙占쏙옙 占쏙옙占싱몌옙 占싻어서 占쏙옙占쌘울옙占쏙옙 占쏙옙占싱몌옙큼 占쏙옙占쏙옙占싶몌옙 占쏙옙占쏙옙 占쏙옙占쌜울옙 占쏙옙占쏙옙
    iFormatLength = kStrLen( pcFormatString );
    for( i = 0 ; i < iFormatLength ; i++ )
    {
        // %占쏙옙 占쏙옙占쏙옙占싹몌옙 占쏙옙占쏙옙占쏙옙 타占쏙옙 占쏙옙占쌘뤄옙 처占쏙옙
        if( pcFormatString[ i ] == '%' )
        {
            // % 占쏙옙占쏙옙占쏙옙 占쏙옙占쌘뤄옙 占싱듸옙
            i++;
            switch( pcFormatString[ i ] )
            {
                // 占쏙옙占쌘울옙 占쏙옙占쏙옙
            case 's':
                // 占쏙옙占쏙옙 占쏙옙占쌘울옙 占쏙옙占쏙옙占쌍댐옙 占식띰옙占쏙옙占싶몌옙 占쏙옙占쌘울옙 타占쏙옙占쏙옙占쏙옙 占쏙옙환
                pcCopyString = ( char* ) ( va_arg(ap, char* ));
                iCopyLength = kStrLen( pcCopyString );
                // 占쏙옙占쌘울옙占쏙옙 占쏙옙占싱몌옙큼占쏙옙 占쏙옙占쏙옙 占쏙옙占쌜뤄옙 占쏙옙占쏙옙占싹곤옙 占쏙옙占쏙옙占쏙옙 占쏙옙占싱몌옙큼
                // 占쏙옙占쏙옙占쏙옙 占싸듸옙占쏙옙占쏙옙 占싱듸옙
                kMemCpy( pcBuffer + iBufferIndex, pcCopyString, iCopyLength );
                iBufferIndex += iCopyLength;
                break;

                // 占쏙옙占쏙옙 占쏙옙占쏙옙
            case 'c':
                // 占쏙옙占쏙옙 占쏙옙占쌘울옙 占쏙옙占쏙옙占쌍댐옙 占식띰옙占쏙옙占싶몌옙 占쏙옙占쏙옙 타占쏙옙占쏙옙占쏙옙 占쏙옙환占싹울옙
                // 占쏙옙占쏙옙 占쏙옙占쌜울옙 占쏙옙占쏙옙占싹곤옙 占쏙옙占쏙옙占쏙옙 占싸듸옙占쏙옙占쏙옙 1占쏙옙큼 占싱듸옙
                pcBuffer[ iBufferIndex ] = ( char ) ( va_arg( ap, int ) );
                iBufferIndex++;
                break;

                // 占쏙옙占쏙옙 占쏙옙占쏙옙
            case 'd':
            case 'i':
                // 占쏙옙占쏙옙 占쏙옙占쌘울옙 占쏙옙占쏙옙占쌍댐옙 占식띰옙占쏙옙占싶몌옙 占쏙옙占쏙옙 타占쏙옙占쏙옙占쏙옙 占쏙옙환占싹울옙
                // 占쏙옙占쏙옙 占쏙옙占쌜울옙 占쏙옙占쏙옙占싹곤옙 占쏙옙占쏙옙占쏙옙 占쏙옙占싱몌옙큼 占쏙옙占쏙옙占쏙옙 占싸듸옙占쏙옙占쏙옙 占싱듸옙
                iValue = ( int ) ( va_arg( ap, int ) );
                iBufferIndex += kIToA( iValue, pcBuffer + iBufferIndex, 10 );
                break;

                // 4占쏙옙占쏙옙트 Hex 占쏙옙占쏙옙
            case 'x':
            case 'X':
                // 占쏙옙占쏙옙 占쏙옙占쌘울옙 占쏙옙占쏙옙占쌍댐옙 占식띰옙占쏙옙占싶몌옙 DWORD 타占쏙옙占쏙옙占쏙옙 占쏙옙환占싹울옙
                // 占쏙옙占쏙옙 占쏙옙占쌜울옙 占쏙옙占쏙옙占싹곤옙 占쏙옙占쏙옙占쏙옙 占쏙옙占싱몌옙큼 占쏙옙占쏙옙占쏙옙 占싸듸옙占쏙옙占쏙옙 占싱듸옙
                qwValue = ( DWORD ) ( va_arg( ap, DWORD ) ) & 0xFFFFFFFF;
                iBufferIndex += kIToA( qwValue, pcBuffer + iBufferIndex, 16 );
                break;

                // 8占쏙옙占쏙옙트 Hex 占쏙옙占쏙옙
            case 'q':
            case 'Q':
            case 'p':
                // 占쏙옙占쏙옙 占쏙옙占쌘울옙 占쏙옙占쏙옙占쌍댐옙 占식띰옙占쏙옙占싶몌옙 QWORD 타占쏙옙占쏙옙占쏙옙 占쏙옙환占싹울옙
                // 占쏙옙占쏙옙 占쏙옙占쌜울옙 占쏙옙占쏙옙占싹곤옙 占쏙옙占쏙옙占쏙옙 占쏙옙占싱몌옙큼 占쏙옙占쏙옙占쏙옙 占싸듸옙占쏙옙占쏙옙 占싱듸옙
                qwValue = ( QWORD ) ( va_arg( ap, QWORD ) );
                iBufferIndex += kIToA( qwValue, pcBuffer + iBufferIndex, 16 );
                break;

                // 占쏙옙占쏙옙 占쌔댐옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쌘몌옙 占쌓댐옙占쏙옙 占쏙옙占쏙옙占싹곤옙 占쏙옙占쏙옙占쏙옙 占싸듸옙占쏙옙占쏙옙
                // 1占쏙옙큼 占싱듸옙
            default:
                pcBuffer[ iBufferIndex ] = pcFormatString[ i ];
                iBufferIndex++;
                break;
            }
        }
        // 占싹뱄옙 占쏙옙占쌘울옙 처占쏙옙
        else
        {
            // 占쏙옙占쌘몌옙 占쌓댐옙占쏙옙 占쏙옙占쏙옙占싹곤옙 占쏙옙占쏙옙占쏙옙 占싸듸옙占쏙옙占쏙옙 1占쏙옙큼 占싱듸옙
            pcBuffer[ iBufferIndex ] = pcFormatString[ i ];
            iBufferIndex++;
        }
    }

    // NULL占쏙옙 占쌩곤옙占싹울옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쌘울옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占싱몌옙 占쏙옙환
    pcBuffer[ iBufferIndex ] = '\0';
    return iBufferIndex;
}

/**
 *  Tick Count를 반환
 */
QWORD kGetTickCount( void )
{
    return g_qwTickCount;
}

/**
 *  밀리세컨드(milisecond) 동안 대기
 */
void kSleep( QWORD qwMillisecond )
{
    QWORD qwLastTickCount;

    qwLastTickCount = g_qwTickCount;

    while( ( g_qwTickCount - qwLastTickCount ) <= qwMillisecond )
    {
        kSchedule();
    }
}
