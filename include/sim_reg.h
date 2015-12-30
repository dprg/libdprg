/* 
 * sim_reg.h - map definitions for the m68k System Integration Module
 *
 * History
 * 19 December 2004 - srainwater@ncc.com
 *  - minor tweaks
 *  - added this comment header
 */
 

#define SIM_BASE (0xfffffa00L)

/*
 * SIM Global registers
 */

#define SIM_MCR		(*(unsigned short int *) (SIM_BASE_0x00))
#define SIM_SIMTR	(*(unsigned short int *) (SIM_BASE+0x02))
#define SIM_SYNCR	(*(unsigned short int *) (SIM_BASE+0X04))
#define SIM_RSR		(*(unsigned char *) (SIM_BASE+0X07))
#define SIM_SIMTRE	(*(unsigned short int *) (SIM_BASE+0X08))
#define SIM_PORTE	(*(unsigned char *) (SIM_BASE+0X11))
#define SIM_DDRE	(*(unsigned char *) (SIM_BASE+0X15))
#define SIM_PEPAR	(*(unsigned char *) (SIM_BASE+0X17))
#define SIM_PORTF	(*(unsigned char *) (SIM_BASE+0X19))
#define SIM_DDRF	(*(unsigned char *) (SIM_BASE+0X1d))
#define SIM_PFPAR	(*(unsigned char *) (SIM_BASE+0X1f))
#define SIM_SYPCR	(*(unsigned char *) (SIM_BASE+0X21))
#define SIM_PICR	(*(unsigned short int *) (SIM_BASE+0X22))
#define SIM_PITR	(*(unsigned short int *) (SIM_BASE+0X24))
#define SIM_SWSR	(*(unsigned char *) (SIM_BASE+0X27))
#define SIM_TSTMSRA	(*(unsigned short int *) (SIM_BASE+0X30))
#define SIM_TSTMSRB	(*(unsigned short int *) (SIM_BASE+0X32))
#define SIM_TSTSCA	(*(unsigned char *) (SIM_BASE+0X34))
#define SIM_TSTSCB	(*(unsigned char *) (SIM_BASE+0X35))
#define SIM_TSTRC	(*(unsigned short int *) (SIM_BASE+0X36))
#define SIM_CREG	(*(unsigned short int *) (SIM_BASE+0X38))
#define SIM_DREG	(*(unsigned short int *) (SIM_BASE+0X3a))
#define SIM_CSPDR	(*(unsigned char *) (SIM_BASE+0X41))
#define SIM_CSPAR0	(*(unsigned char *) (SIM_BASE+0X44))
#define SIM_CSPAR1	(*(unsigned short int *) (SIM_BASE+0X46))
#define SIM_CSBARBT	(*(unsigned short int *) (SIM_BASE+0X48))
#define SIM_CSORBT	(*(unsigned short int *) (SIM_BASE+0X4a))
#define SIM_CSBAR0	(*(unsigned short int *) (SIM_BASE+0X4c))
#define SIM_CSOR0	(*(unsigned short int *) (SIM_BASE+0X4e))
#define SIM_CSBAR1	(*(unsigned short int *) (SIM_BASE+0X50))
#define SIM_CSOR1	(*(unsigned short int *) (SIM_BASE+0X52))
#define SIM_CSBAR2	(*(unsigned short int *) (SIM_BASE+0X54))
#define SIM_CSOR2	(*(unsigned short int *) (SIM_BASE+0X56))
#define SIM_CSBAR3	(*(unsigned short int *) (SIM_BASE+0X58))
#define SIM_CSOR3	(*(unsigned short int *) (SIM_BASE+0X5a))
#define SIM_CSBAR4	(*(unsigned short int *) (SIM_BASE+0X5c))
#define SIM_CSOR4	(*(unsigned short int *) (SIM_BASE+0X5e))
#define SIM_CSBAR5	(*(unsigned short int *) (SIM_BASE+0X60))
#define SIM_CSOR5	(*(unsigned short int *) (SIM_BASE+0X62))
#define SIM_CSBAR6	(*(unsigned short int *) (SIM_BASE+0X64))
#define SIM_CSOR6	(*(unsigned short int *) (SIM_BASE+0X66))
#define SIM_CSBAR7	(*(unsigned short int *) (SIM_BASE+0X68))
#define SIM_CSOR7	(*(unsigned short int *) (SIM_BASE+0X6a))
#define SIM_CSBAR8	(*(unsigned short int *) (SIM_BASE+0X6c))
#define SIM_CSOR8	(*(unsigned short int *) (SIM_BASE+0X6e))
#define SIM_CSBAR9	(*(unsigned short int *) (SIM_BASE+0X70))
#define SIM_CSOR9	(*(unsigned short int *) (SIM_BASE+0X72))
#define SIM_CSBAR10	(*(unsigned short int *) (SIM_BASE+0X74))
#define SIM_CSOR10	(*(unsigned short int *) (SIM_BASE+0X76))

