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
**	Source File:	fetch.c
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
 * 20/2/92 removed IBM 370 stuff;
 *         changed error output to stderr
 *         changed output to file if [FST | SND]LEVEL cache defined
 * 27/2/92 added command line option -o4 so that bus output is identical
 *         to the input format.  This makes changes to [fst|snd]-level
 *         caches no longer necessary.
 * 17/3/92 added multiprogramming & context switch special input record.
 * 25/3/92 bug fix: on a context switch, flush cache...
 * 7/3/92  on reading a context switch input record, the cache is flushed
 *         only if the write policy is copy-back.
 * 26/4/92 in breakupaddr(), on XFLUSH and XSWITCH, the tag is set to -1
 *         to make tag comparisons to always fail.
 ************************************************************************/

#include "global.h"

void exit(int status);


fetch(cachep,ctrlp,metricp,policyp,dap)/* Get addr from input stream & decode*/

CACHETYPE *cachep;			/* <  */
CTRLTYPE *ctrlp;			/* <  */
METRICTYPE *metricp;			/* <> */
POLICYTYPE *policyp;                    /* <  */
register DECODEDADDRTYPE *dap;		/*  > */

/* affects: many things indirectly
** returns: EOF or ~EOF
*/
{
  int addr;
  int thelabel;
  static int flushcount = -2;	/* No of addr since last cache flush,
					-1=>no flushing */

  if (flushcount==-2) {		/* Initialize */
    flushcount = ((ctrlp->Q<1) ? -1 : 0); /* flush? */
  }	/* end init */

  if ((flushcount>=0) && (flushcount++ >= ctrlp->Q)) { 
    flushcount=1;
    flushcache(cachep,ctrlp,metricp);
  }

  if (readfrominputstream(&thelabel,&addr,ctrlp) == EOF) {
    return(EOF);
  } 
  else {
    switch (thelabel) {
    case XFLUSH:
      flushcache(cachep,ctrlp,metricp);
      break;
    case XSWITCH:
      if (policyp->write == COPYBACK) {
	flushcache(cachep,ctrlp,metricp);
      }
      currpid = addr;
      break;
    default:
      break;
    }
    breakupaddr(addr,thelabel,dap,cachep);

    /* ******* debug -D ***** */
    /* dumpaddr(0, dap, ctrlp);	*/
    return(~EOF);
  }

} /* ********************************************************************* */


prefetch(				
	cachep,policyp,dap,miss,stackptr)
CACHETYPE *cachep;			/* <  */
POLICYTYPE *policyp;			/* <  */
register DECODEDADDRTYPE *dap;		/* < */
int miss;
STACKNODETYPE *stackptr;


/* 
**  This routine decides whether to start a prefetch access.
**  If so the prefetch is pushed on the addrstack to be read
**  instead of the next address trace address.
*/
{
int prefetchaddr;
extern long random();

/*
**  See if prefetch needs to be aborted (because of data reference 
**  interference.  Note: 0 <= random() <= 2^31-1 and
**  0 <= random()/MAXINTPERCENT < 100.
*/
if (policyp->abortprefetchpercent > 0 ) {
	if (random()/MAXINTPERCENT < policyp->abortprefetchpercent) {
		return;
	}
 }

/*
** Calculate address to prefetch
*/
prefetchaddr = dap->address + cachep->prefetchdisplacement;

/*
** Switch on prefetch policy
*/
switch (policyp->fetch) {

case ALWAYSPREFETCH:
	push_addrstack((PREFETCH+dap->accesstype),prefetchaddr);
	break;

case LOADFORWARDPREFETCH:
	/*
	** Don't prefetch into next block.
	*/
	if (
	  (dap->address/cachep->blocksize)==(prefetchaddr/cachep->blocksize)
		) {
		push_addrstack((PREFETCH+dap->accesstype),prefetchaddr);
	}
	break;

case SUBBLOCKPREFETCH:
	/*
	** Don't prefetch into next block; wrap around within block instead.
	*/
	if (
	   (dap->address/cachep->blocksize)!=(prefetchaddr/cachep->blocksize)
		) {
		prefetchaddr = prefetchaddr - cachep->blocksize;
	}
	push_addrstack((PREFETCH+dap->accesstype),prefetchaddr);
	break;

case MISSPREFETCH:
	if (miss) {
		push_addrstack((PREFETCH+dap->accesstype),prefetchaddr);
	}
	break;

/* 
**      Tagged prefetch (see Smith, "cache Memories," ~p.20) initiates 
**      a prefetch on the first demand reference to a (sub)-block.  Thus, 
**      a prefetch is initiated on a demand miss or the first demand 
**      reference to a (sub)-block that was brought into the cache by a 
**      prefetch. 
** 
**      Tagged prefetching is implemented using demand reference bits 
**      that are active only when tagged prefetching is selected.  A 
**      prefetch is started on a demand miss and on a refernce to a 
**      (sub)-block whose reference bit was not previously set.
*/
case TAGGEDPREFETCH:
	if ((miss) || ((dap->validbit & stackptr->reference)==0)) {
		push_addrstack((PREFETCH+dap->accesstype),prefetchaddr);
	}
	break;

case DEMAND:
default :
	fprintf(stderr,"dinero: error in prefetch policy:%c\n",policyp->fetch);
	exit(ERR);
 }

} /* ********************************************************************* */



readfrominputstream(labelp,addrp,ctrlp)	/* get next addr */
int *labelp;
int *addrp;
CTRLTYPE *ctrlp;
{
register int readflag;
char linebuffer[255];
char *gets();	/* Gets chars from stdin until a newline;
		** changes newline to nullchar; returns ptr
		** to the sting.
		*/

/* 
**	Read in ASCII from standard input 
**	Expect ONE label and addr in hex per line.
**	Rest of data input line is ignored so it may
**	be used for comments.
**
**	WARNING: If more than one tuple is put on a line,
**	all but the first tuple will be ignored.
*/

/*
**	Get tuple from addrstack if any are there.
*/
if (pop_addrstack(labelp,addrp)>=0) return(~EOF);

 linebuffer[0] = '\0'; // NULL;
gets(linebuffer);

#ifdef FAST_BUT_DANGEROUS_INPUT
readflag = sscanxx(linebuffer,labelp,addrp);
#else
readflag = sscanf(linebuffer,"%x %x",labelp,addrp);
#endif

if (readflag==2) {
	ctrlp->tracecount++;
	if (ctrlp->tracecount > ctrlp->maxcount) {
		return(EOF);
	}
	else {
		return(~EOF);
	}
 }
else {
	if (readflag!=EOF) {
	  fprintf(stderr,"dinero: error in standard input @ access %d.\n",
		  ctrlp->tracecount);
	}
	return(EOF);
 }
} /* ********************************************************************* */




breakupaddr(			/* Decode address */
	    addr,labl,dap,cachep)
register int addr;		/* <  address */
register int labl;		/* <  type of reference */
register DECODEDADDRTYPE *dap;	/* <> fields of this ptr are altered */
CACHETYPE *cachep;			/* <  */
/*
** affects: none
** returns: OK
*/
{
/*	These unsigned integers are so that 32-bit address with
 *	an MSB of 1 will be shifted correctly with a divide.
 */
  unsigned int theaddr;
  unsigned int theblocksize, thenumUorDsets, thenumIsets;

  theaddr = addr;

  theblocksize = cachep->blocksize;
  thenumUorDsets = cachep->numUorDsets;
  thenumIsets = cachep->numIsets;

  dap->address = theaddr;
  dap->accesstype = labl;
  dap->block = theaddr % theblocksize;
  dap->blockaddr = theaddr - dap->block;
  dap->pid = currpid;

  if (cachep->subblocksize==0) {
    dap->validbit = VALID;
  }
  else {
    dap->subblocknum = dap->block / cachep->subblocksize;
    dap->validbit = VALID<<dap->subblocknum;
  }

  switch ( labl ) {
  case XREAD :
  case XWRITE : 
  case PREFETCH+XREAD :
  case PREFETCH+XWRITE : 
    dap->set = (theaddr / theblocksize) % thenumUorDsets;
    dap->tag = (theaddr / theblocksize) / thenumUorDsets;
    break;

  case XINSTRN : 
  case PREFETCH+XINSTRN : 
    if (thenumIsets != 0) { /* I-Cache */
      dap->set = ((theaddr / theblocksize) % thenumIsets)
	         + thenumUorDsets;
      dap->tag = (theaddr / theblocksize) / thenumIsets;
    }
    else {	/*  Mixed Cache; put in data cache set */
      dap->set = (theaddr / theblocksize) % thenumUorDsets;
      dap->tag = (theaddr / theblocksize) / thenumUorDsets;
    }
    break;

  case PREFETCH+XMISC : 
    dap->set = (theaddr / theblocksize) % thenumUorDsets;
    dap->tag = (theaddr / theblocksize) / thenumUorDsets;
    break;

  case XFLUSH:
  case XSWITCH:
    dap->validbit = NOTVALID;  /*!*/
    dap->blockaddr = theaddr;
    dap->tag = -1;             /* will never match a valid tag */
    dap->set = 0;              /* so that stack[dap->set] is valid */
    break;

    default :
      dap->accesstype = XMISC;
      dap->set = (theaddr / theblocksize) % thenumUorDsets;
      dap->tag = (theaddr / theblocksize) / thenumUorDsets;
      break;
  }

} /* ********************************************************************* */



flushcache(cachep,ctrlp,metricp)	/* Flush cache */

CACHETYPE *cachep;			/* <  */
CTRLTYPE *ctrlp;			/* <  */
METRICTYPE *metricp;			/* <> */
/* affects: 
** returns:
*/
{
  extern int copybackstack();
  extern int putonfreelist();

  int stacknum;

  for (stacknum=0; stacknum<(cachep->numsets); stacknum++) {
    copybackstack(cachep,ctrlp,metricp,stacknum);
    putonfreelist(stacknum,&(stack[stacknum]));
  }

} /* *************************************************** */



init_addrstack()
/*
**	NOTE: size_addrstack points beyond valid data.  A 3-address
**	stack would have array entries 0, 1, and 2 filled and
**	size_addrstack equal to 3.
*/
{
  int i;

  for (i=0; i<MAXNUMADDRSTACK; i++) {
    addrstack[i].label = XMISC;
    addrstack[i].address = 0;
  }

  size_addrstack = 0;
} /* ******************************************************************** */



push_addrstack(labl, addr)
int labl;
int addr;

{
if (size_addrstack == MAXNUMADDRSTACK) {
	/* error: addr stack overflow */
	fprintf(stderr,"dinero: addr stack overflow @ push_addrstack()\n");
	return(-2);
 }
else {
	addrstack[size_addrstack].label = labl;
	addrstack[size_addrstack].address = addr;
	size_addrstack++;
	return(size_addrstack);
 }
} /* ******************************************************************** */


pop_addrstack(lablp, addrp)
int *lablp;
int *addrp;

{
  if (size_addrstack == 0) {
    /* addr stack empty */
    return(-1);
  }
  else {
    size_addrstack--;
    *lablp = addrstack[size_addrstack].label;
    *addrp = addrstack[size_addrstack].address;
    return(size_addrstack);
  }
} /* ******************************************************************** */


print_addrstack()
{
  int i;

  fprintf(stdout, "addrstack(%d of %d): ", size_addrstack, MAXNUMADDRSTACK);

  for (i = size_addrstack - 1; i >= 0; i--)
    fprintf(stdout, "%d@%d, ", addrstack[i].label, addrstack[i].address);
  fprintf(stdout, "end.\n");

} /* ******************************************************************** */




#ifdef FAST_BUT_DANGEROUS_INPUT
/*
**	The above "ifdef" enables a fast C-function called "sscanxx" 
**	to interpret input characters instead of the library function 
**	"sscanf" because a profile showed that dineroIII was spending 
**	35% to 50% of this time in sscanf.  The function "sscanxx" runs 
**	about 6 times faster than "sscanf."  The function "sscanxx,"
**	selected by the compile-time flag FAST_BUT_DANGEROUS_INPUT in 
**	global.h, make dineroIII run in 60 to 70% of the time with the
**	option diabled.
*/

#define CHAR_ZERO		'0'
#define CHAR_NINE		'9'
#define CHAR_LOWERCASE_A	'a'
#define CHAR_LOWERCASE_F	'f'

#define HEX	4

#define CHAR_DIGIT_OFFSET		'0'
#define CHAR_LOWERCASE_ABCDEF_OFFSET	'a'-10

#define WHITE_SPACE	((*ptr==' ')||(*ptr=='\t'))

#define CHAR_HEX_DIGIT		\
	((((digit = *ptr)>=CHAR_ZERO) && (digit<=CHAR_NINE)) \
	|| ((digit>=CHAR_LOWERCASE_A) && (digit<=CHAR_LOWERCASE_F)))

#define HEX_CHAR_OFFSET		\
	((digit>=CHAR_LOWERCASE_A) && (digit<=CHAR_LOWERCASE_F) \
	? /* a-f */		\
	CHAR_LOWERCASE_ABCDEF_OFFSET	\
	: /* 0-9 */		\
	CHAR_DIGIT_OFFSET	\
	)


sscanxx(linebuffer,num1p,num2p)
char linebuffer[];
int *num1p;
int *num2p;
/*
**	This routine is designed to do the same function as
**	sscanf(linebuffer,"%x %x",labelp,addrp).  It was added
**	because gprof said that 35 to 50% of dineroIII's run time
**	was being spent in sscanf.   This routine runs eight
**	times faster.
**
**	The routine is not exactly the same as sscanf:
**	
**	(1)	It is implementation dependent.
**	(2)	Hex numbers cannot be preceded by "0x."
**	(3)	Legal digits are "0-9" and "a-f;" "A-F" will not 
**		be interpretted correctly.
**	(4)	The reponse to input errors is undefined.
*/
{
register char digit;
register char *ptr;
register int num;

/*
**	Exit with EOF if the buffer is empty.
*/
// if (*(ptr = linebuffer)==NULL) return(EOF);
if ( linebuffer[0] == '\0' ) return(EOF);

 ptr = linebuffer;


while (WHITE_SPACE) ptr++;

/*
**	Convert first hex number.
*/
num = 0;
while (CHAR_HEX_DIGIT) {
	num = (num<<HEX) + digit - HEX_CHAR_OFFSET;
	ptr++;
 }
*num1p = num;

/*	Skip white space.	*/
while (WHITE_SPACE) ptr++;

/*
**	Convert second hex number.
*/
num = 0;
while (CHAR_HEX_DIGIT) {
	num = (num<<HEX) + digit - HEX_CHAR_OFFSET;
	ptr++;
 }
*num2p = num;

return(2);
}

#endif


