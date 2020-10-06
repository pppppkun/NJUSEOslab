section	.bss
	x	resb	20
	y	resb	20
	xLen	resb	1
	yLen	resb	1
	sum_1   resb    21
	sum_2   resb    21
	sum1_len    resb    1
	sum2_len    resb    1
	copy_len    resb    1
	sum	resb	21
	mu	resb	41

section	.data
	newLine	db	10
	space	db	0x40
    val db "Please input two numbers and split by ' ' ", 0x0a
    len equ $-val
    maxLen equ 19

section .text

global	main

exit:
    mov eax,1
    mov ebx,0
    int 80h
;; the param in ecx and edx is prepared before call this fuction
;; ecx: string you want to print
;; edx: the len of string
printf:
    mov eax,4
    mov ebx,1
    int 80h
    ret
printfn:
    mov	ecx,newLine
    mov	edx,1
    call printf
    ret
;; get len of x or y
strlen:
    mov byte[ebx], al
    ret
;; the param in ecx and edx is prepared before call this fuction
scanf:
    add	ecx,eax
    mov	edx,1
    mov eax,3
    mov ebx,0
    int 80h
    ret

;;eax:source head
;;ebx:source index，
;;ecx:target head
;;edx:target index
;;copy_len:len
copy:
    mov esi,eax
    mov al,byte[eax+ebx]
    mov byte[ecx+edx],al
    mov al,byte[copy_len]
    sub al,1
    mov byte[copy_len],al
    add ebx,1
    add edx,1
    cmp al,0
    mov eax,esi
    jg copy
    ret

;;copy_pre
;;eax:source index
;;ebx:source len
;;ecx:target index
;;edx:target len
copy_pre:
    push eax
    mov al,byte[ebx]
    mov byte[copy_len],al
    pop eax
    pusha
    mov ebx,0
    mov edx,0
    call copy
    popa
    mov al,byte[ebx]
    mov byte[edx],al
    ret
;;eax:param1
;;ebx:param2
;;dl:flag
;;return eax:result edx:flag
;; al = 0x30 + number bl = 0x30 + number 
;; so al = al + bl - 0x30
add_bit:
	sub	al,0x30
	add	al,bl
	add	al,dl
	mov	dl,0
	cmp	al,0x3a
	jb	no_in
	sub	al,10
	add	dl,1
    no_in:
    ret

;;eax sum1_index
;;ebx sum2_index
;;ecx sum len
;;dl flag
;;return 
calculate_sum:
	movzx eax,byte[sum1_len]
	movzx ebx,byte[sum2_len]
	mov	ecx,21
	mov	dl,0
cycle_sum:
    cmp ecx,0
    jl to_return
    push eax
    push ebx
    push ecx

    ;;x is end
    mov cl,0
    cmp eax,0
    jg no_end_x
    add cl,0x30
    sub cl,byte[sum_1+eax-1]
    no_end_x:
    add cl,byte[sum_1+eax-1]
    mov al,cl

    ;;y is end
    mov cl,0
    cmp ebx,0
    jg no_end_y
    sub cl,byte[sum_2+ebx-1]
    add cl,0x30
    no_end_y:
    add cl,byte[sum_2+ebx-1]
    mov bl,cl

    pop ecx

    call add_bit
    mov byte[sum+ecx-1],al
    sub ecx,1
    pop ebx
    sub ebx,1
    pop eax
    sub eax,1
    ;;jg = false jump true no jump
    judge:
        cmp eax,0
        jg cycle_sum
        whetherB:
        cmp ebx,0
        jg cycle_sum
        jp to_return
    to_return:
        ret


;对x进行按位循环:乘数x；被乘数y；位数21；eax-x位移（xLen）；ebx-y位移；ecx存储到sum_1的位移；edx-进位
;edx-下一位进位
mul_x:
    movzx eax,byte[xLen]
    mov ecx,21
    mov edx,0

    repeat_x_bit:
        ;判断x是否结束
        cmp eax,0
        jng al_zero
        push eax
        push ebx
        push ecx
        mov cl,byte[x+eax-1]
        sub cl,0x30
        mov eax,0
        mov al,cl
        mov_y:
            cmp ebx,0
            jng call_mul_bit
            mov cl,byte[y+ebx-1]
            sub cl,0x30
            mov ebx,0
            mov bl,cl

        call_mul_bit:
        call mul_bit

        pop ecx
        mov byte[sum_1+ecx-1],al
        pop ebx
        pop eax
        sub ecx,1
        cmp ecx,0
        je to_return
        cmp eax,0
        je  repeat_x_bit
        sub eax,1
        jmp repeat_x_bit

    al_zero:
        push eax
        push ebx
        push ecx
        jmp mov_y

;参数（寄存器传递）：eax-x位置的值(0~9)，ebx-y位置的值(0~9)，dl-上一位的进位(0~9)
;返回（寄存器）：eax-结果(0~9)，edx-进位(0~9)
mul_bit:
	mul	bl
	add	al,dl
	and edx,0
	check:
        cmp	al,10
        jnl cal_addin
	ret
    cal_addin:
        sub al,10
        add dl,1
        jmp check

;;
add_zero_for_sum:
    mov bl,byte[ecx+eax-1]
    add bl,0x30
    mov byte[ecx+eax-1],bl
    sub eax,1
    cmp eax,0
    jg add_zero_for_sum
    ret
;求和指定位并放回mul:被加数sum_1，21位；被加数（从mul中拷贝到sum_2）；ecx-mul的当前位置指针
mul_sum_bit:
    push ecx
    mov eax,21
    mov byte[copy_len],al
    mov eax,mu
    mov ebx,ecx
    mov ecx,sum_2
    mov edx,0
    call copy

    mov eax,21
    mov ecx,sum_1
    call add_zero_for_sum

    mov eax,21
    mov ecx,sum_2
    call add_zero_for_sum

    mov eax,21
    mov byte[sum1_len],al
    mov byte[sum2_len],al
    call calculate_sum
    mov eax,21
    remove_zero_for_sum:
        mov bl,byte[sum+eax-1]
        sub bl,0x30
        mov byte[sum+eax-1],bl
        sub eax,1
        cmp eax,0
        jg remove_zero_for_sum

    mov eax,21
    mov byte[copy_len],al
    mov eax,sum
    mov ebx,0
    mov ecx,mu
    pop edx
    call copy
    ret


main:

    pusha
    mov ecx, val
    mov edx, len
    call printf
    popa

	mov	eax,0
    ;; get param one by one
    ;; x is the first index of x
    ;; eax is the last index of x
    get_X:
        push eax
        mov	ecx,x
        call scanf
        pop eax
        ;;deal ' '
        cmp	byte[x+eax],32
        je	deal_last_x
        ;;
        add	al,1
        cmp	al,maxLen
        jg	getXLen
        jmp	get_X
    deal_last_x:
        mov ah,0
        mov	byte[x+eax],0
    getXLen:
        mov ebx, xLen
        call strlen
    mov	eax,0
    get_Y:
        push eax
        mov	ecx,y
        call scanf
        pop eax
        ;;deal ' '
        cmp	byte[y+eax],32
        je	get_Y
        ;;deal '\n'
        cmp	byte[y+eax],10
        je	deal_last_y
        ;;
        add	al,1
        cmp	al,maxLen
        jg	getYLen
        jmp	get_Y
    deal_last_y:
        mov	byte[y+eax],0
    getYLen:
        mov ebx, yLen
        call strlen
    before_sum:

        mov eax,x
        mov ebx,xLen
        mov ecx,sum_1
        mov edx,sum1_len
        call copy_pre

        mov eax,y 
        mov ebx,yLen 
        mov ecx,sum_2
        mov edx,sum2_len
        call copy_pre

        call calculate_sum
    ;;
    output_sum:
        pusha
        mov edx,ecx
        sub edx,21
        not edx
        add edx,1
        add ecx,sum
        call printf
        call printfn
        popa

	mov	eax,22
    init_bits:
        mov byte[sum_1+eax-1],0
        sub eax,1
        cmp eax,0
        jg init_bits

calculate_mul:
;被乘数y；位数21；ebx-y位移；ecx存储位移；edx-进位
        mov ebx,0
        mov	bl,byte[yLen]
        mov ecx,0
        mov	cl,20
        mov	edx,0

    ;对y进行按位循环
    mul_cycle_sum:
        cmp ecx,-1
        je form_output
        push ebx
        push ecx

        call mul_x

        pop ecx
        push ecx
        call mul_sum_bit
        pop ecx
        sub ecx,1
        pop ebx
        cmp ebx,0
        je mul_cycle_sum
        sub ebx,1
        jmp mul_cycle_sum


form_output:
;计算完成，将mul用0补位
    mov eax,42
    add_zero:
        mov bl, [mu+eax-1]
        add bl, 0x30
        mov [mu+eax-1],   bl
        sub eax,    1
        cmp eax,    0
        jg  add_zero

    output_mul:
        mov ecx,mu
        sub ecx,1
        mov edx,1
    deleteZero:
        add ecx,1
        cmp ecx,mu+41
        je end
        cmp byte[ecx],0x30
        je judgeZero
        mov esi,1
    print:
        call printf
        jmp deleteZero
    end:
        call printfn
        call exit
    judgeZero:
        cmp esi,1
        je print
        jmp deleteZero

    ;; output_mul:
    ;;     mov	ecx,mu
    ;;     sub ecx,1
    ;;     mov	edx,1
    ;; deleteZero:
    ;;     add ecx,1
    ;;     cmp ecx,mu+41
    ;;     je end
    ;;     cmp byte[ecx],0x30
    ;;     je judgeZero
    ;; print:
    ;;     call printf
    ;;     jmp deleteZero
    ;; end:
    ;;     call printfn
    ;;     mov eax,1
    ;;     mov ebx,0
    ;;     int 80h
    ;; judgeZero:
    ;;     cmp ecx,mu+40
    ;;     je print
    ;;     jmp deleteZero
