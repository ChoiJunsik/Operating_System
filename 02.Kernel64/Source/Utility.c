/**
 *  file    Utility.h
 *  date    2009/01/17
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   OS���� ����� ��ƿ��Ƽ �Լ��� ���õ� ����
 */

#include "Utility.h"
#include "AssemblyUtility.h"
#include <stdarg.h>
volatile QWORD g_qwTickCount = 0;

/**
 *  �޸𸮸� Ư�� ������ ä��
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
 *  �޸� ����
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
 *  �޸� ��
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
 *  RFLAGS ���������� ���ͷ�Ʈ �÷��׸� �����ϰ� ���� ���ͷ�Ʈ �÷����� ���¸� ��ȯ
 */
BOOL kSetInterruptFlag( BOOL bEnableInterrupt )
{
    QWORD qwRFLAGS;

    // ������ RFLAGS �������� ���� ���� �ڿ� ���ͷ�Ʈ ����/�Ұ� ó��
    qwRFLAGS = kReadRFLAGS();
    if( bEnableInterrupt == TRUE )
    {
        kEnableInterrupt();
    }
    else
    {
        kDisableInterrupt();
    }

    // ���� RFLAGS ���������� IF ��Ʈ(��Ʈ 9)�� Ȯ���Ͽ� ������ ���ͷ�Ʈ ���¸� ��ȯ
    if( qwRFLAGS & 0x0200 )
    {
        return TRUE;
    }
    return FALSE;
}

/**
 *  ���ڿ��� ���̸� ��ȯ
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

// ���� �� ũ��(Mbyte ����)
static gs_qwTotalRAMMBSize = 0;

/**
 *  64Mbyte �̻��� ��ġ���� �� ũ�⸦ üũ
 *      ���� ���� �������� �ѹ��� ȣ���ؾ� ��
 */
void kCheckTotalRAMSize( void )
{
    DWORD* pdwCurrentAddress;
    DWORD dwPreviousValue;
    
    // 64Mbyte(0x4000000)���� 4Mbyte������ �˻� ����
    pdwCurrentAddress = ( DWORD* ) 0x4000000;
    while( 1 )
    {
        // ������ �޸𸮿� �ִ� ���� ����
        dwPreviousValue = *pdwCurrentAddress;
        // 0x12345678�� �Ἥ �о��� �� ������ ���� �������� ��ȿ�� �޸� 
        // �������� ����
        *pdwCurrentAddress = 0x12345678;
        if( *pdwCurrentAddress != 0x12345678 )
        {
            break;
        }
        // ���� �޸� ������ ����
        *pdwCurrentAddress = dwPreviousValue;
        // ���� 4Mbyte ��ġ�� �̵�
        pdwCurrentAddress += ( 0x400000 / 4 );
    }
    // üũ�� ������ ��巹���� 1Mbyte�� ������ Mbyte ������ ���
    gs_qwTotalRAMMBSize = ( QWORD ) pdwCurrentAddress / 0x100000;
}

/**
 *  RAM ũ�⸦ ��ȯ
 */
QWORD kGetTotalRAMSize( void )
{
    return gs_qwTotalRAMMBSize;
}

/**
 *  atoi() �Լ��� ���� ����
 */
long kAToI( const char* pcBuffer, int iRadix )
{
    long lReturn;
    
    switch( iRadix )
    {
        // 16����
    case 16:
        lReturn = kHexStringToQword( pcBuffer );
        break;
        
        // 10���� �Ǵ� ��Ÿ
    case 10:
    default:
        lReturn = kDecimalStringToLong( pcBuffer );
        break;
    }
    return lReturn;
}

/**
 *  16���� ���ڿ��� QWORD�� ��ȯ 
 */
QWORD kHexStringToQword( const char* pcBuffer )
{
    QWORD qwValue = 0;
    int i;
    
    // ���ڿ��� ���鼭 ���ʷ� ��ȯ
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
 *  10���� ���ڿ��� long���� ��ȯ
 */
long kDecimalStringToLong( const char* pcBuffer )
{
    long lValue = 0;
    int i;
    
    // �����̸� -�� �����ϰ� �������� ���� long���� ��ȯ
    if( pcBuffer[ 0 ] == '-' )
    {
        i = 1;
    }
    else
    {
        i = 0;
    }
    
    // ���ڿ��� ���鼭 ���ʷ� ��ȯ
    for( ; pcBuffer[ i ] != '\0' ; i++ )
    {
        lValue *= 10;
        lValue += pcBuffer[ i ] - '0';
    }
    
    // �����̸� - �߰�
    if( pcBuffer[ 0 ] == '-' )
    {
        lValue = -lValue;
    }
    return lValue;
}

/**
 *  itoa() �Լ��� ���� ����
 */
int kIToA( long lValue, char* pcBuffer, int iRadix )
{
    int iReturn;
    
    switch( iRadix )
    {
        // 16����
    case 16:
        iReturn = kHexToString( lValue, pcBuffer );
        break;
        
        // 10���� �Ǵ� ��Ÿ
    case 10:
    default:
        iReturn = kDecimalToString( lValue, pcBuffer );
        break;
    }
    
    return iReturn;
}

/**
 *  16���� ���� ���ڿ��� ��ȯ
 */
int kHexToString( QWORD qwValue, char* pcBuffer )
{
    QWORD i;
    QWORD qwCurrentValue;

    // 0�� ������ �ٷ� ó��
    if( qwValue == 0 )
    {
        pcBuffer[ 0 ] = '0';
        pcBuffer[ 1 ] = '\0';
        return 1;
    }
    
    // ���ۿ� 1�� �ڸ����� 16, 256, ...�� �ڸ� ������ ���� ����
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
    
    // ���ۿ� ����ִ� ���ڿ��� ����� ... 256, 16, 1�� �ڸ� ������ ����
    kReverseString( pcBuffer );
    return i;
}

/**
 *  10���� ���� ���ڿ��� ��ȯ
 */
int kDecimalToString( long lValue, char* pcBuffer )
{
    long i;

    // 0�� ������ �ٷ� ó��
    if( lValue == 0 )
    {
        pcBuffer[ 0 ] = '0';
        pcBuffer[ 1 ] = '\0';
        return 1;
    }
    
    // ���� �����̸� ��� ���ۿ� '-'�� �߰��ϰ� ����� ��ȯ
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

    // ���ۿ� 1�� �ڸ����� 10, 100, 1000 ...�� �ڸ� ������ ���� ����
    for( ; lValue > 0 ; i++ )
    {
        pcBuffer[ i ] = '0' + lValue % 10;        
        lValue = lValue / 10;
    }
    pcBuffer[ i ] = '\0';
    
    // ���ۿ� ����ִ� ���ڿ��� ����� ... 1000, 100, 10, 1�� �ڸ� ������ ����
    if( pcBuffer[ 0 ] == '-' )
    {
        // ������ ���� ��ȣ�� �����ϰ� ���ڿ��� ������
        kReverseString( &( pcBuffer[ 1 ] ) );
    }
    else
    {
        kReverseString( pcBuffer );
    }
    
    return i;
}

/**
 *  ���ڿ��� ������ ������
 */
void kReverseString( char* pcBuffer )
{
   int iLength;
   int i;
   char cTemp;
   
   
   // ���ڿ��� ����� �߽����� ��/�츦 �ٲ㼭 ������ ������
   iLength = kStrLen( pcBuffer );
   for( i = 0 ; i < iLength / 2 ; i++ )
   {
       cTemp = pcBuffer[ i ];
       pcBuffer[ i ] = pcBuffer[ iLength - 1 - i ];
       pcBuffer[ iLength - 1 - i ] = cTemp;
   }
}

/**
 *  sprintf() �Լ��� ���� ����
 */
int kSPrintf( char* pcBuffer, const char* pcFormatString, ... )
{
    va_list ap;
    int iReturn;
    
    // ���� ���ڸ� ������ vsprintf() �Լ��� �Ѱ���
    va_start( ap, pcFormatString );
    iReturn = kVSPrintf( pcBuffer, pcFormatString, ap );
    va_end( ap );
    
    return iReturn;
}

/**
 *  vsprintf() �Լ��� ���� ����
 *      ���ۿ� ���� ���ڿ��� ���� �����͸� ����
 */
int kVSPrintf( char* pcBuffer, const char* pcFormatString, va_list ap )
{
    QWORD i, j;
    int iBufferIndex = 0;
    int iFormatLength, iCopyLength;
    char* pcCopyString;
    QWORD qwValue;
    int iValue;
    
    // ���� ���ڿ��� ���̸� �о ���ڿ��� ���̸�ŭ �����͸� ��� ���ۿ� ���
    iFormatLength = kStrLen( pcFormatString );
    for( i = 0 ; i < iFormatLength ; i++ ) 
    {
        // %�� �����ϸ� ������ Ÿ�� ���ڷ� ó��
        if( pcFormatString[ i ] == '%' ) 
        {
            // % ������ ���ڷ� �̵�
            i++;
            switch( pcFormatString[ i ] ) 
            {
                // ���ڿ� ���  
            case 's':
                // ���� ���ڿ� ����ִ� �Ķ���͸� ���ڿ� Ÿ������ ��ȯ
                pcCopyString = ( char* ) ( va_arg(ap, char* ));
                iCopyLength = kStrLen( pcCopyString );
                // ���ڿ��� ���̸�ŭ�� ��� ���۷� �����ϰ� ����� ���̸�ŭ 
                // ������ �ε����� �̵�
                kMemCpy( pcBuffer + iBufferIndex, pcCopyString, iCopyLength );
                iBufferIndex += iCopyLength;
                break;
                
                // ���� ���
            case 'c':
                // ���� ���ڿ� ����ִ� �Ķ���͸� ���� Ÿ������ ��ȯ�Ͽ� 
                // ��� ���ۿ� �����ϰ� ������ �ε����� 1��ŭ �̵�
                pcBuffer[ iBufferIndex ] = ( char ) ( va_arg( ap, int ) );
                iBufferIndex++;
                break;

                // ���� ���
            case 'd':
            case 'i':
                // ���� ���ڿ� ����ִ� �Ķ���͸� ���� Ÿ������ ��ȯ�Ͽ�
                // ��� ���ۿ� �����ϰ� ����� ���̸�ŭ ������ �ε����� �̵�
                iValue = ( int ) ( va_arg( ap, int ) );
                iBufferIndex += kIToA( iValue, pcBuffer + iBufferIndex, 10 );
                break;
                
                // 4����Ʈ Hex ���
            case 'x':
            case 'X':
                // ���� ���ڿ� ����ִ� �Ķ���͸� DWORD Ÿ������ ��ȯ�Ͽ�
                // ��� ���ۿ� �����ϰ� ����� ���̸�ŭ ������ �ε����� �̵�
                qwValue = ( DWORD ) ( va_arg( ap, DWORD ) ) & 0xFFFFFFFF;
                iBufferIndex += kIToA( qwValue, pcBuffer + iBufferIndex, 16 );
                break;

                // 8����Ʈ Hex ���
            case 'q':
            case 'Q':
            case 'p':
                // ���� ���ڿ� ����ִ� �Ķ���͸� QWORD Ÿ������ ��ȯ�Ͽ�
                // ��� ���ۿ� �����ϰ� ����� ���̸�ŭ ������ �ε����� �̵�
                qwValue = ( QWORD ) ( va_arg( ap, QWORD ) );
                iBufferIndex += kIToA( qwValue, pcBuffer + iBufferIndex, 16 );
                break;
            
                // ���� �ش����� ������ ���ڸ� �״�� ����ϰ� ������ �ε�����
                // 1��ŭ �̵�
            default:
                pcBuffer[ iBufferIndex ] = pcFormatString[ i ];
                iBufferIndex++;
                break;
            }
        } 
        // �Ϲ� ���ڿ� ó��
        else
        {
            // ���ڸ� �״�� ����ϰ� ������ �ε����� 1��ŭ �̵�
            pcBuffer[ iBufferIndex ] = pcFormatString[ i ];
            iBufferIndex++;
        }
    }
    
    // NULL�� �߰��Ͽ� ������ ���ڿ��� ����� ����� ������ ���̸� ��ȯ
    pcBuffer[ iBufferIndex ] = '\0';
    return iBufferIndex;
}
