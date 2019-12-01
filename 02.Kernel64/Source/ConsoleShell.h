/**
 *  file    ConsoleShell.h
 *  date    2009/01/31
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   占쌤쇽옙 占싻울옙 占쏙옙占시듸옙 占쏙옙占� 占쏙옙占쏙옙
 */

#ifndef __CONSOLESHELL_H__
#define __CONSOLESHELL_H__

#include "Types.h"

////////////////////////////////////////////////////////////////////////////////
//
// 占쏙옙크占쏙옙
//
////////////////////////////////////////////////////////////////////////////////
#define CONSOLESHELL_MAXCOMMANDBUFFERCOUNT  300
#define CONSOLESHELL_PROMPTMESSAGE          "MINT64:"

// 占쏙옙占쌘울옙 占쏙옙占쏙옙占싶몌옙 占식띰옙占쏙옙庫占� 占쌨댐옙 占쌉쇽옙 占쏙옙占쏙옙占쏙옙 타占쏙옙 占쏙옙占쏙옙
typedef void ( * CommandFunction ) ( const char* pcParameter );


////////////////////////////////////////////////////////////////////////////////
//
// 占쏙옙占쏙옙체
//
////////////////////////////////////////////////////////////////////////////////
// 1占쏙옙占쏙옙트占쏙옙 占쏙옙占쏙옙
#pragma pack( push, 1 )

// 占쏙옙占쏙옙 커占실드를 占쏙옙占쏙옙占싹댐옙 占쌘료구占쏙옙
typedef struct kShellCommandEntryStruct
{
    // 커占실듸옙 占쏙옙占쌘울옙
    char* pcCommand;
    // 커占실듸옙占쏙옙 占쏙옙占쏙옙
    char* pcHelp;
    // 커占실드를 占쏙옙占쏙옙占싹댐옙 占쌉쇽옙占쏙옙 占쏙옙占쏙옙占쏙옙
    CommandFunction pfFunction;
} SHELLCOMMANDENTRY;

// 占식띰옙占쏙옙拷占� 처占쏙옙占싹깍옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占싹댐옙 占쌘료구占쏙옙
typedef struct kParameterListStruct
{
    // 占식띰옙占쏙옙占� 占쏙옙占쏙옙占쏙옙 占쏙옙藥뱄옙占�
    const char* pcBuffer;
    // 占식띰옙占쏙옙占쏙옙占� 占쏙옙占쏙옙
    int iLength;
    // 占쏙옙占쏙옙 처占쏙옙占쏙옙 占식띰옙占쏙옙叩占� 占쏙옙占쏙옙占싹댐옙 占쏙옙치
    int iCurrentPosition;
} PARAMETERLIST;

#pragma pack( pop )

////////////////////////////////////////////////////////////////////////////////
//
// 占쌉쇽옙
//
////////////////////////////////////////////////////////////////////////////////
// 占쏙옙占쏙옙 占쏙옙 占쌘듸옙
void kStartConsoleShell( void );
void kExecuteCommand( const char* pcCommandBuffer );
void kInitializeParameter( PARAMETERLIST* pstList, const char* pcParameter );
int kGetNextParameter( PARAMETERLIST* pstList, char* pcParameter );

// 커占실드를 처占쏙옙占싹댐옙 占쌉쇽옙
static void kHelp( const char* pcParameterBuffer );
static void kCls( const char* pcParameterBuffer );
static void kShowTotalRAMSize( const char* pcParameterBuffer );
static void kStringToDecimalHexTest( const char* pcParameterBuffer );
static void kShutdown( const char* pcParamegerBuffer );
static void ypchoLove();
static void ypchang();
static void ypkim();
static void kRaiseFault();
static void kSetTimer( const char* pcParameterBuffer );
static void kWaitUsingPIT( const char* pcParameterBuffer );
static void kReadTimeStampCounter( const char* pcParameterBuffer );
static void kMeasureProcessorSpeed( const char* pcParameterBuffer );
static void kShowDateAndTime( const char* pcParameterBuffer );
static void kCreateTestTask( const char* pcParameterBuffer );
static void kChangeTaskPriority( const char* pcParameterBuffer );
static void kShowTaskList( const char* pcParameterBuffer );
static void kKillTask( const char* pcParameterBuffer );
static void kCPULoad( const char* pcParameterBuffer );
static void kTestMutex( const char* pcParameterBuffer );
static void kCreateThreadTask( void );
static void kTestThread( const char* pcParameterBuffer );
static void kRand();
static void kShowMatrix( const char* pcParameterBuffer );
static void kTestPIE( const char* pcParameterBuffer );
static void kShowDyanmicMemoryInformation( const char* pcParameterBuffer );
static void kTestSequentialAllocation( const char* pcParameterBuffer );
static void kTestRandomAllocation( const char* pcParameterBuffer );
static void kRandomAllocationTask( void );
static void kShowHDDInformation( const char* pcParameterBuffer );
static void kReadSector( const char* pcParameterBuffer );
static void kWriteSector( const char* pcParameterBuffer );
static void kMountHDD( const char* pcParameterBuffer );
static void kFormatHDD( const char* pcParameterBuffer );
static void kShowFileSystemInformation( const char* pcParameterBuffer );
static void kCreateFileInRootDirectory( const char* pcParameterBuffer );
static void kDeleteFileInRootDirectory( const char* pcParameterBuffer );
static void kShowRootDirectory( const char* pcParameterBuffer );
static void kWriteDataToFile( const char* pcParameterBuffer );
static void kReadDataFromFile( const char* pcParameterBuffer );
static void kTestFileIO( const char* pcParameterBuffer );
static void kFlushCache( const char* pcParameterBuffer );
static void kTestPerformance( const char* pcParameterBuffer );
static void kMkdir( const char* pcParamegerBuffer );
#endif /*__CONSOLESHELL_H__*/
