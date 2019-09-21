[ORG 0x00]          ; 코드의 시작 어드레스를 0x00으로 설정
[BITS 16]           ; 이하의 코드는 16비트 코드로 설정

SECTION .text       ; text 섹션(세그먼트)을 정의

jmp 0x07C0:START    ; CS 세그먼트 레지스터에 0x10000을 복사하면서, START 레이블로 이동
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   코드 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
    mov ax, 0x07C0  ; 부트 로더의 시작 어드레스(0x7C00)를 세그먼트 레지스터 값으로 변환
    mov ds, ax      ; DS 세그먼트 레지스터에 설정
    mov ax, 0xB800  ; 비디오 메모리의 시작 어드레스(0xB800)를 세그먼트 레지스터 값으로 변환
    mov es, ax      ; ES 세그먼트 레지스터에 설정
    ; 스택을 0x0000:0000~0x0000:FFFF 영역에 64Kb 크기로 생성
    mov ax, 0x0000  ; 스택 세그먼트의 시작 어드레스(0x0000)를 세그먼트 레지스터 값으로 변환
    mov ss, ax      ; SS 세그먼트 레지스터에 설정
    mov sp, 0xFFFE  ; SP 레지스터의 어드레스를 0xFFFE로 설정
    mov bp, 0xFFFE  ; BP 레지스터의 어드레스를 0xFFFE로 설정
    mov si,    0                    ; SI 레지스터(문자열 원본 인덱스 레지스터)를 초기화
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; 화면을 모두 지우고, 속성값을 녹색으로 설정
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        
.SCREENCLEARLOOP:                   ; 화면을 지우는 루프
    mov byte [ es: si ], 0          ; 비디오 메모리의 문자가 위치하는 어드레스에
                                    ; 0을 복사하여 문자를 삭제
    mov byte [ es: si + 1 ], 0x0A   ; 비디오 메모리의 속성이 위치하는 어드레스에
                                    ; 0x0A(검은 바탕에 밝은 녹색)을 복사
    add si, 2                       ; 문자와 속성을 설정했으므로 다음 위치로 이동
    cmp si, 80 * 25 * 2     ; 화면의 전체 크기는 80 문자 * 25 라인임
                            ; 출력한 문자의 수를 의미하는 SI 레지스터와 비교
    jl .SCREENCLEARLOOP     ; SI 레지스터가 80 * 25 * 2보다 작다면 아직 지우지 
                            ; 못한 영역이 있으므로 .SCREENCLEARLOOP 레이블로 이동
                                  
    mov ah, 0x04
    int 0x1a
SetTime:
    ;day
    mov al,dl
    mov ah,al
    shr ah,4 ; 10
    and al, 0x0F ;1
    add ah,48
    add al,48
    mov [CURDATAMESSAGE2+0],ah
    mov [CURDATAMESSAGE2+1],al

    ;month
    mov al,dh
    mov ah,al
    shr ah,4 ; 10
    and al, 0x0F ;1
    add ah,48
    add al,48
    mov [CURDATAMESSAGE2+3],ah
    mov [CURDATAMESSAGE2+4],al

    ; year
    mov al, ch
    shr al, 4                   
    mov di, 1000
    mul di                 
    mov bx, ax                
    div di
    add al, 48
    mov [CURDATAMESSAGE2 + 6], al   
    mov al, ch
    and al, 0x0F               
    mov di, 100
    mul di                     
    add bx, ax                
    div di
    add al, 48
    mov [CURDATAMESSAGE2 + 7], al
       
    mov al, cl
    shr al, 4                   
    mov di, 10
    mul di                  
    add bx, ax               
    div di
    add al, 48
    mov [CURDATAMESSAGE2 + 8], al   
    mov al, cl
    and al, 0x0F               
    add bx, ax                 
    add al, 48
    mov [CURDATAMESSAGE2 + 9], al
    
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;계산
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    dec bx      
    call GetLeapCount
    mov word[LEAPA], di
    mov cx, di
    inc bx
    call GetLeapCount
    mov word[LEAPB], di
    sub cx, 460         
    sub bx, 1900        
    imul bx,bx,365      
    add bx, cx 
    and di,0
    mov al, byte[CURDATAMESSAGE2 + 3]
    sub al, 48
    mov cx, 10
    mul cx
    mov cl, byte[CURDATAMESSAGE2 + 4]
    sub cl, 48
    add cl, al           
    mov si, di
    dec di
    shl di, 1
    add bx, [incMonth+ecx]
    mov ax, word[LEAPB]
    mov di, word[LEAPA] 
    cmp ax,di
    je .CommonYear ;평년
    inc bx ;윤년

.CommonYear:
    and cx,0
    mov al, byte[CURDATAMESSAGE2 + 0]
    sub al, 0x30
    mov di, 10
    mul di
    mov cl, byte[CURDATAMESSAGE2 + 1]
    sub cl, 0x30
    add cl, al          
    add bx, cx
    xor dx, dx
    mov ax, bx 
    mov cx, 7
    div cx           
    mov ax, dx
    mov bx, 3
    mul bx
    mov di, ax
    mov cl, byte[WHATDAY+di] ;11
    mov [CURDATAMESSAGE2 + 11], cl
    add di,1
    mov cl, byte[WHATDAY+di]
    mov [CURDATAMESSAGE2 + 12], cl
    add di,1
    mov cl, byte[WHATDAY+di]
    mov [CURDATAMESSAGE2 + 13], cl
    and si,0
    mov di,188
RESULT:
    mov al,byte[CURDATAMESSAGE2+si]
    cmp al,0
    je RESETDISK
    mov byte[es:di],al
    add si,1
    add di,2
    jmp RESULT
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; 디스크에서 OS 이미지를 로딩
    ; 디스크를 읽기 전에 먼저 리셋
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
RESETDISK:                          ; 디스크를 리셋하는 코드의 시작
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; BIOS Reset Function 호출
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; 서비스 번호 0, 드라이브 번호(0=Floppy)
    mov ax, 0
    mov dl, 0              
    int 0x13     
    ; 에러가 발생하면 에러 처리로 이동
    jc  HANDLEDISKERROR
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; 디스크에서 섹터를 읽음
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; 디스크의 내용을 메모리로 복사할 어드레스(ES:BX)를 0x10000으로 설정
    mov si, 0x1000                  ; OS 이미지를 복사할 어드레스(0x10000)를 
                                    ; 세그먼트 레지스터 값으로 변환
    mov es, si                      ; ES 세그먼트 레지스터에 값 설정
    mov bx, 0x0000                  ; BX 레지스터에 0x0000을 설정하여 복사할 
                                    ; 어드레스를 0x1000:0000(0x10000)으로 최종 설정
READDATA:                           ; 디스크를 읽는 코드의 시작
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; BIOS Read Function 호출
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov ah, 0x02                        ; BIOS 서비스 번호 2(Read Sector)
    mov al, 0x1                         ; 읽을 섹터 수는 1
    mov ch, byte [ TRACKNUMBER ]        ; 읽을 트랙 번호 설정
    mov cl, byte [ SECTORNUMBER ]       ; 읽을 섹터 번호 설정
    mov dh, byte [ HEADNUMBER ]         ; 읽을 헤드 번호 설정
    mov dl, 0x00                        ; 읽을 드라이브 번호(0=Floppy) 설정
    int 0x13                            ; 인터럽트 서비스 수행
    jc HANDLEDISKERROR                  ; 에러가 발생했다면 HANDLEDISKERROR로 이동
READEND:
    jmp 0x1000:0x0000
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   함수 코드 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;윤년 갯수 구하는 함수
GetLeapCount: ; 벤다이어그램 2 ) 100 ) 400
    mov di, bx
    shr di, 2
    mov ax, bx
    mov si, 100
    and dx, 0
    div si
    sub di, ax
    mov ax, bx
    mov si, 400
    and dx, 0
    div si
    add di, ax 
    ret
; 디스크 에러를 처리하는 함수   
HANDLEDISKERROR:    
    jmp $                   ; 현재 위치에서 무한 루프 수행
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   데이터 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 부트 로더 시작 메시지

CURDATAMESSAGE2:       db  '00/00/0000 000', 0
WHATDAY:    db  'SUNMONTUEWEDTHUFRISAT', 0
incMonth DW 0,31,59,90,120,151,181,212,243,273,304,334 ;배열
LEAPA:                dw  0x00
LEAPB:               dw  0x00
; 디스크 읽기에 관련된 변수들
SECTORNUMBER:           db  0x02    ; OS 이미지가 시작하는 섹터 번호를 저장하는 영역
HEADNUMBER:             db  0x00    ; OS 이미지가 시작하는 헤드 번호를 저장하는 영역
TRACKNUMBER:            db  0x00    ; OS 이미지가 시작하는 트랙 번호를 저장하는 영역
    
times 510 - ( $ - $$ )    db    0x00    ; $ : 현재 라인의 어드레스
                                        ; $$ : 현재 섹션(.text)의 시작 어드레스
                                        ; $ - $$ : 현재 섹션을 기준으로 하는 오프셋
                                        ; 510 - ( $ - $$ ) : 현재부터 어드레스 510까지
                                        ; db 0x00 : 1바이트를 선언하고 값은 0x00
                                        ; time : 반복 수행
                                        ; 현재 위치에서 어드레스 510까지 0x00으로 채움

db 0x55             ; 1바이트를 선언하고 값은 0x55
db 0xAA             ; 1바이트를 선언하고 값은 0xAA
                    ; 어드레스 511, 512에 0x55, 0xAA를 써서 부트 섹터로 표기함