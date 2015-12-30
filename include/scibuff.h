

#define scivec 0x56
#define qsize  160
#define RX     0
#define TX     1

typedef unsigned short int word;

typedef struct {
  int in;
  int out;
  char q[qsize+10];
} queue_struct;

void qinit(void);
int  qstat(int que);
void sciinit(long int baud);
long int  get_baud(void);
void sci_handler(void);
void sci_int(void);
char  inbyte(void);
void outbyte(char b);
int  havebyte(void);
int  sendingbyte(void);
int  get_sci_status(void);
void clear_sci_status(void);
