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
**	Source File:	main.c
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
 *         to this file is sent to stdout.
 * 27/2/92 added command line option -o4 so that bus output is identical
 *         to the input format.  This makes changes to [fst|snd]-level
 *         caches no longer necessary.
 * 17/3/92 added multiprogramming & context switch special input record.
 * 24/3/92 default max-trace-count is now 20e6 (was 10e6)
 * 25/3/92 bug fix: on a context switch, flush cache...
 * 7/3/92  on reading a context switch input record, the cache is flushed
 *         only if the write policy is copy-back.
 ************************************************************************/


#define PUBLIC
#include "global.h"

void exit(int status);

main(argc,argv)		/* Dinero III Cache Simulator (see doc.h). */
int argc;		/* < */
char *argv[];		/* < */
{
extern int init();
extern int outputmetric();
extern int srandom();	
POLICYTYPE policy;
METRICTYPE metric;
CTRLTYPE ctrl;
CACHETYPE cache;

/*!**!**!**!**!**!**!**!*
 * fprintf(stderr, "\n---Dinero III by Mark D. Hill.\n");
 * fprintf(stderr, "---Version %s, Released %s.\n", "3.3", "May 1989");
 *!**!**!**!**!**!**!**!*/

/*	printf(    "---Unimplemented: %s\n", 
	    "none."
	    );	*/
/*	printf(    "---Untested: %s\n", 
	    "none."
	    );	*/
/*	printf(    "---Known bugs: %s\n", 
	    "none."
	    );	*/

/*
**	Exit with extra messages if there are no args.
*/
if (argc==1) {
  fprintf(stderr, "usage: %s\n   or: %s\n",
	  "dinero -bN [-SN] -uNNN -d0 -i0 [-aN -rC] [-wC -AC] [-oN] [-Ofile]",
	  "dinero -bN [-SN] -u0 -dNNN -iNNN [-aN -rC] [-wC -AC] [-oN] [-Ofile]"
);
	exit(1);
}

/*!* fprintf(stderr,"\n---Execution begins.\n");
 *!*/

init(argc,argv,&cache,&policy,&ctrl,&metric);

srandom(1);	

/*!*fprintf(stderr,"\n---Simulation begins.\n");
 *!*/

mainloop(&cache,&policy,&ctrl,&metric);

/*!* fprintf(stderr,"---Simulation complete.\n");
 *!*/
outputmetric(&cache,&policy,&ctrl,&metric);

if (fclose(ctrl.ofp) != 0) {
    fprintf(stderr, "dinero: fclose() failed\n");
    exit(1);
  }

/*!*fprintf(stderr,"---Execution complete.\n");
 *!*/

} /* ****************************************************************** */



mainloop(cachep,policyp,ctrlp,metricp)  /* Initiates fetch, & updates */

CACHETYPE *cachep;                      /* <> */
register POLICYTYPE *policyp;           /* <  */
register CTRLTYPE *ctrlp;               /* <  */
register METRICTYPE *metricp;           /* <> */
{
extern int dumpaddr();
extern int dumpstate();
extern int fetch();
extern int outputmetric();
extern int update();
extern int flushcache();
DECODEDADDRTYPE decodedaddr;


/*
**  Repeat until:  
**	1) "max_count" trace addresses (i.e. demand references) have 
**	   been read.
**	2) the input trace has been exhausted.
**
**  For each address (including any prefetch addresses):  
**	1) call fetch() to get and interpret the address..
**  	2) call update() to find the address in the priority stacks,
**	   perform side-effects (like starting a prefetch), update
**	   priority stacks, and update metrics.
**	3) If debugging is enabled, print some information.
** 
*/
while (fetch(cachep, ctrlp, metricp, policyp, &decodedaddr) != EOF) {

#if 0
  dumpaddr(ctrlp->tracecount, &decodedaddr, ctrlp);
#endif

	update(cachep, policyp, ctrlp, metricp, &decodedaddr);

	if (ctrlp->debug >= DEBUG1) {
		dumpaddr(ctrlp->tracecount, &decodedaddr, ctrlp);
		if (ctrlp->debug >= DEBUG2) {
			outputmetric(cachep, policyp, ctrlp, metricp);
		}
		dumpstate(cachep, policyp, ctrlp);
	}

	if (ctrlp->output == VERBOSE && (ctrlp->tracecount % 500001) == 0) {
		outputmetric(cachep, policyp, ctrlp, metricp);
	}

 } /* END OF MAIN LOOP */

/* flush cache at end of run */
flushcache(cachep, ctrlp, metricp);

if (ctrlp->tracecount > ctrlp->maxcount) {
	fprintf(stderr,"dinero: maximum address count exceeded.\n");
 }

} /* ****************************************************************** */

