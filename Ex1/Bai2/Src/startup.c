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
 * Cortex-M3 Vector Table
 */
__attribute__((section(".isr_vector")))
const isr_handler_t vector_table[] =
{
    (isr_handler_t)&_estack,    /* Initial Stack Pointer */
    Reset_Handler,              /* Reset */
    Default_Handler,            /* NMI */
    Default_Handler,            /* HardFault */
    Default_Handler,            /* MemManage */
    Default_Handler,            /* BusFault */
    Default_Handler,            /* UsageFault */

    0,
    0,
    0,
    0,

    Default_Handler,            /* SVCall */
    Default_Handler,            /* Debug Monitor */
    0,
    Default_Handler,            /* PendSV */
    Default_Handler             /* SysTick */
};


void Reset_Handler(void)
{
    uint32_t *src;
    uint32_t *dst;

    /*
     * Copy .data từ FLASH -> RAM
     */
    src = &_sidata;
    dst = &_sdata;

    while (dst < &_edata)
    {
        *dst++ = *src++;
    }

    /*
     * Clear .bss
     */
    dst = &_sbss;

    while (dst < &_ebss)
    {
        *dst++ = 0;
    }

    /*
     * Run application
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