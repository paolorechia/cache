/************************************************************************
 *
 *	sched -- a realistic trace generator
 *
 *      Roberto Hexsel
 *      Dept of Computer Science
 *      Edinburgh University
 *      rh@dcs.ed.ac.uk
 *                                                 Source File:  main.c
 *
 ************************************************************************/

/*************************************************************************
 * CHANGES:
 * 23/3/92 added command line option for variable size quantum
 * 7/4/92  make sure that pclose() catches misbehaved processes at the
 *         other end of the pipes.
 ************************************************************************/

#include "decls.h"

void exit(int status);

main(argc,argv)         /* sched */
int argc;               /* <  */
char *argv[];           /* <  */
{
  int count;           /* how many input records copied ? */
  char *str;
  char linebuffer[128];

  SCHEDCTRLTYPE ctrl;
  PIPESCTRLTYPE pipes;

  char *fgets();   /* fgets() does not remove the \n it finds on the input */

  int quantum;
  extern srand();
  extern int rand();

  FILE *input;
  FILE *restartinp();
  extern openpipes();
  extern closepipes();

  /*      Exit with extra messages if there are no args.
   */
  if (argc == 1) {
    fprintf(stderr,
	"usage: sched -p'zcat trace.Z' [-mNNN] [-qNN] [-c] [-f] [-r] [-v]\n");
    exit(ERR);
  }

  init(argc, argv, &ctrl, &pipes);
  openpipes(&pipes);

  input = pipes.ifp[0];       /* start reading from first trace */
  count = 0;
  pipes.currpid = 0;

  ctrl.tracecount = 0;
  quantum = ctrl.quantum;
  srand(time(NULL));

  /* send out pid for first process */
  if (ctrl.flag_ctxtsws) {
    fprintf(stdout, "%d %x   -- context switch\n", XSWITCH, pipes.currpid);
  }

  while (ctrl.tracecount < ctrl.maxcount) {

    while ( (str = fgets(linebuffer, 128, input)) == NULL ) {
      /* run out of input; restart file */
      input = restartinp(&pipes);

      if (ctrl.resch_on_eot) {
	reschedule(&ctrl, &pipes, &input, &quantum, &count);
      }
    }

    /* output record & check for context switch */
    fprintf(stdout, "%s", str);

    if (count >= quantum) {       /* quantum expired, reschedule! */
      reschedule(&ctrl, &pipes, &input, &quantum, &count);
    }
    else {
      count++;
    }
    ctrl.tracecount++;
  }

  closepipes(&pipes);

} /* ***************************************************************** */


reschedule(ctrlp,pipesp,inputp,quantump,countp)      /* round-robin */
SCHEDCTRLTYPE *ctrlp;       /* <  */
PIPESCTRLTYPE *pipesp;      /* <> */
FILE **inputp;              /* <> */
int *quantump;              /* <> */
int *countp;                /* <> */
{
  extern int rand();

  *countp = 0;
  if (ctrlp->variable_quantum)
    *quantump = (int)(ctrlp->quantum * 0.75) + (rand() % (ctrlp->quantum / 2));

  pipesp->currpid = (pipesp->currpid + 1) % pipesp->numproc;

  *inputp = pipesp->ifp[pipesp->currpid];

  if (ctrlp->flag_ctxtsws) {
    fprintf(stdout, "%x %x   -- context switch\n", XSWITCH, pipesp->currpid);
  }

  if (ctrlp->flush_on_ctxtsw) {
    fprintf(stdout, "%x %x   -- flush\n", XFLUSH, pipesp->currpid);
  }

} /* ***************************************************************** */


init(argc,argv,ctrlp,pipesp)
int argc;                       /* <  */
char *argv[];                   /* <  */
SCHEDCTRLTYPE *ctrlp;           /* <> */
PIPESCTRLTYPE *pipesp;          /* <> */
{
  extern int getcmdargs();

  int flag;
  int i, j;

  pipesp->numproc = 0;
  pipesp->currpid = 0;
  for (i = 0; i < MAXNUMPIDS; i++) {
    pipesp->ifp[i] == NULL;
    for (j = 0; j < MAXSTRINGLENGHT; j++)
      pipesp->procstrings[i][0] = '\0';
  }

  /*      Get command line arguments and echo them. Illegal entries
   *      cause premature termination.
   */

  flag = getcmdargs(argc, argv, pipesp, ctrlp);

  if (flag != OK) {               /* premature termination */
    fprintf(stderr, "sched: FATAL ERROR -- execution terminated.\n\n");
    exit(ERR);
  }

} /* ***************************************************************** */

