/**
 *  file    Descriptor.h
 *  date    2009/01/16
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   GDT �� IDT�� ���õ� ���� ��ũ���Ϳ� ���� ��� ����
 */

#ifndef __DESCRIPTOR_H__
#define __DESCRIPTOR_H__

#include "Types.h"

////////////////////////////////////////////////////////////////////////////////
//
// ��ũ��
//
////////////////////////////////////////////////////////////////////////////////
//==============================================================================
// GDT
//==============================================================================
// ���տ� ����� �⺻ ��ũ��
#define GDT_TYPE_CODE           0x0A
#define GDT_TYPE_DATA           0x02
#define GDT_TYPE_TSS            0x09
#define GDT_FLAGS_LOWER_S       0x10
#define GDT_FLAGS_LOWER_DPL0    0x00
#define GDT_FLAGS_LOWER_DPL1    0x20
#define GDT_FLAGS_LOWER_DPL2    0x40
#define GDT_FLAGS_LOWER_DPL3    0x60
#define GDT_FLAGS_LOWER_P       0x80
#define GDT_FLAGS_UPPER_L       0x20
#define GDT_FLAGS_UPPER_DB      0x40
#define GDT_FLAGS_UPPER_G       0x80

// ������ ����� ��ũ��
// Lower Flags�� Code/Data/TSS, DPL0, Present�� ����
#define GDT_FLAGS_LOWER_KERNELCODE ( GDT_TYPE_CODE | GDT_FLAGS_LOWER_S | \
        GDT_FLAGS_LOWER_DPL0 | GDT_FLAGS_LOWER_P )
#define GDT_FLAGS_LOWER_KERNELDATA ( GDT_TYPE_DATA | GDT_FLAGS_LOWER_S | \
        GDT_FLAGS_LOWER_DPL0 | GDT_FLAGS_LOWER_P )
#define GDT_FLAGS_LOWER_TSS ( GDT_FLAGS_LOWER_DPL0 | GDT_FLAGS_LOWER_P )
#define GDT_FLAGS_LOWER_USERCODE ( GDT_TYPE_CODE | GDT_FLAGS_LOWER_S | \
        GDT_FLAGS_LOWER_DPL3 | GDT_FLAGS_LOWER_P )
#define GDT_FLAGS_LOWER_USERDATA ( GDT_TYPE_DATA | GDT_FLAGS_LOWER_S | \
        GDT_FLAGS_LOWER_DPL3 | GDT_FLAGS_LOWER_P )

// Upper Flags�� Granulaty�� �����ϰ� �ڵ� �� �����ʹ� 64��Ʈ �߰�
#define GDT_FLAGS_UPPER_CODE ( GDT_FLAGS_UPPER_G | GDT_FLAGS_UPPER_L )
#define GDT_FLAGS_UPPER_DATA ( GDT_FLAGS_UPPER_G | GDT_FLAGS_UPPER_L )
#define GDT_FLAGS_UPPER_TSS ( GDT_FLAGS_UPPER_G )

// ���׸�Ʈ ��ũ���� ������
#define GDT_KERNELCODESEGMENT 0x08
#define GDT_KERNELDATASEGMENT 0x10
#define GDT_TSSSEGMENT        0x18

// ��Ÿ GDT�� ���õ� ��ũ��
// GDTR�� ���� ��巹��, 1Mbyte���� 264Kbyte������ ������ ���̺� ����
#define GDTR_STARTADDRESS   0x143000
// 8����Ʈ ��Ʈ���� ����, �� ��ũ����/Ŀ�� �ڵ�/Ŀ�� ������
#define GDT_MAXENTRY8COUNT  3
// 16����Ʈ ��Ʈ���� ����, TSS
#define GDT_MAXENTRY16COUNT 1
// GDT ���̺��� ũ��
#define GDT_TABLESIZE       ( ( sizeof( GDTENTRY8 ) * GDT_MAXENTRY8COUNT ) + \
        ( sizeof( GDTENTRY16 ) * GDT_MAXENTRY16COUNT ) )
#define TSS_SEGMENTSIZE     ( sizeof( TSSSEGMENT ) )

//==============================================================================
// IDT
//==============================================================================
// ���տ� ����� �⺻ ��ũ��
#define IDT_TYPE_INTERRUPT      0x0E
#define IDT_TYPE_TRAP           0x0F
#define IDT_FLAGS_DPL0          0x00
#define IDT_FLAGS_DPL1          0x20
#define IDT_FLAGS_DPL2          0x40
#define IDT_FLAGS_DPL3          0x60
#define IDT_FLAGS_P             0x80
#define IDT_FLAGS_IST0          0
#define IDT_FLAGS_IST1          1

// ������ ����� ��ũ��
#define IDT_FLAGS_KERNEL        ( IDT_FLAGS_DPL0 | IDT_FLAGS_P )
#define IDT_FLAGS_USER          ( IDT_FLAGS_DPL3 | IDT_FLAGS_P )

// ��Ÿ IDT�� ���õ� ��ũ��
// IDT ��Ʈ���� ����
#define IDT_MAXENTRYCOUNT       100
// IDTR�� ���� ��巹��, TSS ���׸�Ʈ�� ���ʿ� ��ġ
#define IDTR_STARTADDRESS       ( GDTR_STARTADDRESS + sizeof( GDTR ) + \
        GDT_TABLESIZE + TSS_SEGMENTSIZE )
// IDT ���̺��� ���� ��巹��
#define IDT_STARTADDRESS        ( IDTR_STARTADDRESS + sizeof( IDTR ) )
// IDT ���̺��� ��ü ũ��
#define IDT_TABLESIZE           ( IDT_MAXENTRYCOUNT * sizeof( IDTENTRY ) )

// IST�� ���� ��巹��
#define IST_STARTADDRESS        0x700000
// IST�� ũ��
#define IST_SIZE                0x100000

////////////////////////////////////////////////////////////////////////////////
//
// ����ü
//
////////////////////////////////////////////////////////////////////////////////
// 1����Ʈ�� ����
#pragma pack( push, 1 )

// GDTR �� IDTR ����ü
typedef struct kGDTRStruct
{
    WORD wLimit;
    QWORD qwBaseAddress;
    // 16����Ʈ ��巹�� ������ ���� �߰�
    WORD wPading;
    DWORD dwPading;
} GDTR, IDTR;

// 8����Ʈ ũ���� GDT ��Ʈ�� ����
typedef struct kGDTEntry8Struct
{
    WORD wLowerLimit;
    WORD wLowerBaseAddress;
    BYTE bUpperBaseAddress1;
    // 4��Ʈ Type, 1��Ʈ S, 2��Ʈ DPL, 1��Ʈ P
    BYTE bTypeAndLowerFlag;
    // 4��Ʈ Segment Limit, 1��Ʈ AVL, L, D/B, G
    BYTE bUpperLimitAndUpperFlag;
    BYTE bUpperBaseAddress2;
} GDTENTRY8;

// 16����Ʈ ũ���� GDT ��Ʈ�� ����
typedef struct kGDTEntry16Struct
{
    WORD wLowerLimit;
    WORD wLowerBaseAddress;
    BYTE bMiddleBaseAddress1;
    // 4��Ʈ Type, 1��Ʈ 0, 2��Ʈ DPL, 1��Ʈ P
    BYTE bTypeAndLowerFlag;
    // 4��Ʈ Segment Limit, 1��Ʈ AVL, 0, 0, G
    BYTE bUpperLimitAndUpperFlag;
    BYTE bMiddleBaseAddress2;
    DWORD dwUpperBaseAddress;
    DWORD dwReserved;
} GDTENTRY16;

// TSS Data ����ü
typedef struct kTSSDataStruct
{
    DWORD dwReserved1;
    QWORD qwRsp[ 3 ];
    QWORD qwReserved2;
    QWORD qwIST[ 7 ];
    QWORD qwReserved3;
    WORD wReserved;
    WORD wIOMapBaseAddress;
} TSSSEGMENT;

// IDT ����Ʈ ��ũ���� ����ü
typedef struct kIDTEntryStruct
{
    WORD wLowerBaseAddress;
    WORD wSegmentSelector;
    // 3��Ʈ IST, 5��Ʈ 0
    BYTE bIST;
    // 4��Ʈ Type, 1��Ʈ 0, 2��Ʈ DPL, 1��Ʈ P
    BYTE bTypeAndFlags;
    WORD wMiddleBaseAddress;
    DWORD dwUpperBaseAddress;
    DWORD dwReserved;
} IDTENTRY;

#pragma pack ( pop )

////////////////////////////////////////////////////////////////////////////////
//
//  �Լ�
//
////////////////////////////////////////////////////////////////////////////////
void kInitializeGDTTableAndTSS( void );
void kSetGDTEntry8( GDTENTRY8* pstEntry, DWORD dwBaseAddress, DWORD dwLimit,
        BYTE bUpperFlags, BYTE bLowerFlags, BYTE bType );
void kSetGDTEntry16( GDTENTRY16* pstEntry, QWORD qwBaseAddress, DWORD dwLimit,
        BYTE bUpperFlags, BYTE bLowerFlags, BYTE bType );
void kInitializeTSSSegment( TSSSEGMENT* pstTSS );

void kInitializeIDTTables( void );
void kSetIDTEntry( IDTENTRY* pstEntry, void* pvHandler, WORD wSelector, 
        BYTE bIST, BYTE bFlags, BYTE bType );
void kDummyHandler( void );

#endif
