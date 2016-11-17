/************************************************************************
 *
 *	sched -- a realistic trace generator
 *
 *      Roberto Hexsel
 *      Dept of Computer Science
 *      Edinburgh University
 *      rh@dcs.ed.ac.uk
 *                                                 Source File:  pipes.c
 *
 ************************************************************************/

/************************************************************************
 * CHANMGES:
 * 7/4/92  make sure that pclose() catches misbehaved processes at the
 *         other end of the pipes.
 ************************************************************************/

#include "decls.h"

void exit(int status);

openpipes(pipesp)        /* open all pipe connections to trace generators */ 
PIPESCTRLTYPE *pipesp;   /* <> */
{
  char *pipe;
  int i;

  for (i = 0; i < pipesp->numproc; i++) {
    pipe = pipesp->procstrings[i];
    if ( (pipesp->ifp[i] = popen(pipe, "r")) == NULL) {
      fprintf(stderr,
	      "sched -- cannot open: %s\n",
	      pipe);
      exit(ERR);
    }
  }

} /* ****************************************************************** */


closepipes(pipesp)       /* close all pipe connections to trace generators */ 
PIPESCTRLTYPE *pipesp;   /* <> */
{
  int i; int c;

  for (i = 0; i < pipesp->numproc; i++)
/***    if ((c = pclose(pipesp->ifp[i])) != 0)
 *         fprintf(stderr,
 *		   "sched -- cannot close: *%s* c: %d\n",
 *		   pipesp->procstrings[i], c);
 ***/
    pclose(pipesp->ifp[i]);

} /* ****************************************************************** */



FILE *restartinp(pipesp) /* restart the pipe whose trace run out */
PIPESCTRLTYPE *pipesp;   /* <> */
{
  char *pipe;
  int pid;

  pid = pipesp->currpid;

  if (pclose(pipesp->ifp[pid]) != 0) {
    fprintf(stderr,
	    "sched -- cannot close for re-opening: %s\n",
	    pipesp->procstrings[pid]);
    exit(ERR);
  }
  pipe = pipesp->procstrings[pid];
  if ( (pipesp->ifp[pid] = popen(pipe, "r")) == NULL) {
    fprintf(stderr,
	    "sched -- cannot re-open: %s\n",
	    pipe);
    exit(ERR);
  }
  return(pipesp->ifp[pid]);

}
