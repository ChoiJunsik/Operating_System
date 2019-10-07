#include "Types.h"

// 함수 선언
void kPrintString( int iX, int iY, const char* pcString );
BOOL canRead(DWORD* addr);
BOOL canWrite(DWORD* addr);
/**
 *  아래 함수는 C 언어 커널의 시작 부분임
 */
void Main( void )
{
    kPrintString( 0, 12, "Switch To IA-32e Mode Success~!!" );
    kPrintString( 0, 13, "IA-32e C Language Kernel Start..............[Pass]" );
    kPrintString( 0, 14, "This message is printed through the video memory relocated to 0xAB8000" );
    if(canRead((DWORD*)0x1fe000)){
        kPrintString( 0, 15, "Read from 0x1fe000 [Ok]" );
    }else kPrintString( 0, 15, "Read from 0x1fe000 [No]" );
    if(canWrite((DWORD*)0x1fe000)){
        kPrintString( 0, 16, "Write to 0x1fe000 [Ok]" );
    }else kPrintString( 0, 16, "Write to 0x1fe000 [No]" );
    if(canRead((DWORD*)0x1FF000)){
        kPrintString( 0, 17, "Read from 0x1ff000 [Ok]" );
    }else kPrintString( 0, 17, "Read from 0x1ff000 [No]" );
    // if(canWrite((DWORD*)0x1FF000)){
    //     kPrintString( 0, 18, "Write to 0x1ff000 [Ok]" );
    // }else kPrintString( 0, 18, "Write to 0x1ff000 [No]" );
}
BOOL canRead(DWORD* addr){
    DWORD val = 0x1994;
    val = *addr;
    if(val==0x1994){
        return FALSE;
    }
    return TRUE;
}
BOOL canWrite(DWORD* addr){
    DWORD temp = *addr;
    *addr = 0x1994;
    if(*addr==0x1994){
        return TRUE;
    }
    *addr = temp;
    return FALSE;
}
/**
 *  문자열을 X, Y 위치에 출력
 */
void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;
    
    // X, Y 좌표를 이용해서 문자열을 출력할 어드레스를 계산
    pstScreen += ( iY * 80 ) + iX;

    // NULL이 나올 때까지 문자열 출력
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}
void kPrintString2( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xAB8000;
    int i;
    
    // X, Y 좌표를 이용해서 문자열을 출력할 어드레스를 계산
    pstScreen += ( iY * 80 ) + iX;

    // NULL이 나올 때까지 문자열 출력
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}
