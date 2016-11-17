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
**	Source File:	cmdargs.c
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
 * 20/2/92 changed fatal error output to stderr;
 * 26/2/92 added command line option -Ooutfile so that statistics can be
 *         written to "outfile" rather than "stdout".
 * 27/2/92 added command line option -o4 so that bus output is identical
 *         to the input format.
 * 17/3/92 added multiprogramming & context switch special input record.
 ************************************************************************/


#include "global.h"

void exit(int status);
size_t strlen(const char *s);

int atoiKMG(char *);
int errormessage(int, int, char *);
int warningmessage(int, int, char *);


int getcmdargs(argc,argv,		/* Analyze command line arguments. */
		  cachep,policyp,ctrlp)
register int argc;		/* <  */
register char *argv[];		/* <  */
CACHETYPE *cachep;		/* <> */
POLICYTYPE *policyp;		/* <> */
CTRLTYPE *ctrlp;		/* <> */
/* returns: OK if command line arguments are valid
**	    ERR otherwise
*/
{
extern char *strcpy();
extern char *strcat();

register int i;
register int nextnum;
int returnflag;
char nextchar;

returnflag = OK;

/* defaults */

cachep->blocksize = ILLEGALNUM;
cachep->Usize = 0;
cachep->Dsize = 0;
cachep->Isize = 0;
cachep->assoc = 1;
cachep->subblocksize = 0;

policyp->replacement = LRU;
policyp->fetch = DEMAND;
policyp->prefetchdist = 1;
policyp->abortprefetchpercent = 0;
policyp->write = COPYBACK;
policyp->writeallocate = WRITEALLOCATE;

ctrlp->debug = NODEBUG;
ctrlp->output = TERSE;
ctrlp->skipcount = 0;
ctrlp->maxcount = 2000000000;     /* was 2*10e6 */
ctrlp->Q = 0;
ctrlp->ofp = stdout;      /* output sent to stdout unless -Ooutfile is used */
strcpy(ctrlp->outfilename, "dinero.stats");

/* 
**	Get arguments 
*/
for (i=1; i<argc; i++) 
  if (argv[i][0]!='-') {
    fprintf(stderr,
	    "dinero: argument %d (%s) must be preceded by '-'.\n",i,argv[i]);
    returnflag = ERR;
  }
  else {
    switch (argv[i][1])	/* switch on second char. */
      {			/* allow: "-m100" or "-m 100" */

/*
**	CACHE
*/
case 'b' :
	nextnum = atoiKMG(argv[i][2] == '\0' ? argv[++i] : &argv[i][2]);
	if (nextnum >= 1 )
		cachep->blocksize = nextnum;
	else {
		cachep->blocksize = ILLEGALNUM;
		returnflag = ERR;
	}
	break;

case 'S' :
	nextnum = atoiKMG(argv[i][2] == '\0' ? argv[++i] : &argv[i][2]);
	if (nextnum >= 0 )
		cachep->subblocksize = nextnum;
	else {
		cachep->subblocksize = ILLEGALNUM;
		returnflag = ERR;
	}
	break;

case 'u' :
	nextnum = atoiKMG(argv[i][2] == '\0' ? argv[++i] : &argv[i][2]);
	if (nextnum >= 0 )	/* size 0 implies no unified cache. */
		cachep->Usize = nextnum;
	else {
		cachep->Usize = ILLEGALNUM;
		returnflag = ERR;
	}
	break;

case 'i' :
	nextnum = atoiKMG(argv[i][2] == '\0' ? argv[++i] : &argv[i][2]);
	if (nextnum >= 0 )
		cachep->Isize = nextnum;
	else {
		cachep->Isize = ILLEGALNUM;
		returnflag = ERR;
	}
	break;

case 'd' :
	nextnum = atoiKMG(argv[i][2] == '\0' ? argv[++i] : &argv[i][2]);
	if (nextnum >= 0 )
		cachep->Dsize = nextnum;
	else {
		cachep->Dsize = ILLEGALNUM;
		returnflag = ERR;
	}
	break;

case 'a' :
	nextnum = atoiKMG(argv[i][2] == '\0' ? argv[++i] : &argv[i][2]);
	if (nextnum >= 1 )
		cachep->assoc = nextnum;
	else {
		cachep->assoc = ILLEGALNUM;
		returnflag = ERR;
	}
	break;

/*
**	POLICY
*/
case 'r' :
	nextchar = (argv[i][2] == '\0' ? argv[++i][0] : argv[i][2]);
	if (nextchar == LRU || nextchar == FIFO || nextchar == RANDOM)
		policyp->replacement = nextchar;
	else {
		policyp->replacement = ILLEGAL;
		returnflag = ERR;
	}
	break;

case 'f' :
	nextchar = (argv[i][2] == '\0' ? argv[++i][0] : argv[i][2]);
	if ( nextchar == DEMAND
	  || nextchar == ALWAYSPREFETCH
	  || nextchar == LOADFORWARDPREFETCH
	  || nextchar == SUBBLOCKPREFETCH
	  || nextchar == MISSPREFETCH
	  || nextchar == TAGGEDPREFETCH
	  )
		policyp->fetch = nextchar;
	else {
		policyp->fetch = ILLEGAL;
		returnflag = ERR;
	}
	break;

case 'p' :
	nextnum = atoiKMG(argv[i][2] == '\0' ? argv[++i] : &argv[i][2]);
	if (nextnum >= 1 )
		policyp->prefetchdist = nextnum;
	else {
		policyp->prefetchdist = ILLEGALNUM;
		returnflag = ERR;
	}
	break;

case 'P' :
	nextnum = atoiKMG(argv[i][2] == '\0' ? argv[++i] : &argv[i][2]);
	if ((nextnum >= 0 ) && (nextnum <= 100))
		policyp->abortprefetchpercent = nextnum;
	else {
		policyp->abortprefetchpercent = ILLEGALNUM;
		returnflag = ERR;
	}
	break;

case 'w' :
	nextchar = (argv[i][2] == '\0' ? argv[++i][0] : argv[i][2]);
	if (nextchar == WRITETHROUGH || nextchar == COPYBACK)
		policyp->write = nextchar;
	else {
		policyp->write = ILLEGAL;
		returnflag = ERR;
	}
	break;

case 'A' :
	nextchar = (argv[i][2] == '\0' ? argv[++i][0] : argv[i][2]);
	if (nextchar == NOWRITEALLOCATE || nextchar == WRITEALLOCATE)
		policyp->writeallocate = nextchar;
	else {
		policyp->writeallocate = ILLEGAL;
		returnflag = ERR;
	}
	break;

/*
**	CTRL
*/
case 'Q' :
	nextnum = atoiKMG(argv[i][2] == '\0' ? argv[++i] : &argv[i][2]);
	if (nextnum >= 0)
		ctrlp->Q = nextnum;
	else {
		ctrlp->Q = ILLEGALNUM;
		returnflag = ERR;
	}
	break;

case 'D' :
	nextnum = atoiKMG(argv[i][2] == '\0' ? argv[++i] : &argv[i][2]);
	if (nextnum == NODEBUG || nextnum == DEBUG1 || nextnum == DEBUG2)
		ctrlp->debug = nextnum;
	else {
		ctrlp->debug = ILLEGALNUM;
		returnflag = ERR;
	}
	break;

case 'o' :
	nextnum = atoiKMG(argv[i][2] == '\0' ? argv[++i] : &argv[i][2]);
	if (nextnum == TERSE || nextnum == VERBOSE || nextnum == BUS
		|| nextnum == BUS_SNOOP || nextnum == BUS_TRANSP)
		ctrlp->output = nextnum;
	else {
		ctrlp->output = ILLEGALNUM;
		returnflag = ERR;
	}
	break;

case 'O' :
        if (argv[i][2] == '\0') {
               strcpy(ctrlp->outfilename, argv[++i]);
	}
        else {
               strcpy(ctrlp->outfilename, &argv[i][2]);
	}
        if( (ctrlp->ofp = fopen(ctrlp->outfilename,"w")) == NULL ) {
                fprintf(stderr,"dinero: fopen() failed\n");
                exit(1);
        }
        break;

case 'Z' :
	nextnum = atoiKMG(argv[i][2] == '\0' ? argv[++i] : &argv[i][2]);
	if (nextnum >= 0)
		ctrlp->skipcount = nextnum;
	else {
		ctrlp->skipcount = ILLEGALNUM;
		returnflag = ERR;
	}
	break;

case 'z' :
	nextnum = atoiKMG(argv[i][2] == '\0' ? argv[++i] : &argv[i][2]);
	if (nextnum >= 0)
		ctrlp->maxcount = nextnum;
	else {
		ctrlp->maxcount = ILLEGALNUM;
		returnflag = ERR;
	}
	break;

default :
	fprintf(stderr,"dinero: argument %d is invalid: %s\n",i,argv[i]);
	returnflag = ERR;
	break;
      }
     }

/* 
**	Echo command line 
*/

// fprintf(ctrlp->ofp,"\n");
// fprintf(ctrlp->ofp,"CMDLINE: ");
// for (i = 0; i < argc; i++)
//     fprintf(ctrlp->ofp,"%s%c", argv[i], (i < argc-1) ? ' ' : '\n');

returnflag = errormessage(returnflag,
(returnflag == ERR),
"Illegal parameter or value."
);

returnflag = errormessage(returnflag,
(cachep->blocksize == ILLEGALNUM),
"Blocksize not (correctly) specified."
);

returnflag = errormessage(returnflag,
((cachep->Usize<=0)&&(cachep->Isize<=0)&&(cachep->Dsize<=0)),
"No cache size (correctly) specified."
);

returnflag = errormessage(returnflag,
((cachep->Usize>0)&&(cachep->Isize>0)),
"A unified and instruction cache may not be simulated in one run."
);

returnflag = errormessage(returnflag,
((cachep->Usize>0)&&(cachep->Dsize>0)),
"A unified and data cache may not be simulated in one run."
);

returnflag = warningmessage(returnflag,
((cachep->Usize<=0)&&(cachep->Isize<=0)&&(cachep->Dsize>0)),
"Some versions of dineroIII allowed: -d unified_cache_size -i0."
);
returnflag = warningmessage(returnflag,
((cachep->Usize<=0)&&(cachep->Isize<=0)&&(cachep->Dsize>0)),
"Unified caches should now be specified with: -u unified_cache_size."
);
returnflag = errormessage(returnflag,
((cachep->Usize<=0)&&(cachep->Isize<=0)&&(cachep->Dsize>0)),
"An instruction cache must be simulated with a data cache."
);

returnflag = errormessage(returnflag,
((cachep->Usize<=0)&&(cachep->Isize>0)&&(cachep->Dsize<=0)),
"A data cache must be simulated with an instruction cache."
);


if (returnflag == OK) {

	if (cachep->Usize > 0) {  /* If there is an U-cache */

		returnflag = errormessage(returnflag,
		(((cachep->Usize % cachep->blocksize) != 0) 
	   	|| (cachep->Usize < cachep->blocksize)),
	"Unifed cache size must be multiple of the blocksize."
		);

		returnflag = errormessage(returnflag,
		(((cachep->Usize % (cachep->blocksize*cachep->assoc)) != 0) 
	   	|| (cachep->Usize < (cachep->blocksize*cachep->assoc))),
"Unified cache size must be multiple of the blocksize times the associativity."
		);
	}
	
	if (cachep->Dsize > 0) {  /* If there is an D-cache */

		returnflag = errormessage(returnflag,
		(((cachep->Dsize % cachep->blocksize) != 0) 
	   	|| (cachep->Dsize < cachep->blocksize)),
	"Data cache size must be multiple of the blocksize."
		);

		returnflag = errormessage(returnflag,
		(((cachep->Dsize % (cachep->blocksize*cachep->assoc)) != 0) 
	   	|| (cachep->Dsize < (cachep->blocksize*cachep->assoc))),
"Data cache size must be multiple of the blocksize times the associativity."
		);
	}
	
	if (cachep->Isize > 0) {  /* If there is an I-cache */

		returnflag = errormessage(returnflag,
		(((cachep->Isize % cachep->blocksize) != 0)
	   	|| (cachep->Isize < cachep->blocksize)),
	"Instruction cache size must be multiple of the blocksize."
		);

		returnflag = errormessage(returnflag,
		(((cachep->Isize % (cachep->blocksize*cachep->assoc)) != 0)
	   	|| (cachep->Isize < (cachep->blocksize*cachep->assoc))),
"I-cache size must be multiple of the blocksize times the associativity."
		);
	}
	
	if (cachep->subblocksize > 0) {  /* If there are sub-blocks  */

		returnflag = errormessage(returnflag,
		(((cachep->blocksize % cachep->subblocksize) != 0)
	   	|| (cachep->blocksize <= cachep->subblocksize)),
	"Blocksize must be a proper multiple of the sub-blocksize."
		);

		returnflag = errormessage(returnflag,
		((cachep->blocksize/cachep->subblocksize) > MAXNUMSUBBLOCKS),
"Too many sub-blocks per block; implementation restriction is 32."
		);
	}
	else {   /* No sub-blocks */
		returnflag = errormessage(returnflag,
		(policyp->fetch == LOADFORWARDPREFETCH),
		"LOADFORWARDPREFETCH (-fl) requires sub-blocks."
		);

		returnflag = errormessage(returnflag,
		(policyp->fetch == SUBBLOCKPREFETCH),
		"SUBBLOCKPREFETCH (-fS) requires sub-blocks."
		);
	}
 }

return(returnflag);
} /* ************************************************************ */



int errormessage(int olderrorflag, int condition, char *message)	/* Prints error message */
{
int newerrorflag;

/*
** E.G. USE:
**
**	returnflag = errormessage(returnflag,
**	    (cachep->Isize == ILLEGALNUM),
**	    "Instruction cache size not specified."
**	);
*/

newerrorflag = olderrorflag;

if (condition) {
	fprintf(stderr,"dinero: %s\n", message);
	newerrorflag = ERR;
 }

return(newerrorflag);
} /* ************************************************************ */



int warningmessage(int olderrorflag, int condition, char *message)	/* Prints error message */
{

if (condition) {
  fprintf(stderr,"dinero WARNING: %s\n", message);
 }

return(olderrorflag); /* warning does not affect errorflag */
} /* ************************************************************ */



void echocmdargs(CACHETYPE *cachep, POLICYTYPE *policyp, CTRLTYPE *ctrlp)	/* Echo command line arguments. */
{

/*
**	Echo parameters
*/
fprintf(ctrlp->ofp,"CACHE (bytes): ");
fprintf(ctrlp->ofp,
	"blocksize=%d, sub-blocksize=%d, Usize=%d, Dsize=%d, Isize=%d.\n",
		cachep->blocksize, cachep->subblocksize,
		cachep->Usize,cachep->Dsize,cachep->Isize);

fprintf(ctrlp->ofp,"POLICIES: ");
fprintf(ctrlp->ofp,
    "assoc=%d-way, replacement=%c, fetch=%c(%d,%d), write=%c, allocate=%c.\n",
	   	cachep->assoc, policyp->replacement, policyp->fetch,
		policyp->prefetchdist,policyp->abortprefetchpercent,
		policyp->write, policyp->writeallocate);

fprintf(ctrlp->ofp,"CTRL: ");
fprintf(ctrlp->ofp,"debug=%d, output=%d, skipcount=%d, maxcount=%d, Q=%d",
		ctrlp->debug, ctrlp->output, 
		ctrlp->skipcount, ctrlp->maxcount,ctrlp->Q);
fprintf(ctrlp->ofp,".\n");
}


int atoiKMG(char *str)
/*
**  Return integer equal to value of ascii string that is sequence
**  of digits possibly followed by one of the following suffixes:
**
**	K, k	1024
**	M, m	1024**2 = 1048576
**	G, g	1024**3 = 1073741824
**
**  E.g., "64K" evaluated to integer 65536.
**  ILLEGALNUM is returned for illegal suffixes or on overflow.
*/
{
char str_buffer[256];
int length;
char suffix;
int root, multiplier, value;

extern char *strcpy();
// extern int strlen();
extern int atoi();

strcpy(str_buffer,str);

length = strlen(str_buffer);
suffix = str_buffer[length-1];

switch (suffix) {

case 'K' :
case 'k' :
	multiplier = 1024;
	str_buffer[length-1] = '\0'; // (int)NULL;
	break;

case 'M' :
case 'm' :
	multiplier = 1048576;		/* 1024**2 */
	str_buffer[length-1] = '\0'; // (int)NULL;
	break;

case 'G' :
case 'g' :
	multiplier = 1073741824;	/* 1024**3 */
	str_buffer[length-1] = '\0'; // (int)NULL;
	break;

case '0' :
case '1' :
case '2' :
case '3' :
case '4' :
case '5' :
case '6' :
case '7' :
case '8' :
case '9' :
	multiplier = 1;
	break;

default :
	multiplier = ILLEGALNUM;
	break;
 
 }


if (multiplier!=ILLEGALNUM) {
	root = atoi(str_buffer);
	value = root*multiplier;

	if (root!=(value/multiplier)) {
		value = ILLEGALNUM;
	}
 }
else {
	value = ILLEGALNUM;
 }


return(value);
} /* ************************************************************ */

