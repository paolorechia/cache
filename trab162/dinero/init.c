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
**	Source File:	init.c
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
 * 19/2/92 inserted conditional compilation for the textual output
 *         (ie. banners) which are then written to a file called either
 *         "fst.stats" for first level-caches, or "snd.stats" for
 *         second-level caches.  The conditional compilation flags
 *         are defined in the Makefile. *** changed -- see below ***
 *         All the stuff for the IBM 370 has been removed.
 * 26/2/92 added new command line option (-Ofile) to allow for statistics
 *         to be written to the given file; all the output not written
 *         to this file is sent to stderr.
 * 27/2/92 added command line option -o4 so that bus output is identical
 *         to the input format.  This makes changes to [fst|snd]-level
 *         caches no longer necessary.
 * 17/3/92 added multiprogramming & context switch special input record.
 * 10/7/92 changed initial value of tags so that XFLUSH+XSWITCH won't
 *         match good tags.
 ************************************************************************/


#include "global.h"

void exit(int status);
void *calloc(size_t nmemb, size_t size);


init(argc,argv,			/* Initialize. */
		cachep,policyp,ctrlp,metricp)
int argc;			/* <  */
char *argv[];			/* <  */
CACHETYPE *cachep;		/* <> */
POLICYTYPE *policyp;		/* <> */
CTRLTYPE *ctrlp;		/* <> */
METRICTYPE *metricp;		/* <> */
/* returns: OK
*/

{
extern int getcmdargs();
extern int echocmdargs();

int flag;
/*
**	Get command line arguments and echo them. Illegal entries
**	cause premature termination.
*/
flag = getcmdargs(argc,argv,cachep,policyp,ctrlp);
/*!* echocmdargs(cachep,policyp,ctrlp); *!*/

if (flag != OK) {		/* premature termination */
	fprintf(stderr, "dinero: execution prematurely terminated.\n");
	exit(ERR);
 }
/*
**	Initialize
*/
initmisc(cachep,policyp,ctrlp);	/* Init "set" params. */
initstacks(cachep);		/* Init priority stacks */
initmetric(metricp);		/* Init measurement stuff */

currpid = 0;

} /* ****************************************************************** */



initmisc(cachep,policyp,ctrlp)          /* Set up "set" params. */
CACHETYPE *cachep;              /* <>  */
POLICYTYPE *policyp;            /* <  */
CTRLTYPE *ctrlp;                /* <>  */
/* affects: Cachep->numIsets,numUorDsets,numsets.
** returns: OK
*/
{
extern int readfrominputstream();
extern int init_addrstack();
int theblocksize;
int label,addr;

theblocksize = cachep->blocksize;

if (cachep->Isize > 0) { /* I-Cache */
	cachep->numIsets = ( cachep->Isize / theblocksize) / cachep->assoc;
 }
else { /* No I-Cache */
cachep->numIsets = 0;
 }

if (cachep->Dsize > 0) { /* D-Cache */
	cachep->numUorDsets = ( cachep->Dsize / theblocksize) / cachep->assoc;
 }
else { /* U-Cache */
	cachep->numUorDsets = ( cachep->Usize / theblocksize) / cachep->assoc;
 }
cachep->numsets = cachep->numIsets + cachep->numUorDsets;

/*
**	Compute transfer size and prefetch displacement
**	policyp->prefetchdist is in (sub)-blocks whereas
**	cachep->prefetchdisplacement is in bytes.
*/
if (cachep->subblocksize==0) {
	cachep->transfersize = cachep->blocksize;
 }
else {
	cachep->transfersize = cachep->subblocksize;
 }
cachep->prefetchdisplacement = cachep->transfersize * policyp->prefetchdist;
	

/*
**	Initialize addrstack
*/
init_addrstack();

/*
**	Skip over trace addresses?
*/
ctrlp->tracecount = 0 - ctrlp->skipcount;

while (ctrlp->tracecount < 0) {
  readfrominputstream(&label,&addr,ctrlp);
}

} /* ****************************************************************** */



initstacks(cachep)			/* Set up priority stacks */
register CACHETYPE *cachep;		/* <  */
/* affects: stack, stack[i].tag,aux,next, 
**	    freelist.tag,aux,next, & bufferindex 
** returns: OK
*/
/*
**	WARNING: stack, etc. are GLOBAL variables.
*/
{
  // extern char *calloc();
extern STACKNODETYPE *stack;	/* global ptr to top of stacks */
extern int bufferindex;		/* global index to buffer */
extern STACKNODETYPE freelist;	/* List head for free list */
extern int numnodes;		/* Count on storage allocated */
register int i;
register int numberofsets;

unsigned int unsignednumofsets;

numberofsets = cachep->numsets;
/*
**	Get block for top of stacks. "stack" is a global variable.
*/
unsignednumofsets = numberofsets;
stack = (STACKNODETYPE *)calloc(unsignednumofsets,SIZEOFSTACKNODETYPE);

/*
**	Zero TOSs.
*/
for (i = 0; i < numberofsets; i++) {
	stack[i].tag = -2;
	stack[i].next = NULL;
 }
/*
**	Zero freelist; init bufferindex
*/
freelist.tag = 0;
freelist.next = NULL;
bufferindex = BUFFERSIZE;
numnodes = 0;

} /* ****************************************************************** */



initmetric(metricp)		/* Init performance variables */
METRICTYPE *metricp;			/* <> */
/* affects: 
** returns: OK
*/
{
int i;

metricp->Icount = metricp->Rcount = 0;
metricp->bus_traffic_in = metricp->bus_traffic_out = 0;

for (i = 0; i < (PREFETCH + NUMACCESSTYPES); i++) {
	metricp->fetch[i] = metricp->miss[i] = metricp->blockmiss[i] = 0;
 }

} /* ****************************************************************** */

