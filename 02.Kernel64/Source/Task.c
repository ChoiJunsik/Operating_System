/**
 *  file    Task.c
 *  date    2009/02/19
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ï¿½Â½ï¿½Å©ï¿½ï¿½ Ã³ï¿½ï¿½ï¿½Ï´ï¿½ ï¿½Ô¼ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ãµï¿½ ï¿½ï¿½ï¿½ï¿½
 */

#include "Task.h"
#include "Descriptor.h"

// ï¿½ï¿½ï¿½ï¿½ï¿½Ù·ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ú·á±¸ï¿½ï¿½
static SCHEDULER gs_stScheduler;
static TCBPOOLMANAGER gs_stTCBPoolManager;

// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ß»ï¿½ï¿½ï¿½Å°ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
static volatile QWORD gs_qwRandomTaskValue = 0;

/**
 *  ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½È¯
 */
QWORD kTaskRandom( void )
{
    gs_qwRandomTaskValue = ( gs_qwRandomTaskValue * 412153 + 5571031 ) >> 16;
    return gs_qwRandomTaskValue;
}

//==============================================================================
//  ï¿½Â½ï¿½Å© Ç®ï¿½ï¿½ ï¿½Â½ï¿½Å© ï¿½ï¿½ï¿½ï¿½
//==============================================================================
/**
 *  ï¿½Â½ï¿½Å© Ç® ï¿½Ê±ï¿½È­
 */
static void kInitializeTCBPool( void )
{
    int i;

    kMemSet( &( gs_stTCBPoolManager ), 0, sizeof( gs_stTCBPoolManager ) );

    // ï¿½Â½ï¿½Å© Ç®ï¿½ï¿½ ï¿½ï¿½ï¿½å·¹ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï°ï¿½ ï¿½Ê±ï¿½È­
    gs_stTCBPoolManager.pstStartAddress = ( TCB* ) TASK_TCBPOOLADDRESS;
    kMemSet( TASK_TCBPOOLADDRESS, 0, sizeof( TCB ) * TASK_MAXCOUNT );

    // TCBï¿½ï¿½ ID ï¿½Ò´ï¿½
    for( i = 0 ; i < TASK_MAXCOUNT ; i++ )
    {
        gs_stTCBPoolManager.pstStartAddress[ i ].stLink.qwID = i;
    }

    // TCBï¿½ï¿½ ï¿½Ö´ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ò´ï¿½ï¿½ï¿½ È½ï¿½ï¿½ï¿½ï¿½ ï¿½Ê±ï¿½È­
    gs_stTCBPoolManager.iMaxCount = TASK_MAXCOUNT;
    gs_stTCBPoolManager.iAllocatedCount = 1;
}

/**
 *  TCBï¿½ï¿½ ï¿½Ò´ï¿½ ï¿½ï¿½ï¿½ï¿½
 */
static TCB* kAllocateTCB( void )
{
    TCB* pstEmptyTCB;
    int i;

    if( gs_stTCBPoolManager.iUseCount == gs_stTCBPoolManager.iMaxCount )
    {
        return NULL;
    }

    for( i = 0 ; i < gs_stTCBPoolManager.iMaxCount ; i++ )
    {
        // IDï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ 32ï¿½ï¿½Æ®ï¿½ï¿½ 0ï¿½Ì¸ï¿½ ï¿½Ò´ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ TCB
        if( ( gs_stTCBPoolManager.pstStartAddress[ i ].stLink.qwID >> 32 ) == 0 )
        {
            pstEmptyTCB = &( gs_stTCBPoolManager.pstStartAddress[ i ] );
            break;
        }
    }

    // ï¿½ï¿½ï¿½ï¿½ 32ï¿½ï¿½Æ®ï¿½ï¿½ 0ï¿½ï¿½ ï¿½Æ´ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ø¼ï¿½ ï¿½Ò´ï¿½ï¿½ï¿½ TCBï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    pstEmptyTCB->stLink.qwID = ( ( QWORD ) gs_stTCBPoolManager.iAllocatedCount << 32 ) | i;
    gs_stTCBPoolManager.iUseCount++;
    gs_stTCBPoolManager.iAllocatedCount++;
    if( gs_stTCBPoolManager.iAllocatedCount == 0 )
    {
        gs_stTCBPoolManager.iAllocatedCount = 1;
    }

    return pstEmptyTCB;
}

/**
 *  TCBï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
 */
static void kFreeTCB( QWORD qwID )
{
    int i;

    // ï¿½Â½ï¿½Å© IDï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ 32ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½Îµï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½
    i = GETTCBOFFSET( qwID );

    // TCBï¿½ï¿½ ï¿½Ê±ï¿½È­ï¿½Ï°ï¿½ ID ï¿½ï¿½ï¿½ï¿½
    kMemSet( &( gs_stTCBPoolManager.pstStartAddress[ i ].stContext ), 0, sizeof( CONTEXT ) );
    gs_stTCBPoolManager.pstStartAddress[ i ].stLink.qwID = i;

    gs_stTCBPoolManager.iUseCount--;
}

/**
 *  ÅÂ½ºÅ©¸¦ »ý¼º
 *      ÅÂ½ºÅ© ID¿¡ µû¶ó¼­ ½ºÅÃ Ç®¿¡¼­ ½ºÅÃ ÀÚµ¿ ÇÒ´ç
 *      ÇÁ·Î¼¼½º ¹× ½º·¹µå ¸ðµÎ »ý¼º °¡´É
 */
TCB* kCreateTask( QWORD qwFlags, void* pvMemoryAddress, QWORD qwMemorySize,
                  QWORD qwEntryPointAddress )
{
    TCB* pstTask, * pstProcess;
    void* pvStackAddress;
    BOOL bPreviousFlag;

    // ÀÓ°è ¿µ¿ª ½ÃÀÛ
    bPreviousFlag = kLockForSystemData();
    pstTask = kAllocateTCB();
    if( pstTask == NULL )
    {
        // ÀÓ°è¿µ¿ª ³¡
        kUnlockForSystemData( bPreviousFlag );
        return NULL;
    }

    // ÇöÀç ÇÁ·Î¼¼½º ¶Ç´Â ½º·¹µå°¡ ¼ÓÇÑ ÇÁ·Î¼¼½º¸¦ °Ë»ö
    pstProcess = kGetProcessByThread( kGetRunningTask() );
    // ¸¸¾à ÇÁ·Î¼¼½º°¡ ¾ø´Ù¸é ¾Æ¹«·± ÀÛ¾÷µµ ÇÏÁö ¾ÊÀ½
    if( pstProcess == NULL )
    {
        kFreeTCB( pstTask->stLink.qwID );
        // ÀÓ°è ¿µ¿ª ³¡
        kUnlockForSystemData( bPreviousFlag );
        return NULL;
    }

    // ½º·¹µå¸¦ »ý¼ºÇÏ´Â °æ¿ì¶ó¸é ³»°¡ ¼ÓÇÑ ÇÁ·Î¼¼½ºÀÇ ÀÚ½Ä ½º·¹µå ¸®½ºÆ®¿¡ ¿¬°áÇÔ
    if( qwFlags & TASK_FLAGS_THREAD )
    {
        // ÇöÀç ½º·¹µåÀÇ ÇÁ·Î¼¼½º¸¦ Ã£¾Æ¼­ »ý¼ºÇÒ ½º·¹µå¿¡ ÇÁ·Î¼¼½º Á¤º¸¸¦ »ó¼Ó
        pstTask->qwParentProcessID = pstProcess->stLink.qwID;
        pstTask->pvMemoryAddress = pstProcess->pvMemoryAddress;
        pstTask->qwMemorySize = pstProcess->qwMemorySize;

        // ºÎ¸ð ÇÁ·Î¼¼½ºÀÇ ÀÚ½Ä ½º·¹µå ¸®½ºÆ®¿¡ Ãß°¡
        kAddListToTail( &( pstProcess->stChildThreadList ), &( pstTask->stThreadLink ) );
    }
    // ÇÁ·Î¼¼½º´Â ÆÄ¶ó¹ÌÅÍ·Î ³Ñ¾î¿Â °ªÀ» ±×´ë·Î ¼³Á¤
    else
    {
        pstTask->qwParentProcessID = pstProcess->stLink.qwID;
        pstTask->pvMemoryAddress = pvMemoryAddress;
        pstTask->qwMemorySize = qwMemorySize;
    }

    // ½º·¹µåÀÇ ID¸¦ ÅÂ½ºÅ© ID¿Í µ¿ÀÏÇÏ°Ô ¼³Á¤
    pstTask->stThreadLink.qwID = pstTask->stLink.qwID;
    // ÀÓ°è ¿µ¿ª ³¡
    kUnlockForSystemData( bPreviousFlag );

    // ÅÂ½ºÅ© ID·Î ½ºÅÃ ¾îµå·¹½º °è»ê, ÇÏÀ§ 32ºñÆ®°¡ ½ºÅÃ Ç®ÀÇ ¿ÀÇÁ¼Â ¿ªÇÒ ¼öÇà
    pvStackAddress = ( void* ) ( TASK_STACKPOOLADDRESS + ( TASK_STACKSIZE *
            GETTCBOFFSET( pstTask->stLink.qwID ) ) );

    // TCB¸¦ ¼³Á¤ÇÑ ÈÄ ÁØºñ ¸®½ºÆ®¿¡ »ðÀÔÇÏ¿© ½ºÄÉÁÙ¸µµÉ ¼ö ÀÖµµ·Ï ÇÔ
    kSetUpTask( pstTask, qwFlags, qwEntryPointAddress, pvStackAddress,
            TASK_STACKSIZE );

    // ÀÚ½Ä ½º·¹µå ¸®½ºÆ®¸¦ ÃÊ±âÈ­
    kInitializeList( &( pstTask->stChildThreadList ) );

    // ÀÓ°è ¿µ¿ª ½ÃÀÛ
    bPreviousFlag = kLockForSystemData();

    // ÅÂ½ºÅ©¸¦ ÁØºñ ¸®½ºÆ®¿¡ »ðÀÔ
    kAddTaskToReadyList( pstTask );

    // ÀÓ°è ¿µ¿ª ³¡
    kUnlockForSystemData( bPreviousFlag );

    return pstTask;
}

/**
 *  ÆÄ¶ó¹ÌÅÍ¸¦ ÀÌ¿ëÇØ¼­ TCB¸¦ ¼³Á¤
 */
static void kSetUpTask( TCB* pstTCB, QWORD qwFlags, QWORD qwEntryPointAddress,
                 void* pvStackAddress, QWORD qwStackSize )
{
    // ÄÜÅØ½ºÆ® ÃÊ±âÈ­
    kMemSet( pstTCB->stContext.vqRegister, 0, sizeof( pstTCB->stContext.vqRegister ) );

    // ½ºÅÃ¿¡ °ü·ÃµÈ RSP, RBP ·¹Áö½ºÅÍ ¼³Á¤
    pstTCB->stContext.vqRegister[ TASK_RSPOFFSET ] = ( QWORD ) pvStackAddress +
            qwStackSize - 8;
    pstTCB->stContext.vqRegister[ TASK_RBPOFFSET ] = ( QWORD ) pvStackAddress +
            qwStackSize - 8;

    // Return Address ¿µ¿ª¿¡ kExitTask() ÇÔ¼öÀÇ ¾îµå·¹½º¸¦ »ðÀÔÇÏ¿© ÅÂ½ºÅ©ÀÇ ¿£Æ®¸®
    // Æ÷ÀÎÆ® ÇÔ¼ö¸¦ ºüÁ®³ª°¨°ú µ¿½Ã¿¡ kExitTask() ÇÔ¼ö·Î ÀÌµ¿ÇÏµµ·Ï ÇÔ
    *( QWORD * ) ( ( QWORD ) pvStackAddress + qwStackSize - 8 ) = ( QWORD ) kExitTask;

    // ¼¼±×¸ÕÆ® ¼¿·ºÅÍ ¼³Á¤
    pstTCB->stContext.vqRegister[ TASK_CSOFFSET ] = GDT_KERNELCODESEGMENT;
    pstTCB->stContext.vqRegister[ TASK_DSOFFSET ] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[ TASK_ESOFFSET ] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[ TASK_FSOFFSET ] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[ TASK_GSOFFSET ] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[ TASK_SSOFFSET ] = GDT_KERNELDATASEGMENT;

    // RIP ·¹Áö½ºÅÍ¿Í ÀÎÅÍ·´Æ® ÇÃ·¡±× ¼³Á¤
    pstTCB->stContext.vqRegister[ TASK_RIPOFFSET ] = qwEntryPointAddress;

    // RFLAGS ·¹Áö½ºÅÍÀÇ IF ºñÆ®(ºñÆ® 9)¸¦ 1·Î ¼³Á¤ÇÏ¿© ÀÎÅÍ·´Æ® È°¼ºÈ­
    pstTCB->stContext.vqRegister[ TASK_RFLAGSOFFSET ] |= 0x0200;

    // ½ºÅÃ°ú ÇÃ·¡±× ÀúÀå
    pstTCB->pvStackAddress = pvStackAddress;
    pstTCB->qwStackSize = qwStackSize;
    pstTCB->qwFlags = qwFlags;
}

//==============================================================================
//  ½ºÄÉÁÙ·¯ °ü·Ã
//==============================================================================
/**
 *  ½ºÄÉÁÙ·¯¸¦ ÃÊ±âÈ­
 *      ½ºÄÉÁÙ·¯¸¦ ÃÊ±âÈ­ÇÏ´Âµ¥ ÇÊ¿äÇÑ TCB Ç®°ú init ÅÂ½ºÅ©µµ °°ÀÌ ÃÊ±âÈ­
 */
void kInitializeScheduler( void )
{
    int i;
    TCB* pstTask;

    // ÅÂ½ºÅ© Ç® ÃÊ±âÈ­
    kInitializeTCBPool();

    // ÁØºñ ¸®½ºÆ®¿Í ¿ì¼± ¼øÀ§º° ½ÇÇà È½¼ö¸¦ ÃÊ±âÈ­ÇÏ°í ´ë±â ¸®½ºÆ®µµ ÃÊ±âÈ­
    for( i = 0 ; i < TASK_MAXREADYLISTCOUNT ; i++ )
    {
        kInitializeList( &( gs_stScheduler.vstReadyList[ i ] ) );
        gs_stScheduler.viExecuteCount[ i ] = 0;
    }
    kInitializeList( &( gs_stScheduler.stWaitList ) );

    // TCB¸¦ ÇÒ´ç ¹Þ¾Æ ºÎÆÃÀ» ¼öÇàÇÑ ÅÂ½ºÅ©¸¦ Ä¿³Î ÃÖÃÊÀÇ ÇÁ·Î¼¼½º·Î ¼³Á¤
    pstTask = kAllocateTCB();
    gs_stScheduler.pstRunningTask = pstTask;
    pstTask->qwFlags = TASK_FLAGS_HIGHEST | TASK_FLAGS_PROCESS | TASK_FLAGS_SYSTEM;
    pstTask->qwParentProcessID = pstTask->stLink.qwID;
    pstTask->pvMemoryAddress = ( void* ) 0x100000;
    pstTask->qwMemorySize = 0x500000;
    pstTask->pvStackAddress = ( void* ) 0x600000;
    pstTask->qwStackSize = 0x100000;

    // ÇÁ·Î¼¼¼­ »ç¿ë·üÀ» °è»êÇÏ´Âµ¥ »ç¿ëÇÏ´Â ÀÚ·á±¸Á¶ ÃÊ±âÈ­
    gs_stScheduler.qwSpendProcessorTimeInIdleTask = 0;
    gs_stScheduler.qwProcessorLoad = 0;
}

/**
 *  ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
 */
void kSetRunningTask( TCB* pstTask )
{
    BOOL bPreviousFlag;

    // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    bPreviousFlag = kLockForSystemData();

    gs_stScheduler.pstRunningTask = pstTask;

    // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½
    kUnlockForSystemData( bPreviousFlag );
}

/**
 *  ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½È¯
 */
TCB* kGetRunningTask( void )
{
    BOOL bPreviousFlag;
    TCB* pstRunningTask;

    // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    bPreviousFlag = kLockForSystemData();

    pstRunningTask = gs_stScheduler.pstRunningTask;

    // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½
    kUnlockForSystemData( bPreviousFlag );

    return pstRunningTask;
}

/**
 *  ï¿½Â½ï¿½Å© ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
 */
static TCB* kGetNextTaskToRun( void )
{
    TCB* pstTarget = NULL;
    int iTaskCount, i, j;

    // Å¥ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ Å¥ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ 1È¸ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½, ï¿½ï¿½ï¿½ï¿½ Å¥ï¿½ï¿½ ï¿½ï¿½ï¿½Î¼ï¿½ï¿½ï¿½ï¿½ï¿½
    // ï¿½çº¸ï¿½Ï¿ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ NULLï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ñ¹ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    for( j = 0 ; j < 2 ; j++ )
    {
        // ï¿½ï¿½ï¿½ï¿½ ï¿½ì¼± ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ì¼± ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ È®ï¿½ï¿½ï¿½Ï¿ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ù¸ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
        for( i = 0 ; i < TASK_MAXREADYLISTCOUNT ; i++ )
        {
            iTaskCount = kGetListCount( &( gs_stScheduler.vstReadyList[ i ] ) );

            // ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ È½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½Â½ï¿½Å© ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ì¼± ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
            // ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
            if( gs_stScheduler.viExecuteCount[ i ] < iTaskCount )
            {
                pstTarget = ( TCB* ) kRemoveListFromHeader(
                                        &( gs_stScheduler.vstReadyList[ i ] ) );
                gs_stScheduler.viExecuteCount[ i ]++;
                break;
            }
            // ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ È½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ È½ï¿½ï¿½ï¿½ï¿½ ï¿½Ê±ï¿½È­ï¿½Ï°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ì¼± ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½çº¸ï¿½ï¿½
            else
            {
                gs_stScheduler.viExecuteCount[ i ] = 0;
            }
        }

        // ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ Ã£ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
        if( pstTarget != NULL )
        {
            break;
        }
    }
    return pstTarget;
}

/**
 *  ï¿½Â½ï¿½Å© ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ( Tickets )
 */
static TCB* kGetNextLotteryToRun( void )
{
    TCB* pstTarget = NULL;
    TCB* fndTarget = NULL;
    int i, j;
    int iTaskCount[TASK_MAXREADYLISTCOUNT];
    int TotalTaskCount, EachTaskTicket;
    int EachTickets[TASK_MAXREADYLISTCOUNT];
    int currentRandomTicket;

    for( i = 0 ; i < TASK_MAXREADYLISTCOUNT ; i++ ){
      iTaskCount[i] = kGetListCount( &( gs_stScheduler.vstReadyList[ i ] ) );
      TotalTaskCount = TotalTaskCount + (iTaskCount[i] * (TASK_MAXREADYLISTCOUNT - i) );
    }
    EachTaskTicket = TOTAL_TICKETS / TotalTaskCount;
    for( i = 0 ; i < TASK_MAXREADYLISTCOUNT ; i++ ){
      EachTickets[i] = (TASK_MAXREADYLISTCOUNT - i) * EachTaskTicket;
    }

    currentRandomTicket = (kTaskRandom() % 100000);



    for( i = 0 ; i < TASK_MAXREADYLISTCOUNT ; i++ )
    {
        if( iTaskCount[i] > 0 ){

            fndTarget = ( TCB* ) kGetHeaderFromList(
                                    &( gs_stScheduler.vstReadyList[ i ] ) );
            currentRandomTicket = currentRandomTicket - EachTickets[i];

            if( currentRandomTicket <= 0 ){
              break;
            }

            for( j = 1; j < iTaskCount[i]; j++ ){
                fndTarget = ( TCB* ) kGetNextFromList(
                                        &( gs_stScheduler.vstReadyList[ i ] ) , fndTarget );
                currentRandomTicket = currentRandomTicket - EachTickets[i];
                if( currentRandomTicket <= 0 ){
                  break;
                }
            }

            if( currentRandomTicket <= 0 ){
              break;
            }

        }

    }

    pstTarget = kRemoveList( &( gs_stScheduler.vstReadyList[ i ] ) , ((LISTLINK*)fndTarget)->qwID );



    return pstTarget;
}


static TCB* kGetNextStrideToRun( void )
{
    TCB* pstTarget = gs_stScheduler.pstRunningTask;
    int iTaskCount;
    int iMinStride = pstTarget->qwStrideSum;
    TCB* pstSearch = NULL;
    int iMini = 9999;

        for( int i = 0 ; i < TASK_MAXREADYLISTCOUNT ; i++ )
        {
          iTaskCount = kGetListCount( &( gs_stScheduler.vstReadyList[ i ] ) );

          if(iTaskCount > 0){
            pstSearch = (TCB*)kGetHeaderFromList(&( gs_stScheduler.vstReadyList[ i ] ));

            if(iMinStride > pstSearch->qwStrideSum ){
              pstTarget = pstSearch;
              iMinStride = pstTarget->qwStrideSum;
              iMini = i;
            }

            for(int k = 1; k < iTaskCount; k++){
              pstSearch = (TCB*)kGetNextFromList(&( gs_stScheduler.vstReadyList[ i ] ), pstSearch);

              if(iMinStride > pstSearch->qwStrideSum ){
                pstTarget = pstSearch;
                iMinStride = pstTarget->qwStrideSum;
                iMini = i;
              }

            }
          }
        }

    if(iMini == 9999){
      pstTarget->qwStrideSum += pstTarget->qwStride;
      kRemoveList(&( gs_stScheduler.vstReadyList[ 0 ] ), ((LISTLINK*) pstTarget)->qwID);
      return pstTarget;
    }

    pstTarget->qwStrideSum += pstTarget->qwStride;
    kRemoveList(&( gs_stScheduler.vstReadyList[ iMini ] ), ((LISTLINK*) pstTarget)->qwID);
    return pstTarget;
}


/**
 *  ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ù·ï¿½ï¿½ï¿½ ï¿½Øºï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
 */
static BOOL kAddTaskToReadyList( TCB* pstTask )
{
    BYTE bPriority;

    bPriority = GETPRIORITY( pstTask->qwFlags );
    if( bPriority == TASK_FLAGS_WAIT )
    {
        kAddListToTail( &( gs_stScheduler.stWaitList ), pstTask );
        return TRUE;
    }
    else if( bPriority >= TASK_MAXREADYLISTCOUNT )
    {
        return FALSE;
    }

    kAddListToTail( &( gs_stScheduler.vstReadyList[ bPriority ] ), pstTask );
    return TRUE;
}

/**
 *  ï¿½Øºï¿½ Å¥ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
 */
static TCB* kRemoveTaskFromReadyList( QWORD qwTaskID )
{
    TCB* pstTarget;
    BYTE bPriority;

    // ï¿½Â½ï¿½Å© IDï¿½ï¿½ ï¿½ï¿½È¿ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    if( GETTCBOFFSET( qwTaskID ) >= TASK_MAXCOUNT )
    {
        return NULL;
    }

    // TCB Ç®ï¿½ï¿½ï¿½ï¿½ ï¿½Ø´ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ TCBï¿½ï¿½ Ã£ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ IDï¿½ï¿½ ï¿½ï¿½Ä¡ï¿½Ï´Â°ï¿½ È®ï¿½ï¿½
    pstTarget = &( gs_stTCBPoolManager.pstStartAddress[ GETTCBOFFSET( qwTaskID ) ] );
    if( pstTarget->stLink.qwID != qwTaskID )
    {
        return NULL;
    }

    // ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï´ï¿½ ï¿½Øºï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å© ï¿½ï¿½ï¿½ï¿½
    bPriority = GETPRIORITY( pstTarget->qwFlags );
    if( bPriority >= TASK_MAXREADYLISTCOUNT )
    {
        return NULL;
    }

    pstTarget = kRemoveList( &( gs_stScheduler.vstReadyList[ bPriority ]),
                     qwTaskID );
    return pstTarget;
}

/**
 *  ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ì¼± ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
 */
BOOL kChangePriority( QWORD qwTaskID, BYTE bPriority )
{
    TCB* pstTarget;
    BOOL bPreviousFlag;

    if( bPriority > TASK_MAXREADYLISTCOUNT )
    {
        return FALSE;
    }

    // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    bPreviousFlag = kLockForSystemData();

    // ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½Ì¸ï¿½ ï¿½ì¼± ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    // PIT ï¿½ï¿½Æ®ï¿½Ñ·ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Í·ï¿½Æ®(IRQ 0)ï¿½ï¿½ ï¿½ß»ï¿½ï¿½Ï¿ï¿½ ï¿½Â½ï¿½Å© ï¿½ï¿½È¯ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
    // ï¿½ì¼± ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½Ìµï¿½
    pstTarget = gs_stScheduler.pstRunningTask;
    if( pstTarget->stLink.qwID == qwTaskID )
    {
        SETPRIORITY( pstTarget->qwFlags, bPriority );
    }
    // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½Æ´Ï¸ï¿½ ï¿½Øºï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ï¿½ï¿½ Ã£ï¿½Æ¼ï¿½ ï¿½Ø´ï¿½ ï¿½ì¼± ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½Ìµï¿½
    else
    {
        // ï¿½Øºï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ Ã£ï¿½ï¿½ ï¿½ï¿½ï¿½Ï¸ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ Ã£ï¿½Æ¼ï¿½ ï¿½ì¼± ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
        pstTarget = kRemoveTaskFromReadyList( qwTaskID );
        if( pstTarget == NULL )
        {
            // ï¿½Â½ï¿½Å© IDï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ Ã£ï¿½Æ¼ï¿½ ï¿½ï¿½ï¿½ï¿½
            pstTarget = kGetTCBInTCBPool( GETTCBOFFSET( qwTaskID ) );
            if( pstTarget != NULL )
            {
                // ï¿½ì¼± ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
                SETPRIORITY( pstTarget->qwFlags, bPriority );
            }
        }
        else
        {
            // ï¿½ì¼± ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï°ï¿½ ï¿½Øºï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½Ù½ï¿½ ï¿½ï¿½ï¿½ï¿½
            SETPRIORITY( pstTarget->qwFlags, bPriority );
            kAddTaskToReadyList( pstTarget );
        }
    }
    // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½
    kUnlockForSystemData( bPreviousFlag );
    return TRUE;
}

/**
 *  ï¿½Ù¸ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ Ã£ï¿½Æ¼ï¿½ ï¿½ï¿½È¯
 *      ï¿½ï¿½ï¿½Í·ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½ï¿½Ü°ï¿½ ï¿½ß»ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ È£ï¿½ï¿½ï¿½Ï¸ï¿½ ï¿½Èµï¿½
 */
void kSchedule( void )
{
    TCB* pstRunningTask, * pstNextTask;
    BOOL bPreviousFlag;

    // ï¿½ï¿½È¯ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½Ö¾ï¿½ï¿½ï¿½ ï¿½ï¿½
    if( kGetReadyTaskCount() < 1 )
    {
        return ;
    }

    // ï¿½ï¿½È¯ï¿½Ï´ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Í·ï¿½Æ®ï¿½ï¿½ ï¿½ß»ï¿½ï¿½Ï¿ï¿½ ï¿½Â½ï¿½Å© ï¿½ï¿½È¯ï¿½ï¿½ ï¿½ï¿½ ï¿½Ï¾î³ªï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï¹Ç·ï¿½ ï¿½ï¿½È¯ï¿½Ï´ï¿½
    // ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Í·ï¿½Æ®ï¿½ï¿½ ï¿½ß»ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ïµï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    bPreviousFlag = kLockForSystemData();

    // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    pstNextTask = kGetNextTaskToRun();
    if( pstNextTask == NULL )
    {
        // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½
        kUnlockForSystemData( bPreviousFlag );
        return ;
    }

    // ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½Ø½ï¿½Æ® ï¿½ï¿½È¯
    pstRunningTask = gs_stScheduler.pstRunningTask;
    gs_stScheduler.pstRunningTask = pstNextTask;

    // ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½È¯ï¿½Ç¾ï¿½ï¿½Ù¸ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Î¼ï¿½ï¿½ï¿½ ï¿½Ã°ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Å´
    if( ( pstRunningTask->qwFlags & TASK_FLAGS_IDLE ) == TASK_FLAGS_IDLE )
    {
        gs_stScheduler.qwSpendProcessorTimeInIdleTask +=
            TASK_PROCESSORTIME - gs_stScheduler.iProcessorTime;
    }

    // ï¿½Â½ï¿½Å© ï¿½ï¿½ï¿½ï¿½ ï¿½Ã·ï¿½ï¿½×°ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ø½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ê¿ä°¡ ï¿½ï¿½ï¿½ï¿½ï¿½Ç·ï¿½, ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½
    // ï¿½ï¿½ï¿½ï¿½ï¿½Ï°ï¿½ ï¿½ï¿½ï¿½Ø½ï¿½Æ® ï¿½ï¿½È¯
    if( pstRunningTask->qwFlags & TASK_FLAGS_ENDTASK )
    {
        kAddListToTail( &( gs_stScheduler.stWaitList ), pstRunningTask );
        kSwitchContext( NULL, &( pstNextTask->stContext ) );
    }
    else
    {
        kAddTaskToReadyList( pstRunningTask );
        kSwitchContext( &( pstRunningTask->stContext ), &( pstNextTask->stContext ) );
    }

    // ï¿½ï¿½ï¿½Î¼ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ã°ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ®
    gs_stScheduler.iProcessorTime = TASK_PROCESSORTIME;

    // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½
    kUnlockForSystemData( bPreviousFlag );
}

/**
 *  ï¿½ï¿½ï¿½Í·ï¿½Æ®ï¿½ï¿½ ï¿½ß»ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½, ï¿½Ù¸ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ Ã£ï¿½ï¿½ ï¿½ï¿½È¯
 *      ï¿½Ýµï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Í·ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½ï¿½Ü°ï¿½ ï¿½ß»ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ È£ï¿½ï¿½ï¿½Ø¾ï¿½ ï¿½ï¿½
 */
BOOL kScheduleInInterrupt( void )
{
    TCB* pstRunningTask, * pstNextTask;
    char* pcContextAddress;
    BOOL bPreviousFlag;

    // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    bPreviousFlag = kLockForSystemData();

    // ï¿½ï¿½È¯ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    pstNextTask = kGetNextTaskToRun();
    if( pstNextTask == NULL )
    {
        // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½
        kUnlockForSystemData( bPreviousFlag );
        return FALSE;
    }

    //==========================================================================
    //  ï¿½Â½ï¿½Å© ï¿½ï¿½È¯ Ã³ï¿½ï¿½
    //      ï¿½ï¿½ï¿½Í·ï¿½Æ® ï¿½Úµé·¯ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ø½ï¿½Æ®ï¿½ï¿½ ï¿½Ù¸ï¿½ ï¿½ï¿½ï¿½Ø½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½ï¿½î¾²ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Ã³ï¿½ï¿½
    //==========================================================================
    pcContextAddress = ( char* ) IST_STARTADDRESS + IST_SIZE - sizeof( CONTEXT );

    // ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½Ø½ï¿½Æ® ï¿½ï¿½È¯
    pstRunningTask = gs_stScheduler.pstRunningTask;
    gs_stScheduler.pstRunningTask = pstNextTask;

    // ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½È¯ï¿½Ç¾ï¿½ï¿½Ù¸ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Tick Countï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Å´
    if( ( pstRunningTask->qwFlags & TASK_FLAGS_IDLE ) == TASK_FLAGS_IDLE )
    {
        gs_stScheduler.qwSpendProcessorTimeInIdleTask += TASK_PROCESSORTIME;
    }

    // ï¿½Â½ï¿½Å© ï¿½ï¿½ï¿½ï¿½ ï¿½Ã·ï¿½ï¿½×°ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½, ï¿½ï¿½ï¿½Ø½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ê°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    if( pstRunningTask->qwFlags & TASK_FLAGS_ENDTASK )
    {
        kAddListToTail( &( gs_stScheduler.stWaitList ), pstRunningTask );
    }
    // ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ISTï¿½ï¿½ ï¿½Ö´ï¿½ ï¿½ï¿½ï¿½Ø½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï°ï¿½, ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½Øºï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½
    // ï¿½Å±ï¿½
    else
    {
        kMemCpy( &( pstRunningTask->stContext ), pcContextAddress, sizeof( CONTEXT ) );
        kAddTaskToReadyList( pstRunningTask );
    }
    // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½
    kUnlockForSystemData( bPreviousFlag );

    // ï¿½ï¿½È¯ï¿½Ø¼ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ Running Taskï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï°ï¿½ ï¿½ï¿½ï¿½Ø½ï¿½Æ®ï¿½ï¿½ ISTï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ø¼ï¿½
    // ï¿½Úµï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å© ï¿½ï¿½È¯ï¿½ï¿½ ï¿½Ï¾î³ªï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½
    kMemCpy( pcContextAddress, &( pstNextTask->stContext ), sizeof( CONTEXT ) );

    // ï¿½ï¿½ï¿½Î¼ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ã°ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ®
    gs_stScheduler.iProcessorTime = TASK_PROCESSORTIME;
    return TRUE;
}

/**
 *  ï¿½ï¿½ï¿½Î¼ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½Ö´ï¿½ ï¿½Ã°ï¿½ï¿½ï¿½ ï¿½Ï³ï¿½ ï¿½ï¿½ï¿½ï¿½
 */
void kDecreaseProcessorTime( void )
{
    if( gs_stScheduler.iProcessorTime > 0 )
    {
        gs_stScheduler.iProcessorTime--;
    }
}

/**
 *  ï¿½ï¿½ï¿½Î¼ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½Ö´ï¿½ ï¿½Ã°ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½Ç¾ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Î¸ï¿½ ï¿½ï¿½È¯
 */
BOOL kIsProcessorTimeExpired( void )
{
    if( gs_stScheduler.iProcessorTime <= 0 )
    {
        return TRUE;
    }
    return FALSE;
}

/**
 *  ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
 */
BOOL kEndTask( QWORD qwTaskID )
{
    TCB* pstTarget;
    BYTE bPriority;
    BOOL bPreviousFlag;

    // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    bPreviousFlag = kLockForSystemData();

    // ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½Ì¸ï¿½ EndTask ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï°ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½È¯
    pstTarget = gs_stScheduler.pstRunningTask;
    if( pstTarget->stLink.qwID == qwTaskID )
    {
        pstTarget->qwFlags |= TASK_FLAGS_ENDTASK;
        SETPRIORITY( pstTarget->qwFlags, TASK_FLAGS_WAIT );

        // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½
        kUnlockForSystemData( bPreviousFlag );

        kSchedule();

        // ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½È¯ ï¿½Ç¾ï¿½ï¿½ï¿½ï¿½Ç·ï¿½ ï¿½Æ·ï¿½ ï¿½Úµï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
        while( 1 ) ;
    }
    // ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½Æ´Ï¸ï¿½ ï¿½Øºï¿½ Å¥ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ Ã£ï¿½Æ¼ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    else
    {
        // ï¿½Øºï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ Ã£ï¿½ï¿½ ï¿½ï¿½ï¿½Ï¸ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ Ã£ï¿½Æ¼ï¿½ ï¿½Â½ï¿½Å© ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Æ®ï¿½ï¿½
        // ï¿½ï¿½ï¿½ï¿½
        pstTarget = kRemoveTaskFromReadyList( qwTaskID );
        if( pstTarget == NULL )
        {
            // ï¿½Â½ï¿½Å© IDï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ Ã£ï¿½Æ¼ï¿½ ï¿½ï¿½ï¿½ï¿½
            pstTarget = kGetTCBInTCBPool( GETTCBOFFSET( qwTaskID ) );
            if( pstTarget != NULL )
            {
                pstTarget->qwFlags |= TASK_FLAGS_ENDTASK;
                SETPRIORITY( pstTarget->qwFlags, TASK_FLAGS_WAIT );
            }
            // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½
            kUnlockForSystemData( bPreviousFlag );
            return TRUE;
        }

        pstTarget->qwFlags |= TASK_FLAGS_ENDTASK;
        SETPRIORITY( pstTarget->qwFlags, TASK_FLAGS_WAIT );
        kAddListToTail( &( gs_stScheduler.stWaitList ), pstTarget );
    }
    // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½
    kUnlockForSystemData( bPreviousFlag );
    return TRUE;
}

/**
 *  ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½Ú½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
 */
void kExitTask( void )
{
    kEndTask( gs_stScheduler.pstRunningTask->stLink.qwID );
}

/**
 *  ï¿½Øºï¿½ Å¥ï¿½ï¿½ ï¿½Ö´ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½È¯
 */
int kGetReadyTaskCount( void )
{
    int iTotalCount = 0;
    int i;
    BOOL bPreviousFlag;

    // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    bPreviousFlag = kLockForSystemData();

    // ï¿½ï¿½ï¿½ï¿½ ï¿½Øºï¿½ Å¥ï¿½ï¿½ È®ï¿½ï¿½ï¿½Ï¿ï¿½ ï¿½Â½ï¿½Å© ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    for( i = 0 ; i < TASK_MAXREADYLISTCOUNT ; i++ )
    {
        iTotalCount += kGetListCount( &( gs_stScheduler.vstReadyList[ i ] ) );
    }

    // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½
    kUnlockForSystemData( bPreviousFlag );
    return iTotalCount ;
}

/**
 *  ï¿½ï¿½Ã¼ ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½È¯
 */
int kGetTaskCount( void )
{
    int iTotalCount;
    BOOL bPreviousFlag;

    // ï¿½Øºï¿½ Å¥ï¿½ï¿½ ï¿½Â½ï¿½Å© ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½, ï¿½ï¿½ï¿½ï¿½ Å¥ï¿½ï¿½ ï¿½Â½ï¿½Å© ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Â½ï¿½Å© ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    iTotalCount = kGetReadyTaskCount();

    // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    bPreviousFlag = kLockForSystemData();

    iTotalCount += kGetListCount( &( gs_stScheduler.stWaitList ) ) + 1;

    // ï¿½Ó°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½
    kUnlockForSystemData( bPreviousFlag );
    return iTotalCount;
}

/**
 *  TCB Ç®ï¿½ï¿½ï¿½ï¿½ ï¿½Ø´ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ TCBï¿½ï¿½ ï¿½ï¿½È¯
 */
TCB* kGetTCBInTCBPool( int iOffset )
{
    if( ( iOffset < -1 ) && ( iOffset > TASK_MAXCOUNT ) )
    {
        return NULL;
    }

    return &( gs_stTCBPoolManager.pstStartAddress[ iOffset ] );
}

/**
 *  ï¿½Â½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï´ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Î¸ï¿½ ï¿½ï¿½È¯
 */
BOOL kIsTaskExist( QWORD qwID )
{
    TCB* pstTCB;

    // IDï¿½ï¿½ TCBï¿½ï¿½ ï¿½ï¿½È¯
    pstTCB = kGetTCBInTCBPool( GETTCBOFFSET( qwID ) );
    // TCBï¿½ï¿½ ï¿½ï¿½ï¿½Å³ï¿½ IDï¿½ï¿½ ï¿½ï¿½Ä¡ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ê´ï¿½ ï¿½ï¿½ï¿½ï¿½
    if( ( pstTCB == NULL ) || ( pstTCB->stLink.qwID != qwID ) )
    {
        return FALSE;
    }
    return TRUE;
}

/**
 *  ï¿½ï¿½ï¿½Î¼ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½È¯
 */
QWORD kGetProcessorLoad( void )
{
    return gs_stScheduler.qwProcessorLoad;
}

/**
 *  ½º·¹µå°¡ ¼Ò¼ÓµÈ ÇÁ·Î¼¼½º¸¦ ¹ÝÈ¯
 */
static TCB* kGetProcessByThread( TCB* pstThread )
{
    TCB* pstProcess;

    // ¸¸¾à ³»°¡ ÇÁ·Î¼¼½ºÀÌ¸é ÀÚ½ÅÀ» ¹ÝÈ¯
    if( pstThread->qwFlags & TASK_FLAGS_PROCESS )
    {
        return pstThread;
    }

    // ³»°¡ ÇÁ·Î¼¼½º°¡ ¾Æ´Ï¶ó¸é, ºÎ¸ð ÇÁ·Î¼¼½º·Î ¼³Á¤µÈ ÅÂ½ºÅ© ID¸¦ ÅëÇØ
    // TCB Ç®¿¡¼­ ÅÂ½ºÅ© ÀÚ·á±¸Á¶ ÃßÃâ
    pstProcess = kGetTCBInTCBPool( GETTCBOFFSET( pstThread->qwParentProcessID ) );

    // ¸¸¾à ÇÁ·Î¼¼½º°¡ ¾ø°Å³ª, ÅÂ½ºÅ© ID°¡ ÀÏÄ¡ÇÏÁö ¾Ê´Â´Ù¸é NULLÀ» ¹ÝÈ¯
    if( ( pstProcess == NULL ) || ( pstProcess->stLink.qwID != pstThread->qwParentProcessID ) )
    {
        return NULL;
    }

    return pstProcess;
}

//==============================================================================
//  À¯ÈÞ ÅÂ½ºÅ© °ü·Ã
//==============================================================================
/**
 *  À¯ÈÞ ÅÂ½ºÅ©
 *      ´ë±â Å¥¿¡ »èÁ¦ ´ë±âÁßÀÎ ÅÂ½ºÅ©¸¦ Á¤¸®
 */
void kIdleTask( void )
{
    TCB* pstTask, * pstChildThread, * pstProcess;
    QWORD qwLastMeasureTickCount, qwLastSpendTickInIdleTask;
    QWORD qwCurrentMeasureTickCount, qwCurrentSpendTickInIdleTask;
    BOOL bPreviousFlag;
    int i, iCount;
    QWORD qwTaskID;
    void* pstThreadLink;

    // ÇÁ·Î¼¼¼­ »ç¿ë·® °è»êÀ» À§ÇØ ±âÁØ Á¤º¸¸¦ ÀúÀå
    qwLastSpendTickInIdleTask = gs_stScheduler.qwSpendProcessorTimeInIdleTask;
    qwLastMeasureTickCount = kGetTickCount();

    while( 1 )
    {
        // ÇöÀç »óÅÂ¸¦ ÀúÀå
        qwCurrentMeasureTickCount = kGetTickCount();
        qwCurrentSpendTickInIdleTask = gs_stScheduler.qwSpendProcessorTimeInIdleTask;

        // ÇÁ·Î¼¼¼­ »ç¿ë·®À» °è»ê
        // 100 - ( À¯ÈÞ ÅÂ½ºÅ©°¡ »ç¿ëÇÑ ÇÁ·Î¼¼¼­ ½Ã°£ ) * 100 / ( ½Ã½ºÅÛ ÀüÃ¼¿¡¼­
        // »ç¿ëÇÑ ÇÁ·Î¼¼¼­ ½Ã°£ )
        if( qwCurrentMeasureTickCount - qwLastMeasureTickCount == 0 )
        {
            gs_stScheduler.qwProcessorLoad = 0;
        }
        else
        {
            gs_stScheduler.qwProcessorLoad = 100 -
                ( qwCurrentSpendTickInIdleTask - qwLastSpendTickInIdleTask ) *
                100 /( qwCurrentMeasureTickCount - qwLastMeasureTickCount );
        }

        // ÇöÀç »óÅÂ¸¦ ÀÌÀü »óÅÂ¿¡ º¸°ü
        qwLastMeasureTickCount = qwCurrentMeasureTickCount;
        qwLastSpendTickInIdleTask = qwCurrentSpendTickInIdleTask;

        // ÇÁ·Î¼¼¼­ÀÇ ºÎÇÏ¿¡ µû¶ó ½¬°Ô ÇÔ
        kHaltProcessorByLoad();

        // ´ë±â Å¥¿¡ ´ë±âÁßÀÎ ÅÂ½ºÅ©°¡ ÀÖÀ¸¸é ÅÂ½ºÅ©¸¦ Á¾·áÇÔ
        if( kGetListCount( &( gs_stScheduler.stWaitList ) ) >= 0 )
        {
            while( 1 )
            {
                // ÀÓ°è ¿µ¿ª ½ÃÀÛ
                bPreviousFlag = kLockForSystemData();
                pstTask = kRemoveListFromHeader( &( gs_stScheduler.stWaitList ) );
                if( pstTask == NULL )
                {
                    // ÀÓ°è ¿µ¿ª ³¡
                    kUnlockForSystemData( bPreviousFlag );
                    break;
                }

                if( pstTask->qwFlags & TASK_FLAGS_PROCESS )
                {
                    // ÇÁ·Î¼¼½º¸¦ Á¾·áÇÒ ¶§ ÀÚ½Ä ½º·¹µå°¡ Á¸ÀçÇÏ¸é ½º·¹µå¸¦ ¸ðµÎ
                    // Á¾·áÇÏ°í, ´Ù½Ã ÀÚ½Ä ½º·¹µå ¸®½ºÆ®¿¡ »ðÀÔ
                    iCount = kGetListCount( &( pstTask->stChildThreadList ) );
                    for( i = 0 ; i < iCount ; i++ )
                    {
                        // ½º·¹µå ¸µÅ©ÀÇ ¾îµå·¹½º¿¡¼­ ²¨³» ½º·¹µå¸¦ Á¾·á½ÃÅ´
                        pstThreadLink = ( TCB* ) kRemoveListFromHeader(
                                &( pstTask->stChildThreadList ) );
                        if( pstThreadLink == NULL )
                        {
                            break;
                        }

                        // ÀÚ½Ä ½º·¹µå ¸®½ºÆ®¿¡ ¿¬°áµÈ Á¤º¸´Â ÅÂ½ºÅ© ÀÚ·á±¸Á¶¿¡ ÀÖ´Â
                        // stThreadLinkÀÇ ½ÃÀÛ ¾îµå·¹½ºÀÌ¹Ç·Î, ÅÂ½ºÅ© ÀÚ·á±¸Á¶ÀÇ ½ÃÀÛ
                        // ¾îµå·¹½º¸¦ ±¸ÇÏ·Á¸é º°µµÀÇ °è»êÀÌ ÇÊ¿äÇÔ
                        pstChildThread = GETTCBFROMTHREADLINK( pstThreadLink );

                        // ´Ù½Ã ÀÚ½Ä ½º·¹µå ¸®½ºÆ®¿¡ »ðÀÔÇÏ¿© ÇØ´ç ½º·¹µå°¡ Á¾·áµÉ ¶§
                        // ÀÚ½Ä ½º·¹µå°¡ ÇÁ·Î¼¼½º¸¦ Ã£¾Æ ½º½º·Î ¸®½ºÆ®¿¡¼­ Á¦°ÅÇÏµµ·Ï ÇÔ
                        kAddListToTail( &( pstTask->stChildThreadList ),
                                &( pstChildThread->stThreadLink ) );

                        // ÀÚ½Ä ½º·¹µå¸¦ Ã£¾Æ¼­ Á¾·á
                        kEndTask( pstChildThread->stLink.qwID );
                    }

                    // ¾ÆÁ÷ ÀÚ½Ä ½º·¹µå°¡ ³²¾ÆÀÖ´Ù¸é ÀÚ½Ä ½º·¹µå°¡ ´Ù Á¾·áµÉ ¶§±îÁö
                    // ±â´Ù·Á¾ß ÇÏ¹Ç·Î ´Ù½Ã ´ë±â ¸®½ºÆ®¿¡ »ðÀÔ
                    if( kGetListCount( &( pstTask->stChildThreadList ) ) > 0 )
                    {
                        kAddListToTail( &( gs_stScheduler.stWaitList ), pstTask );

                        // ÀÓ°è ¿µ¿ª ³¡
                        kUnlockForSystemData( bPreviousFlag );
                        continue;
                    }
                    // ÇÁ·Î¼¼½º¸¦ Á¾·áÇØ¾ß ÇÏ¹Ç·Î ÇÒ´ç ¹ÞÀº ¸Þ¸ð¸® ¿µ¿ªÀ» »èÁ¦
                    else
                    {
                        // TODO: ÃßÈÄ¿¡ ÄÚµå »ðÀÔ
                    }
                }
                else if( pstTask->qwFlags & TASK_FLAGS_THREAD )
                {
                    // ½º·¹µå¶ó¸é ÇÁ·Î¼¼½ºÀÇ ÀÚ½Ä ½º·¹µå ¸®½ºÆ®¿¡¼­ Á¦°Å
                    pstProcess = kGetProcessByThread( pstTask );
                    if( pstProcess != NULL )
                    {
                        kRemoveList( &( pstProcess->stChildThreadList ), pstTask->stLink.qwID );
                    }
                }

                qwTaskID = pstTask->stLink.qwID;
                kFreeTCB( qwTaskID );
                // ÀÓ°è ¿µ¿ª ³¡
                kUnlockForSystemData( bPreviousFlag );

                kPrintf( "IDLE: Task ID[0x%q] is completely ended.\n",
                        qwTaskID );
            }
        }

        kSchedule();
    }
}

/**
 *  ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Î¼ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ï¿ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Î¼ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½
 */
void kHaltProcessorByLoad( void )
{
    if( gs_stScheduler.qwProcessorLoad < 40 )
    {
        kHlt();
        kHlt();
        kHlt();
    }
    else if( gs_stScheduler.qwProcessorLoad < 80 )
    {
        kHlt();
        kHlt();
    }
    else if( gs_stScheduler.qwProcessorLoad < 95 )
    {
        kHlt();
    }
}
