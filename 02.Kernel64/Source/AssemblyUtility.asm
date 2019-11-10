# file      Assembly Utility
# date      2009/01/07
# author    kkamagui 
#           Copyright(c)2008 All rights reserved by kkamagui
# brief     ��������� ��ƿ��Ƽ �Լ��� ���õ� �ҽ� ����

[BITS 64]           ; ������ �ڵ�� 64��Ʈ �ڵ�� ����

SECTION .text       ; text ����(���׸�Ʈ)�� ����

; C ���� ȣ���� �� �ֵ��� �̸��� ������(Export)
global kInPortByte, kOutPortByte, kLoadGDTR, kLoadTR, kLoadIDTR
global kEnableInterrupt, kDisableInterrupt, kReadRFLAGS,kGetCr3,kGetCr2
global kReadTSC
global kSwitchContext

; ��Ʈ�κ��� 1����Ʈ�� ����
;   PARAM: ��Ʈ ��ȣ
kInPortByte:
    push rdx        ; �Լ����� �ӽ÷� ����ϴ� �������͸� ���ÿ� ����
                    ; �Լ��� ������ �κп��� ���ÿ� ���Ե� ���� ���� ����

    mov rdx, rdi    ; RDX �������Ϳ� �Ķ���� 1(��Ʈ ��ȣ)�� ����
    mov rax, 0      ; RAX �������͸� �ʱ�ȭ
    in al, dx       ; DX �������Ϳ� ����� ��Ʈ ��巹������ �� ����Ʈ�� �о�
                    ; AL �������Ϳ� ����, AL �������ʹ� �Լ��� ��ȯ ������ ����

    pop rdx         ; �Լ����� ����� ���� �������͸� ����
    ret             ; �Լ��� ȣ���� ���� �ڵ��� ��ġ�� ����

; ��Ʈ�� 1����Ʈ�� ��
;   PARAM: ��Ʈ ��ȣ, ������
kOutPortByte:
    push rdx        ; �Լ����� �ӽ÷� ����ϴ� �������͸� ���ÿ� ����
    push rax        ; �Լ��� ������ �κп��� ���ÿ� ���Ե� ���� ���� ����

    mov rdx, rdi    ; RDX �������Ϳ� �Ķ���� 1(��Ʈ ��ȣ)�� ����
    mov rax, rsi    ; RAX �������Ϳ� �Ķ���� 2(������)�� ����
    out dx, al      ; DX �������Ϳ� ����� ��Ʈ ��巹���� AL �������Ϳ� �����
                    ; �� ����Ʈ�� ��

    pop rax         ; �Լ����� ����� ���� �������͸� ����
    pop rdx
    ret             ; �Լ��� ȣ���� ���� �ڵ��� ��ġ�� ����


; GDTR �������Ϳ� GDT ���̺��� ����
;   PARAM: GDT ���̺��� ������ �����ϴ� �ڷᱸ���� ��巹��
kLoadGDTR:
    lgdt [ rdi ]    ; �Ķ���� 1(GDTR�� ��巹��)�� ���μ����� �ε��Ͽ�
                    ; GDT ���̺��� ����
    ret

; TR �������Ϳ� TSS ���׸�Ʈ ��ũ���� ����
;   PARAM: TSS ���׸�Ʈ ��ũ������ ������
kLoadTR:
    ltr di          ; �Ķ���� 1(TSS ���׸�Ʈ ��ũ������ ������)�� ���μ�����
                    ; �����Ͽ� TSS ���׸�Ʈ�� �ε�
    ret

; IDTR �������Ϳ� IDT ���̺��� ����
;   PARAM: IDT ���̺��� ������ �����ϴ� �ڷᱸ���� ��巹��
kLoadIDTR:
    lidt [ rdi ]    ; �Ķ���� 1(IDTR�� ��巹��)�� ���μ����� �ε��Ͽ�
                    ; IDT ���̺��� ����
    ret

; ���ͷ�Ʈ�� Ȱ��ȭ
;   PARAM: ����
kEnableInterrupt:
    sti             ; ���ͷ�Ʈ�� Ȱ��ȭ
    ret

; ���ͷ�Ʈ�� ��Ȱ��ȭ
;   PARAM: ����
kDisableInterrupt:
    cli             ; ���ͷ�Ʈ�� ��Ȱ��ȭ
    ret

; RFLAGS �������͸� �о �ǵ�����
;   PARAM: ����
kReadRFLAGS:
    pushfq                  ; RFLAGS �������͸� ���ÿ� ����
    pop rax                 ; ���ÿ� ����� RFLAGS �������͸� RAX �������Ϳ� �����Ͽ�
                            ; �Լ��� ��ȯ ������ ����
    ret
kGetCr3:
    mov rax,cr3
    ret
kGetCr2:
    mov rax,cr2
    ret
; Ÿ�� ������ ī���͸� �о ��ȯ 
;   PARAM: ����    
kReadTSC:
    push rdx                ; RDX �������͸� ���ÿ� ����
    
    rdtsc                   ; Ÿ�� ������ ī���͸� �о RDX:RAX�� ����
    
    shl rdx, 32             ; RDX �������Ϳ� �ִ� ���� 32��Ʈ TSC ���� RAX �������Ϳ�
    or rax, rdx             ; �ִ� ���� 32��Ʈ TSC ���� OR�Ͽ� RAX �������Ϳ� 64��Ʈ 
                            ; TSC ���� ����
    
    pop rdx
    ret
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   태스크 관련 어셈블리어 함수
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 콘텍스트를 저장하고 셀렉터를 교체하는 매크로
%macro KSAVECONTEXT 0       ; 파라미터를 전달받지 않는 KSAVECONTEXT 매크로 정의
    ; RBP 레지스터부터 GS 세그먼트 셀렉터까지 모두 스택에 삽입
    push rbp
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
    
    mov ax, ds      ; DS 세그먼트 셀렉터와 ES 세그먼트 셀렉터는 스택에 직접
    push rax        ; 삽입할 수 없으므로, RAX 레지스터에 저장한 후 스택에 삽입
    mov ax, es
    push rax
    push fs
    push gs 
%endmacro       ; 매크로 끝


; 콘텍스트를 복원하는 매크로
%macro KLOADCONTEXT 0   ; 파라미터를 전달받지 않는 KSAVECONTEXT 매크로 정의
    ; GS 세그먼트 셀렉터부터 RBP 레지스터까지 모두 스택에서 꺼내 복원
    pop gs
    pop fs
    pop rax
    mov es, ax      ; ES 세그먼트 셀렉터와 DS 세그먼트 셀렉터는 스택에서 직접
    pop rax         ; 꺼내 복원할 수 없으므로, RAX 레지스터에 저장한 뒤에 복원
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
%endmacro       ; 매크로 끝

; Current Context에 현재 콘텍스트를 저장하고 Next Task에서 콘텍스트를 복구
;   PARAM: Current Context, Next Context
kSwitchContext:
    push rbp        ; 스택에 RBP 레지스터를 저장하고 RSP 레지스터를 RBP에 저장
    mov rbp, rsp
    
    ; Current Context가 NULL이면 콘텍스트를 저장할 필요 없음
    pushfq          ; 아래의 cmp의 결과로 RFLAGS 레지스터가 변하지 않도록 스택에 저장
    cmp rdi, 0      ; Current Context가 NULL이면 콘텍스트 복원으로 바로 이동
    je .LoadContext 
    popfq           ; 스택에 저장한 RFLAGS 레지스터를 복원

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; 현재 태스크의 콘텍스트를 저장
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    push rax            ; 콘텍스트 영역의 오프셋으로 사용할 RAX 레지스터를 스택에 저장
    
    ; SS, RSP, RFLAGS, CS, RIP 레지스터 순서대로 삽입
    mov ax, ss                          ; SS 레지스터 저장
    mov qword[ rdi + ( 23 * 8 ) ], rax

    mov rax, rbp                        ; RBP에 저장된 RSP 레지스터 저장
    add rax, 16                         ; RSP 레지스터는 push rbp와 Return Address를
    mov qword[ rdi + ( 22 * 8 ) ], rax  ; 제외한 값으로 저장
    
    pushfq                              ; RFLAGS 레지스터 저장
    pop rax
    mov qword[ rdi + ( 21 * 8 ) ], rax

    mov ax, cs                          ; CS 레지스터 저장
    mov qword[ rdi + ( 20 * 8 ) ], rax
    
    mov rax, qword[ rbp + 8 ]           ; RIP 레지스터를 Return Address로 설정하여 
    mov qword[ rdi + ( 19 * 8 ) ], rax  ; 다음 콘텍스트 복원 시에 이 함수를 호출한 
                                        ; 위치로 이동하게 함
    
    ; 저장한 레지스터를 복구한 후 인터럽트가 발생했을 때처럼 나머지 콘텍스트를 모두 저장
    pop rax
    pop rbp
    
    ; 가장 끝부분에 SS, RSP, RFLAGS, CS, RIP 레지스터를 저장했으므로, 이전 영역에
    ; push 명령어로 콘텍스트를 저장하기 위해 스택을 변경
    add rdi, ( 19 * 8 )
    mov rsp, rdi
    sub rdi, ( 19 * 8 )
    
    ; 나머지 레지스터를 모두 Context 자료구조에 저장
    KSAVECONTEXT

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; 다음 태스크의 콘텍스트 복원
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.LoadContext:
    mov rsp, rsi
    
    ; Context 자료구조에서 레지스터를 복원
    KLOADCONTEXT
    iretq
