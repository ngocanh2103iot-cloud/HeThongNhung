ENTRY(Reset_Handler)


MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}


/*
 * Stack nằm cuối RAM
 */
_estack = ORIGIN(RAM) + LENGTH(RAM);


SECTIONS
{
    /*
     * Vector Table
     * phải nằm đầu FLASH
     */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH


    /*
     * Program code
     */
    .text :
    {
        . = ALIGN(4);

        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)

        . = ALIGN(4);
    } > FLASH


    /*
     * .data chạy trong RAM
     * nhưng giá trị ban đầu lưu trong FLASH
     */
    .data :
    {
        . = ALIGN(4);

        _sdata = .;

        *(.data)
        *(.data*)

        . = ALIGN(4);

        _edata = .;
    } > RAM AT > FLASH


    /*
     * địa chỉ .data trong FLASH
     */
    _sidata = LOADADDR(.data);


    /*
     * Uninitialized data
     */
    .bss :
    {
        . = ALIGN(4);

        _sbss = .;

        *(.bss)
        *(.bss*)
        *(COMMON)

        . = ALIGN(4);

        _ebss = .;
    } > RAM
}