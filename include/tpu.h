/* 
 * tpu.h - TPU definitions
 *
 * History
 * 25 October 2005 - srainwater@ncc.com
 *  - minor tweaks
 *  - added this comment header
 *  - added tpu_qom_ram struct
 *  - merged May 2005 dpa changes
 */

/* base address of TPU registers on MRM332 */
#define TPU_BASE (0xfffffe00L)
/* TPU register map */
#define TPU_TMCR	(*(short int *) (TPU_BASE))
#define TPU_TTCR	(*(short int *) (TPU_BASE+0x02))
#define TPU_DSCR     	(*(short int *) (TPU_BASE+0X04))
#define TPU_DSSR        (*(short int *) (TPU_BASE+0x06))
#define TPU_TICR 	(*(short int *) (TPU_BASE+0x08))
#define TPU_CIER	(*(short int *) (TPU_BASE+0x0a))
#define TPU_CFSR0	(*(short int *) (TPU_BASE+0x0c))
#define TPU_CFSR1 	(*(short int *) (TPU_BASE+0x0e))
#define TPU_CFSR2     	(*(short int *) (TPU_BASE+0x10))
#define TPU_CFSR3	(*(short int *) (TPU_BASE+0x12))
#define TPU_HSQR0	(*(short int *) (TPU_BASE+0x14))
#define TPU_HSQR1	(*(short int *) (TPU_BASE+0x16))
#define TPU_HSRR0     	(*(short int *) (TPU_BASE+0x18))
#define TPU_HSRR1     	(*(short int *) (TPU_BASE+0x1a))
#define TPU_CPR0      	(*(short int *) (TPU_BASE+0x1c))
#define TPU_CPR1      	(*(short int *) (TPU_BASE+0x1e))
#define TPU_CISR      	(*(short int *) (TPU_BASE+0x20))
#define TPU_LR      	(*(short int *) (TPU_BASE+0x22))
#define TPU_SGLR      	(*(short int *) (TPU_BASE+0x24))
#define TPU_DCNR      	(*(short int *) (TPU_BASE+0x26))
#define TPU_RAM   	(TPU_BASE+0x100)

struct tpu_qom_ram
{
    volatile unsigned char ref_addr_b;
    volatile unsigned char last_off_addr_a;
    volatile unsigned char loop_cnt;
    volatile unsigned char off_ptr_c;
    volatile unsigned short offset_1;
    volatile unsigned short offset_2;
    volatile unsigned short offset_3;
    volatile unsigned short offset_4;
    volatile unsigned short offset_5;
};

/* TPU timer/counter 1 ticks per second at 16.778 MHz clock */
/* #define TCR1_CLOCK 4194500 */
/* TPU timer/counter 1 ticks per second at 26.166 MHz clock */
#define TCR1_CLOCK 6291500
