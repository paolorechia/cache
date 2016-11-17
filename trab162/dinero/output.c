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
**	Source File:	output.c
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
 * 20/2/92 changed error output to stderr;
 *         changed output to file if [FST|SND]LEVEL caches defined ***
 *         CHANGED: see below ***
 * 26/2/92 changed all output to fprintf(ofp,...), where "ofp" can be
 *         set up by the new command line flag -Ooutfile;  if -O flag
 *         is not used, all output is sent to stderr.
 * 27/2/92 added command line option -o4 so that bus output is identical
 *         to the input format.  This makes changes to [fst|snd]-level
 *         caches no longer necessary.  Prefetches with -o4 are signaled
 *         according to the type of the access (i.e. ifetch or read).
 * 17/3/92 added multiprogramming & context switch special input record.
 ************************************************************************/

#include "global.h"

/*
**	Used so that "numerator / NONZERO(denomenator)" will
**	never cause a divide-by-zero exception.
*/
#define NONZERO(i)	((i==0) ? 1.0 : (float)i)

outputmetric(cachep,policyp,ctrlp,m)
CACHETYPE *cachep;		/* <  */
POLICYTYPE *policyp;		/* <  */
CTRLTYPE *ctrlp;		/* <  */
METRICTYPE *m;			/* <  Too lazy to write "metricp" */
/*
** affects: none
** returns: OK
*/
{
int	demand_fetch_data,
	demand_fetch_alltype;
int	prefetch_fetch_data,
	prefetch_fetch_alltype;

int	demand_data,
	demand_alltype;
int	prefetch_data,
	prefetch_alltype;

float	floatnum;

/* Used in bus traffic calculations even if no prefetching. */
prefetch_fetch_alltype = 0;

/*
** Print Header
*/
fprintf(ctrlp->ofp, "\n");
fprintf(ctrlp->ofp, " Metrics\t\tAccess Type:\n");
fprintf(ctrlp->ofp, 
  " (totals,fraction)\tTotal      Instrn     Data       Read       Write      Misc\n");
fprintf(ctrlp->ofp, 
  " -----------------\t---------  ---------  ---------  ---------  ---------  ------\n");

/*
** Print Fetch Numbers
*/
demand_fetch_data = m->fetch[XMISC] 
		  + m->fetch[XREAD]
	      	  + m->fetch[XWRITE];
demand_fetch_alltype = demand_fetch_data 
		  + m->fetch[XINSTRN];

fprintf(ctrlp->ofp,
  " Demand Fetches\t\t%9d  %9d  %9d  %9d  %9d  %6d\n",
		demand_fetch_alltype,
		m->fetch[XINSTRN],
		demand_fetch_data,
		m->fetch[XREAD],
		m->fetch[XWRITE],
		m->fetch[XMISC]
	);

floatnum = NONZERO(demand_fetch_alltype);

fprintf(ctrlp->ofp, " din_dem\t\t%9.4f  %9.4f  %9.4f  %9.4f  %9.4f  %6.4f\n",
		demand_fetch_alltype / floatnum,
		m->fetch[XINSTRN] / floatnum,
		demand_fetch_data / floatnum,
		m->fetch[XREAD] / floatnum,
		m->fetch[XWRITE] / floatnum,
		m->fetch[XMISC] / floatnum
	);

/*
** Prefetching?
*/
if (policyp->fetch!=DEMAND) {
prefetch_fetch_data = m->fetch[PREFETCH+XMISC] 
		  + m->fetch[PREFETCH+XREAD]
	      	  + m->fetch[PREFETCH+XWRITE];
prefetch_fetch_alltype = prefetch_fetch_data 
		  + m->fetch[PREFETCH+XINSTRN];

fprintf(ctrlp->ofp,
	" Prefetch Fetches\t%9d  %9d  %9d  %9d  %9d  %6d\n", //	%6d	%6d	%6d	%6d	%6d	%6d\n",
		prefetch_fetch_alltype,
		m->fetch[PREFETCH+XINSTRN],
		prefetch_fetch_data,
		m->fetch[PREFETCH+XREAD],
		m->fetch[PREFETCH+XWRITE],
		m->fetch[PREFETCH+XMISC]
	);

floatnum = NONZERO(prefetch_fetch_alltype);

fprintf(ctrlp->ofp," din_pff\t\t%9.4f  %9.4f  %9.4f  %9.4f  %9.4f  %6.4f\n",
	//  "			%6.4f	%6.4f	%6.4f	%6.4f	%6.4f	%6.4f\n",
		prefetch_fetch_alltype / floatnum,
		m->fetch[PREFETCH+XINSTRN] / floatnum,
		prefetch_fetch_data / floatnum,
		m->fetch[PREFETCH+XREAD] / floatnum,
		m->fetch[PREFETCH+XWRITE] / floatnum,
		m->fetch[PREFETCH+XMISC] / floatnum
	);

  fprintf(ctrlp->ofp,
  " Total Fetches\t\t%9d  %9d  %9d  %9d  %9d  %6d\n", //			%6d	%6d	%6d	%6d	%6d	%6d\n",
		demand_fetch_alltype + prefetch_fetch_alltype,
		m->fetch[XINSTRN] + m->fetch[PREFETCH+XINSTRN],
		demand_fetch_data + prefetch_fetch_data,
		m->fetch[XREAD] + m->fetch[PREFETCH+XREAD],
		m->fetch[XWRITE] + m->fetch[PREFETCH+XWRITE],
		m->fetch[XMISC] + m->fetch[PREFETCH+XMISC]
	);

floatnum = NONZERO(demand_fetch_alltype + prefetch_fetch_alltype);

fprintf(ctrlp->ofp," din_ttf\t\t%9.4f  %9.4f  %9.4f  %9.4f  %9.4f  %6.4f\n",
	//  "			%6.4f	%6.4f	%6.4f	%6.4f	%6.4f	%6.4f\n",
		(demand_fetch_alltype + prefetch_fetch_alltype) / floatnum,
		(m->fetch[XINSTRN] + m->fetch[PREFETCH+XINSTRN]) / floatnum,
		(demand_fetch_data + prefetch_fetch_data) / floatnum,
		(m->fetch[XREAD] + m->fetch[PREFETCH+XREAD]) / floatnum,
		(m->fetch[XWRITE] + m->fetch[PREFETCH+XWRITE]) / floatnum,
		(m->fetch[XMISC] + m->fetch[PREFETCH+XMISC]) / floatnum
	);

 } /* End of prefetching. */

fprintf(ctrlp->ofp, "\n");


/*
** End of Fetch Numbers
*/

/*
** Print Miss Numbers
*/
demand_data = m->miss[XMISC] 
		  + m->miss[XREAD]
	      	  + m->miss[XWRITE];
demand_alltype = demand_data 
		  + m->miss[XINSTRN];

fprintf(ctrlp->ofp,
  " Demand Misses\t\t%9d  %9d  %9d  %9d  %9d  %6d\n", //			%6d	%6d	%6d	%6d	%6d	%6d\n",
		demand_alltype,
		m->miss[XINSTRN],
		demand_data,
		m->miss[XREAD],
		m->miss[XWRITE],
		m->miss[XMISC]
	);
fprintf(ctrlp->ofp," din_dmm\t\t%9.4f  %9.4f  %9.4f  %9.4f  %9.4f  %6.4f\n",
	//  "			%6.4f	%6.4f	%6.4f	%6.4f	%6.4f	%6.4f\n",
		demand_alltype / NONZERO(demand_fetch_alltype),
		m->miss[XINSTRN] / NONZERO(m->fetch[XINSTRN]),
		demand_data / NONZERO(demand_fetch_data),
		m->miss[XREAD] / NONZERO(m->fetch[XREAD]),
		m->miss[XWRITE] / NONZERO(m->fetch[XWRITE]),
		m->miss[XMISC] / NONZERO(m->fetch[XMISC])
	);

/*
** Prefetching?
*/
if (policyp->fetch!=DEMAND) {
prefetch_data = m->miss[PREFETCH+XMISC] 
		  + m->miss[PREFETCH+XREAD]
	      	  + m->miss[PREFETCH+XWRITE];
prefetch_alltype = prefetch_data 
		  + m->miss[PREFETCH+XINSTRN];

fprintf(ctrlp->ofp,
  " Prefetch Misses\t%9d  %9d  %9d  %9d  %9d  %6d\n", //			%6d	%6d	%6d	%6d	%6d	%6d\n",
		prefetch_alltype,
		m->miss[PREFETCH+XINSTRN],
		prefetch_data,
		m->miss[PREFETCH+XREAD],
		m->miss[PREFETCH+XWRITE],
		m->miss[PREFETCH+XMISC]
	);
fprintf(ctrlp->ofp," din_prm\t\t%9.4f  %9.4f  %9.4f  %9.4f  %9.4f  %6.4f\n",
	//  "			%6.4f	%6.4f	%6.4f	%6.4f	%6.4f	%6.4f\n",
		prefetch_alltype
			/ NONZERO(prefetch_fetch_alltype),
		m->miss[PREFETCH+XINSTRN] 
			/ NONZERO(m->fetch[PREFETCH+XINSTRN]),
		prefetch_data
			/ NONZERO(prefetch_fetch_data),
		m->miss[PREFETCH+XREAD] 
			/ NONZERO(m->fetch[PREFETCH+XREAD]),
		m->miss[PREFETCH+XWRITE] 
			/ NONZERO(m->fetch[PREFETCH+XWRITE]),
		m->miss[PREFETCH+XMISC] 
			/ NONZERO(m->fetch[PREFETCH+XMISC])
	);
fprintf(ctrlp->ofp,
  " Total Misses\t\t%9d  %9d  %9d  %9d  %9d  %6d\n", //			%6d	%6d	%6d	%6d	%6d	%6d\n",
		demand_alltype + prefetch_alltype,
		m->miss[XINSTRN] + m->miss[PREFETCH+XINSTRN],
		demand_data + prefetch_data,
		m->miss[XREAD] + m->miss[PREFETCH+XREAD],
		m->miss[XWRITE] + m->miss[PREFETCH+XWRITE],
		m->miss[XMISC] + m->miss[PREFETCH+XMISC]
	);
fprintf(ctrlp->ofp," din_ttm\t\t%9.4f  %9.4f  %9.4f  %9.4f  %9.4f  %6.4f\n",
	//  "			%6.4f	%6.4f	%6.4f	%6.4f	%6.4f	%6.4f\n",
		(demand_alltype + prefetch_alltype)
		     / NONZERO(demand_fetch_alltype + prefetch_fetch_alltype),
		(m->miss[XINSTRN] + m->miss[PREFETCH+XINSTRN])
		     / NONZERO(m->fetch[XINSTRN] + m->fetch[PREFETCH+XINSTRN]),
		(demand_data + prefetch_data)
			/ NONZERO(demand_fetch_data + prefetch_fetch_data),
		(m->miss[XREAD] + m->miss[PREFETCH+XREAD])
			/ NONZERO(m->fetch[XREAD] + m->fetch[PREFETCH+XREAD]),
		(m->miss[XWRITE] + m->miss[PREFETCH+XWRITE])
		      / NONZERO(m->fetch[XWRITE] + m->fetch[PREFETCH+XWRITE]),
		(m->miss[XMISC] + m->miss[PREFETCH+XMISC])
			/ NONZERO(m->fetch[XMISC] + m->fetch[PREFETCH+XMISC])
	);

 } /* End of prefetching. */
fprintf(ctrlp->ofp, "\n");

/*
** End of Misses Numbers
*/


/*
** Print Block Miss Numbers
*/
if (cachep->subblocksize!=0) {
demand_data = m->blockmiss[XMISC] 
		  + m->blockmiss[XREAD]
	      	  + m->blockmiss[XWRITE];
demand_alltype = demand_data 
		  + m->blockmiss[XINSTRN];

fprintf(ctrlp->ofp,
  " Demand Block Misses\t%9d  %9d  %9d  %9d  %9d  %6d\n", //	%6d	%6d	%6d	%6d	%6d	%6d\n",
		demand_alltype,
		m->blockmiss[XINSTRN],
		demand_data,
		m->blockmiss[XREAD],
		m->blockmiss[XWRITE],
		m->blockmiss[XMISC]
	);
fprintf(ctrlp->ofp," din_dbm\t\t%9.4f  %9.4f  %9.4f  %9.4f  %9.4f  %6.4f\n",
	//  "			%6.4f	%6.4f	%6.4f	%6.4f	%6.4f	%6.4f\n",
		demand_alltype / NONZERO(demand_fetch_alltype),
		m->blockmiss[XINSTRN] / NONZERO(m->fetch[XINSTRN]),
		demand_data / NONZERO(demand_fetch_data),
		m->blockmiss[XREAD] / NONZERO(m->fetch[XREAD]),
		m->blockmiss[XWRITE] / NONZERO(m->fetch[XWRITE]),
		m->blockmiss[XMISC] / NONZERO(m->fetch[XMISC])
	);

/*
** Prefetching?
*/
if (policyp->fetch!=DEMAND) {
prefetch_data = m->blockmiss[PREFETCH+XMISC] 
		  + m->blockmiss[PREFETCH+XREAD]
	      	  + m->blockmiss[PREFETCH+XWRITE];
prefetch_alltype = prefetch_data 
		  + m->blockmiss[PREFETCH+XINSTRN];

fprintf(ctrlp->ofp,
  " Prefetch Block Misses\t%9d  %9d  %9d  %9d  %9d  %6d\n", //	%6d	%6d	%6d	%6d	%6d	%6d\n",
		prefetch_alltype,
		m->blockmiss[PREFETCH+XINSTRN],
		prefetch_data,
		m->blockmiss[PREFETCH+XREAD],
		m->blockmiss[PREFETCH+XWRITE],
		m->blockmiss[PREFETCH+XMISC]
	);
fprintf(ctrlp->ofp," din_pbm\t\t%9.4f  %9.4f  %9.4f  %9.4f  %9.4f  %6.4f\n",
	//  "			%6.4f	%6.4f	%6.4f	%6.4f	%6.4f	%6.4f\n",
		prefetch_alltype
			/ NONZERO(prefetch_fetch_alltype),
		m->blockmiss[PREFETCH+XINSTRN] 
			/ NONZERO(m->fetch[PREFETCH+XINSTRN]),
		prefetch_data
			/ NONZERO(prefetch_fetch_data),
		m->blockmiss[PREFETCH+XREAD] 
			/ NONZERO(m->fetch[PREFETCH+XREAD]),
		m->blockmiss[PREFETCH+XWRITE] 
			/ NONZERO(m->fetch[PREFETCH+XWRITE]),
		m->blockmiss[PREFETCH+XMISC] 
			/ NONZERO(m->fetch[PREFETCH+XMISC])
	);
fprintf(ctrlp->ofp, 
  " Total Block Misses\t%9d  %9d  %9d  %9d  %9d  %6d\n", //	%6d	%6d	%6d	%6d	%6d	%6d\n",
		demand_alltype + prefetch_alltype,
		m->blockmiss[XINSTRN] + m->blockmiss[PREFETCH+XINSTRN],
		demand_data + prefetch_data,
		m->blockmiss[XREAD] + m->blockmiss[PREFETCH+XREAD],
		m->blockmiss[XWRITE] + m->blockmiss[PREFETCH+XWRITE],
		m->blockmiss[XMISC] + m->blockmiss[PREFETCH+XMISC]
	);
fprintf(ctrlp->ofp," din_tbm\t\t%9.4f  %9.4f  %9.4f  %9.4f  %9.4f  %6.4f\n",
	//  "			%6.4f	%6.4f	%6.4f	%6.4f	%6.4f	%6.4f\n",
		(demand_alltype + prefetch_alltype)
		     / NONZERO(demand_fetch_alltype + prefetch_fetch_alltype),
		(m->blockmiss[XINSTRN] + m->blockmiss[PREFETCH+XINSTRN])
		     / NONZERO(m->fetch[XINSTRN] + m->fetch[PREFETCH+XINSTRN]),
		(demand_data + prefetch_data)
			/ NONZERO(demand_fetch_data + prefetch_fetch_data),
		(m->blockmiss[XREAD] + m->blockmiss[PREFETCH+XREAD])
			/ NONZERO(m->fetch[XREAD] + m->fetch[PREFETCH+XREAD]),
		(m->blockmiss[XWRITE] + m->blockmiss[PREFETCH+XWRITE])
		      / NONZERO(m->fetch[XWRITE] + m->fetch[PREFETCH+XWRITE]),
		(m->blockmiss[XMISC] + m->blockmiss[PREFETCH+XMISC])
			/ NONZERO(m->fetch[XMISC] + m->fetch[PREFETCH+XMISC])
	);

 } /* End of prefetching. */

fprintf(ctrlp->ofp, "\n");

 } /* End of block miss. */
/*
** End of Block Misses Numbers
*/

/*
**	Print Bus Traffic Numbers
*/

fprintf(ctrlp->ofp, 
  " Words From Memory\t%9d\n",
		m->bus_traffic_in / WORDSIZE
	);
fprintf(ctrlp->ofp, 
  " ( / Demand Fetches)\t%9.4f\n",
		(m->bus_traffic_in / WORDSIZE)
		     / NONZERO(demand_fetch_alltype)
	);
fprintf(ctrlp->ofp, 
  " Words Copied-Back\t%9d\n",
		m->bus_traffic_out / WORDSIZE
	);
fprintf(ctrlp->ofp, 
  " ( / Demand Writes)\t%9.4f\n",
		(m->bus_traffic_out / WORDSIZE)
		     / NONZERO(m->fetch[XWRITE])
	);
fprintf(ctrlp->ofp, 
  " Total Traffic (words)\t%9d\n",
		(m->bus_traffic_in + m->bus_traffic_out) / WORDSIZE
	);
fprintf(ctrlp->ofp, 
  " ( / Demand Fetches)\t%9.4f\n",
		((m->bus_traffic_in + m->bus_traffic_out) / WORDSIZE)
		     / NONZERO(demand_fetch_alltype)
	);
fprintf(ctrlp->ofp, 
  " Specials -- context switches: %d   flushes: %d\n",
	m->fetch[XSWITCH],
	m->fetch[XFLUSH]
	);
fprintf(ctrlp->ofp, "\n");

/*
**	End of Bus Traffic
*/

} /* ***************************************************************** */



dumpaddr(number,dap,ctrlp)	/* Dumps decoded address */
int number;			/* <  */
register DECODEDADDRTYPE *dap;	/* <  */
CTRLTYPE *ctrlp;		/* <  */
/*
** affects: none
** returns: OK
*/
{
  char *s;

  switch (dap->accesstype) {
    case XREAD:
    s = "rd";
    break;
  case XWRITE:
    s = "wr";
    break;
  case XINSTRN:
    s = "if";
    break;
  case XMISC:
    s = "es";
    break;
  case XFLUSH:
    s = "fl";
    break;
  case XSWITCH:
    s = "cs";
    break;
  default:
    s = "??";
    break;
  }

  printf("## %d ## %d:%s @ 0x%x, TSB=0x%x:%x:%x, valid=0x%x\n",
	 number, dap->accesstype, s, dap->address, dap->tag, dap->set,
	 dap->block, dap->validbit);

/******
 * fprintf(ctrlp->ofp,
 *      "\n### %d ### Access=0x%x, Addr=0x%x, TSB=0x%x@%x@%x, subbit=0x%x\n",
 *	number,dap->accesstype,dap->address,
 *	dap->tag,dap->set,dap->block,dap->validbit);
 *****/

} /* ***************************************************************** */


dumpstate(cachep,policyp,ctrlp)		/* Dumps info on stacks & free list */
register CACHETYPE *cachep;		/* <  */
POLICYTYPE *policyp;		        /* <  */
CTRLTYPE *ctrlp;		        /* <  */
/*
** affects: none
** returns: OK
*/
{
extern STACKNODETYPE *stack;	/* global ptr to top of stacks */
extern int bufferindex;		/* global index to buffer */
extern STACKNODETYPE freelist;	/* List head for free list */
extern int numnodes;		/* Count on storage allocated */
register int i;
register int sum;
register STACKNODETYPE *ptr;
int num_of_sets;

sum = 0;
num_of_sets = cachep->numIsets + cachep->numUorDsets;

fprintf(ctrlp->ofp, "\nStack Number  Size   Cum Size\n");
fprintf(ctrlp->ofp,   "-----------   ----   --------\n");

  for (i=0; i<num_of_sets; i++) 
	if (stack[i].tag>0) {
	   fprintf(ctrlp->ofp,
                   "%10x %6d %10d\t",i,stack[i].tag,(sum += stack[i].tag));
	   for (ptr=stack[i].next; ptr!=NULL; ptr=ptr->next) {
		fprintf(ctrlp->ofp, "(%x,%x",ptr->tag,ptr->blockaddr);
		if ((cachep->subblocksize>0)||(ptr->valid!=VALID)) {
			fprintf(ctrlp->ofp, ",V%x",ptr->valid);
		}
		if ((policyp->fetch==TAGGEDPREFETCH)
		   || (ptr->reference!=NOTREFERENCED)){
			fprintf(ctrlp->ofp, ",R%x",ptr->reference);
		}
		if ((policyp->write==COPYBACK)||(ptr->dirty!=NOTDIRTY)) {
			fprintf(ctrlp->ofp, ",D%x",ptr->dirty);
		}
	   	fprintf(ctrlp->ofp, ")");
	   }
	   fprintf(ctrlp->ofp, "\n");
	}

fprintf(ctrlp->ofp,
          "%d elements allocated, %d on priority stacks, %d on freelist,\n",
	numnodes, sum, freelist.tag);
fprintf(ctrlp->ofp, "%d unused in buffer%s\n", (BUFFERSIZE-bufferindex),
	((numnodes - sum - freelist.tag - BUFFERSIZE + bufferindex)==0
	? "." : "(*** error: stacks are inconsistent)."));

} /* ***************************************************************** */




dumpmetric(metricp,cachep)	/* Dumps cache performance measures */
METRICTYPE *metricp;		/* <  */
CACHETYPE *cachep;		/* <  */
/*
** affects: none
** returns: OK
*/
{
fprintf(stderr, "dinero BUG: dumpmetric() unimplemented.\n\n");
		
} /* ***************************************************************** */

