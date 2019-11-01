#include "Types.h"
#include "Keyboard.h"
#include "Descriptor.h"
#include "PIC.h"
// 함수 선언
void kPrintString( int iX, int iY, const char* pcString );
BOOL canRead(DWORD* addr);
BOOL canWrite(DWORD* addr);
/**
 *  아래 함수는 C 언어 커널의 시작 부분임
 */
void Main( void )
{
    char vcTemp[ 2 ] = { 0, };
    BYTE bFlags;
    BYTE bTemp;
    int i = 0;
    KEYDATA stData;

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
    kPrintString( 0, 19, "GDT Initialize And Switch For IA-32e Mode...[    ]" );
    kInitializeGDTTableAndTSS();
    kLoadGDTR( GDTR_STARTADDRESS );
    kPrintString( 45, 19, "Pass" );
    
    kPrintString( 0, 20, "TSS Segment Load............................[    ]" );
    kLoadTR( GDT_TSSSEGMENT );
    kPrintString( 45, 20, "Pass" );
    
    kPrintString( 0, 21, "IDT Initialize..............................[    ]" );
    kInitializeIDTTables();    
    kLoadIDTR( IDTR_STARTADDRESS );
    kPrintString( 45, 21, "Pass" );
    kPrintString( 0, 22, "Keyboard Activate And Queue Initialize......[    ]" );
    // 키보드를 활성화
    if( kInitializeKeyboard() == TRUE )
    {
        kPrintString( 45, 22, "Pass" );
        kChangeKeyboardLED( FALSE, FALSE, FALSE );
    }
    else
    {
        kPrintString( 45, 22, "Fail" );
        while( 1 ) ;
    }
    kPrintString( 0, 23, "PIC Controller And Interrupt Initialize.....[    ]" );
    // PIC 컨트롤러 초기화 및 모든 인터럽트 활성화
    kInitializePIC();
    kMaskPICInterrupt( 0 );
    kEnableInterrupt();
    kPrintString( 45, 23, "Pass" );    
    while( 1 )
    {
        // 키 큐에 데이터가 있으면 키를 처리함
        if( kGetKeyFromKeyQueue( &stData ) == TRUE )
        {
            // 키가 눌러졌으면 키의 ASCII 코드 값을 화면에 출력
            if( stData.bFlags & KEY_FLAGS_DOWN )
            {
                // 키 데이터의 ACII 코드 값을 저장
                vcTemp[ 0 ] = stData.bASCIICode;
                kPrintString( i++, 24, vcTemp );

                // 0이 입력되면 변수를 0으로 나누어 Divide Error 예외(벡터 0번)을
                // 발생시킴
                if( vcTemp[ 0 ] == '0' )
                {
                    // 아래 코드를 수행하면 Divide Error 예외가 발생하여
                    // 커널의 임시 핸들러가 수행됨
                    bTemp = bTemp / 0;
                }
            }
        }
    }
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
