MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    RAM   (xrw) : ORIGIN = 0x20000000, LENGTH = 20K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
    /* =====================================================
     * CODE + READ ONLY DATA
     * ===================================================== */

    .text :
    {
        . = ALIGN(4);

        KEEP(*(.isr_vector))

        *(.text*)
        *(.rodata*)

        . = ALIGN(4);

        /*
         * ARM exception handling tables.
         * Phải nằm trong FLASH trước khi tính _etext.
         */
        *(.ARM.extab*)
        *(.ARM.exidx*)

        . = ALIGN(4);

        _etext = .;
    } > FLASH


    /* =====================================================
     * INITIALIZED DATA
     * ===================================================== */

    .data :
    {
        . = ALIGN(4);

        _sdata = .;

        *(.data*)

        . = ALIGN(4);

        _edata = .;
    } > RAM AT > FLASH

    _sidata = LOADADDR(.data);


    /* =====================================================
     * BSS
     * ===================================================== */

    .bss :
    {
        . = ALIGN(4);

        _sbss = .;

        *(.bss*)
        *(COMMON)

        . = ALIGN(4);

        _ebss = .;
    } > RAM


    /* =====================================================
     * END OF RAM
     * ===================================================== */

    . = ALIGN(4);

    end = .;
    _end = .;


    /* =====================================================
     * DISCARD
     * ===================================================== */

    /DISCARD/ :
    {
        *(.note.GNU-stack)
    }
}
