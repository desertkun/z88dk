
SECTION code_clib
SECTION code_l_sccz80

PUBLIC l_debug_pop_frame
PUBLIC l_debug_pop_frame_bc1
PUBLIC l_debug_pop_frame_bc2
PUBLIC l_debug_pop_frame_bc3
EXTERN __debug_framepointer

l_debug_pop_frame_bc3:
    pop     bc

l_debug_pop_frame_bc2:
    pop     bc

l_debug_pop_frame_bc1:
    pop     bc

l_debug_pop_frame:
    pop     bc      ;return code
    ex      (sp),hl ;hl=old frame pointer
    ld      (__debug_framepointer),hl
    pop     hl
    push    bc
    ret
