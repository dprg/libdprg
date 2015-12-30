/* 
 * qsm_reg.h - map definitions for the m68k Queued Serial Module 
 *
 * History
 * 19 December 2004 - srainwater@ncc.com
 *  - minor tweaks
 *  - added this comment header
 * 25 October 2005 - srainwater@ncc.com
 *  - merged May 2005 dpa changes
 */
 

#define QSM_BASE (0xfffffc00L)

/*
 * QSM Global registers
 */

#define QSM_MCR		(*(short int *) (QSM_BASE))
#define QSM_TEST	(*(short int *) (QSM_BASE+0x02))
#define QSM_QILR_QIVR	(*(short int *) (QSM_BASE+0X04))
#define QSM_SCCR0       (*(short int *) (QSM_BASE+0x08))
#define QSM_SCCR1	(*(short int *) (QSM_BASE+0x0a))
#define QSM_SCSR	(*(short int *) (QSM_BASE+0x0c))
#define QSM_SCDR	(*(short int *) (QSM_BASE+0x0e))
#define QSM_PORTQS	(*(unsigned char *) (QSM_BASE+0x15))
#define QSM_PQSPAR_DDR	(*(short int *) (QSM_BASE+0x16))
#define QSM_SPCR0	(*(short int *) (QSM_BASE+0x18))
#define QSM_SPCR1	(*(short int *) (QSM_BASE+0x1a))
#define QSM_SPCR2	(*(short int *) (QSM_BASE+0x1c))
#define QSM_SPCR3_SPSR	(*(short int *) (QSM_BASE+0x1e))

/*
 * QSM Queue RAM
 */

#define QSM_RXRAM 	(QSM_BASE+0x100)
#define QSM_TXRAM	(QSM_BASE+0x120)
#define QSM_CMDRAM	(QSM_BASE+0x140)
