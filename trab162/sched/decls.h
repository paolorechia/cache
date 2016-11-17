/************************************************************************
 *
 *	sched -- a realistic trace generator
 *
 *      Roberto Hexsel
 *      Dept of Computer Science
 *      Edinburgh University
 *      rh@dcs.ed.ac.uk
 *                                            Source File:  decls.h
 *
 ************************************************************************/


#include <stdio.h>

#define OK    0
#define ERR   1
#define TRUE  1
#define FALSE 0
#define ILLEGAL    '?'
#define ILLEGALNUM -1

#define YES 1
#define NO  0

/********* for testing only ********
 *#define MAXCOUNT 150
 *#define QUANTUM  5  
 *#define MAXNUMPIDS  4
 ********/

#define MAXCOUNT   2000000000  /* default */
#define QUANTUM        100000  /* default */
#define MAXNUMPIDS          8


#define XFLUSH   4     /* escape command for tlb & cache -- flush */
#define XSWITCH  5     /* escape command for tlb & cache -- context switch */


typedef struct schedctrltype {          /* CTRL holds flags to turn       */
                                        /* control options on and off     */
  int maxcount;
  int tracecount;
  int quantum;
  int flag_ctxtsws;
  int resch_on_eot;
  int flush_on_ctxtsw;
  int variable_quantum;
} SCHEDCTRLTYPE;


#define MAXSTRINGLENGHT 84

typedef struct pipesctrltype {
  int numproc;
  int currpid;
  FILE *ifp[MAXNUMPIDS];
  char procstrings[MAXNUMPIDS][MAXSTRINGLENGHT];
} PIPESCTRLTYPE;
