/************************************************************************
 *
 *	sched -- a realistic trace generator
 *
 *      Roberto Hexsel
 *      Dept of Computer Science
 *      Edinburgh University
 *      rh@dcs.ed.ac.uk
 *                                            Source File:  cmdargs.c
 *
 ************************************************************************/

/* 
 *  Copyright 1985, 1989 Mark D. Hill
 *
 *  Permission to use, copy, modify, and distribute this
 *  software and its documentation for any purpose and without
 *  fee is hereby granted, provided that the above copyright
 *  notice appear in all copies.  Mark D. Hill makes no
 *  representations about the suitability of this software
 *  for any purpose.  It is provided "as is" without expressed
 *  or implied warranty.
 */  

/*
 *  cmdargs.c was adapted from Mark Hill's Dinero Cache Simulator.
 */

#include "decls.h"



getcmdargs(argc,argv,pipesp,ctrlp)   /* Analyze command line arguments. */

register int argc;		/* <  */
register char *argv[];		/* <  */
PIPESCTRLTYPE *pipesp;          /* <> */
SCHEDCTRLTYPE *ctrlp;           /* <> */
/* returns: OK if command line arguments are valid; ERR otherwise
 */
{
  extern char *strcpy();

  register int i;
  register int nextnum;
  int returnflag;
  char nextchar;

  returnflag = OK;

  /* defaults */

  ctrlp->maxcount = MAXCOUNT;
  ctrlp->tracecount = 0;
  ctrlp->quantum = QUANTUM;
  ctrlp->flag_ctxtsws = NO;
  ctrlp->resch_on_eot = NO;
  ctrlp->flush_on_ctxtsw = NO;
  ctrlp->variable_quantum = NO;

  pipesp->numproc = 0;

  /*	Get arguments 
   */
  for (i = 1; i < argc; i++) 
    if (argv[i][0] != '-') {
      fprintf(stderr,
	      "sched: Argument %d (%s) must be preceded by '-'.\n",i,argv[i]);
      returnflag = ERR;
    }
    else {
      switch (argv[i][1]) {	/* switch on second char. */
	          		/* allow: "-m100" or "-m 100" */

	  /* CONTROL
	   */
	case 'm' :              /* max number of events */
	  nextnum = atoiKMG(argv[i][2]=='\0' ? argv[++i] : &argv[i][2]);
	  if (nextnum >= 1 )
	    ctrlp->maxcount = nextnum;
	  else {
	    ctrlp->maxcount = ILLEGALNUM;
	    returnflag = ERR;
	  }
	  break;

	case 'q' :             /* count of events between ctxt switches */
	nextnum = atoiKMG(argv[i][2]=='\0' ? argv[++i] : &argv[i][2]);
	if (nextnum >= 0 )
		ctrlp->quantum = nextnum;
	else {
		ctrlp->quantum = ILLEGALNUM;
		returnflag = ERR;
	}
	break;

	case 'c' :             /* emit an XSWITCH on a ctxt switch ??? */
	  ctrlp->flag_ctxtsws = YES;
	  break;

	case 'f' :             /* emit an XFLUSH on a ctxt switch ??? */
	  ctrlp->flush_on_ctxtsw = YES;
	  break;

	case 'r' :             /* reschedule when trace runs out ??? */
	  ctrlp->resch_on_eot = YES;
	  break;

	case 'v' :             /* variable num events between ctxt sw's ?? */
	  ctrlp->variable_quantum = YES;
	  break;

	  /*
           * PIPES
	   */
	case 'p' :
	  if (argv[i][2] == '\0') {
	    strcpy(pipesp->procstrings[pipesp->numproc], argv[++i]);
	  }
	  else {
	    strcpy(pipesp->procstrings[pipesp->numproc], &argv[i][2]);
	  }
	  ++pipesp->numproc;
	  if ( (pipesp->numproc > MAXNUMPIDS)
	      || (pipesp->numproc < 1) ) {
	    returnflag = ERR;
	  }
	  break;

	default :
	  fprintf(stderr,"sched: Argument %d is invalid: %s\n",i,argv[i]);
	  returnflag = ERR;
	  break;
	}
    }

  returnflag = errormessage(returnflag,
			    (returnflag == ERR),
			    "Illegal parameter or value.");

  returnflag = errormessage(returnflag,
			    (ctrlp->maxcount == ILLEGALNUM),
			    "Maximum event count not correctly specified.");

  returnflag = errormessage(returnflag,
			    (ctrlp->quantum == ILLEGALNUM),
			    "Quantum not correctly specified.");

  returnflag = errormessage(returnflag,
			    (ctrlp->quantum >= ctrlp->maxcount),
			    "Quantum must be smaller than max event count.");

  returnflag = errormessage(returnflag,
			    ((pipesp->numproc < 1)
			     || (pipesp->numproc > MAXNUMPIDS)),
			    "Too few or too many input traces.");

  return(returnflag);

} /* ************************************************************ */



errormessage(olderrorflag,condition,message)	/* Prints error message */
int olderrorflag;
int condition;
char *message;
{
  int newerrorflag;

  newerrorflag = olderrorflag;

  if (condition) {
    fprintf(stderr,"sched: %s\n", message);
    newerrorflag = ERR;
  }

  return(newerrorflag);
} /* ************************************************************ */



warningmessage(olderrorflag,condition,message)	/* Prints error message */
int olderrorflag;
int condition;
char *message;
{

  if (condition) {
    fprintf(stderr,"sched WARNING: %s\n", message);
  }

  return(olderrorflag); /* warning does not affect errorflag */
} /* ************************************************************ */



int atoiKMG(str)
char *str;
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
size_t strlen(const char *s);
// extern int strlen();
extern int atoi();

strcpy(str_buffer,str);

length = strlen(str_buffer);
suffix = str_buffer[length-1];

switch (suffix) {

case 'K' :
case 'k' :
	multiplier = 1024;
	str_buffer[length-1] = '\0';
	break;

case 'M' :
case 'm' :
	multiplier = 1048576;		/* 1024**2 */
	str_buffer[length-1] = '\0';
	break;

case 'G' :
case 'g' :
	multiplier = 1073741824;	/* 1024**3 */
	str_buffer[length-1] = '\0';
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

