/* ----------------------------------------------- */
/* queue.h	definitions for mrm fifos

04 Oct 04 dpa	Created


/* ----------------------------------------------- */

typedef struct {
	unsigned char *in;
	unsigned char *out;
	unsigned char *buf;
	unsigned char *end;
	int size;
} queue_struct;


/* ----------------------------------------------- */
/* eof */
