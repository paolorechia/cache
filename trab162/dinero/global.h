/************************************************************************
**
**	DineroX Cache Simulator
**
**      Adapted from DineroIII (similar to Version 3.1, Released 8/7/85)
**      by Roberto Hexsel,
**      Dept. of Computer Science
**      Edinburgh University
**      rh@dcs.ed.ac.uk
**
**	Mark D. Hill
**	Computer Sciences Dept.
**	Univ. of Wisconsin
**	Madison, WI 53706
**	markhill@cs.wisc.edu
**
**	Developed DineroIII While Affiliated With:
**
**	Computer Science Division
**	University of California
**	Berkeley, California 94720
**
**	Source File:	global.h
**
************************************************************************/

/* 
**  Copyright 1985, 1989 Mark D. Hill (DineroIII)
**  Copyright 1992 Roberto A. Hexsel (DineroX)
**
**  Permission to use, copy, modify, and distribute this
**  software and its documentation for any purpose and without
**  fee is hereby granted, provided that the above copyright
**  notice appear in all copies.  Mark D. Hill and Roberto A. Hexsel
**  make no representations about the suitability of this software
**  for any purpose.  It is provided "as is" without expressed
**  or implied warranty.
*/  


/************************************************************************
 * CHANGES:
 * 19/2/92 all the stuff for the IBM 370 has been removed.
 * 26/2/92 added new command line option (-Ofile) to allow for statistics
 *         to be written to the given file; all the output not written
 *         to this file is sent to stderr.
 * 27/2/92 added command line option -o4 so that bus output is identical
 *         to the input format.  This makes changes to [fst|snd]-level
 *         caches no longer necessary.
 * 17/3/92 added multiprogramming & context switch special input record.
 ************************************************************************/

/*	Enable fast, implemenation-dependent input character 
**	interpretation.  Comment-out "define" to disable.
*/
#define FAST_BUT_DANGEROUS_INPUT

#include <stdio.h>

/*
**	Make global variables declarations just "extern" declarations
**	as long as PUBLIC is not defined. File "main.c" will define
**	PUBLIC to be null before including this file so that the 
**	global variables are declared once and only once.
*/	
#ifndef PUBLIC
#define PUBLIC extern
#endif

/* 
**	Global flags 
*/
#define OK		0
#define	ERR		1
#define ILLEGAL 	'?'
#define ILLEGALNUM 	-1
#define TRUE            1
#define FALSE           0

/*
**	Major structures of data structure 
*/
/*
**	CACHE
*/
typedef struct cachetype {		/* CACHE holds cache parameters */
	int blocksize;
	int subblocksize;
	int transfersize;
	int Usize;
	int Isize;
	int Dsize;
	int assoc;
	int numIsets;
	int numUorDsets;
	int numsets;
	int prefetchdisplacement;
} CACHETYPE;
#define MAXNUMSUBBLOCKS	 32	/* Number of bits in an "int." */

/*
**	POLICY
*/
typedef struct policytype {		/* POLICY holds policies */
	char replacement;
	char fetch;
	int  prefetchdist;
	int  abortprefetchpercent;
	char write;
	char writeallocate;
} POLICYTYPE;
#define LRU		'l'	/* replacement policies */
#define FIFO		'f'
#define RANDOM		'r'

#define DEMAND		'd'	/* fetch policies */
#define ALWAYSPREFETCH	'a'
#define LOADFORWARDPREFETCH 'l'
#define SUBBLOCKPREFETCH 'S'
#define MISSPREFETCH 	 'm'
#define TAGGEDPREFETCH   't'


#define WRITETHROUGH	'w'	/* write policies */
#define COPYBACK	'c'
#define NOWRITEALLOCATE	'n'
#define WRITEALLOCATE	'w'

#define MAXINTPERCENT	21474836	/* (2^31)/100 */

/*
** 	CTRL
*/

#define MAXNAMELENGHT 81                /* length of output file name     */

typedef struct ctrltype {		/* CTRL holds flags to turn       */
					/* control options on and off     */
	int debug;
	int output;
	int skipcount;
	int maxcount;
	int tracecount;
	int Q;
	FILE *ofp;
        char outfilename[MAXNAMELENGHT];
} CTRLTYPE;

#define NODEBUG		0	/* Debug options */
#define DEBUG1		1
#define DEBUG2		2

#define TERSE		0	/* Output options */
#define VERBOSE		1
#define BUS		2
#define BUS_SNOOP	3
#define BUS_TRANSP	4

/*
**	METRIC
*/
#define NUMACCESSTYPES	6
#define PREFETCH	10
typedef struct metrictype {		/* METRIC holds cache performance */
					/* numbers			  */
	int fetch[PREFETCH+NUMACCESSTYPES];
	int miss[PREFETCH+NUMACCESSTYPES];
	int blockmiss[PREFETCH+NUMACCESSTYPES];
	int bus_traffic_in;
	int bus_traffic_out;
	int Icount;
	int Rcount;
} METRICTYPE;

#define	BUSREAD		'r'
#define BUSWRITE	'w'
#define BUSPREFETCH	'p'
#define BUSSNOOP	's'
#define BUSCONTROL      'c'

/* 
** Internal Accesstypes
**
** E.g. fetch[2] holds the number of instrn demand fetches,
** and fetch[12] holds the number of instrn prefetch fetches.
*/
#define	XREAD		0
#define	XWRITE		1
#define	XINSTRN		2
#define XMISC		3
#define	XFLUSH		4
#define	XSWITCH		5

#define DEMANDFETCHP(type)	((type) < PREFETCH)

/*
**	DAP
*/
typedef struct decodedaddrtype {	/* DECODEADDR is the type used   */
	int address;			/* to pass around analyzed addrs */
	int blockaddr;
	int accesstype;
	int tag;
	int set;
	int block;
	int subblocknum;
	int pid;
	int validbit;
} DECODEDADDRTYPE;

/*
**	STACKNODE: stack[i] is is priority stack for i-th set.
*/
typedef struct stacknodetype {		/* STACKNODE is the type used  */
	int tag;			/* for nodes in priority stacks */
	int blockaddr;
	int valid;
	int reference;
	int dirty;
	int pid;
	struct stacknodetype *next;
} STACKNODETYPE;
#define SIZEOFSTACKNODETYPE	sizeof(STACKNODETYPE)
#define VALID		1
#define NOTVALID	0
#define LOWVALIDMASK    0x00000001
#define NOTREFERENCED   0
#define NOTDIRTY   0
#define LOWDIRTYMASK    0x00000001

/* #define BUFFERSIZE    64 */		/* num of elements in malloc buffer */
/* #define FREETHRESHOLD 64 */	        /* Number to free at a time */
/* Modified to save space for simlating large caches */
#define BUFFERSIZE	4096		/* num of elements in malloc buffer */
#define FREETHRESHOLD	16		/* Number to free at a time */

#define WORDSIZE	4	        /* 4 bytes */


/*
** 	Addresses are read first from this stack and then from the
**	input trace.  This stack is used for prefetch address and
**	(maybe) for virtual memory translation.
*/
typedef struct addrstacktype {
	int label;
	int address;
 } ADDRSTACKTYPE;
#define MAXNUMADDRSTACK	65

/*
**	MULTIPROGRAMMING (multitracing)
*/
#define MAXNUMPIDS 8            /* maximum number of processes/trace streams */

/*
**	GLOBAL VARIABLES
*/
PUBLIC int bufferindex;			/* global index to buffer */
PUBLIC int numnodes;			/* Count on storage allocated */
PUBLIC STACKNODETYPE freelist;		/* List head for free list */
PUBLIC STACKNODETYPE *stack;		/* global ptr to top of stacks */

PUBLIC ADDRSTACKTYPE addrstack[MAXNUMADDRSTACK]; /* addr of be processed */
PUBLIC int size_addrstack;

PUBLIC int currpid;                     /* holds pid of running process */
