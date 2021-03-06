
    SECTION code_driver
    SECTION code_driver_character_input

    PUBLIC asm_asci0_flush_Rx

    EXTERN asci0RxCount, asci0RxIn, asci0RxOut, asci0RxBuffer

    asm_asci0_flush_Rx:

        xor a
        ld (asci0RxCount), a        ; reset the Rx counter (set 0)  		

        ld hl, asci0RxBuffer        ; load Rx buffer pointer home
        ld (asci0RxIn), hl
        ld (asci0RxOut), hl

        ret

    EXTERN asm_asci0_need
    defc NEED = asm_asci0_need
