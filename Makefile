# ------------------------------------------------------------------------
# makefile for libdprg
# ------------------------------------------------------------------------


comp=m68k-elf-gcc -c -mcpu32 -isystem include

link=m68k-elf-ld -L/opt/gcc4-mrm/m68k-elf/lib/mcpu32

linkopt=

# ------------------------------------------------------------------------

# C startup and exit
sysobjs = 

# libdprg.a 
libobjs =  cpu32bug.o led.o mrm_crt0.o read.o sysclock.o write.o \
           analog.o cpu_init.o iochar.o lcd.o queue.o sci.o system_init.o \
           task.o tpu.o servo.o pwm_nmi0010.o fqd.o

# ------------------------------------------------------------------------

all: libfiles

clean:	
	rm -f *.o  libdprg.a

%.o:%.S
	$(comp) $<

%.o:%.c
	$(comp) $<

# ------------------------------------------------------------------------

libfiles:	$(libobjs) $(sysobjs)
		ar -r libdprg.a $(libobjs)

docs:
		doxygen libdprg.cfg

# ------------------------------------------------------------------------
# eof
