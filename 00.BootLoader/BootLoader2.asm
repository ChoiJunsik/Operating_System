;Project by WHJ,USB,JSH,CJS
[ORG 0x00]          
[BITS 16]           

SECTION .text       
jmp 0x1000:START

TOTALSECTORCOUNT:   dw  1024    
                                
START:
    ; What day is it today?
    mov ax, 0x1000  
    mov ds, ax      
    mov ax, 0xB800  
    mov es, ax      
    mov ax, 0x0000  
    mov ss, ax      
    mov sp, 0xFFFE  
    mov bp, 0xFFFE  
    mov si,    0                    


    mov ah, 0x04          
    int 0x1a

    mov al, ch                      
    shr al, 4
    mov bl, 10
    mul bl
    and ch, 0x0F
    add al, ch
    mov bx, 100
    mul bx
    mov di, ax

    mov al, cl                       
    shr al, 4
    mov bl, 10
    mul bl
    and cl, 0x0F
    add al, cl
    add di, ax

    mov ax, di
    sub ax, 1900     
    mov di, ax
    mov bl, 4
    div bl
    mov ah, al
    mov ah, 0
    add di, ax
    sub di, 1                  

    mov ah, 0x04              
    int 0x1a

    mov al, dh                      
    shr al, 4
    mov bl, 10
    mul bl
    and dh, 0x0F
    add al, dh

    mov ah, 0
    mov bl, 2
    mul bl
    sub ax, 2
    add di, [MONTHLYDAY + eax]


    mov al, dl                     
    shr al, 4
    mov bl, 10
    mul bl
    and dl, 0x0F
    add al, dl
    mov ah, 0

    add di, ax                  

    mov ax, di
    mov bx, 7
    div bl                           

    mov al, ah
    mov ah, 0
    mov bl, 4
    mul bl

    mov edi, [DAYOFWEEK]
    add edi, eax

PRINTDAYEND:
    push edi
    push 1
    push 25
    call PRINTMESSAGE
    add  sp, 6

ENDDDDD:
    push IMAGELOADINGMESSAGE   
    push 2                      
    push 0                      
    call PRINTMESSAGE           
    add  sp, 6

; virtual OS Read    
RESETDISK:                          
    mov ax, 0
    mov dl, 0              
    int 0x13     
    jc  HANDLEDISKERROR
    mov si, 0x1020                               
    mov es, si                      
    mov bx, 0x0000                  
    mov di, word [ TOTALSECTORCOUNT ] 

READDATA:                           
    cmp di, 0               
    je  READEND             
    sub di, 0x1             
    mov ah, 0x02                        
    mov al, 0x1                         
    mov ch, byte [ TRACKNUMBER ]        
    mov cl, byte [ SECTORNUMBER ]       
    mov dh, byte [ HEADNUMBER ]         
    mov dl, 0x00                        
    int 0x13                            
    jc HANDLEDISKERROR                  
    add si, 0x0020      
    mov es, si          
    mov al, byte [ SECTORNUMBER ]       
    add al, 0x01                        
    mov byte [ SECTORNUMBER ], al       
    cmp al, 19                          
    jl READDATA                        
    xor byte [ HEADNUMBER ], 0x01       
    mov byte [ SECTORNUMBER ], 0x01     
    cmp byte [ HEADNUMBER ], 0x00       
    jne READDATA                        
    add byte [ TRACKNUMBER ], 0x01      
    jmp READDATA                        
READEND:
    push LOADINGCOMPLETEMESSAGE     
    push 2                          
    push 20                         
    call PRINTMESSAGE               
    add  sp, 6                      
    jmp 0x1020:0x0000
HANDLEDISKERROR:
    push DISKERRORMESSAGE   
    push 1                  
    push 20                 
    call PRINTMESSAGE       
    jmp $                   
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

DISKERRORMESSAGE:       db  'DISK Error~!!', 0
IMAGELOADINGMESSAGE:    db  'OS Image Loading...', 0
LOADINGCOMPLETEMESSAGE: db  'Complete~!!', 0

MONTHLYDAY dw 0,31,59,90,120,151,181,212,243,273,304,334
MONDAYDATE db 'Mon', 0
TUEDAYDATE db 'Tue', 0
WEDDAYDATE db 'Wed', 0
THRDAYDATE db 'Thu', 0
FRIDAYDATE db 'Fri', 0
SATDAYDATE db 'Sat', 0
SUNDAYDATE db 'Sun', 0

DAYOFWEEK: dd MONDAYDATE, TUEDAYDATE, WEDDAYDATE, THRDAYDATE, FRIDAYDATE, SATDAYDATE, SUNDAYDATE, 0

SECTORNUMBER:           db  0x03    
HEADNUMBER:             db  0x00    
TRACKNUMBER:            db  0x00    

times 510 - ( $ - $$ )    db    0x00    
db 0x55             
db 0xAA             
                    
 
