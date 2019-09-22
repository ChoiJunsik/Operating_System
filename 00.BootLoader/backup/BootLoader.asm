;Project by WHJ,USB,JSH,CJS
[ORG 0x00]          
[BITS 16]           

SECTION .text       

jmp 0x07C0:START

START:
    mov ax, 0x07C0  
    mov ds, ax      
    mov ax, 0xB800  
    mov es, ax      
    
    mov ax, 0x0000  
    mov ss, ax      
    mov sp, 0xFFFE  
    mov bp, 0xFFFE  
    mov si,    0                    

.SCREENCLEARLOOP:                   
    mov byte [ es: si ], 0                                       
    mov byte [ es: si + 1 ], 0x0A                        
    add si, 2                       
    cmp si, 80 * 25 * 2                
    jl .SCREENCLEARLOOP     
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;계산
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    mov ah, 0x04
    int 0x1a

    movzx ax, ch
    mov bx, ax
    and bx, 0xF
    shr ax, 4
    mov si, 1000
    mul si
    mov di, ax
    mov ax, bx
    mov si, 100
    mul si
    add ax, di
    push ax
    ;;; 세기 저장됨;;;

    movzx ax, cl
    shr ax, 4 ;10
    and cx, 0xF
    mov si, 10
    mul si
    add ax, cx
    pop cx
    add ax, cx
    push ax
    ;;; 세기+연도 저장됨 ;;;


    mov di, 1900
    push di
YEARLOOP :
     push di
     call CALLEAP  ; 스택에 1(윤년)or0(평년) 들어가있는 상태
     pop cx

     pop di
     cmp di, word[esp]
     jnb YEARLOOPEND ; si가 현재년과 같으면 루프 탈출

     add cx, 1
     add word[ENUMDAY], cx ; 윤년이면+2(366mod7=2), 평년이면+1(365mod7=1) 됨
     add di, 1
     push di
     jmp YEARLOOP
 YEARLOOPEND :
     mov si, MONTHDAY
     add si, 1
     mov byte[si], cl    ;윤년이면 달 배열의 2월이 1, 평년이면 0이 됨

     mov ah, 0x04
     int 0x1a
 ;;;; 월 계산 ;;;;;;
     movzx ax, dh
     mov bx, ax
     shr ax, 4 ; 10
     and bx, 0xF ;1
     mov di, 10
     mul di
     add ax, bx
     sub ax, 1
 ;;;; 월별 날짜 더하기 ;;;;
      mov si, 0
      DAYLOOP :
      cmp si, ax
      jnb DAYEND
      movzx di, byte[MONTHDAY+si]

      add word[ENUMDAY], di
      add si, 1
      cmp si, ax
      jb DAYLOOP

      DAYEND :

     mov ah, 0x04
     int 0x1a
     movzx ax, dl
     mov bx, ax
     shr ax, 4
     and bx, 0x0F
     mov di, 10
     mul di
     add ax, bx
     sub ax, 1

      add ax, word[ENUMDAY]
      mov di, 7
      mov dx, 0
      div di
      mov ax, 4
      mul dx

      add ax, MONDAY

      push ax    ; 출력할 메시지의 어드레스를 스택에 삽입
      push 1                      ; 화면 Y 좌표(1)를 스택에 삽입
      push 25                      ; 화면 X 좌표(0)를 스택에 삽입
      call PRINTMESSAGE           ; PRINTMESSAGE 함수 호출
      add sp,6


; Func Section
RESETDISK:                          
    mov ax, 0
    mov dl, 0
    int 0x13
    jc  HANDLEDISKERROR

    mov si, 0x1000                            
    mov es, si                      
    mov bx, 0x0000                  
                                    
READDATA:                           
    
    mov ah, 0x02                        
    mov al, 0x1                         
    mov ch, byte [ TRACKNUMBER ]        
    mov cl, byte [ SECTORNUMBER ]       
    mov dh, byte [ HEADNUMBER ]         
    mov dl, 0x00                        
    int 0x13                            
    jc HANDLEDISKERROR                  
READEND:
    jmp 0x1000:0x0000

PRINTMESSAGE:
    push bp         
    mov bp, sp      

    push es         
    push si         
    push di         
    push ax
    push cx
    push dx

    mov ax, 0xB800                    
    mov es, ax                  

    mov ax, word [ bp + 6 ]     
    mov si, 160                 
    mul si                      
    mov di, ax                  

    mov ax, word [ bp + 4 ]     
    mov si, 2                   
    mul si                      
    add di, ax                  
    
    mov si, word [ bp + 8 ]     

.MESSAGELOOP:               
    mov cl, byte [ si ]     
    cmp cl, 0               
    je .MESSAGEEND             

    mov byte [ es: di ], cl 

    add si, 1               
    add di, 2               

    jmp .MESSAGELOOP        

.MESSAGEEND:
    pop dx      
    pop cx      
    pop ax      
    pop di      
    pop si      
    pop es
    pop bp      
    ret         
HANDLEDISKERROR:
    push DISKERRORMESSAGE   
    push 1                  
    push 20                 
    call PRINTMESSAGE       
    jmp $               

CALLEAP :
    push bp
    mov bp, sp
    mov si, [bp+4] 

    mov ax, si
    mov di, 4
    mov dx, 0
    div di
    cmp dx, 0 
    jne NOTLEAPYEAR

    mov ax, si
    mov di, 100
    mov dx, 0
    div di
    cmp dx, 0 
    jne LEAPYEAR

    mov ax, si
    mov di, 400
    mov dx, 0
    div di
    cmp dx, 0 
    jne NOTLEAPYEAR

LEAPYEAR :
    mov word[bp+4], 0x1
    pop bp
    ret
NOTLEAPYEAR :
    
    mov word[bp+4], 0x0
    pop bp
    ret

; Data Section
MONDAY :  db 'Mon',0
TUESDAY : db 'Tue',0
WEDNESDAY : db 'Wed',0
THURSDAY : db 'Thu',0
FRYDAY : db 'Fri', 0
SATURDAY : db 'Sat',0
SUNDAY : db 'Sun', 0
MONTHDAY : db 3, 0, 3, 2, 3, 2, 3, 3, 2, 3, 2, 3
ENUMDAY :         dw  0

TEST : db '0', 0


SECTORNUMBER:           db  0x02    
HEADNUMBER:             db  0x00    
TRACKNUMBER:            db  0x00    

times 510 - ( $ - $$ )    db    0x00    

db 0x55             
db 0xAA             
