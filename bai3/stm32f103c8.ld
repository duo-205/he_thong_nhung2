MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    RAM   (xrw) : ORIGIN = 0x20000000, LENGTH = 20K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
    /*
     * =========================
     * CODE + CONSTANT
     * =========================
     */
    .text :
    {
        . = ALIGN(4);

        KEEP(*(.isr_vector))

        *(.text*)
        *(.text.*)

        *(.rodata*)
        *(.rodata.*)

        /*
         * Các section do GCC/newlib
         * có thể tạo ra.
         */
        KEEP(*(.init))
        KEEP(*(.fini))

        . = ALIGN(4);

        _etext = .;
    } > FLASH


    /*
     * =========================
     * ARM exception tables
     * =========================
     */
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH


    .ARM.exidx :
    {
        __exidx_start = .;

        *(.ARM.exidx* .gnu.linkonce.armexidx.*)

        __exidx_end = .;
    } > FLASH


    /*
     * =========================
     * DATA
     *
     * Biến khởi tạo có giá trị.
     *
     * Lưu trong FLASH khi nạp,
     * sau đó startup copy sang RAM.
     * =========================
     */
    .data : AT(_etext)
    {
        . = ALIGN(4);

        _sdata = .;

        *(.data*)
        *(.data.*)

        . = ALIGN(4);

        _edata = .;
    } > RAM

    _sidata = LOADADDR(.data);


    /*
     * =========================
     * BSS
     *
     * Biến global/static
     * không khởi tạo.
     * =========================
     */
    .bss :
    {
        . = ALIGN(4);

        _sbss = .;

        *(.bss*)
        *(.bss.*)
        *(COMMON)

        . = ALIGN(4);

        _ebss = .;
    } > RAM


    /*
     * =========================
     * STACK
     * =========================
     */

    ._user_heap_stack :
    {
        . = ALIGN(8);

        . = . + 0x000;

        . = ALIGN(8);
    } > RAM


    /*
     * =========================
     * Loại bỏ các section
     * không cần thiết
     * =========================
     */

    /DISCARD/ :
    {
        *(.note.GNU-stack)
        *(.comment)
    }
}

