/**
 *  file    ConsoleShell.c
 *  date    2009/01/31
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   �ܼ� �п� ���õ� �ҽ� ����
 */

#include "ConsoleShell.h"
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"

// Ŀ�ǵ� ���̺� ����
SHELLCOMMANDENTRY gs_vstCommandTable[] =
{
        { "help", "Show Help", kHelp },
        { "cls", "Clear Screen", kCls },
        { "totalram", "Show Total RAM Size", kShowTotalRAMSize },
        { "strtod", "String To Decial/Hex Convert", kStringToDecimalHexTest },
        { "shutdown", "Shutdown And Reboot OS", kShutdown },
        { "ypcholove", "Dummy1", ypchoLove },
        { "ypchang", "Dummy2", ypchang },
        { "ypkim", "Dummy3", ypkim },
        { "raisefault", "0x1ff000 read or write", kRaiseFault},

};

//==============================================================================
//  ���� ���� �����ϴ� �ڵ�
//==============================================================================
/**
 *  ���� ���� ����
 */
void kStartConsoleShell( void )
{
    char vcCommandBuffer[ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
    int iCommandBufferIndex = 0;
    BYTE bKey;
    int tabCnt=0;
    int iCursorX, iCursorY;
    int iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );

    int hishead = -1;   // head index num
    int hiscur; // index with up down keyboard input
    char history[10][300];
    int hisfull = 0;
    int cursorFirst;
    int count = -1;
    // ������Ʈ ���
    kPrintf( CONSOLESHELL_PROMPTMESSAGE );
    kGetCursor(&iCursorX, &iCursorY);
    cursorFirst = iCursorX;
    while( 1 )
    {
        // Ű�� ���ŵ� ������ ���
        bKey = kGetCh();
        if(bKey!=KEY_TAB) tabCnt=0;
        // Backspace Ű ó��
        if( bKey == KEY_BACKSPACE )
        {
            if( iCommandBufferIndex > 0 )
            {
                // ���� Ŀ�� ��ġ�� �� �� ���� ������ �̵��� ���� ������ ����ϰ�
                // Ŀ�ǵ� ���ۿ��� ������ ���� ����
                kGetCursor( &iCursorX, &iCursorY );
                kPrintStringXY( iCursorX - 1, iCursorY, " " );
                kSetCursor( iCursorX - 1, iCursorY );
                iCommandBufferIndex--;
            }
        }
        // ���� Ű ó��
        else if( bKey == KEY_ENTER )
        {
            kPrintf( "\n" );

            if( iCommandBufferIndex > 0 )
            {
                // Ŀ�ǵ� ���ۿ� �ִ� ������ ����
                vcCommandBuffer[ iCommandBufferIndex ] = '\0';
                count = -1;
                hishead++;
                hisfull++;
                if(hishead > 9)
                  hishead = 0;

                hiscur = hishead + 1;
                int len = kStrLen(vcCommandBuffer);
                kMemSet(history[hishead], '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT);
                for(int i = 0; i < len; i++){
                  history[hishead][i] = vcCommandBuffer[i];
                }
                kExecuteCommand( vcCommandBuffer );
            }

            // ������Ʈ ��� �� Ŀ�ǵ� ���� �ʱ�ȭ
            kPrintf( "%s", CONSOLESHELL_PROMPTMESSAGE );
            kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            iCommandBufferIndex = 0;
        }
        // ����Ʈ Ű, CAPS Lock, NUM Lock, Scroll Lock�� ����
        else if( ( bKey == KEY_LSHIFT ) || ( bKey == KEY_RSHIFT ) ||
                 ( bKey == KEY_CAPSLOCK ) || ( bKey == KEY_NUMLOCK ) ||
                 ( bKey == KEY_SCROLLLOCK ) )
        {
            ;
        }
        else if( bKey == KEY_UP ){

          if(hisfull > 10)
            hisfull = 10;

          if(count+1 == hisfull){ //queue is not full &&
            ;
          }else{

          count++;
          hiscur--;

          if(hiscur <= -1 && hisfull == 10){
            hiscur = 9;
          }

          kGetCursor( &iCursorX, &iCursorY );

          for(int i = cursorFirst; i < iCursorX; i++)
            kPrintStringXY( i, iCursorY, " " );

          kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
          iCommandBufferIndex = 0;

          iCommandBufferIndex = kStrLen(history[hiscur]);

          for(int i = 0; i < iCommandBufferIndex; i++){
            vcCommandBuffer[i] = history[hiscur][i];
          }

        kGetCursor( &iCursorX, &iCursorY );
        kPrintStringXY( cursorFirst, iCursorY, history[hiscur]);
        kSetCursor( cursorFirst + iCommandBufferIndex, iCursorY );
          }

        }else if( bKey == KEY_DOWN){

          if(hisfull > 10)
            hisfull = 10;
          if(count ==  -1){
            ;
          }else if(count == 0){

            hiscur++;
            count--;
            kGetCursor( &iCursorX, &iCursorY );
            for(int i = cursorFirst; i < iCursorX; i++)
              kPrintStringXY( i, iCursorY, " " );
            kSetCursor( cursorFirst, iCursorY );
            kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            iCommandBufferIndex = 0;
          }else{

            hiscur++;
            count--;

            if(hiscur >= 10 && hisfull == 10){
              hiscur = 0;
            }

            kGetCursor( &iCursorX, &iCursorY );
            for(int i = cursorFirst; i < iCursorX; i++)
              kPrintStringXY( i, iCursorY, " " );

            kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            iCommandBufferIndex = 0;

            iCommandBufferIndex = kStrLen(history[hiscur]);
            for(int i = 0; i < iCommandBufferIndex; i++){
              vcCommandBuffer[i] = history[hiscur][i];
            }

            kGetCursor( &iCursorX, &iCursorY );
            kPrintStringXY( cursorFirst, iCursorY, history[hiscur]);
            kSetCursor( cursorFirst + iCommandBufferIndex, iCursorY );
          }



        }else if( bKey == KEY_TAB )
        {
            ++tabCnt;
            if( tabCnt==2 ){
                tabCnt=0;
                kPrintf( "\n" );
                for(int i=0; i<iCount; ++i){
                    if(kMemCmp(vcCommandBuffer,gs_vstCommandTable[i].pcCommand,iCommandBufferIndex) == 0){
                        kPrintf("%s\n",gs_vstCommandTable[i].pcCommand);
                    }
                }
                kPrintf( "%s", CONSOLESHELL_PROMPTMESSAGE );
                kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
                iCommandBufferIndex = 0;
            }
            else{
                int idx;
                int idxCnt=0;
                for(int i=0; i<iCount; ++i){
                    if(kMemCmp(vcCommandBuffer,gs_vstCommandTable[i].pcCommand,iCommandBufferIndex) == 0){
                        ++idxCnt;
                        idx = i;
                    }
                }
                if(idxCnt == 1){
                    int len = kStrLen(gs_vstCommandTable[idx].pcCommand);
                    kGetCursor( &iCursorX, &iCursorY );
                    kSetCursor(iCursorX-iCommandBufferIndex,iCursorY);
                    kMemCpy(vcCommandBuffer,gs_vstCommandTable[idx].pcCommand,len);
                    iCommandBufferIndex = len;
                    kPrintf("%s",gs_vstCommandTable[idx].pcCommand);
                    tabCnt=0;
                }
            }
        }
        else
        {
            // ���ۿ� ������ �������� ���� ����
            if( iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT )
            {
                vcCommandBuffer[ iCommandBufferIndex++ ] = bKey;
                kPrintf( "%c", bKey );
            }
        }
    }
}

/*
 *  Ŀ�ǵ� ���ۿ� �ִ� Ŀ�ǵ带 ���Ͽ� �ش� Ŀ�ǵ带 ó���ϴ� �Լ��� ����
 */
void kExecuteCommand( const char* pcCommandBuffer )
{
    int i, iSpaceIndex;
    int iCommandBufferLength, iCommandLength;
    int iCount;

    // �������� ���е� Ŀ�ǵ带 ����
    iCommandBufferLength = kStrLen( pcCommandBuffer );
    for( iSpaceIndex = 0 ; iSpaceIndex < iCommandBufferLength ; iSpaceIndex++ )
    {
        if( pcCommandBuffer[ iSpaceIndex ] == ' ' )
        {
            break;
        }
    }

    // Ŀ�ǵ� ���̺��� �˻��ؼ� ������ �̸��� Ŀ�ǵ尡 �ִ��� Ȯ��
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );
    for( i = 0 ; i < iCount ; i++ )
    {
        iCommandLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        // Ŀ�ǵ��� ���̿� ������ ������ ��ġ�ϴ��� �˻�
        if( ( iCommandLength == iSpaceIndex ) &&
            ( kMemCmp( gs_vstCommandTable[ i ].pcCommand, pcCommandBuffer,
                       iSpaceIndex ) == 0 ) )
        {
            gs_vstCommandTable[ i ].pfFunction( pcCommandBuffer + iSpaceIndex + 1 );
            break;
        }
    }

    // ����Ʈ���� ã�� �� ���ٸ� ���� ���
    if( i >= iCount )
    {
        kPrintf( "'%s' is not found.\n", pcCommandBuffer );
    }
}

/**
 *  �Ķ���� �ڷᱸ���� �ʱ�ȭ
 */
void kInitializeParameter( PARAMETERLIST* pstList, const char* pcParameter )
{
    pstList->pcBuffer = pcParameter;
    pstList->iLength = kStrLen( pcParameter );
    pstList->iCurrentPosition = 0;
}

/**
 *  �������� ���е� �Ķ������ ����� ���̸� ��ȯ
 */
int kGetNextParameter( PARAMETERLIST* pstList, char* pcParameter )
{
    int i;
    int iLength;

    // �� �̻� �Ķ���Ͱ� ������ ����
    if( pstList->iLength <= pstList->iCurrentPosition )
    {
        return 0;
    }

    // ������ ���̸�ŭ �̵��ϸ鼭 ������ �˻�
    for( i = pstList->iCurrentPosition ; i < pstList->iLength ; i++ )
    {
        if( pstList->pcBuffer[ i ] == ' ' )
        {
            break;
        }
    }

    // �Ķ���͸� �����ϰ� ���̸� ��ȯ
    kMemCpy( pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i );
    iLength = i - pstList->iCurrentPosition;
    pcParameter[ iLength ] = '\0';

    // �Ķ������ ��ġ ������Ʈ
    pstList->iCurrentPosition += iLength + 1;
    return iLength;
}

//==============================================================================
//  Ŀ�ǵ带 ó���ϴ� �ڵ�
//==============================================================================
/**
 *  �� ������ ���
 */
void kHelp( const char* pcCommandBuffer )
{
    int i;
    int iCount;
    int iCursorX, iCursorY;
    int iLength, iMaxCommandLength = 0;


    kPrintf( "=========================================================\n" );
    kPrintf( "                    MINT64 Shell Help                    \n" );
    kPrintf( "=========================================================\n" );

    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );

    // ���� �� Ŀ�ǵ��� ���̸� ���
    for( i = 0 ; i < iCount ; i++ )
    {
        iLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        if( iLength > iMaxCommandLength )
        {
            iMaxCommandLength = iLength;
        }
    }

    // ���� ���
    for( i = 0 ; i < iCount ; i++ )
    {
        kPrintf( "%s", gs_vstCommandTable[ i ].pcCommand );
        kGetCursor( &iCursorX, &iCursorY );
        kSetCursor( iMaxCommandLength, iCursorY );
        kPrintf( "  - %s\n", gs_vstCommandTable[ i ].pcHelp );
    }
}

/**
 *  ȭ���� ����
 */
void kCls( const char* pcParameterBuffer )
{
    // �� ������ ����� ������ ����ϹǷ� ȭ���� ���� ��, ���� 1�� Ŀ�� �̵�
    kClearScreen();
    kSetCursor( 0, 1 );
}

/**
 *  �� �޸� ũ�⸦ ���
 */
void kShowTotalRAMSize( const char* pcParameterBuffer )
{
    kPrintf( "Total RAM Size = %d MB\n", kGetTotalRAMSize() );
}

/**
 *  ���ڿ��� �� ���ڸ� ���ڷ� ��ȯ�Ͽ� ȭ�鿡 ���
 */
void kStringToDecimalHexTest( const char* pcParameterBuffer )
{
    char vcParameter[ 100 ];
    int iLength;
    PARAMETERLIST stList;
    int iCount = 0;
    long lValue;

    // �Ķ���� �ʱ�ȭ
    kInitializeParameter( &stList, pcParameterBuffer );

    while( 1 )
    {
        // ���� �Ķ���͸� ����, �Ķ������ ���̰� 0�̸� �Ķ���Ͱ� ���� ���̹Ƿ�
        // ����
        iLength = kGetNextParameter( &stList, vcParameter );
        if( iLength == 0 )
        {
            break;
        }

        // �Ķ���Ϳ� ���� ������ ����ϰ� 16�������� 10�������� �Ǵ��Ͽ� ��ȯ�� ��
        // ����� printf�� ���
        kPrintf( "Param %d = '%s', Length = %d, ", iCount + 1,
                 vcParameter, iLength );

        // 0x�� �����ϸ� 16����, �׿ܴ� 10������ �Ǵ�
        if( kMemCmp( vcParameter, "0x", 2 ) == 0 )
        {
            lValue = kAToI( vcParameter + 2, 16 );
            kPrintf( "HEX Value = %q\n", lValue );
        }
        else
        {
            lValue = kAToI( vcParameter, 10 );
            kPrintf( "Decimal Value = %d\n", lValue );
        }

        iCount++;
    }
}

/**
 *  PC�� �����(Reboot)
 */
void kShutdown( const char* pcParamegerBuffer )
{
    kPrintf( "System Shutdown Start...\n" );

    // Ű���� ��Ʈ�ѷ��� ���� PC�� �����
    kPrintf( "Press Any Key To Reboot PC..." );
    kGetCh();
    kReboot();
}

void ypchoLove(){
    kPrintf( "We love ypcho!\n" );
}
void ypchang(){
    kPrintf( "Who are you?\n");
}
void ypkim(){
    kPrintf( "Who are you!\n" );
}
void kRaiseFault(){
    QWORD* addr = 0x1FF000;
    //write
    *addr = 0x1994;
    //read
    // DWORD dummy = *addr;
}
