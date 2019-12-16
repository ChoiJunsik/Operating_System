/**
 *  file    ConsoleShell.c
 *  date    2009/01/31
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ??? ?��? ????? ??? ????
 */

 #include "ConsoleShell.h"
 #include "Console.h"
 #include "Keyboard.h"
 #include "Utility.h"
 #include "PIT.h"
 #include "RTC.h"
 #include "AssemblyUtility.h"
 #include "Task.h"
 #include "Synchronization.h"
 #include "DynamicMemory.h"
 #include "HardDisk.h"
 #include "FileSystem.h"


// ��??? ????? ????
char pwd[150] = "/";
DWORD curDirectoryClusterIdx = 0;
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
        { "settimer", "Set PIT Controller Counter0, ex)settimer 10(ms) 1(periodic)",
                kSetTimer },
        { "wait", "Wait ms Using PIT, ex)wait 100(ms)", kWaitUsingPIT },
        { "rdtsc", "Read Time Stamp Counter", kReadTimeStampCounter },
        { "cpuspeed", "Measure Processor Speed", kMeasureProcessorSpeed },
        { "date", "Show Date And Time", kShowDateAndTime },
        { "createtask", "Create Task, ex)createtask 1(type) 10(count)", kCreateTestTask },
        { "changepriority", "Change Task Priority, ex)changepriority 1(ID) 2(Priority)",
                kChangeTaskPriority },
        { "tasklist", "Show Task List", kShowTaskList },
        { "killtask", "End Task, ex)killtask 1(ID) or 0xffffffff(All Task)", kKillTask },
        { "cpuload", "Show Processor Load", kCPULoad },
        { "testmutex", "Test Mutex Function", kTestMutex },
        { "rand", "Random Generator", kRand },
        { "testthread", "Test Thread And Process Function", kTestThread },
        { "showmatrix", "Show Matrix Screen", kShowMatrix },
        { "testpie", "Test PIE Calculation", kTestPIE },
        { "dynamicmeminfo", "Show Dyanmic Memory Information", kShowDyanmicMemoryInformation },
        { "testseqalloc", "Test Sequential Allocation & Free", kTestSequentialAllocation },
        { "testranalloc", "Test Random Allocation & Free", kTestRandomAllocation },
        { "hddinfo", "Show HDD Information", kShowHDDInformation },
        { "readsector", "Read HDD Sector, ex)readsector 0(LBA) 10(count)",
                kReadSector },
        { "writesector", "Write HDD Sector, ex)writesector 0(LBA) 10(count)",
                kWriteSector },
        { "mounthdd", "Mount HDD", kMountHDD },
        { "formathdd", "Format HDD", kFormatHDD },
        { "filesysteminfo", "Show File System Information", kShowFileSystemInformation },
        { "createfile", "Create File, ex)createfile a.txt", kCreateFileInRootDirectory },
        { "deletefile", "Delete File, ex)deletefile a.txt", kDeleteFileInRootDirectory },
        { "dir", "Show Directory", kShowRootDirectory },
        { "writefile", "Write Data To File, ex) writefile a.txt", kWriteDataToFile },
        { "readfile", "Read Data From File, ex) readfile a.txt", kReadDataFromFile },
        { "testfileio", "Test File I/O Function", kTestFileIO },
        { "testperformance", "Test File Read/WritePerformance", kTestPerformance },
        { "flush", "Flush File System Cache", kFlushCache },
        { "mkdir", "Make Dir", kMkdir},
        { "cd", "move", kCd},
        { "rmdir", "Remove", kRmdir},

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
    // ??????? ???
    kPrintf( CONSOLESHELL_PROMPTMESSAGE );
    kPrintf(pwd);
    kPrintf(">");
    kSetClusterIndex(curDirectoryClusterIdx);
    kGetCursor(&iCursorX, &iCursorY);
    cursorFirst = iCursorX;
    while( 1 )
    {
        // ??? ????? ?????? ???
        bKey = kGetCh();
        if(bKey!=KEY_TAB) tabCnt=0;
        // Backspace ? ???
        if( bKey == KEY_BACKSPACE )
        {
            if( iCommandBufferIndex > 0 )
            {
                // ???? ��?? ????? ??? ?? ???? ?????? ????? ???? ?????? ??????
                // ��??? ??????? ?????? ???? ????
                kGetCursor( &iCursorX, &iCursorY );
                kPrintStringXY( iCursorX - 1, iCursorY, " " );
                kSetCursor( iCursorX - 1, iCursorY );
                iCommandBufferIndex--;
            }
        }
        // ???? ? ???
        else if( bKey == KEY_ENTER )
        {
            kPrintf( "\n" );

            if( iCommandBufferIndex > 0 )
            {
                // ��??? ????? ??? ?????? ????
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

            // ??????? ??? ?? ��??? ???? ????
            kPrintf( "%s", CONSOLESHELL_PROMPTMESSAGE );
            kPrintf(pwd);
            kPrintf(">");
            kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            iCommandBufferIndex = 0;
        }
        // ????? ?, CAPS Lock, NUM Lock, Scroll Lock?? ????
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
        //kPrintf("%s", history[hiscur]);
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
            // ????? ?????? ???????? ???? ????
            if( iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT )
            {
                vcCommandBuffer[ iCommandBufferIndex++ ] = bKey;
                kPrintf( "%c", bKey );
            }
        }
    }
}

/*
 *  ��??? ????? ??? ��??? ????? ??? ��??? ?????? ????? ????
 */
void kExecuteCommand( const char* pcCommandBuffer )
{
    int i, iSpaceIndex;
    int iCommandBufferLength, iCommandLength;
    int iCount;

    // ???????? ???��? ��??? ????
    iCommandBufferLength = kStrLen( pcCommandBuffer );
    for( iSpaceIndex = 0 ; iSpaceIndex < iCommandBufferLength ; iSpaceIndex++ )
    {
        if( pcCommandBuffer[ iSpaceIndex ] == ' ' )
        {
            break;
        }
    }

    // ��??? ??????? ?????? ?????? ????? ��??? ????? ???
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );
    for( i = 0 ; i < iCount ; i++ )
    {
        iCommandLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        // ��????? ????? ?????? ?????? ???????? ???
        if( ( iCommandLength == iSpaceIndex ) &&
            ( kMemCmp( gs_vstCommandTable[ i ].pcCommand, pcCommandBuffer,
                       iSpaceIndex ) == 0 ) )
        {
            gs_vstCommandTable[ i ].pfFunction( pcCommandBuffer + iSpaceIndex + 1 );
            break;
        }
    }

    // ????????? ??? ?? ????? ???? ???
    if( i >= iCount )
    {
        kPrintf( "'%s' is not found.\n", pcCommandBuffer );
    }
}

/**
 *  ?????? ??????? ????
 */
void kInitializeParameter( PARAMETERLIST* pstList, const char* pcParameter )
{
    pstList->pcBuffer = pcParameter;
    pstList->iLength = kStrLen( pcParameter );
    pstList->iCurrentPosition = 0;
}

/**
 *  ???????? ???��? ???????? ????? ????? ???
 */
int kGetNextParameter( PARAMETERLIST* pstList, char* pcParameter )
{
    int i;
    int iLength;

    // ?? ??? ??????? ?????? ????
    if( pstList->iLength <= pstList->iCurrentPosition )
    {
        return 0;
    }

    // ?????? ?????? ?????? ?????? ???
    for( i = pstList->iCurrentPosition ; i < pstList->iLength ; i++ )
    {
        if( pstList->pcBuffer[ i ] == ' ' )
        {
            break;
        }
    }

    // ??????? ??????? ????? ???
    kMemCpy( pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i );
    iLength = i - pstList->iCurrentPosition;
    pcParameter[ iLength ] = '\0';

    // ???????? ??? ???????
    pstList->iCurrentPosition += iLength + 1;
    return iLength;
}

//==============================================================================
//  Ŀ�ǵ带 ó���ϴ� �ڵ�
//==============================================================================
/**
 *  �� ������ ����?
 */
static void kHelp( const char* pcCommandBuffer )
{
    int i;
    int iCount;
    int iCursorX, iCursorY;
    int iLength, iMaxCommandLength = 0;


    kPrintf( "=========================================================\n" );
    kPrintf( "                    MINT64 Shell Help                    \n" );
    kPrintf( "=========================================================\n" );

    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );

    // ���� �� Ŀ�ǵ��� ���̸� ����?
    for( i = 0 ; i < iCount ; i++ )
    {
        iLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        if( iLength > iMaxCommandLength )
        {
            iMaxCommandLength = iLength;
        }
    }

    // ���� ����?
    for( i = 0 ; i < iCount ; i++ )
    {
        kPrintf( "%s", gs_vstCommandTable[ i ].pcCommand );
        kGetCursor( &iCursorX, &iCursorY );
        kSetCursor( iMaxCommandLength, iCursorY );
        kPrintf( "  - %s\n", gs_vstCommandTable[ i ].pcHelp );

        // ������? ���� ����? ������ ������
        if( ( i != 0 ) && ( ( i % 20 ) == 0 ) )
        {
            kPrintf( "Press any key to continue... ('q' is exit) : " );
            if( kGetCh() == 'q' )
            {
                kPrintf( "\n" );
                break;
            }
            kPrintf( "\n" );
        }
    }
}

/**
 *  ????? ????
 */
static void kCls( const char* pcParameterBuffer )
{
    // ?? ?????? ????? ?????? ??????? ????? ???? ??, ???? 1?? ��?? ???
    kClearScreen();
    kSetCursor( 0, 1 );
}

/**
 *  ?? ??? ??? ???
 */
static void kShowTotalRAMSize( const char* pcParameterBuffer )
{
    kPrintf( "Total RAM Size = %d MB\n", kGetTotalRAMSize() );
}

/**
 *  ??????? ?? ????? ????? ?????? ??? ???
 */
static void kStringToDecimalHexTest( const char* pcParameterBuffer )
{
    char vcParameter[ 100 ];
    int iLength;
    PARAMETERLIST stList;
    int iCount = 0;
    long lValue;

    // ?????? ????
    kInitializeParameter( &stList, pcParameterBuffer );

    while( 1 )
    {
        // ???? ??????? ????, ???????? ????? 0??? ??????? ???? ??????
        // ????
        iLength = kGetNextParameter( &stList, vcParameter );
        if( iLength == 0 )
        {
            break;
        }

        // ??????? ???? ?????? ?????? 16???????? 10???????? ?????? ????? ??
        // ????? printf?? ???
        kPrintf( "Param %d = '%s', Length = %d, ", iCount + 1,
                 vcParameter, iLength );

        // 0x?? ??????? 16????, ???? 10?????? ???
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
 *  PC?? ?????(Reboot)
 */
static void kShutdown( const char* pcParamegerBuffer )
{
    kPrintf( "System Shutdown Start...\n" );

    // ????? ???????? ???? PC?? ?????
    kPrintf( "Press Any Key To Reboot PC..." );
    kGetCh();
    kReboot();
}

static void ypchoLove(){
    kPrintf( "We love ypcho!\n" );
}
static void ypchang(){
    kPrintf( "Who are you?\n");
}
static void ypkim(){
    kPrintf( "Who are you!\n" );
}
static void kRaiseFault(){
    QWORD* addr = 0x1FF000;
    //write
    *addr = 0x1994;
    //read
    // DWORD dummy = *addr;
}

/**
 *  PIT ��Ʈ�ѷ��� ī���� 0 ����
 */
static void kSetTimer( const char* pcParameterBuffer )
{
    char vcParameter[ 100 ];
    PARAMETERLIST stList;
    long lValue;
    BOOL bPeriodic;

    // �Ķ�����? �ʱ�ȭ
    kInitializeParameter( &stList, pcParameterBuffer );

    // milisecond ����
    if( kGetNextParameter( &stList, vcParameter ) == 0 )
    {
        kPrintf( "ex)settimer 10(ms) 1(periodic)\n" );
        return ;
    }
    lValue = kAToI( vcParameter, 10 );

    // Periodic ����
    if( kGetNextParameter( &stList, vcParameter ) == 0 )
    {
        kPrintf( "ex)settimer 10(ms) 1(periodic)\n" );
        return ;
    }
    bPeriodic = kAToI( vcParameter, 10 );

    kInitializePIT( MSTOCOUNT( lValue ), bPeriodic );
    kPrintf( "Time = %d ms, Periodic = %d Change Complete\n", lValue, bPeriodic );
}

/**
 *  PIT ��Ʈ�ѷ��� ���� ����Ͽ��? ms ���� ����?
 */
static void kWaitUsingPIT( const char* pcParameterBuffer )
{
    char vcParameter[ 100 ];
    int iLength;
    PARAMETERLIST stList;
    long lMillisecond;
    int i;

    // �Ķ�����? �ʱ�ȭ
    kInitializeParameter( &stList, pcParameterBuffer );
    if( kGetNextParameter( &stList, vcParameter ) == 0 )
    {
        kPrintf( "ex)wait 100(ms)\n" );
        return ;
    }

    lMillisecond = kAToI( pcParameterBuffer, 10 );
    kPrintf( "%d ms Sleep Start...\n", lMillisecond );

    // ���ͷ�Ʈ�� ��Ȱ��ȭ�ϰ� PIT ��Ʈ�ѷ��� ���� ���� �ð��� ����
    kDisableInterrupt();
    for( i = 0 ; i < lMillisecond / 30 ; i++ )
    {
        kWaitUsingDirectPIT( MSTOCOUNT( 30 ) );
    }
    kWaitUsingDirectPIT( MSTOCOUNT( lMillisecond % 30 ) );
    kEnableInterrupt();
    kPrintf( "%d ms Sleep Complete\n", lMillisecond );

    // Ÿ�̸� ����
    kInitializePIT( MSTOCOUNT( 1 ), TRUE );
}

/**
 *  Ÿ�� ������ ī���͸� ����
 */
static void kReadTimeStampCounter( const char* pcParameterBuffer )
{
    QWORD qwTSC;

    qwTSC = kReadTSC();
    kPrintf( "Time Stamp Counter = %q\n", qwTSC );
}

/**
 *  ���μ����� �ӵ��� ����
 */
static void kMeasureProcessorSpeed( const char* pcParameterBuffer )
{
    int i;
    QWORD qwLastTSC, qwTotalTSC = 0;

    kPrintf( "Now Measuring." );

    // 10�� ���� ��ȭ�� Ÿ�� ������ ī���͸� �̿��Ͽ� ���μ����� �ӵ��� ���������� ����
    kDisableInterrupt();
    for( i = 0 ; i < 200 ; i++ )
    {
        qwLastTSC = kReadTSC();
        kWaitUsingDirectPIT( MSTOCOUNT( 50 ) );
        qwTotalTSC += kReadTSC() - qwLastTSC;

        kPrintf( "." );
    }
    // Ÿ�̸� ����
    kInitializePIT( MSTOCOUNT( 1 ), TRUE );
    kEnableInterrupt();

    kPrintf( "\nCPU Speed = %d MHz\n", qwTotalTSC / 10 / 1000 / 1000 );
}

/**
 *  RTC ��Ʈ�ѷ��� ������? ���� �� �ð� ������ ǥ��
 */
static void kShowDateAndTime( const char* pcParameterBuffer )
{
    BYTE bSecond, bMinute, bHour;
    BYTE bDayOfWeek, bDayOfMonth, bMonth;
    WORD wYear;

    // RTC ��Ʈ�ѷ����� �ð� �� ���ڸ� ����
    kReadRTCTime( &bHour, &bMinute, &bSecond );
    kReadRTCDate( &wYear, &bMonth, &bDayOfMonth, &bDayOfWeek );

    kPrintf( "Date: %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth,
             kConvertDayOfWeekToString( bDayOfWeek ) );
    kPrintf( "Time: %d:%d:%d\n", bHour, bMinute, bSecond );
}

/**
 *  �½�ũ 1
 *      ȭ�� �׵θ��� ���鼭 ���ڸ� ����?
 */
static void kTestTask1( void )
{
    BYTE bData;
    int i = 0, iX = 0, iY = 0, iMargin, j;
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    TCB* pstRunningTask;

    // �ڽ��� ID�� ����? ȭ�� ���������� ����?
    pstRunningTask = kGetRunningTask();
    iMargin = ( pstRunningTask->stLink.qwID & 0xFFFFFFFF ) % 10;

    // ȭ�� �� �����̸� ���鼭 ���� ����?
    for( j = 0 ; j < 20000 ; j++ )
    {
        switch( i )
        {
        case 0:
            iX++;
            if( iX >= ( CONSOLE_WIDTH - iMargin ) )
            {
                i = 1;
            }
            break;

        case 1:
            iY++;
            if( iY >= ( CONSOLE_HEIGHT - iMargin ) )
            {
                i = 2;
            }
            break;

        case 2:
            iX--;
            if( iX < iMargin )
            {
                i = 3;
            }
            break;

        case 3:
            iY--;
            if( iY < iMargin )
            {
                i = 0;
            }
            break;
        }

        // ���� �� ���� ����
        pstScreen[ iY * CONSOLE_WIDTH + iX ].bCharactor = bData;
        pstScreen[ iY * CONSOLE_WIDTH + iX ].bAttribute = bData & 0x0F;
        bData++;

        // �ٸ� �½�ũ�� ��ȯ
        //kSchedule();
    }

    kExitTask();
}

/**
 *  �½�ũ 2
 *      �ڽ��� ID�� �����Ͽ� Ư�� ��ġ�� ȸ���ϴ� �ٶ����� ����?
 */
static void kTestTask2( void )
{
    int i = 0, iOffset;
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    TCB* pstRunningTask;
    char vcData[ 4 ] = { '-', '\\', '|', '/' };

    // �ڽ��� ID�� ����? ȭ�� ���������� ����?
    pstRunningTask = kGetRunningTask();
    iOffset = ( pstRunningTask->stLink.qwID & 0xFFFFFFFF ) * 2;
    iOffset = CONSOLE_WIDTH * CONSOLE_HEIGHT -
        ( iOffset % ( CONSOLE_WIDTH * CONSOLE_HEIGHT ) );

    while( 1 )
    {
        // ȸ���ϴ� �ٶ����� ǥ��
        pstScreen[ iOffset ].bCharactor = vcData[ i % 4 ];
        // ���� ����
        pstScreen[ iOffset ].bAttribute = ( iOffset % 15 ) + 1;
        i++;

        // �ٸ� �½�ũ�� ��ȯ
        //kSchedule();
    }
}

/**
 *  �½�ũ�� �����ؼ� ��Ƽ �½�ŷ ����
 */
static void kCreateTestTask( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcType[ 30 ];
    char vcCount[ 30 ];
    int i;

    // �Ķ���͸��? ����
    kInitializeParameter( &stList, pcParameterBuffer );
    kGetNextParameter( &stList, vcType );
    kGetNextParameter( &stList, vcCount );

    switch( kAToI( vcType, 10 ) )
    {
    // Ÿ�� 1 �½�ũ ����
    case 1:
        for( i = 0 ; i < kAToI( vcCount, 10 ) ; i++ )
        {
            if( kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kTestTask1 ) == NULL )
            {
                break;
            }
        }

        kPrintf( "Task1 %d Created\n", i );
        break;

    // Ÿ�� 2 �½�ũ ����
    case 2:
    default:
        for( i = 0 ; i < kAToI( vcCount, 10 ) ; i++ )
        {
            if( kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kTestTask2 ) == NULL )
            {
                break;
            }
        }
        kPrintf( "Task2 %d Created\n", i );
        break;
    }
}

/**
 *  �½�ũ�� �켱 ������ ����
 */
static void kChangeTaskPriority( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    char vcPriority[ 30 ];
    QWORD qwID;
    BYTE bPriority;

    // �Ķ���͸��? ����
    kInitializeParameter( &stList, pcParameterBuffer );
    kGetNextParameter( &stList, vcID );
    kGetNextParameter( &stList, vcPriority );

    // �½�ũ�� �켱 ������ ����
    if( kMemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = kAToI( vcID + 2, 16 );
    }
    else
    {
        qwID = kAToI( vcID, 10 );
    }

    bPriority = kAToI( vcPriority, 10 );

    kPrintf( "Change Task Priority ID [0x%q] Priority[%d] ", qwID, bPriority );
    if( kChangePriority( qwID, bPriority ) == TRUE )
    {
        kPrintf( "Success\n" );
    }
    else
    {
        kPrintf( "Fail\n" );
    }
}

/**
 *  ���� ������ ����? �½�ũ�� ������ ����?
 */
static void kShowTaskList( const char* pcParameterBuffer )
{
    int i;
    TCB* pstTCB;
    int iCount = 0;

    kPrintf( "=========== Task Total Count [%d] ===========\n", kGetTaskCount() );
    for( i = 0 ; i < TASK_MAXCOUNT ; i++ )
    {
        // TCB�� ���ؼ� TCB�� ����? ���̸� ID�� ����?
        pstTCB = kGetTCBInTCBPool( i );
        if( ( pstTCB->stLink.qwID >> 32 ) != 0 )
        {
            // �½�ũ�� 10�� ��µ��? ������, ����? �½�ũ ������ ǥ������ ���θ� Ȯ��
            if( ( iCount != 0 ) && ( ( iCount % 10 ) == 0 ) )
            {
                kPrintf( "Press any key to continue... ('q' is exit) : " );
                if( kGetCh() == 'q' )
                {
                    kPrintf( "\n" );
                    break;
                }
                kPrintf( "\n" );
            }

            kPrintf( "[%d] Task ID[0x%Q], Priority[%d], Flags[0x%Q], Thread[%d]\n", 1 + iCount++,
                     pstTCB->stLink.qwID, GETPRIORITY( pstTCB->qwFlags ),
                     pstTCB->qwFlags, kGetListCount( &( pstTCB->stChildThreadList ) ) );
            kPrintf( "    Parent PID[0x%Q], Memory Address[0x%Q], Size[0x%Q]\n",
                    pstTCB->qwParentProcessID, pstTCB->pvMemoryAddress, pstTCB->qwMemorySize );
        }
    }
}

/**
 *  �½�ũ�� ����
 */
static void kKillTask( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    QWORD qwID;
    TCB* pstTCB;
    int i;

    // �Ķ���͸��? ����
    kInitializeParameter( &stList, pcParameterBuffer );
    kGetNextParameter( &stList, vcID );

    // �½�ũ�� ����
    if( kMemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = kAToI( vcID + 2, 16 );
    }
    else
    {
        qwID = kAToI( vcID, 10 );
    }

    // Ư�� ID�� �����ϴ� ����?
    if( qwID != 0xFFFFFFFF )
    {
        pstTCB = kGetTCBInTCBPool( GETTCBOFFSET( qwID ) );
        qwID = pstTCB->stLink.qwID;

        // �ý��� �׽�Ʈ�� ����
        if( ( ( qwID >> 32 ) != 0 ) && ( ( pstTCB->qwFlags & TASK_FLAGS_SYSTEM ) == 0x00 ) )
        {
            kPrintf( "Kill Task ID [0x%q] ", qwID );
            if( kEndTask( qwID ) == TRUE )
            {
                kPrintf( "Success\n" );
            }
            else
            {
                kPrintf( "Fail\n" );
            }
        }
        else
        {
            kPrintf( "Task does not exist or task is system task\n" );
        }
    }
    // �ܼ� �а� ���� �½�ũ�� �����ϰ� ����? �½�ũ ����
    else
    {
        for( i = 0 ; i < TASK_MAXCOUNT ; i++ )
        {
            pstTCB = kGetTCBInTCBPool( i );
            qwID = pstTCB->stLink.qwID;

            // �ý��� �׽�Ʈ�� ���� ��Ͽ����? ����
            if( ( ( qwID >> 32 ) != 0 ) && ( ( pstTCB->qwFlags & TASK_FLAGS_SYSTEM ) == 0x00 ) )
            {
                kPrintf( "Kill Task ID [0x%q] ", qwID );
                if( kEndTask( qwID ) == TRUE )
                {
                    kPrintf( "Success\n" );
                }
                else
                {
                    kPrintf( "Fail\n" );
                }
            }
        }
    }
}

/**
 *  ���μ����� ������ ǥ��
 */
static void kCPULoad( const char* pcParameterBuffer )
{
    kPrintf( "Processor Load : %d%%\n", kGetProcessorLoad() );
}

// ���ؽ� �׽�Ʈ�� ���ؽ��� ����
static MUTEX gs_stMutex;
static volatile QWORD gs_qwAdder;

/**
 *  ���ؽ��� �׽�Ʈ�ϴ� �½�ũ
 */
static void kPrintNumberTask( void )
{
    int i;
    int j;
    QWORD qwTickCount;

    // 50ms ���� ����Ͽ��? �ܼ� ���� ����ϴ��? �޽����� ��ġ�� �ʵ��� ��
    qwTickCount = kGetTickCount();
    while( ( kGetTickCount() - qwTickCount ) < 50 )
    {
        kSchedule();
    }

    // ������ ���鼭 ���ڸ� ����?
    for( i = 0 ; i < 5 ; i++ )
    {
        kLock( &( gs_stMutex ) );
        kPrintf( "Task ID [0x%Q] Value[%d]\n", kGetRunningTask()->stLink.qwID,
                gs_qwAdder );

        gs_qwAdder += 1;
        kUnlock( & ( gs_stMutex ) );

        // ���μ��� �Ҹ� �ø����� �߰��� �ڵ�
        for( j = 0 ; j < 30000 ; j++ ) ;
    }

    // ����? �½�ũ�� ������ ������ 1��(100ms) ���� ����?
    qwTickCount = kGetTickCount();
    while( ( kGetTickCount() - qwTickCount ) < 1000 )
    {
        kSchedule();
    }

    // �½�ũ ����
    kExitTask();
}

/**
 *  ���ؽ��� �׽�Ʈ�ϴ� �½�ũ ����
 */
static void kTestMutex( const char* pcParameterBuffer )
{
    int i;

    gs_qwAdder = 1;

    // ���ؽ� �ʱ�ȭ
    kInitializeMutex( &gs_stMutex );

    for( i = 0 ; i < 3 ; i++ )
    {
        // ���ؽ��� �׽�Ʈ�ϴ� �½�ũ�� 3�� ����
        kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kPrintNumberTask );
    }
    kPrintf( "Wait Util %d Task End...\n", i );
    kGetCh();
}

/**
 *  �½�ũ 2�� �ڽ��� ��������? �����ϴ� �½�ũ
 */
static void kCreateThreadTask( void )
{
    int i;

    for( i = 0 ; i < 3 ; i++ )
    {
        kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kTestTask2 );
    }

    while( 1 )
    {
        kSleep( 1 );
    }
}

/**
 *  �����带 �׽�Ʈ�ϴ� �½�ũ ����
 */
static void kTestThread( const char* pcParameterBuffer )
{
    TCB* pstProcess;

    pstProcess = kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_PROCESS, ( void * )0xEEEEEEEE, 0x1000,
                              ( QWORD ) kCreateThreadTask );
    if( pstProcess != NULL )
    {
        kPrintf( "Process [0x%Q] Create Success\n", pstProcess->stLink.qwID );
    }
    else
    {
        kPrintf( "Process Create Fail\n" );
    }
}

// ������ �߻���Ű�� ���� ����
static volatile QWORD gs_qwRandomValue = 0;

/**
 *  ������ ������ ��ȯ
 */
QWORD kRandom( void )
{
    gs_qwRandomValue = ( gs_qwRandomValue * 412153 + 5571031 ) >> 16;
    return gs_qwRandomValue;
}

static void kRand(){
    // unsigned int m_z ;
    // unsigned int m_w ;
    // m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    // m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    // kPrintf("%d", (m_z << 16) + m_w );

    // int a, m;
    // int seed = kTimeSeed();
    // int random;
    //
    // a = 16807;
    // m = 2147483647;
    // seed = (a * seed) % m;
    // random = seed / m;
    kPrintf("%d", (kRandom() % 100000) );

    //kPrintf("%d", randomnum() );

}

/**
 *  ö�ڸ� �귯������ �ϴ� ������
 */
static void kDropCharactorThread( void )
{
    int iX, iY;
    int i;
    char vcText[ 2 ] = { 0, };

    iX = kRandom() % CONSOLE_WIDTH;

    while( 1 )
    {
        // ����? ������?
        kSleep( kRandom() % 20 );

        if( ( kRandom() % 20 ) < 16 )
        {
            vcText[ 0 ] = ' ';
            for( i = 0 ; i < CONSOLE_HEIGHT - 1 ; i++ )
            {
                kPrintStringXY( iX, i , vcText );
                kSleep( 50 );
            }
        }
        else
        {
            for( i = 0 ; i < CONSOLE_HEIGHT - 1 ; i++ )
            {
                vcText[ 0 ] = i + kRandom();
                kPrintStringXY( iX, i, vcText );
                kSleep( 50 );
            }
        }
    }
}

/**
 *  �����带 �����Ͽ� ��Ʈ���� ȭ��ó�� �����ִ� ���μ���
 */
static void kMatrixProcess( void )
{
    int i;

    for( i = 0 ; i < 300 ; i++ )
    {
        if( kCreateTask( TASK_FLAGS_THREAD | TASK_FLAGS_LOW, 0, 0,
                         ( QWORD ) kDropCharactorThread ) == NULL )
        {
            break;
        }

        kSleep( kRandom() % 5 + 5 );
    }

    kPrintf( "%d Thread is created\n", i );

    // Ű�� �ԷµǸ� ���μ��� ����
    kGetCh();
}

/**
 *  ��Ʈ���� ȭ���� ������
 */
static void kShowMatrix( const char* pcParameterBuffer )
{
    TCB* pstProcess;

    pstProcess = kCreateTask( TASK_FLAGS_PROCESS | TASK_FLAGS_LOW, ( void* ) 0xE00000, 0xE00000,
                              ( QWORD ) kMatrixProcess );
    if( pstProcess != NULL )
    {
        kPrintf( "Matrix Process [0x%Q] Create Success\n" );

        // �½�ũ�� ���� �� ������ ����?
        while( ( pstProcess->stLink.qwID >> 32 ) != 0 )
        {
            kSleep( 100 );
        }
    }
    else
    {
        kPrintf( "Matrix Process Create Fail\n" );
    }
}

/**
 *  FPU�� �׽�Ʈ�ϴ� �½�ũ
 */
static void kFPUTestTask( void )
{
    double dValue1;
    double dValue2;
    TCB* pstRunningTask;
    QWORD qwCount = 0;
    QWORD qwRandomValue;
    int i;
    int iOffset;
    char vcData[ 4 ] = { '-', '\\', '|', '/' };
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;

    pstRunningTask = kGetRunningTask();

    // �ڽ��� ID�� ����? ȭ�� ���������� ����?
    iOffset = ( pstRunningTask->stLink.qwID & 0xFFFFFFFF ) * 2;
    iOffset = CONSOLE_WIDTH * CONSOLE_HEIGHT -
        ( iOffset % ( CONSOLE_WIDTH * CONSOLE_HEIGHT ) );

    // ������ ������ �ݺ��ϸ鼭 ������ ������? ����
    while( 1 )
    {
        dValue1 = 1;
        dValue2 = 1;

        // �׽�Ʈ�� ���� ������ ������? 2�� �ݺ��ؼ� ����
        for( i = 0 ; i < 10 ; i++ )
        {
            qwRandomValue = kRandom();
            dValue1 *= ( double ) qwRandomValue;
            dValue2 *= ( double ) qwRandomValue;

            kSleep( 1 );

            qwRandomValue = kRandom();
            dValue1 /= ( double ) qwRandomValue;
            dValue2 /= ( double ) qwRandomValue;
        }

        if( dValue1 != dValue2 )
        {
            kPrintf( "Value Is Not Same~!!! [%f] != [%f]\n", dValue1, dValue2 );
            break;
        }
        qwCount++;

        // ȸ���ϴ� �ٶ����� ǥ��
        pstScreen[ iOffset ].bCharactor = vcData[ qwCount % 4 ];

        // ���� ����
        pstScreen[ iOffset ].bAttribute = ( iOffset % 15 ) + 1;
    }
}

/**
 *  ������(PIE)�� ����?
 */
static void kTestPIE( const char* pcParameterBuffer )
{
    double dResult;
    int i;

    kPrintf( "PIE Cacluation Test\n" );
    kPrintf( "Result: 355 / 113 = " );
    dResult = ( double ) 355 / 113;
    kPrintf( "%d.%d%d\n", ( QWORD ) dResult, ( ( QWORD ) ( dResult * 10 ) % 10 ),
             ( ( QWORD ) ( dResult * 100 ) % 10 ) );

    // �Ǽ��� ����ϴ��? �½�ũ�� ����
    for( i = 0 ; i < 100 ; i++ )
    {
        kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kFPUTestTask );
    }
}

/**
 *  ���� �޸� ������ ǥ��
 */
static void kShowDyanmicMemoryInformation( const char* pcParameterBuffer )
{
    QWORD qwStartAddress, qwTotalSize, qwMetaSize, qwUsedSize;

    kGetDynamicMemoryInformation( &qwStartAddress, &qwTotalSize, &qwMetaSize,
            &qwUsedSize );

    kPrintf( "============ Dynamic Memory Information ============\n" );
    kPrintf( "Start Address: [0x%Q]\n", qwStartAddress );
    kPrintf( "Total Size:    [0x%Q]byte, [%d]MB\n", qwTotalSize,
            qwTotalSize / 1024 / 1024 );
    kPrintf( "Meta Size:     [0x%Q]byte, [%d]KB\n", qwMetaSize,
            qwMetaSize / 1024 );
    kPrintf( "Used Size:     [0x%Q]byte, [%d]KB\n", qwUsedSize, qwUsedSize / 1024 );
}

/**
 *  ����? ���� ����Ʈ�� ������ ���������� �Ҵ��ϰ� �����ϴ� �׽�Ʈ
 */
static void kTestSequentialAllocation( const char* pcParameterBuffer )
{
    DYNAMICMEMORY* pstMemory;
    long i, j, k;
    QWORD* pqwBuffer;

    kPrintf( "============ Dynamic Memory Test ============\n" );
    pstMemory = kGetDynamicMemoryManager();

    for( i = 0 ; i < pstMemory->iMaxLevelCount ; i++ )
    {
        kPrintf( "Block List [%d] Test Start\n", i );
        kPrintf( "Allocation And Compare: ");

        // ����? ������ �Ҵ� �޾Ƽ� ���� ä�� �� �˻�
        for( j = 0 ; j < ( pstMemory->iBlockCountOfSmallestBlock >> i ) ; j++ )
        {
            pqwBuffer = kAllocateMemory( DYNAMICMEMORY_MIN_SIZE << i );
            if( pqwBuffer == NULL )
            {
                kPrintf( "\nAllocation Fail\n" );
                return ;
            }

            // ���� ä�� �� �ٽ� �˻�
            for( k = 0 ; k < ( DYNAMICMEMORY_MIN_SIZE << i ) / 8 ; k++ )
            {
                pqwBuffer[ k ] = k;
            }

            for( k = 0 ; k < ( DYNAMICMEMORY_MIN_SIZE << i ) / 8 ; k++ )
            {
                if( pqwBuffer[ k ] != k )
                {
                    kPrintf( "\nCompare Fail\n" );
                    return ;
                }
            }
            // ���� ������ . ���� ǥ��
            kPrintf( "." );
        }

        kPrintf( "\nFree: ");
        // �Ҵ� ���� ������ ����? ��ȯ
        for( j = 0 ; j < ( pstMemory->iBlockCountOfSmallestBlock >> i ) ; j++ )
        {
            if( kFreeMemory( ( void * ) ( pstMemory->qwStartAddress +
                         ( DYNAMICMEMORY_MIN_SIZE << i ) * j ) ) == FALSE )
            {
                kPrintf( "\nFree Fail\n" );
                return ;
            }
            // ���� ������ . ���� ǥ��
            kPrintf( "." );
        }
        kPrintf( "\n" );
    }
    kPrintf( "Test Complete~!!!\n" );
}

/**
 *  ���Ƿ� �޸𸮸� �Ҵ��ϰ� �����ϴ� ���� �ݺ��ϴ� �½�ũ
 */
static void kRandomAllocationTask( void )
{
    TCB* pstTask;
    QWORD qwMemorySize;
    char vcBuffer[ 200 ];
    BYTE* pbAllocationBuffer;
    int i, j;
    int iY;

    pstTask = kGetRunningTask();
    iY = ( pstTask->stLink.qwID ) % 15 + 9;

    for( j = 0 ; j < 10 ; j++ )
    {
        // 1KB ~ 32M���� �Ҵ��ϵ��� ��
        do
        {
            qwMemorySize = ( ( kRandom() % ( 32 * 1024 ) ) + 1 ) * 1024;
            pbAllocationBuffer = kAllocateMemory( qwMemorySize );

            // ���� ���۸� �Ҵ� ���� ���ϸ� �ٸ� �½�ũ�� �޸𸮸� ����ϰ��?
            // ���� �� �����Ƿ� ����? ������? �� �ٽ� �õ�
            if( pbAllocationBuffer == 0 )
            {
                kSleep( 1 );
            }
        } while( pbAllocationBuffer == 0 );

        kSPrintf( vcBuffer, "|Address: [0x%Q] Size: [0x%Q] Allocation Success",
                  pbAllocationBuffer, qwMemorySize );
        // �ڽ��� ID�� Y ��ǥ�� �Ͽ� �����͸� ����?
        kPrintStringXY( 20, iY, vcBuffer );
        kSleep( 200 );

        // ���۸� ������ ������ ������ �����͸� �Ȱ��� ä��
        kSPrintf( vcBuffer, "|Address: [0x%Q] Size: [0x%Q] Data Write...     ",
                  pbAllocationBuffer, qwMemorySize );
        kPrintStringXY( 20, iY, vcBuffer );
        for( i = 0 ; i < qwMemorySize / 2 ; i++ )
        {
            pbAllocationBuffer[ i ] = kRandom() & 0xFF;
            pbAllocationBuffer[ i + ( qwMemorySize / 2 ) ] = pbAllocationBuffer[ i ];
        }
        kSleep( 200 );

        // ä�� �����Ͱ� ���������� �ٽ� Ȯ��
        kSPrintf( vcBuffer, "|Address: [0x%Q] Size: [0x%Q] Data Verify...   ",
                  pbAllocationBuffer, qwMemorySize );
        kPrintStringXY( 20, iY, vcBuffer );
        for( i = 0 ; i < qwMemorySize / 2 ; i++ )
        {
            if( pbAllocationBuffer[ i ] != pbAllocationBuffer[ i + ( qwMemorySize / 2 ) ] )
            {
                kPrintf( "Task ID[0x%Q] Verify Fail\n", pstTask->stLink.qwID );
                kExitTask();
            }
        }
        kFreeMemory( pbAllocationBuffer );
        kSleep( 200 );
    }

    kExitTask();
}

/**
 *  �½�ũ�� ���� �� �����Ͽ� ������ �޸𸮸� �Ҵ��ϰ� �����ϴ� ���� �ݺ��ϴ� �׽�Ʈ
 */
static void kTestRandomAllocation( const char* pcParameterBuffer )
{
    int i;

    for( i = 0 ; i < 1000 ; i++ )
    {
        kCreateTask( TASK_FLAGS_LOWEST | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kRandomAllocationTask );
    }
}

/**
 *  �ϵ� ��ũ�� ������ ǥ��
 */
static void kShowHDDInformation( const char* pcParameterBuffer )
{
    HDDINFORMATION stHDD;
    char vcBuffer[ 100 ];

    // �ϵ� ��ũ�� ������ ����
    if( kGetHDDInformation( &stHDD ) == FALSE )
    {
        kPrintf( "HDD Information Read Fail\n" );
        return ;
    }

    kPrintf( "============ Primary Master HDD Information ============\n" );

    // �� ��ȣ ����?
    kMemCpy( vcBuffer, stHDD.vwModelNumber, sizeof( stHDD.vwModelNumber ) );
    vcBuffer[ sizeof( stHDD.vwModelNumber ) - 1 ] = '\0';
    kPrintf( "Model Number:\t %s\n", vcBuffer );

    // �ø��� ��ȣ ����?
    kMemCpy( vcBuffer, stHDD.vwSerialNumber, sizeof( stHDD.vwSerialNumber ) );
    vcBuffer[ sizeof( stHDD.vwSerialNumber ) - 1 ] = '\0';
    kPrintf( "Serial Number:\t %s\n", vcBuffer );

    // ����?, �Ǹ���, �Ǹ��� �� ���� ���� ����?
    kPrintf( "Head Count:\t %d\n", stHDD.wNumberOfHead );
    kPrintf( "Cylinder Count:\t %d\n", stHDD.wNumberOfCylinder );
    kPrintf( "Sector Count:\t %d\n", stHDD.wNumberOfSectorPerCylinder );

    // �� ���� �� ����?
    kPrintf( "Total Sector:\t %d Sector, %dMB\n", stHDD.dwTotalSectors,
            stHDD.dwTotalSectors / 2 / 1024 );
}

/**
 *  �ϵ� ��ũ�� �Ķ���ͷ��? �Ѿ���? LBA ��巹�������? ���� �� ��ŭ ����
 */
static void kReadSector( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcLBA[ 50 ], vcSectorCount[ 50 ];
    DWORD dwLBA;
    int iSectorCount;
    char* pcBuffer;
    int i, j;
    BYTE bData;
    BOOL bExit = FALSE;

    // �Ķ�����? ����Ʈ�� �ʱ�ȭ�Ͽ� LBA ��巹�����? ���� �� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    if( ( kGetNextParameter( &stList, vcLBA ) == 0 ) ||
        ( kGetNextParameter( &stList, vcSectorCount ) == 0 ) )
    {
        kPrintf( "ex) readsector 0(LBA) 10(count)\n" );
        return ;
    }
    dwLBA = kAToI( vcLBA, 10 );
    iSectorCount = kAToI( vcSectorCount, 10 );

    // ���� ����ŭ �޸𸮸� �Ҵ� �޾� �б� ����
    pcBuffer = kAllocateMemory( iSectorCount * 512 );
    if( kReadHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) == iSectorCount )
    {
        kPrintf( "LBA [%d], [%d] Sector Read Success~!!", dwLBA, iSectorCount );
        // ������ ������ ������ ����?
        for( j = 0 ; j < iSectorCount ; j++ )
        {
            for( i = 0 ; i < 512 ; i++ )
            {
                if( !( ( j == 0 ) && ( i == 0 ) ) && ( ( i % 256 ) == 0 ) )
                {
                    kPrintf( "\nPress any key to continue... ('q' is exit) : " );
                    if( kGetCh() == 'q' )
                    {
                        bExit = TRUE;
                        break;
                    }
                }

                if( ( i % 16 ) == 0 )
                {
                    kPrintf( "\n[LBA:%d, Offset:%d]\t| ", dwLBA + j, i );
                }

                // ����? �� �ڸ��� ǥ���Ϸ��� 16���� ���� ����? 0�� �߰�����
                bData = pcBuffer[ j * 512 + i ] & 0xFF;
                if( bData < 16 )
                {
                    kPrintf( "0" );
                }
                kPrintf( "%X ", bData );
            }

            if( bExit == TRUE )
            {
                break;
            }
        }
        kPrintf( "\n" );
    }
    else
    {
        kPrintf( "Read Fail\n" );
    }

    kFreeMemory( pcBuffer );
}

/**
 *  �ϵ� ��ũ�� �Ķ���ͷ��? �Ѿ���? LBA ��巹�������? ���� �� ��ŭ ��
 */
static void kWriteSector( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcLBA[ 50 ], vcSectorCount[ 50 ];
    DWORD dwLBA;
    int iSectorCount;
    char* pcBuffer;
    int i, j;
    BOOL bExit = FALSE;
    BYTE bData;
    static DWORD s_dwWriteCount = 0;

    // �Ķ�����? ����Ʈ�� �ʱ�ȭ�Ͽ� LBA ��巹�����? ���� �� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    if( ( kGetNextParameter( &stList, vcLBA ) == 0 ) ||
        ( kGetNextParameter( &stList, vcSectorCount ) == 0 ) )
    {
        kPrintf( "ex) writesector 0(LBA) 10(count)\n" );
        return ;
    }
    dwLBA = kAToI( vcLBA, 10 );
    iSectorCount = kAToI( vcSectorCount, 10 );

    s_dwWriteCount++;

    // ���۸� �Ҵ� �޾� �����͸� ä��.
    // ������ 4 ����Ʈ�� LBA ��巹�����? 4 ����Ʈ�� ���Ⱑ ������? Ƚ���� ����
    pcBuffer = kAllocateMemory( iSectorCount * 512 );
    for( j = 0 ; j < iSectorCount ; j++ )
    {
        for( i = 0 ; i < 512 ; i += 8 )
        {
            *( DWORD* ) &( pcBuffer[ j * 512 + i ] ) = dwLBA + j;
            *( DWORD* ) &( pcBuffer[ j * 512 + i + 4 ] ) = s_dwWriteCount;
        }
    }

    // ���� ����
    if( kWriteHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) != iSectorCount )
    {
        kPrintf( "Write Fail\n" );
        return ;
    }
    kPrintf( "LBA [%d], [%d] Sector Write Success~!!", dwLBA, iSectorCount );

    // ������ ������ ������ ����?
    for( j = 0 ; j < iSectorCount ; j++ )
    {
        for( i = 0 ; i < 512 ; i++ )
        {
            if( !( ( j == 0 ) && ( i == 0 ) ) && ( ( i % 256 ) == 0 ) )
            {
                kPrintf( "\nPress any key to continue... ('q' is exit) : " );
                if( kGetCh() == 'q' )
                {
                    bExit = TRUE;
                    break;
                }
            }

            if( ( i % 16 ) == 0 )
            {
                kPrintf( "\n[LBA:%d, Offset:%d]\t| ", dwLBA + j, i );
            }

            // ����? �� �ڸ��� ǥ���Ϸ��� 16���� ���� ����? 0�� �߰�����
            bData = pcBuffer[ j * 512 + i ] & 0xFF;
            if( bData < 16 )
            {
                kPrintf( "0" );
            }
            kPrintf( "%X ", bData );
        }

        if( bExit == TRUE )
        {
            break;
        }
    }
    kPrintf( "\n" );
    kFreeMemory( pcBuffer );
}

/**
 *  �ϵ� ��ũ�� ����
 */
static void kMountHDD( const char* pcParameterBuffer )
{
    if( kMount() == FALSE )
    {
        kPrintf( "HDD Mount Fail\n" );
        return ;
    }
    kPrintf( "HDD Mount Success\n" );
}

/**
 *  �ϵ� ��ũ�� ���� �ý����� ����(����)
 */
static void kFormatHDD( const char* pcParameterBuffer )
{
    if( kFormat() == FALSE )
    {
        kPrintf( "HDD Format Fail\n" );
        return ;
    }
    kPrintf( "HDD Format Success\n" );
}

/**
 *  ���� �ý��� ������ ǥ��
 */
static void kShowFileSystemInformation( const char* pcParameterBuffer )
{
    FILESYSTEMMANAGER stManager;

    kGetFileSystemInformation( &stManager );

    kPrintf( "================== File System Information ==================\n" );
    kPrintf( "Mouted:\t\t\t\t\t %d\n", stManager.bMounted );
    kPrintf( "Reserved Sector Count:\t\t\t %d Sector\n", stManager.dwReservedSectorCount );
    kPrintf( "Cluster Link Table Start Address:\t %d Sector\n",
            stManager.dwClusterLinkAreaStartAddress );
    kPrintf( "Cluster Link Table Size:\t\t %d Sector\n", stManager.dwClusterLinkAreaSize );
    kPrintf( "Data Area Start Address:\t\t %d Sector\n", stManager.dwDataAreaStartAddress );
    kPrintf( "Total Cluster Count:\t\t\t %d Cluster\n", stManager.dwTotalClusterCount );
}

/**
 *  ��Ʈ ���͸��� �� ������ ����
 */
static void kCreateFileInRootDirectory( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    DWORD dwCluster;
    int i;
    FILE* pstFile;

    // �Ķ�����? ����Ʈ�� �ʱ�ȭ�Ͽ� ���� �̸��� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        kPrintf( "Too Long or Too Short File Name\n" );
        return ;
    }

    pstFile = fopen( vcFileName, "w" );
    if( pstFile == NULL )
    {
        kPrintf( "File Create Fail\n" );
        return;
    }
    fclose( pstFile );
    kPrintf( "File Create Success\n" );
}

/**
 *  ��Ʈ ���͸����� ������ ����
 */
static void kDeleteFileInRootDirectory( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;

    // �Ķ�����? ����Ʈ�� �ʱ�ȭ�Ͽ� ���� �̸��� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        kPrintf( "Too Long or Too Short File Name\n" );
        return ;
    }

    if( remove( vcFileName ) != 0 )
    {
        kPrintf( "File Not Found or File Opened\n" );
        return ;
    }

    kPrintf( "File Delete Success\n" );
}

/**
 *  ��Ʈ ���͸��� ���� ������? ǥ��
 */
static void kShowRootDirectory( const char* pcParameterBuffer )
{
    DIR* pstDirectory;
    int i, iCount, iTotalCount;
    struct dirent* pstEntry;
    char vcBuffer[ 400 ];
    char vcTempValue[ 50 ];
    DWORD dwTotalByte;
    DWORD dwUsedClusterCount;
    FILESYSTEMMANAGER stManager;
    DIRECTORYENTRY* info;

    // ���� �ý��� ������ ����
    kGetFileSystemInformation( &stManager );
    //+
    info = kFindDir(curDirectoryClusterIdx);
    if(info==NULL) return ;
    for( i = 0 ; i < FILESYSTEM_MAXDIRECTORYENTRYCOUNT ; i++ )
    {
        if( info[ i ].dwStartClusterIndex != 0 )
        {

            pstEntry = &info[i];
            // ���� �������� �ʱ�ȭ �� �� �� ��ġ�� ���� ����
            kMemSet( vcBuffer, ' ', sizeof( vcBuffer ) - 1 );
            vcBuffer[ sizeof( vcBuffer ) - 1 ] = '\0';

            if(pstEntry->type == 0){
        // ���� �̸� ����
                kMemCpy( vcBuffer, pstEntry->d_name,
                         kStrLen( pstEntry->d_name ) );

                // ���� ���� ����
                kSPrintf( vcTempValue, "%d Byte", pstEntry->dwFileSize );
                kMemCpy( vcBuffer + 30, vcTempValue, kStrLen( vcTempValue )  );

                // ������ ���� Ŭ������ ����
                kSPrintf( vcTempValue, "0x%X Cluster", pstEntry->dwStartClusterIndex );
                kMemCpy( vcBuffer + 41, vcTempValue, kStrLen( vcTempValue ) );

                kSPrintf( vcTempValue, "%d-%d-%d %d:%d:%d", pstEntry->wYear, pstEntry->bMonth, pstEntry->bDayOfMonth,
                                                            pstEntry->bHour, pstEntry->bMinute, pstEntry->bSecond);
                kMemCpy( vcBuffer + 53, vcTempValue, kStrLen( vcTempValue ) + 1 );

                kPrintf( "    %s\n", vcBuffer );

            }

            else if(pstEntry->type == 1){
            // ���� �̸� ����
                kMemCpy( vcBuffer, pstEntry->d_name, kStrLen( pstEntry->d_name ) );

                // ���� ���� ����
                kSPrintf( vcTempValue, "Directory", 10 );
                kMemCpy( vcBuffer + 30, vcTempValue, kStrLen( vcTempValue ));

                kSPrintf( vcTempValue, "%d-%d-%d %d:%d:%d", pstEntry->wYear, pstEntry->bMonth, pstEntry->bDayOfMonth,
                                                            pstEntry->bHour, pstEntry->bMinute, pstEntry->bSecond);
                kMemCpy( vcBuffer + 53, vcTempValue, kStrLen( vcTempValue ) + 1 );

                kPrintf( "    %s\n", vcBuffer );
            }
        }
    }
    // // ��Ʈ ���͸��� ��
    // pstDirectory = opendir( "/" );
    // if( pstDirectory == NULL )
    // {
    //     kPrintf( "Root Directory Open Fail\n" );
    //     return ;
    // }

    // ���� ������ ���鼭 ���͸��� �ִ� ������ ������ ��ü ������ ������? ũ�⸦ ����?
    // iTotalCount = 0;
    // dwTotalByte = 0;
    // dwUsedClusterCount = 0;
    // while( 1 )
    // {
    //     // ���͸����� ��Ʈ�� �ϳ��� ����
    //     pstEntry = readdir( pstDirectory );
    //     // ���̻� ������ ������ ����
    //     if( pstEntry == NULL )
    //     {
    //         break;
    //     }
    //     iTotalCount++;
    //     dwTotalByte += pstEntry->dwFileSize;

    //     // ������ ���� Ŭ�������� ������ ����?
    //     if( pstEntry->dwFileSize == 0 )
    //     {
    //         // ũ�Ⱑ 0�̶� Ŭ������ 1���� �Ҵ�Ǿ��? ����
    //         dwUsedClusterCount++;
    //     }
    //     else
    //     {
    //         // Ŭ������ ������ �ø��Ͽ� ����
    //         dwUsedClusterCount += ( pstEntry->dwFileSize +
    //             ( FILESYSTEM_CLUSTERSIZE - 1 ) ) / FILESYSTEM_CLUSTERSIZE;
    //     }
    // }

    // // ���� ������ ������ ǥ���ϴ� ����
    // rewinddir( pstDirectory );
    // iCount = 0;
    // while( 1 )
    // {
    //     // ���͸����� ��Ʈ�� �ϳ��� ����
    //     pstEntry = readdir( pstDirectory );
    //     // ���̻� ������ ������ ����
    //     if( pstEntry == NULL )
    //     {
    //         break;
    //     }

    //     // ���� �������� �ʱ�ȭ �� �� �� ��ġ�� ���� ����
    //     kMemSet( vcBuffer, ' ', sizeof( vcBuffer ) - 1 );
    //     vcBuffer[ sizeof( vcBuffer ) - 1 ] = '\0';

    //     // ���� �̸� ����
    //     kMemCpy( vcBuffer, pstEntry->d_name,
    //              kStrLen( pstEntry->d_name ) );

    //     // ���� ���� ����
    //     kSPrintf( vcTempValue, "%d Byte", pstEntry->dwFileSize );
    //     kMemCpy( vcBuffer + 30, vcTempValue, kStrLen( vcTempValue ) );

    //     // ������ ���� Ŭ������ ����
    //     kSPrintf( vcTempValue, "0x%X Cluster", pstEntry->dwStartClusterIndex );
    //     kMemCpy( vcBuffer + 55, vcTempValue, kStrLen( vcTempValue ) + 1 );
    //     kPrintf( "    %s\n", vcBuffer );

    //     if( ( iCount != 0 ) && ( ( iCount % 20 ) == 0 ) )
    //     {
    //         kPrintf( "Press any key to continue... ('q' is exit) : " );
    //         if( kGetCh() == 'q' )
    //         {
    //             kPrintf( "\n" );
    //             break;
    //         }
    //     }
    //     iCount++;
    // }

    // // �� ������ ������ ������ �� ũ�⸦ ����?
    // kPrintf( "\t\tTotal File Count: %d\n", iTotalCount );
    // kPrintf( "\t\tTotal File Size: %d KByte (%d Cluster)\n", dwTotalByte,
    //          dwUsedClusterCount );

    // // ���� Ŭ������ ���� �̿��ؼ� ���� ������ ����?
    // kPrintf( "\t\tFree Space: %d KByte (%d Cluster)\n",
    //          ( stManager.dwTotalClusterCount - dwUsedClusterCount ) *
    //          FILESYSTEM_CLUSTERSIZE / 1024, stManager.dwTotalClusterCount -
    //          dwUsedClusterCount );

    // // ���͸��� ����
    // closedir( pstDirectory );
}

/**
 *  ������ �����Ͽ� Ű������? �Էµ� �����͸� ��
 */
static void kWriteDataToFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    FILE* fp;
    int iEnterCount;
    BYTE bKey;

    // �Ķ�����? ����Ʈ�� �ʱ�ȭ�Ͽ� ���� �̸��� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        kPrintf( "Too Long or Too Short File Name\n" );
        return ;
    }

    // ���� ����
    fp = fopen( vcFileName, "w" );
    if( fp == NULL )
    {
        kPrintf( "%s File Open Fail\n", vcFileName );
        return ;
    }

    // ���� Ű�� �������� 3�� ������ ������ ������ ���Ͽ� ��
    iEnterCount = 0;
    while( 1 )
    {
        bKey = kGetCh();
        // ���� Ű�̸� ���� 3�� �������°� Ȯ���Ͽ� ������ ���� ����
        if( bKey == KEY_ENTER )
        {
            iEnterCount++;
            if( iEnterCount >= 3 )
            {
                break;
            }
        }
        // ���� Ű�� �ƴ϶���? ���� Ű �Է� Ƚ���� �ʱ�ȭ
        else
        {
            iEnterCount = 0;
        }

        kPrintf( "%c", bKey );
        if( fwrite( &bKey, 1, 1, fp ) != 1 )
        {
            kPrintf( "File Wirte Fail\n" );
            break;
        }
    }

    kPrintf( "File Create Success\n" );
    fclose( fp );
}

/**
 *  ������ ��� �����͸� ����
 */
static void kReadDataFromFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    FILE* fp;
    int iEnterCount;
    BYTE bKey;

    // �Ķ�����? ����Ʈ�� �ʱ�ȭ�Ͽ� ���� �̸��� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        kPrintf( "Too Long or Too Short File Name\n" );
        return ;
    }

    // ���� ����
    fp = fopen( vcFileName, "r" );
    if( fp == NULL )
    {
        kPrintf( "%s File Open Fail\n", vcFileName );
        return ;
    }

    // ������ ������ ����ϴ��? ���� �ݺ�
    iEnterCount = 0;
    while( 1 )
    {
        if( fread( &bKey, 1, 1, fp ) != 1 )
        {
            break;
        }
        kPrintf( "%c", bKey );

        // ���� ���� Ű�̸� ���� Ű Ƚ���� ������Ű�� 20���α��� ����ߴٸ��?
        // �� ��������? ���θ� ���
        if( bKey == KEY_ENTER )
        {
            iEnterCount++;

            if( ( iEnterCount != 0 ) && ( ( iEnterCount % 20 ) == 0 ) )
            {
                kPrintf( "Press any key to continue... ('q' is exit) : " );
                if( kGetCh() == 'q' )
                {
                    kPrintf( "\n" );
                    break;
                }
                kPrintf( "\n" );
                iEnterCount = 0;
            }
        }
    }
    fclose( fp );
}

/**
 *  ���� I/O�� ���õ� ������? �׽�Ʈ
 */
static void kTestFileIO( const char* pcParameterBuffer )
{
    FILE* pstFile;
    BYTE* pbBuffer;
    int i;
    int j;
    DWORD dwRandomOffset;
    DWORD dwByteCount;
    BYTE vbTempBuffer[ 1024 ];
    DWORD dwMaxFileSize;

    kPrintf( "================== File I/O Function Test ==================\n" );

    // 4Mbyte�� ���� �Ҵ�
    dwMaxFileSize = 4 * 1024 * 1024;
    pbBuffer = kAllocateMemory( dwMaxFileSize );
    if( pbBuffer == NULL )
    {
        kPrintf( "Memory Allocation Fail\n" );
        return ;
    }
    // �׽�Ʈ�� ������ ����
    remove( "testfileio.bin" );

    //==========================================================================
    // ���� ���� �׽�Ʈ
    //==========================================================================
    kPrintf( "1. File Open Fail Test..." );
    // r �ɼ��� ������ �������� �����Ƿ�, �׽�Ʈ ������ ���� ����? NULL�� �Ǿ���? ��
    pstFile = fopen( "testfileio.bin", "r" );
    if( pstFile == NULL )
    {
        kPrintf( "[Pass]\n" );
    }
    else
    {
        kPrintf( "[Fail]\n" );
        fclose( pstFile );
    }

    //==========================================================================
    // ���� ���� �׽�Ʈ
    //==========================================================================
    kPrintf( "2. File Create Test..." );
    // w �ɼ��� ������ �����ϹǷ�, ���������� �ڵ��� ��ȯ�Ǿ�����?
    pstFile = fopen( "testfileio.bin", "w" );
    if( pstFile != NULL )
    {
        kPrintf( "[Pass]\n" );
        kPrintf( "    File Handle [0x%Q]\n", pstFile );
    }
    else
    {
        kPrintf( "[Fail]\n" );
    }

    //==========================================================================
    // �������� ���� ���� �׽�Ʈ
    //==========================================================================
    kPrintf( "3. Sequential Write Test(Cluster Size)..." );
    // ���� �ڵ��� ������ ���� ����
    for( i = 0 ; i < 100 ; i++ )
    {
        kMemSet( pbBuffer, i, FILESYSTEM_CLUSTERSIZE );
        if( fwrite( pbBuffer, 1, FILESYSTEM_CLUSTERSIZE, pstFile ) !=
            FILESYSTEM_CLUSTERSIZE )
        {
            kPrintf( "[Fail]\n" );
            kPrintf( "    %d Cluster Error\n", i );
            break;
        }
    }
    if( i >= 100 )
    {
        kPrintf( "[Pass]\n" );
    }

    //==========================================================================
    // �������� ���� �б� �׽�Ʈ
    //==========================================================================
    kPrintf( "4. Sequential Read And Verify Test(Cluster Size)..." );
    // ������ ó������ �̵�
    fseek( pstFile, -100 * FILESYSTEM_CLUSTERSIZE, SEEK_END );

    // ���� �ڵ��� ������ �б� ���� ��, ������ ����
    for( i = 0 ; i < 100 ; i++ )
    {
        // ������ ����
        if( fread( pbBuffer, 1, FILESYSTEM_CLUSTERSIZE, pstFile ) !=
            FILESYSTEM_CLUSTERSIZE )
        {
            kPrintf( "[Fail]\n" );
            return ;
        }

        // ������ �˻�
        for( j = 0 ; j < FILESYSTEM_CLUSTERSIZE ; j++ )
        {
            if( pbBuffer[ j ] != ( BYTE ) i )
            {
                kPrintf( "[Fail]\n" );
                kPrintf( "    %d Cluster Error. [%X] != [%X]\n", i, pbBuffer[ j ],
                         ( BYTE ) i );
                break;
            }
        }
    }
    if( i >= 100 )
    {
        kPrintf( "[Pass]\n" );
    }

    //==========================================================================
    // ������ ���� ���� �׽�Ʈ
    //==========================================================================
    kPrintf( "5. Random Write Test...\n" );

    // ���۸� ����? 0���� ä��
    kMemSet( pbBuffer, 0, dwMaxFileSize );
    // ���� ���⿡ �Űܴٴϸ鼭 �����͸� ���� ����
    // ������ ������ �о ���۷� ����
    fseek( pstFile, -100 * FILESYSTEM_CLUSTERSIZE, SEEK_CUR );
    fread( pbBuffer, 1, dwMaxFileSize, pstFile );

    // ������ ��ġ�� �ű���? �����͸� ���ϰ� ���ۿ� ���ÿ� ��
    for( i = 0 ; i < 100 ; i++ )
    {
        dwByteCount = ( kRandom() % ( sizeof( vbTempBuffer ) - 1 ) ) + 1;
        dwRandomOffset = kRandom() % ( dwMaxFileSize - dwByteCount );

        kPrintf( "    [%d] Offset [%d] Byte [%d]...", i, dwRandomOffset,
                dwByteCount );

        // ���� �����͸� �̵�
        fseek( pstFile, dwRandomOffset, SEEK_SET );
        kMemSet( vbTempBuffer, i, dwByteCount );

        // �����͸� ��
        if( fwrite( vbTempBuffer, 1, dwByteCount, pstFile ) != dwByteCount )
        {
            kPrintf( "[Fail]\n" );
            break;
        }
        else
        {
            kPrintf( "[Pass]\n" );
        }

        kMemSet( pbBuffer + dwRandomOffset, i, dwByteCount );
    }

    // �� ���������� �̵��Ͽ� 1����Ʈ�� �Ἥ ������ ũ�⸦ 4Mbyte�� ����
    fseek( pstFile, dwMaxFileSize - 1, SEEK_SET );
    fwrite( &i, 1, 1, pstFile );
    pbBuffer[ dwMaxFileSize - 1 ] = ( BYTE ) i;

    //==========================================================================
    // ������ ���� �б� �׽�Ʈ
    //==========================================================================
    kPrintf( "6. Random Read And Verify Test...\n" );
    // ������ ��ġ�� �ű���? ���Ͽ��� �����͸� �о� ������ ������? ��
    for( i = 0 ; i < 100 ; i++ )
    {
        dwByteCount = ( kRandom() % ( sizeof( vbTempBuffer ) - 1 ) ) + 1;
        dwRandomOffset = kRandom() % ( ( dwMaxFileSize ) - dwByteCount );

        kPrintf( "    [%d] Offset [%d] Byte [%d]...", i, dwRandomOffset,
                dwByteCount );

        // ���� �����͸� �̵�
        fseek( pstFile, dwRandomOffset, SEEK_SET );

        // ������ ����
        if( fread( vbTempBuffer, 1, dwByteCount, pstFile ) != dwByteCount )
        {
            kPrintf( "[Fail]\n" );
            kPrintf( "    Read Fail\n", dwRandomOffset );
            break;
        }

        // ���ۿ� ��
        if( kMemCmp( pbBuffer + dwRandomOffset, vbTempBuffer, dwByteCount )
                != 0 )
        {
            kPrintf( "[Fail]\n" );
            kPrintf( "    Compare Fail\n", dwRandomOffset );
            break;
        }

        kPrintf( "[Pass]\n" );
    }

    //==========================================================================
    // �ٽ� �������� ���� �б� �׽�Ʈ
    //==========================================================================
    kPrintf( "7. Sequential Write, Read And Verify Test(1024 Byte)...\n" );
    // ������ ó������ �̵�
    fseek( pstFile, -dwMaxFileSize, SEEK_CUR );

    // ���� �ڵ��� ������ ���� ����. �պκп��� 2Mbyte�� ��
    for( i = 0 ; i < ( 2 * 1024 * 1024 / 1024 ) ; i++ )
    {
        kPrintf( "    [%d] Offset [%d] Byte [%d] Write...", i, i * 1024, 1024 );

        // 1024 ����Ʈ�� ������ ��
        if( fwrite( pbBuffer + ( i * 1024 ), 1, 1024, pstFile ) != 1024 )
        {
            kPrintf( "[Fail]\n" );
            return ;
        }
        else
        {
            kPrintf( "[Pass]\n" );
        }
    }

    // ������ ó������ �̵�
    fseek( pstFile, -dwMaxFileSize, SEEK_SET );

    // ���� �ڵ��� ������ �б� ���� �� ������ ����. Random Write�� �����Ͱ� �߸�
    // ������? �� �����Ƿ� ������ 4Mbyte ��ü�� ��������? ��
    for( i = 0 ; i < ( dwMaxFileSize / 1024 )  ; i++ )
    {
        // ������ �˻�
        kPrintf( "    [%d] Offset [%d] Byte [%d] Read And Verify...", i,
                i * 1024, 1024 );

        // 1024 ����Ʈ�� ������ ����
        if( fread( vbTempBuffer, 1, 1024, pstFile ) != 1024 )
        {
            kPrintf( "[Fail]\n" );
            return ;
        }

        if( kMemCmp( pbBuffer + ( i * 1024 ), vbTempBuffer, 1024 ) != 0 )
        {
            kPrintf( "[Fail]\n" );
            break;
        }
        else
        {
            kPrintf( "[Pass]\n" );
        }
    }

    //==========================================================================
    // ���� ���� ���� �׽�Ʈ
    //==========================================================================
    kPrintf( "8. File Delete Fail Test..." );
    // ������ �����ִ� �����̹Ƿ� ������ ��������? �ϸ� �����ؾ� ��
    if( remove( "testfileio.bin" ) != 0 )
    {
        kPrintf( "[Pass]\n" );
    }
    else
    {
        kPrintf( "[Fail]\n" );
    }

    //==========================================================================
    // ���� �ݱ� �׽�Ʈ
    //==========================================================================
    kPrintf( "9. File Close Test..." );
    // ������ ���������� ������ ��
    if( fclose( pstFile ) == 0 )
    {
        kPrintf( "[Pass]\n" );
    }
    else
    {
        kPrintf( "[Fail]\n" );
    }

    //==========================================================================
    // ���� ���� �׽�Ʈ
    //==========================================================================
    kPrintf( "10. File Delete Test..." );
    // ������ �������Ƿ� ���������� �������� ��
    if( remove( "testfileio.bin" ) == 0 )
    {
        kPrintf( "[Pass]\n" );
    }
    else
    {
        kPrintf( "[Fail]\n" );
    }

    // �޸𸮸� ����
    kFreeMemory( pbBuffer );
}

/**
 *  ������ �а� ���� �ӵ��� ����
 */
static void kTestPerformance( const char* pcParameterBuffer )
{
    FILE* pstFile;
    DWORD dwClusterTestFileSize;
    DWORD dwOneByteTestFileSize;
    QWORD qwLastTickCount;
    DWORD i;
    BYTE* pbBuffer;

    // Ŭ�����ʹ� 1Mbyte���� ������ �׽�Ʈ
    dwClusterTestFileSize = 1024 * 1024;
    // 1����Ʈ�� �а� ���� �׽�Ʈ�� �ð��� ���� �ɸ��Ƿ� 16Kbyte�� �׽�Ʈ
    dwOneByteTestFileSize = 16 * 1024;

    // �׽�Ʈ�� ���� �޸� �Ҵ�
    pbBuffer = kAllocateMemory( dwClusterTestFileSize );
    if( pbBuffer == NULL )
    {
        kPrintf( "Memory Allocate Fail\n" );
        return ;
    }

    // ���۸� �ʱ�ȭ
    kMemSet( pbBuffer, 0, FILESYSTEM_CLUSTERSIZE );

    kPrintf( "================== File I/O Performance Test ==================\n" );

    //==========================================================================
    // Ŭ������ ������ ������ ���������� ���� �׽�Ʈ
    //==========================================================================
    kPrintf( "1.Sequential Read/Write Test(Cluster Size)\n" );

    // ������ �׽�Ʈ ������ �����ϰ� ���� ����
    remove( "performance.txt" );
    pstFile = fopen( "performance.txt", "w" );
    if( pstFile == NULL )
    {
        kPrintf( "File Open Fail\n" );
        kFreeMemory( pbBuffer );
        return ;
    }

    qwLastTickCount = kGetTickCount();
    // Ŭ������ ������ ���� �׽�Ʈ
    for( i = 0 ; i < ( dwClusterTestFileSize / FILESYSTEM_CLUSTERSIZE ) ; i++ )
    {
        if( fwrite( pbBuffer, 1, FILESYSTEM_CLUSTERSIZE, pstFile ) !=
            FILESYSTEM_CLUSTERSIZE )
        {
            kPrintf( "Write Fail\n" );
            // ������ �ݰ� �޸𸮸� ������
            fclose( pstFile );
            kFreeMemory( pbBuffer );
            return ;
        }
    }
    // �ð� ����?
    kPrintf( "   Sequential Write(Cluster Size): %d ms\n", kGetTickCount() -
             qwLastTickCount );

    //==========================================================================
    // Ŭ������ ������ ������ ���������� �д� �׽�Ʈ
    //==========================================================================
    // ������ ó������ �̵�
    fseek( pstFile, 0, SEEK_SET );

    qwLastTickCount = kGetTickCount();
    // Ŭ������ ������ �д� �׽�Ʈ
    for( i = 0 ; i < ( dwClusterTestFileSize / FILESYSTEM_CLUSTERSIZE ) ; i++ )
    {
        if( fread( pbBuffer, 1, FILESYSTEM_CLUSTERSIZE, pstFile ) !=
            FILESYSTEM_CLUSTERSIZE )
        {
            kPrintf( "Read Fail\n" );
            // ������ �ݰ� �޸𸮸� ������
            fclose( pstFile );
            kFreeMemory( pbBuffer );
            return ;
        }
    }
    // �ð� ����?
    kPrintf( "   Sequential Read(Cluster Size): %d ms\n", kGetTickCount() -
             qwLastTickCount );

    //==========================================================================
    // 1 ����Ʈ ������ ������ ���������� ���� �׽�Ʈ
    //==========================================================================
    kPrintf( "2.Sequential Read/Write Test(1 Byte)\n" );

    // ������ �׽�Ʈ ������ �����ϰ� ���� ����
    remove( "performance.txt" );
    pstFile = fopen( "performance.txt", "w" );
    if( pstFile == NULL )
    {
        kPrintf( "File Open Fail\n" );
        kFreeMemory( pbBuffer );
        return ;
    }

    qwLastTickCount = kGetTickCount();
    // 1 ����Ʈ ������ ���� �׽�Ʈ
    for( i = 0 ; i < dwOneByteTestFileSize ; i++ )
    {
        if( fwrite( pbBuffer, 1, 1, pstFile ) != 1 )
        {
            kPrintf( "Write Fail\n" );
            // ������ �ݰ� �޸𸮸� ������
            fclose( pstFile );
            kFreeMemory( pbBuffer );
            return ;
        }
    }
    // �ð� ����?
    kPrintf( "   Sequential Write(1 Byte): %d ms\n", kGetTickCount() -
             qwLastTickCount );

    //==========================================================================
    // 1 ����Ʈ ������ ������ ���������� �д� �׽�Ʈ
    //==========================================================================
    // ������ ó������ �̵�
    fseek( pstFile, 0, SEEK_SET );

    qwLastTickCount = kGetTickCount();
    // 1 ����Ʈ ������ �д� �׽�Ʈ
    for( i = 0 ; i < dwOneByteTestFileSize ; i++ )
    {
        if( fread( pbBuffer, 1, 1, pstFile ) != 1 )
        {
            kPrintf( "Read Fail\n" );
            // ������ �ݰ� �޸𸮸� ������
            fclose( pstFile );
            kFreeMemory( pbBuffer );
            return ;
        }
    }
    // �ð� ����?
    kPrintf( "   Sequential Read(1 Byte): %d ms\n", kGetTickCount() -
             qwLastTickCount );

    // ������ �ݰ� �޸𸮸� ������
    fclose( pstFile );
    kFreeMemory( pbBuffer );
}

/**
 *  ���� �ý����� ĳ�� ���ۿ� �ִ� �����͸� ����? �ϵ� ��ũ�� ��
 */
static void kFlushCache( const char* pcParameterBuffer )
{
    QWORD qwTickCount;

    qwTickCount = kGetTickCount();
    kPrintf( "Cache Flush... ");
    if( kFlushFileSystemCache() == TRUE )
    {
        kPrintf( "Pass\n" );
    }
    else
    {
        kPrintf( "Fail\n" );
    }
    kPrintf( "Total Time = %d ms\n", kGetTickCount() - qwTickCount );
}
static void kMkdir( const char* pcParamegerBuffer ){
    FILE* pstFile;
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    DWORD dwCluster;

    kInitializeParameter( &stList, pcParamegerBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        kPrintf( "Too Long or Too Short Directory Name\n" );
        return ;
    }



    pstFile = opendir( vcFileName);
    if( pstFile != NULL )
    {
        fclose( pstFile );
        kPrintf( "Done!\n" );
        return ;
    }
    kPrintf( "Fail!\n" );
    return;
}
static void kCd( const char* pcParamegerBuffer){
    PARAMETERLIST stList;
    char vcFileName[150];
    char vcRestName[150];
    int iLength;
    DWORD dwCluster;
    int i;
    FILE* pstFile;
    DIR* pstDirectory;
    FILESYSTEMMANAGER stManager;
    DIRECTORYENTRY* info;
    char backPath[150] = "\0";
    DWORD backIdx = 0;
    char tempPath[150] = "\0";
    DWORD tempIdx = 0;
    BOOL doesFoundDir = FALSE;

    kInitializeParameter( &stList, pcParamegerBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        kPrintf( "Too Long or Too Short Directory Name\n" );
        return ;
    }
    kGetFileSystemInformation( &stManager );
    info = kFindDir(curDirectoryClusterIdx);

    if(kMemCmp(vcFileName,".",2)==0){
        curDirectoryClusterIdx = info[0].parentCluserIdx;
        kSetClusterIndex(curDirectoryClusterIdx);
        kMemCpy(pwd,info[0].parentPath,kStrLen(info[0].parentPath)+1);
     }
    else if(kMemCmp(vcFileName,"..",3)==0){

        curDirectoryClusterIdx = info[1].parentCluserIdx;
        kSetClusterIndex(curDirectoryClusterIdx);

        kMemCpy(pwd,info[1].parentPath,kStrLen(info[1].parentPath)+1);
     }
    // ���ڷ� �����͸� �̸��̶���?
    else{
        doesFoundDir = FALSE;
        if( kMemCmp(vcFileName, "/", 1) == 0 ){
          //kPrintf("Starting with Root");
          kMemCpy(backPath,"/",2);
          backIdx = 0;
          kMemCpy(vcRestName,(vcFileName+1),kStrLen(vcFileName+1)+1);
          info = kFindDir(backIdx);
        }
        else if( kMemCmp(vcFileName,"./",2) == 0 ){
          //kPrintf("Starting with ./");
          kMemCpy(vcRestName,vcFileName + 2,kStrLen(vcFileName)+1);
          kMemCpy(backPath,pwd,kStrLen(pwd));
          backIdx = curDirectoryClusterIdx;
          info = kFindDir(backIdx);
        }
        else if( kMemCmp(vcFileName,"../",3) == 0 ){
          //kPrintf("Starting with ../");
          kMemCpy(vcRestName,vcFileName + 3,kStrLen(vcFileName)+1);
          kMemCpy(backPath,info[1].parentPath,kStrLen(info[1].parentPath)+1);
          backIdx = info[1].parentCluserIdx;;
          info = kFindDir(backIdx);
        }
        else{
          kMemCpy(vcRestName,vcFileName,kStrLen(vcFileName)+1);
          kMemCpy(backPath,pwd,kStrLen(pwd));
          backIdx = curDirectoryClusterIdx;
          info = kFindDir(backIdx);
        }
        //kPrintf("BACK:%s\n", backPath);
        //kPrintf("OIf:%s, %s\n", vcRestName, vcFileName);
        while( vcRestName[0] != '\0' ){
          //kPrintf("Bef:%s\n", vcRestName);
          for( int k = 1; k < 150; k++){
            if( vcRestName[k]=='/' ){
              vcRestName[k] = '\0';
              kMemCpy(vcFileName, vcRestName, k);
              //kPrintf("%dIf:%s, %s\n", k, vcRestName, vcRestName+(k+1));
              kMemCpy(vcRestName, vcRestName+(k+1), 149 - k);
              vcFileName[k] = '\0';
              break;
            }
            else if( vcRestName[k]=='\0' ){
              kMemCpy(vcFileName, vcRestName, k);
              //kPrintf("%dIf:%s\n", k, vcRestName);
              kMemCpy(vcRestName, "\0", 150);
              vcFileName[k] = '\0';
              break;
            }
          }
          //kPrintf("Aft:%s\n", vcRestName);
          for( int j = 0 ; j < 10 ; j++ )
          {
              //kPrintf("INFO[%d] %s : %s (%d %d %d)\n",j, vcFileName ,info[j].vcFileName, info[j].type, info[ j ].dwStartClusterIndex, kMemCmp(info[ j ].vcFileName,vcFileName,kStrLen(vcFileName)) );
              if( info[j].type ==1 && info[ j ].dwStartClusterIndex != 0 && kMemCmp(info[ j ].vcFileName,vcFileName,kStrLen(vcFileName))==0 )
              {
                  kMemCpy(tempPath,backPath,kStrLen(pwd)+1);
                  tempIdx = backIdx;

                  if(kMemCmp(backPath,"/",2)==0)
                  {
                      kMemCpy(backPath + kStrLen(backPath),vcFileName,kStrLen(vcFileName)+1);
                      backIdx = info[ j ].dwStartClusterIndex;
                  }
                  else if( j == 1 ){
                      kMemCpy(backPath, info[j].parentPath, kStrLen(info[j].parentPath)+1);
                      backIdx = info[ j ].parentCluserIdx;
                      info = kFindDir(backIdx);
                      kMemCpy(tempPath, info[1].parentPath, kStrLen(info[j].parentPath)+1);
                      tempIdx = info[ 1 ].parentCluserIdx;
                  }
                  else{
                      kMemCpy(backPath + kStrLen(backPath),"/",1);
                      kMemCpy(backPath + kStrLen(backPath),vcFileName,kStrLen(vcFileName)+1);
                      backIdx = info[ j ].dwStartClusterIndex;
                  }

                  info = kFindDir(backIdx);

                  doesFoundDir = TRUE;

                  break;

              }
              else{
                doesFoundDir = FALSE;
              }
          }

          if(!doesFoundDir){
            break;
          }

        }

        if(doesFoundDir){
          kMemCpy(pwd, backPath, kStrLen(backPath)+1);
          curDirectoryClusterIdx = backIdx;
          kSetClusterIndex(curDirectoryClusterIdx);
          info = kFindDir(curDirectoryClusterIdx);
          if( info[0].dwStartClusterIndex != -1 )
          {
              kMakeDot();
              kUpdateDir(0,".",pwd,curDirectoryClusterIdx);

          }
          info[1].parentCluserIdx = tempIdx;
          kMemCpy(info[1].parentCluserIdx,tempPath,kStrLen(tempPath)+1);
          kUpdateDir( 1,"..",tempPath, tempIdx );
        }
        else if( kMemCmp(vcFileName, "/", 1) == 0 ){
          kMemCpy(pwd, backPath, kStrLen(backPath)+1);
          curDirectoryClusterIdx = backIdx;
          kSetClusterIndex(curDirectoryClusterIdx);
          info = kFindDir(curDirectoryClusterIdx);
        }
        else{
          kPrintf("Directory Not Found\n");
        }


    }
}
static void kRmdir( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    kInitializeParameter( &stList, pcParameterBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        kPrintf( "Too Long or Too Short Directory Name\n" );
        return ;
    }
    if( remove( vcFileName ) != 0 )
    {
        kPrintf( "Directory Not Found or Opened\n" );
        return ;
    }

    kPrintf( "Directory Delete Success\n" );
}
