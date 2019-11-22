/**
 *  file    InterruptHandler.h
 *  date    2009/01/24
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ���ͷ�Ʈ �� ���� �ڵ鷯�� ���õ� ��� ����
 */

#ifndef __INTERRUPTHANDLER_H__
#define __INTERRUPTHANDLER_H__

#include "Types.h"

////////////////////////////////////////////////////////////////////////////////
//
// �Լ�
//
////////////////////////////////////////////////////////////////////////////////
void kCommonExceptionHandler( int iVectorNumber, QWORD qwErrorCode );
void kCommonInterruptHandler( int iVectorNumber );
void kKeyboardHandler( int iVectorNumber );
void kPageFault(int iVectorNumber,QWORD qwErrorCode);
void kTimerHandler( int iVectorNumber );
void kHDDHandler( int iVectorNumber );

static inline void invlpg(void* m){
    asm volatile("invlpg (%0)" ::"b"(m) : "memory");
}

#endif /*__INTERRUPTHANDLER_H__*/
