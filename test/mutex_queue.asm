; _mutex_queue(mtx_idc, mtx_cntr) => return: subscribed index for the thread
mov   eax, 0x02   ; 0x02 is the initial value of the mutex indicator, the first entry thread is guaranteed to be passed
mod_chk:
mov   ebx, 0x00   ; 0x00 is the modification indicator, it shows that this mutex value is updating
xchg  ebx, [ecx]  ; atomic exchange the value of the mutex with ebx, the mutex indicator is set to "updating"
cmp   ebx, 0x00
je    mod_chk     ; if the mutex is being modified, wait until its complete
jmp   subs        ; skip the atomic load for checking the mutex subscription
ld_cntr:
mov   ebx, [ecx]  ; atomic load the mutex indicator
subs:
cmp   ebx, eax    ; check if this is the turn for the current thread to lock the mutex
je    pass
cmp   eax, 0x02
jne   ld_cntr     ; if the value of eax is 0x02, this thread have yet to subscribe to the mutex, else, it have
mov   r8d, [edx]  ; the mutex is under lock state, fetch the counter value
inc   r8d
mov   eax, r8d    ; subscribe to the current counter value
mov   [edx], r8d  ; increment the counter value, the initial value of the counter is 0x02
mov   [ecx], ebx  ; set the mutex indicator back to its initial value
jmp   ld_cntr
pass:
cmp   eax, 0x02
jne   end         ; the subsequent threads after the first thread does not have to set the mutex indicator
mov   r8d, 0x01
mov   [ecx], r8d  ; set the mutex indicator to "locked"
end:
ret

mov   eax, r8d
mod_chk:
mov   ebx, 0x00
xchg  ebx, [ecx]
cmp   ebx, 0x00
je    mod_chk
cmp   ebx, [edx]
je    unlck
inc   ebx
mov   [ecx], ebx
jmp   end
unlck:
mov   ebx, 0x02
mov   [ecx], ebx
end:
ret

mov   eax, 0x50
mov   [edx], eax
ret

mov   eax, 0x02
mod_chk:
mov   ebx, 0x00
xchg  ebx, [ecx]
cmp   ebx, 0x00
je    mod_chk
jmp   subs
ld_cntr:
mov   ebx, [ecx]
subs:
cmp   ebx, eax
je    pass
cmp   eax, 0x02
ja    ld_cntr
mov   eax, [edx]
inc   eax
mov   [edx], eax
mov   [ecx], ebx
jmp   ld_cntr
pass:
mov   ebx, 0x01
mov   [ecx], ebx
end:
ret

