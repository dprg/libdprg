/* This include file is part of the newlib 1.12.0 package */
/*
 * These constants are for the M68332BCC's boot monitor.  They
 * are used with a TRAP 15 call to access the monitor's I/O routines.
 * they must be in the word following the trap call.
 */
	INCHR=0X0
	INSTAT=0X1
	INLN=0X2
	READSTR=0X3
	READLN=0X4
	CHKBRK=0X5

	OUTCHR=0X20
	OUTSTR=0X21
	OUTLN=0X22
	WRITE=0X23
	WRITELN=0X24
	WRITDLN=0X25
	PCRLF=0X26
	ERASELN=0X27
	WRITD=0X28
	SNDBRK=0X29

	TM_INI=0X40
	TM_STR0=0X41
	TM_RD=0X42
	DELAY=0X43

	RETURN=0X63
	BINDEC=0X64

	CHANGEV=0X67
	STRCMP=0X68
	MULU32=0X69
	DIVU32=0X6A
