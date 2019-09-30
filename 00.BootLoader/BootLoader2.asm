;Project by WHJ,USB,JSH,CJS
[ORG 0x00]          
[BITS 16]           

SECTION .text       

jmp 0x07E0:START    

TOTALSECTORCOUNT:   dw  0x02    

START:
    mov ax, 0x07E0  
    mov ds, ax      
    mov ax, 0xB800  
    mov es, ax      

    mov ax, 0x0000  
    mov ss, ax      
    mov sp, 0xFFFE  
    mov bp, 0xFFFE  
    mov si,    0                    

    push MESSAGE1               
    push 0                      
    push 0                      
    call PRINTMESSAGE           
    add  sp, 6                  

    push IMAGELOADINGMESSAGE    
    push 2                      
    push 0                      
    call PRINTMESSAGE           
    add  sp, 6                  

    push CURDATAMESSAGE1    
    push 1                      
    push 0                      
    call PRINTMESSAGE           

    mov bx, CURDATAMESSAGE2
    mov ah, 0x04
    int 0x1a

    mov al,dl
    mov ah,al
    shr ah,4 
    and al, 0x0F 
    add ax, 0x3030
    mov byte[bx],ah
    add bx,1
    mov byte[bx],al
    add bx,2

    mov al,dh
    mov ah,al
    shr ah,4 
    and al, 0x0F 
    add ax, 0x3030
    mov byte[bx],ah
    add bx,1
    mov byte[bx],al
    add bx,2

    mov al,ch
    mov ah,al
    shr ah,4 
    and al, 0x0F 
    add ax, 0x3030
    mov byte[bx],ah
    add bx,1
    mov byte[bx],al
    add bx,1

    mov al,cl
    mov ah,al
    shr ah,4 
    and al, 0x0F 
    add ax, 0x3030
    mov byte[bx],ah
    add bx,1
    mov byte[bx],al
    add bx,2

    push CURDATAMESSAGE2    
    push 1                      
    push 14                      
    call PRINTMESSAGE           
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

    jmp 0x1000:0x0000

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

; Data Section
MESSAGE1:    db 'MINT64 OS Boot Loader Start~!!', 0 
                                                    
                                                    
DISKERRORMESSAGE:       db  'DISK Error~!!', 0
CURDATAMESSAGE1:       db  'Current Data:', 0
CURDATAMESSAGE2:       db  '00/00/0000', 0

IMAGELOADINGMESSAGE:    db  'OS Image Loading...', 0
LOADINGCOMPLETEMESSAGE: db  'Complete~!!', 0


SECTORNUMBER:           db  0x03    
HEADNUMBER:             db  0x00    
TRACKNUMBER:            db  0x00    

times 510 - ( $ - $$ )    db    0x00    

db 0x55             
db 0xAA             
