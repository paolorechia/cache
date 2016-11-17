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
**	                                          Source File:  state.c
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
 * 19/2/92 All the stuff for the IBM 370 has been removed.
 * 26/2/92 added new command line option (-Ofile) to allow for statistics
 *         to be written to the given file; all the output not written
 *         to this file is sent to stderr.
 * 27/2/92 added command line option -o4 so that bus output is identical
 *         to the input format.
 * 14/3/92 bug fix: -o4 output now generates all the bus accesses when a
 *         cache line is being loaded/flushed (e.g. 32/4 fetches on a miss)
 * 17/3/92 added multiprogramming (multitracing) & context_switch input
 *         record.
 * 7/3/92  on reading a context switch input record, the cache is flushed
 *         only if the write policy is copy-back.
 * 20/4/92 bug fix: copybackstack() didn't have output for BUS_TRANSP
 * 20/4/92 accesses are to word-aligned locations only.
 ************************************************************************/

#include  "global.h"

void exit(int status);

update(cachep,policyp,ctrlp,metricp,dap)  /* Update */

CACHETYPE *cachep;		/* <  */
POLICYTYPE *policyp;		/* <  */
CTRLTYPE *ctrlp;		/* <  */
METRICTYPE *metricp;		/* <> */
register DECODEDADDRTYPE *dap;	/* <  */
/*
** affects: priority stacks & metrics
*/
{
  STACKNODETYPE *stackupdate();
  STACKNODETYPE *findnth();

  int setnumber;
  register int stackdepth;
  int elements_per_set;
  int miss;
  int blockmiss;
  STACKNODETYPE *preptr;
  STACKNODETYPE *ptr;
  STACKNODETYPE *replacedptr;

  /*
   **	Find address tag; if not TOS, update based upon replacement
   **	algorithm. Do not update if the access is XWRITE and the 
   **	the policy is not to allocate on writes.
   **
   **	NOTE: stackdepth==0 ==> the block was not found and preptr
   **	and ptr are NULL.
   */
  setnumber = dap->set;
  elements_per_set = cachep->assoc;

  stackdepth = find(dap->tag,elements_per_set,setnumber,&preptr,&ptr);
  blockmiss = ((stackdepth == 0) || (stackdepth > elements_per_set));
  miss = blockmiss || ((dap->validbit & ptr->valid) == 0);

  /*
   **	Prefetch active?  
   **	Prefetch on reads and instruction fetches, but not on
   **	writes, misc, and prefetch references.
   */
  if ( (policyp->fetch != DEMAND)
      && ( (dap->accesstype == XREAD) || (dap->accesstype == XINSTRN) ) ) {
    prefetch(cachep,policyp,dap,miss,ptr);
  }
  
  /*	If address trap occurs, then abort reference. */
  /* if (addrtrap()) return(); */

  /*	Update Priority Stacks   */
  switch (dap->accesstype) {
    case XREAD:
    case XWRITE:
    case XINSTRN:
    case XMISC:
    case (PREFETCH+XREAD):
    case (PREFETCH+XINSTRN):

      if (stackdepth != 1) { 	                   /* if not top of stack */
	if ((policyp->writeallocate == WRITEALLOCATE)
	    || (dap->accesstype != XWRITE)) {
	  ptr = stackupdate(stackdepth,  dap->tag,dap->blockaddr,
			    policyp->replacement,setnumber, elements_per_set,
			    ptr, preptr);
	}
	else {          /* also update on a write hit with NOWRITEALLOCATE */
	  if ((stackdepth > 1) && (stackdepth <= elements_per_set)) {
	    ptr = stackupdate(stackdepth, dap->tag, dap->blockaddr,
			      policyp->replacement, setnumber,
			      elements_per_set, ptr, preptr);
	  }
	}
      }
      /*  Update state bits unless access is a WRITE miss w/o WRITEALLOCATE  */
      if ((policyp->writeallocate == NOWRITEALLOCATE) 
	  && (dap->accesstype == XWRITE)
	  && (miss)) {
	/* do nothing */
      }
      else {     /*  Reset reference, dirty, and valid bits on block miss  */
	if (blockmiss) {
	  ptr->reference = NOTREFERENCED;
	  ptr->dirty = NOTDIRTY;
	  ptr->valid = NOTVALID;
	}
	/*	Set valid bit; Set dirty if bit necessary.
	 */
	ptr->valid |= dap->validbit;
	if ((dap->accesstype == XWRITE) && (policyp->write == COPYBACK)) {
	  ptr->dirty |= dap->validbit;
	}
	/*	Set reference bit for tagged prefetch if demand fetch
	 */
	if ((policyp->fetch == TAGGEDPREFETCH)
	    && ( (dap->accesstype == XINSTRN)
		|| (dap->accesstype == XREAD)
		|| (dap->accesstype == XWRITE)
		|| (dap->accesstype == XMISC) ) ) {
	  ptr->reference |= dap->validbit;
	}
      }
      break;

    default:
      break;
  }

  /*	Update metrics. 
   */
  metricp->fetch[dap->accesstype]++;
  if (miss) {
    metricp->miss[dap->accesstype]++;
    if (blockmiss) {
      metricp->blockmiss[dap->accesstype]++;
    }
  }

  /*	Calculate bus traffic
   */
  if (blockmiss) {
    replacedptr = findnth(setnumber, (elements_per_set + 1));
  }
  else {
    replacedptr = NULL;
  }
  
  busupdate(cachep, policyp, ctrlp, metricp, dap,
	    miss, blockmiss, replacedptr);

} /* ***************************************************************** */



STACKNODETYPE *stackupdate(			/* Update priority stacks */
    stackdepth,addr_tag,block_addr,replace_policy,
    setnum,elements_per_set,ptr,preptr)

int stackdepth;			/* <  */
int addr_tag;			/* <  */
int block_addr;			/* <  */
char replace_policy;		/* <  */
int setnum;			/* <  */
int elements_per_set;		/* <  */
STACKNODETYPE *ptr;		/* <> */
STACKNODETYPE *preptr;		/* <  */
{
int level;
int position;
STACKNODETYPE *ptr1;
STACKNODETYPE *makenode();
extern long random();


switch (replace_policy) {

case LRU :	
if (stackdepth == 0) {	/* All misses? */
		ptr = makenode(addr_tag, block_addr, currpid);
		push(ptr, setnum);
	}
	else { 		/* LRU */
		movetotop(preptr, ptr, setnum);
	}
	break;

case FIFO :
	if (stackdepth == 0) {	/* All misses? */
		ptr = makenode(addr_tag, block_addr, currpid);
		push(ptr, setnum);
	}
	else {	/* FIFO is NOT a stack algorithm; thus,  */
		/* results are valid only for set size 0 */
		/* Note: node does NOT move up in stack  */
		/* unless it was previously out of the   */
		/* cache.				 */

		if (stackdepth > elements_per_set) {
			movetotop(preptr, ptr, setnum);
		}
	}
	break;


case RANDOM	 :
	if (stackdepth == 0) {	/* All misses? */
		ptr = makenode(addr_tag, block_addr, currpid);
		push(ptr, setnum);
		position = ( (elements_per_set < stack[setnum].tag)
			    ? elements_per_set : stack[setnum].tag);
	}
	else {	/* RANDOM is a stack algorithm if updates */
		/* are done properly (See Coffman & Denning */
		/* OS Theory, pp. 260-261). Basically,    */
		/* stack elements between where the new   */
		/* TOS came from and the top have a       */
		/* (level - 1)/level chance of maintaining */
		/* their previous position. Since the     */
		/* "movetotop" function initially pushes  */
		/* elements a level down in the stack, an */
		/* element must be swapped up once to     */
		/* return to its previous spot.		  */

		movetotop(preptr,ptr,setnum);
		position = stackdepth - 1;
	}
	preptr = ptr;	/* adjust priority stack */
	ptr1 = ptr->next;
	for (level = 2; ((level <= position) && (ptr1 != NULL));
	level++) {
		if (random()%level > 0) {
			swap(preptr,ptr1,ptr1->next,setnum);
			preptr = preptr->next;
		}
		else {
			preptr = ptr1;
			ptr1 = ptr1->next;
		}
	}
	break;

	default :
	fprintf(stderr,
		"dinero: illegal replacement algorithm in stackupdate()");
	exit(ERR);

 }

return(ptr);
} /* ************************************************************** */



push(ptr,stacknum)		/* Push node on priority stack "stacknum." */
STACKNODETYPE *ptr;		/* <  ptr to node */
int stacknum;			/* <  which stack */
/*
** affects: priority stacks
** returns: number in stack "stacknum"
*/
{
  extern STACKNODETYPE *stack;	/* global ptr to top of stacks */

  ptr->next = stack[stacknum].next;
  stack[stacknum].next = ptr;

  return(++(stack[stacknum].tag));		/* Return number in stack */
} /* ***************************************************************** */



Remove(preptr,ptr,stacknum)  	/* Remove node from a priority stack  */
STACKNODETYPE *preptr;		/* < ptr to node preceeding the one to be
				   removed */
STACKNODETYPE *ptr;		/* < ptr to node to be removed */
int stacknum;			/* < which stack */
/*
** affects: priority stacks
** returns: number left in stack "stacknum"
*/
{
  extern STACKNODETYPE *stack;	        /* global ptr to top of stacks */
  extern STACKNODETYPE freelist;	/* List head for free list */

  preptr->next = ptr->next;	        /* Remove */

  ptr->next = freelist.next;	        /* Push on freelist */
  freelist.next = ptr;
  freelist.tag++;

  return(--(stack[stacknum].tag));	/* Return number in stack */
} /* ***************************************************************** */



movetotop(preptr,ptr,stacknum)	/* Move node to top of priority stack. */
STACKNODETYPE *preptr;		/* < ptr to node preceeding the one to be
				   moved to top */
STACKNODETYPE *ptr;		/* < ptr to node to be moved */
int stacknum;			/* < which stack */
/*
** affects: priority stacks
** returns: number in stack "stacknum"
*/
{
  extern STACKNODETYPE *stack;     	/* global ptr to top of stacks */

  preptr->next = ptr->next;		/* Remove */

  ptr->next = stack[stacknum].next;	/* Push */
  stack[stacknum].next = ptr;

  return(stack[stacknum].tag);		/* Return number in stack */
} /* ***************************************************************** */



swap(preptr,ptr1,ptr2,stacknum)	/* Swap nodes on priority stack */
STACKNODETYPE *preptr;		/* < node preceeding pair to be swapped */
STACKNODETYPE *ptr1;		/* < first node in pair */
STACKNODETYPE *ptr2;		/* < second node in pair */
int stacknum;			/* < which stack */
/*
** affects: priority stacks
** returns: number in stack "stacknum"
*/
{
  extern STACKNODETYPE *stack;	        /* global ptr to top of stacks */

  if ((ptr1!=NULL)&&(ptr2!=NULL)) {
    preptr->next = ptr2;
    ptr1->next = ptr2->next;
    ptr2->next = ptr1;
  }

  return(stack[stacknum].tag);		/* Return number in stack */
} /* ***************************************************************** */



find(addrtag,setsize,stacknum,preptrptr,ptrptr) /* Find address in stack. */

register int addrtag;		/* <  address tag being looked for */
int setsize;			/* <  associativity */
int stacknum;			/* <  which stack */
STACKNODETYPE **preptrptr;	/* <> points to node preceeding found node */
STACKNODETYPE **ptrptr;		/* <> points to found node (NULL otherwise) */
/*
** affects: none
** returns: stackdepth if found
**	    NULL otherwise
*/
{
  extern STACKNODETYPE *stack;	/* global ptr to top of stacks */
  register int stackdepth;
  register STACKNODETYPE *preptr,*ptr;

  preptr = &stack[stacknum];
  ptr = stack[stacknum].next;

  for (stackdepth = 1;
       ((ptr != NULL)
	&& ( (ptr->tag != addrtag) || (ptr->pid != currpid) )
	&& (stackdepth <= setsize)); 
       stackdepth++) {
    preptr = ptr;
    ptr = ptr->next;
  }

  if ( (ptr != NULL) && (ptr->tag == addrtag) && (ptr->pid == currpid) ) {
    *preptrptr = preptr;	/* found */
    *ptrptr = ptr;
    return(stackdepth);
  }
  else {				/* free balance of stack if there  */
    if ((ptr != NULL)	                /*  are FREETHRESHOLD nodes to free */
	&& (stack[stacknum].tag - setsize >= FREETHRESHOLD))
      putonfreelist(stacknum,preptr);
    *preptrptr = *ptrptr = NULL;	/* not found */
    return(0);
  }
} /* ***************************************************************** */



STACKNODETYPE *findnth(stacknum,n)	/* Find nth address in stack. */

int stacknum;			/* <  which stack */
int n;				/* <  index of record */
/*
** returns: ptr to n-th record in stack; the 1st record is the
**	    one pointed to by the header. If the stack has less
**	    than n reocrds, NULL is returned.
*/
{
  extern STACKNODETYPE *stack;	/* global ptr to top of stacks */
  register int stackdepth;
  register STACKNODETYPE *ptr;

  ptr = stack[stacknum].next;

  /*
   *	Less than n in stack?
   */
  if (stack[stacknum].tag < n) {
    ptr = NULL;
  }
  else {
    for (stackdepth = 1; stackdepth < n; stackdepth++) {
      ptr = ptr->next;
    }
  }

  return(ptr);
} /* ***************************************************************** */



putonfreelist(stacknum,last)	/* Move nodes from prio stack to free list */
int stacknum;			/* <  */
register STACKNODETYPE *last;	/* <  */
/*
** affects: stack[stacknum], freelist
** returns: number of nodes freed 
*/
{
extern STACKNODETYPE *stack;	/* global ptr to top of stacks */
extern STACKNODETYPE freelist;	/* List head for free list */
STACKNODETYPE *first;
register int numtofree;
/*
**	Remove Balance of list from stack & get ready to add to
**	freelist.
*/
if (last->next==NULL)	/* Any to free? */
	return(0);
else {
	first = last->next;
	last->next = NULL;
	last = first;
	/*
 	*	See how many nodes to free.
 	*/
	for (numtofree = 1; last->next!=NULL; numtofree++)
		last = last->next;
	/*
 	*	Put on free list. Adjust counts.
 	*/
	last->next = freelist.next;
	freelist.next = first;
	stack[stacknum].tag -= numtofree;
	freelist.tag += numtofree;
	
	return(numtofree);
 }
} /* ***************************************************************** */

void *calloc(size_t nmemb, size_t size);

STACKNODETYPE *makenode(Tag,Blockaddr,Pid)  /* Returns node from buffer. */

int Tag;
int Blockaddr;
int Pid;
{
  // extern char *calloc();

  static STACKNODETYPE *buffer;
  extern int bufferindex;		/* global index to buffer */
  extern STACKNODETYPE freelist;	/* List head for free list */
  extern int numnodes;		        /* Count on storage allocated */
  STACKNODETYPE *ptr;
  /*
   *	Allocate node from freelist or buffer; if buffer is used up,
   *	then get more.
   */
  if ((ptr = freelist.next) != NULL) {
    freelist.next = ptr->next;	        /* Allocate from freelist */
    freelist.tag--;
  }
  else {		                /* Allocate from buffer */
    if (bufferindex >= BUFFERSIZE) {
      bufferindex = 0;	                /* buffer used up */

      buffer = (STACKNODETYPE *)calloc(BUFFERSIZE,SIZEOFSTACKNODETYPE);

      if (buffer == NULL) {
	printf("dinero: calloc() failed.\n");
	fflush(stdout);                 /* Paraniod flush of stdout buffer */
	fprintf(stderr,"dinero: calloc() failed.\n");
	fflush(stderr);                 /* Paraniod flush of stderr buffer */
	exit(ERR);
      }
      numnodes += BUFFERSIZE;		/* Keep track of storage */
    }
    ptr = buffer + (bufferindex++);	/* Allocate */
  }

  /*	Initialize node 
   */
  ptr->tag = Tag;
  ptr->blockaddr = Blockaddr;
  ptr->pid = Pid;
  ptr->next = NULL;
  
  return(ptr);
} /* ***************************************************************** */



copybackstack(cachep,ctrlp,metricp,stacknum)	/* Copies back dirty blocks */

CACHETYPE *cachep;
CTRLTYPE *ctrlp;
METRICTYPE *metricp;
int stacknum;			/* <  which stack */
/*
 */
{
  extern STACKNODETYPE *stack;	/* global ptr to top of stacks */
  register int stackdepth;
  register int numinstack;		
  register STACKNODETYPE *ptr;

  ptr = stack[stacknum].next;

  /*	How many in stack?
   */
  numinstack = ((stack[stacknum].tag < cachep->assoc) 
		  ? stack[stacknum].tag : cachep->assoc);

  for (stackdepth = 0; stackdepth < numinstack; stackdepth++) {
    if (cachep->subblocksize == 0) {
      /*  No sub-blocks 
       */
      if (ptr->dirty != NOTDIRTY) {
	/*  Write dirty block
	 */
	if (ctrlp->output == BUS_TRANSP) {
	  transpbustraffic(XWRITE, ptr->blockaddr,
			   metricp, cachep->transfersize);
	}
	else {
	  bustraffic(ctrlp->output, metricp, cachep->transfersize, BUSWRITE,
		     ptr->blockaddr);
	}
      }
      /*  Snoop announces clean block replaced.
       */
      else {
	bustraffic(ctrlp->output, metricp, cachep->transfersize, BUSSNOOP,
		   ptr->blockaddr);
      }
    }
    /*  Sub-blocks
     */
    else {
      replacesubblocks(ctrlp, metricp, cachep, ptr);
    }

    /*	Invalidate block and go on to next one.
     */
    ptr->dirty = NOTDIRTY;
    ptr->valid = NOTVALID;
    ptr->reference = NOTREFERENCED;

    ptr = ptr->next;
  }

} /* ***************************************************************** */



busupdate(cachep,policyp,ctrlp,metricp,  	/* Update bus traffic stuff. */
	  dap,miss,blockmiss,replacedptr)
CACHETYPE *cachep;		/* <  */
POLICYTYPE *policyp;		/* <  */
CTRLTYPE *ctrlp;		/* <  */
METRICTYPE *metricp;		/* <> */
register DECODEDADDRTYPE *dap;	/* <  */
int miss;			/* <  */
int blockmiss;			/* <  */
STACKNODETYPE *replacedptr;	/* <  */
{

  /* 
   **  Increment Rcount for all demand fetches.
   **  Increment Icount for all demand instruction fetches.
   */
  if (DEMANDFETCHP(dap->accesstype)) metricp->Rcount++;

  if (dap->accesstype == XINSTRN) metricp->Icount++;

  /*
   **	Handle reference.
   */
  /*
   **	Not a write?
   */
  if (dap->accesstype != XWRITE) {  /* A non-write hit? */
    if (! miss) {
      /* check for special accesses */
      if (dap->accesstype == XFLUSH || dap->accesstype == XSWITCH) {
	if (ctrlp->output == BUS_TRANSP) {
	  transpbustraffic(dap->accesstype, dap->address,
			   metricp, WORDSIZE);
	}
	else {
	  bustraffic(ctrlp->output, metricp, WORDSIZE,
		     BUSCONTROL, dap->address);
	}
      }
    }
    else {  /* a [read | ifetch] miss -- (pre-)fetch (sub-)block */
      if (ctrlp->output == BUS_TRANSP) {
	transpbustraffic(dap->accesstype, dap->blockaddr,
			 metricp, cachep->transfersize);
      }
      else {
	if (DEMANDFETCHP(dap->accesstype)) {
	  bustraffic(ctrlp->output, metricp, cachep->transfersize,
		     BUSREAD, dap->blockaddr);
	}
	else { /* a prefetch */
	  bustraffic(ctrlp->output, metricp, cachep->transfersize,
		     BUSPREFETCH, dap->blockaddr);
	}
      }
    }
  }
/*
**	A write:
**	(CB==copy-back, WT==write-through)
**
**		  write-allocate		no-write-allocate
**		CB		WT		CB		WT
**		--		--		--		--
**	Hit	dirty		w-word		dirty		w-word
**
**	Miss	r-block		r-block	
**		dirty		w-word		w-word		w-word
*/
  else {
    if (! miss) {  /* A write HIT ? */
      if (policyp->write == WRITETHROUGH) {
	/* write word */
	if (ctrlp->output == BUS_TRANSP) {
	  transpbustraffic(XWRITE, dap->address,
			   metricp, WORDSIZE);
	}
	else {
	  bustraffic(ctrlp->output, metricp, WORDSIZE,
		     BUSWRITE, dap->address);
	}
      }
      else { /* copy-back */
	/* block (re)marked dirty in "update" */
      }
    }
    /*
     **	
     */
    else {  /* A write MISS ? */
      if (policyp->writeallocate == WRITEALLOCATE) {
	/* read (sub)-block */
	if (ctrlp->output == BUS_TRANSP) {
	  transpbustraffic(XREAD, dap->blockaddr,
			   metricp, cachep->transfersize);
	}
	else {
	  bustraffic(ctrlp->output, metricp, cachep->transfersize,
		     BUSREAD, dap->blockaddr);
	}
      }
      if ((policyp->write == WRITETHROUGH) || 
	  (policyp->writeallocate == NOWRITEALLOCATE)) {
	/* write word */
	if (ctrlp->output == BUS_TRANSP) {
	  transpbustraffic(XWRITE, dap->address, metricp, WORDSIZE);
	}
	else {
	  bustraffic(ctrlp->output, metricp, WORDSIZE,
		     BUSWRITE, dap->address);
	}
      }
      else {
	/* block marked dirty in "update" */
      }
    }
  }
  /*
   **  If something replaced
   */
  if ((blockmiss) && (replacedptr != NULL)) {
    if (cachep->subblocksize == 0) { /* No sub-blocks */
      if (replacedptr->dirty != NOTDIRTY) { /* Write dirty block */
	if (ctrlp->output == BUS_TRANSP) {
	  transpbustraffic(XWRITE, replacedptr->blockaddr,
			   metricp, cachep->transfersize);
	}
	else {
	  bustraffic(ctrlp->output, metricp,
		     cachep->transfersize, BUSWRITE,
		     replacedptr->blockaddr);
	}
      }
      /*
       **  Snoop announces clean block replaced.
       */
      else {
	bustraffic(ctrlp->output, metricp, cachep->transfersize, BUSSNOOP,
		   replacedptr->blockaddr);
      }
    }
    else { /* Sub-blocks */
      replacesubblocks(ctrlp, metricp, cachep, replacedptr);
    }
    /*
     **	Zero out what was replaced.
     */
    replacedptr->dirty = NOTDIRTY;
    replacedptr->valid = NOTVALID;
    replacedptr->reference = NOTREFERENCED;
  }
} /* ***************************************************************** */



replacesubblocks(ctrlp,metricp,cachep,aptr)

CTRLTYPE *ctrlp;		/* <  */
METRICTYPE *metricp;		/* <> */
CACHETYPE *cachep;		/* <  */
STACKNODETYPE *aptr;
{
unsigned int validmask;
unsigned int dirtymask;
int subblockaddr;

/*
**  PERFORMANCE HACK: If not (-o3 or -o4) and not dirty, then return.
*/
if ( (aptr->dirty == NOTDIRTY)
    && ((ctrlp->output != BUS_TRANSP) || (ctrlp->output != BUS_SNOOP)) ) {
  return;
}

validmask = aptr->valid;
dirtymask = aptr->dirty;
subblockaddr = aptr->blockaddr;

while (validmask != NOTVALID) {
  /*
   ** For a valid sub-block
   */
  if (validmask & LOWVALIDMASK) {
    if (dirtymask & LOWDIRTYMASK) {
      /*
       **  Write dirty sub-block
       */
      if (ctrlp->output == BUS_TRANSP) {
	transpbustraffic(XWRITE, subblockaddr,
			 metricp, cachep->transfersize);
      }
      else {
	bustraffic(ctrlp->output, metricp, cachep->transfersize,
		   BUSWRITE, subblockaddr);
      }
    }
    else {
      /*
       **  Snoop announces clean sub-block replaced.
       */
      bustraffic(ctrlp->output, metricp,
		 cachep->transfersize, BUSSNOOP, subblockaddr);
    }
  }
  validmask >>= 1;
  dirtymask >>= 1;
  subblockaddr += cachep->transfersize;
}

}/* ***************************************************************** */



bustraffic(				/* put stuff on memory bus. */
	   output_option,metricp,size,access,addr)
/*
**	Called for every bus transaction and every (sub)-block
**	replaced.
*/
int output_option;
METRICTYPE *metricp;		/* <> */
int size;
char access;
int addr;
{

switch (access) {

case BUSREAD :
case BUSPREFETCH :
	metricp->bus_traffic_in  += size;
	if (output_option >= BUS) {	/* BUS or BUS_SNOOP */
		fprintf(stdout, "BUS2 %c %d %x ", access, size, addr);
		fprintf(stdout, "%d %d\n", metricp->Rcount, metricp->Icount);
		metricp->Icount = metricp->Rcount = 0;
 	}
	break;

case BUSWRITE :
	metricp->bus_traffic_out += size;
	if (output_option >= BUS) {	/* BUS or BUS_SNOOP */
		fprintf(stdout, "BUS2 %c %d %x ", access, size, addr);
		fprintf(stdout, "%d %d\n",metricp->Rcount, metricp->Icount);
		metricp->Icount = metricp->Rcount = 0;
 	}
	break;

case BUSCONTROL :
	if (output_option >= BUS) {	/* BUS or BUS_SNOOP */
		fprintf(stdout, "BUS2 %c %d %x ", access, size, addr);
		fprintf(stdout, "%d %d\n",metricp->Rcount, metricp->Icount);
		metricp->Icount = metricp->Rcount = 0;
 	}
	break;

case BUSSNOOP :
	if (output_option == BUS_SNOOP) { /* BUS_SNOOP only */
		fprintf(stdout, "BUS2 %c %d %x ", access, size, addr);
		fprintf(stdout, "%d %d\n",metricp->Rcount, metricp->Icount);
		metricp->Icount = metricp->Rcount = 0;
 	}
	break;

default :
	fprintf(stdout,
		"\n---Error in bustraffic() access= %d.\n", access);
	break;
 }

} /* ****************************************************************** */


transpbustraffic(access, addr, metricp, size) /* put stuff on memory bus. */
/*
**	Called for every bus transaction and every (sub)-block
**	replaced.  Whenever block is loaded into the cache or
**      (the whole block) written to memory, all the accesses
**      are generated.
*/
int access;
int addr;
METRICTYPE *metricp;		/* <> */
int size;
{
  unsigned displ, start, rest;

  start = addr;
  displ = addr % size ;

  switch (access) {

    case XREAD :               /* accesses to word-aligned locations */
    case XWRITE :
    case XINSTRN :
      metricp->bus_traffic_in  += size;
      for (rest = size ; rest >= WORDSIZE; rest -= WORDSIZE) {
	fprintf(stdout, "%d %x\n", access, addr);
	if ( (addr+WORDSIZE) % size == 0 ) { addr = start - displ; }
	else { addr += WORDSIZE; }
      }
      metricp->Icount = metricp->Rcount = 0;
      break;

    case XMISC :
    case XFLUSH :
    case XSWITCH:
      fprintf(stdout,"%d %x ---Special access\n", access, addr);
      break;

    default :
      fprintf(stdout,
	      "%d %x ---Error in dinero's transpbustraffic() ???\n",
	      access, addr);
      break;
    }
}
