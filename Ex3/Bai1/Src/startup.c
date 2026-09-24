typedef unsigned int uint32_t;

extern uint32_t _estack;
extern uint32_t _sidata;

extern uint32_t _sdata;
extern uint32_t _edata;

extern uint32_t _sbss;
extern uint32_t _ebss;

extern int main(void);

void Reset_Handler(void);
void Default_Handler(void);

typedef void (*isr_handler_t)(void);


/*
 * Bang vector Cortex-M3
 */
__attribute__((section(".isr_vector")))
const isr_handler_t vector_table[] =
{
    (isr_handler_t)&_estack,    /* Con tro ngan xep dau */
    Reset_Handler,              /* Dat lai */
    Default_Handler,            /* Ngat khong che */
    Default_Handler,            /* Loi nghiem trong */
    Default_Handler,            /* Loi bo nho */
    Default_Handler,            /* Loi bus */
    Default_Handler,            /* Loi su dung */

    0,
    0,
    0,
    0,

    Default_Handler,            /* Goi dich vu */
    Default_Handler,            /* Giam sat go loi */
    0,
    Default_Handler,            /* Dich vu cho */
    Default_Handler             /* Nhip he thong */
};


void Reset_Handler(void)
{
    uint32_t *src;
    uint32_t *dst;

    /*
     * Chep .data tu FLASH sang RAM
     */
    src = &_sidata;
    dst = &_sdata;

    while (dst < &_edata)
    {
        *dst++ = *src++;
    }

    /*
     * Xoa .bss
     */
    dst = &_sbss;

    while (dst < &_ebss)
    {
        *dst++ = 0;
    }

    /*
     * Chay ung dung
     */
    main();

    while (1)
    {
    }
}


void Default_Handler(void)
{
    while (1)
    {
    }
}
