# file		ISR.asm
# date      2009/01/24
# author	kkamagui
#           Copyright(c)2008 All rights reserved by kkamagui
# brief		인터럽트 서비스 루틴(ISR) 관련된 소스 파일

[BITS 64]           ; 이하의 코드는 64비트 코드로 설정

SECTION .text       ; text 섹션(세그먼트)을 정의

; 외부에서 정의된 함수를 쓸 수 있도록 선언함(Import)
extern kCommonExceptionHandler, kCommonInterruptHandler, kKeyboardHandler, kPageFault
extern kTimerHandler, kHDDHandler

; C 언어에서 호출할 수 있도록 이름을 노출함(Export)
; 예외(Exception) 처리를 위한 ISR
global kISRDivideError, kISRDebug, kISRNMI, kISRBreakPoint, kISROverflow
global kISRBoundRangeExceeded, kISRInvalidOpcode, kISRDeviceNotAvailable, kISRDoubleFault,
global kISRCoprocessorSegmentOverrun, kISRInvalidTSS, kISRSegmentNotPresent
global kISRStackSegmentFault, kISRGeneralProtection, kISRPageFault, kISR15
global kISRFPUError, kISRAlignmentCheck, kISRMachineCheck, kISRSIMDError, kISRETCException

; ���ͷ�Ʈ(Interrupt) ó���� ���� ISR
global kISRTimer, kISRKeyboard, kISRSlavePIC, kISRSerial2, kISRSerial1, kISRParallel2
global kISRFloppy, kISRParallel1, kISRRTC, kISRReserved, kISRNotUsed1, kISRNotUsed2
global kISRMouse, kISRCoprocessor, kISRHDD1, kISRHDD2, kISRETCInterrupt

; ���ؽ�Ʈ�� �����ϰ� �����͸� ��ü�ϴ� ��ũ��
%macro KSAVECONTEXT 0       ; �Ķ���͸� ���޹��� �ʴ� KSAVECONTEXT ��ũ�� ����
    ; RBP �������ͺ��� GS ���׸�Ʈ �����ͱ��� ��� ���ÿ� ����
    push rbp
    mov rbp, rsp
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov ax, ds      ; DS ���׸�Ʈ �����Ϳ� ES ���׸�Ʈ �����ʹ� ���ÿ� ����
    push rax        ; ������ �� �����Ƿ�, RAX �������Ϳ� ������ �� ���ÿ� ����
    mov ax, es
    push rax
    push fs
    push gs

    ; ���׸�Ʈ ������ ��ü
    mov ax, 0x10    ; AX �������Ϳ� Ŀ�� ������ ���׸�Ʈ ��ũ���� ����
    mov ds, ax      ; DS ���׸�Ʈ �����ͺ��� FS ���׸�Ʈ �����ͱ��� ���
    mov es, ax      ; Ŀ�� ������ ���׸�Ʈ�� ��ü
    mov gs, ax
    mov fs, ax
%endmacro           ; ��ũ�� ��


; ���ؽ�Ʈ�� �����ϴ� ��ũ��
%macro KLOADCONTEXT 0   ; �Ķ���͸� ���޹��� �ʴ� KSAVECONTEXT ��ũ�� ����
    ; GS ���׸�Ʈ �����ͺ��� RBP �������ͱ��� ��� ���ÿ��� ���� ����
    pop gs
    pop fs
    pop rax
    mov es, ax      ; ES ���׸�Ʈ �����Ϳ� DS ���׸�Ʈ �����ʹ� ���ÿ��� ����
    pop rax         ; ���� ������ �� �����Ƿ�, RAX �������Ϳ� ������ �ڿ� ����
    mov ds, ax

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    pop rbp
%endmacro       ; ��ũ�� ��

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	예외 핸들러
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; #0, Divide Error ISR
kISRDivideError:
    KSAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체

    ; 핸들러에 예외 번호를 삽입하고 핸들러 호출
    mov rdi, 0
    call kCommonExceptionHandler

    KLOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #1, Debug ISR
kISRDebug:
    KSAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체

    ; 핸들러에 예외 번호를 삽입하고 핸들러 호출
    mov rdi, 1
    call kCommonExceptionHandler

    KLOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #2, NMI ISR
kISRNMI:
    KSAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체

    ; 핸들러에 예외 번호를 삽입하고 핸들러 호출
    mov rdi, 2
    call kCommonExceptionHandler

    KLOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #3, BreakPoint ISR
kISRBreakPoint:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���� ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 3
    call kCommonExceptionHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #4, Overflow ISR
kISROverflow:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���� ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 4
    call kCommonExceptionHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #5, Bound Range Exceeded ISR
kISRBoundRangeExceeded:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���� ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 5
    call kCommonExceptionHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #6, Invalid Opcode ISR
kISRInvalidOpcode:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���� ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 6
    call kCommonExceptionHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #7, Device Not Available ISR
kISRDeviceNotAvailable:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���� ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 7
    call kCommonExceptionHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #8, Double Fault ISR
kISRDoubleFault:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���� ��ȣ�� ���� �ڵ带 �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 8
    mov rsi, qword [ rbp + 8 ]
    call kCommonExceptionHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    add rsp, 8      ; ���� �ڵ带 ���ÿ��� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #9, Coprocessor Segment Overrun ISR
kISRCoprocessorSegmentOverrun:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���� ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 9
    call kCommonExceptionHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #10, Invalid TSS ISR
kISRInvalidTSS:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���� ��ȣ�� ���� �ڵ带 �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 10
    mov rsi, qword [ rbp + 8 ]
    call kCommonExceptionHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    add rsp, 8      ; ���� �ڵ带 ���ÿ��� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #11, Segment Not Present ISR
kISRSegmentNotPresent:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���� ��ȣ�� ���� �ڵ带 �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 11
    mov rsi, qword [ rbp + 8 ]
    call kCommonExceptionHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    add rsp, 8      ; ���� �ڵ带 ���ÿ��� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #12, Stack Segment Fault ISR
kISRStackSegmentFault:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���� ��ȣ�� ���� �ڵ带 �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 12
    mov rsi, qword [ rbp + 8 ]
    call kCommonExceptionHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    add rsp, 8      ; ���� �ڵ带 ���ÿ��� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #13, General Protection ISR
kISRGeneralProtection:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���� ��ȣ�� ���� �ڵ带 �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 13
    mov rsi, qword [ rbp + 8 ]
    call kCommonExceptionHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    add rsp, 8      ; ���� �ڵ带 ���ÿ��� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #14, Page Fault ISR
kISRPageFault:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���� ��ȣ�� ���� �ڵ带 �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 14
    mov rsi, qword [ rbp + 8 ]
    call kPageFault

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    add rsp, 8      ; ���� �ڵ带 ���ÿ��� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #15, Reserved ISR
kISR15:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���� ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 15
    call kCommonExceptionHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #16, FPU Error ISR
kISRFPUError:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���� ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 16
    call kCommonExceptionHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #17, Alignment Check ISR
kISRAlignmentCheck:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���� ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 17
    mov rsi, qword [ rbp + 8 ]
    call kCommonExceptionHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    add rsp, 8      ; ���� �ڵ带 ���ÿ��� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #18, Machine Check ISR
kISRMachineCheck:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���� ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 18
    call kCommonExceptionHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #19, SIMD Floating Point Exception ISR
kISRSIMDError:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���� ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 19
    call kCommonExceptionHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #20~#31, Reserved ISR
kISRETCException:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���� ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 20
    call kCommonExceptionHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	인터럽트 핸들러
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; #32, 타이머 ISR
kISRTimer:
    KSAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체

    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 32
    call kTimerHandler

    KLOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #33, Ű���� ISR
kISRKeyboard:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���ͷ�Ʈ ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 33
    call kKeyboardHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #34, �����̺� PIC ISR
kISRSlavePIC:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���ͷ�Ʈ ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 34
    call kCommonInterruptHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #35, �ø��� ��Ʈ 2 ISR
kISRSerial2:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���ͷ�Ʈ ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 35
    call kCommonInterruptHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #36, �ø��� ��Ʈ 1 ISR
kISRSerial1:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���ͷ�Ʈ ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 36
    call kCommonInterruptHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #37, �з��� ��Ʈ 2 ISR
kISRParallel2:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���ͷ�Ʈ ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 37
    call kCommonInterruptHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #38, �÷��� ��ũ ��Ʈ�ѷ� ISR
kISRFloppy:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���ͷ�Ʈ ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 38
    call kCommonInterruptHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #39, �з��� ��Ʈ 1 ISR
kISRParallel1:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���ͷ�Ʈ ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 39
    call kCommonInterruptHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #40, RTC ISR
kISRRTC:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���ͷ�Ʈ ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 40
    call kCommonInterruptHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #41, ����� ���ͷ�Ʈ�� ISR
kISRReserved:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���ͷ�Ʈ ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 41
    call kCommonInterruptHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #42, ��� ����
kISRNotUsed1:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���ͷ�Ʈ ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 42
    call kCommonInterruptHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #43, ��� ����
kISRNotUsed2:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���ͷ�Ʈ ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 43
    call kCommonInterruptHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #44, ���콺 ISR
kISRMouse:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���ͷ�Ʈ ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 44
    call kCommonInterruptHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #45, �����μ��� ISR
kISRCoprocessor:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���ͷ�Ʈ ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 45
    call kCommonInterruptHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #46, 하드 디스크 1 ISR
kISRHDD1:
    KSAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 46
    call kHDDHandler

    KLOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원


; #47, �ϵ� ��ũ 2 ISR
kISRHDD2:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���ͷ�Ʈ ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 47
    call kCommonInterruptHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����

; #48 �̿��� ��� ���ͷ�Ʈ�� ���� ISR
kISRETCInterrupt:
    KSAVECONTEXT    ; ���ؽ�Ʈ�� ������ �� �����͸� Ŀ�� ������ ��ũ���ͷ� ��ü

    ; �ڵ鷯�� ���ͷ�Ʈ ��ȣ�� �����ϰ� �ڵ鷯 ȣ��
    mov rdi, 48
    call kCommonInterruptHandler

    KLOADCONTEXT    ; ���ؽ�Ʈ�� ����
    iretq           ; ���ͷ�Ʈ ó���� �Ϸ��ϰ� ������ �����ϴ� �ڵ�� ����
