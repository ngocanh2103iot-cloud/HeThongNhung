ENTRY(Reset_Handler)


MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}


/*
 * Ngan xep o cuoi RAM
 */
_estack = ORIGIN(RAM) + LENGTH(RAM);


SECTIONS
{
    /*
     * Bang vector o dau FLASH
     */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH


    /*
     * Ma chuong trinh
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
     * Du lieu chua khoi tao
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
