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
**	Source File:	doc.h
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
 * 26/2/92 added command line option -Ooutfile to allow for statistics
 *         to be written to a file rather than to stdout;
 *         all messages (error or otherwise) are written to stderr,
 *         bus output is always sent to stdout.
 * 27/2/92 added command line option -o4 to make bus output in a format
 *         identical to the input format (i.e. one LABEL and ADDRESS (hex)
 *         per line).
 * 17/3/92 added multiprogramming & context switch special input record.
 ************************************************************************/

/* 
**  INPUT FORMAT:
**
**	Read in ASCII from standard input 
**	Expect ONE LABEL and ADDRESS in hex per line.
**	Rest of data input line is ignored so it may
**	be used for comments.
**
**	LABEL =	0	read data
**		1	write data
**		2	instruction fetch
**		3	escape record (treated as unknown access type)
**		4	escape record (causes cache flush)
**              5       escape record (context switch: addr field is new pid)
**
**	0x0 <= ADDRESS <= 0xffffffff
**
**  E.G.:
**	0	0		read data from addr 0x0.
**	2	ffffffff	instruction fetch from 0xffffffff.
**	4	12		flush cache and record miss type UNK
**	1 123
**	     1 2
**
**  NOT OK:
**	<blank line>
**	0x0 0x1
**	0  
**	1  comment
**
** ------------------------------------------------------------
**  WARNING: If more than one tuple is put on a line,
**	all but the first tuple will be ignored.
** ------------------------------------------------------------
*/
/* **********************************************************************
**
**  COMMAND LINE OPTIONS
**
**	J  = {0,1,2,...}
**	J+ = {1,2,3,...}
**	P+ = {2^x | x is an element of J}
**
**
**  -x	NAME		VARIABLE		OPTIONS(* ==> default)	
**  --------		--------		----------------------
**  -b	blocksize	cachep->blocksize	J+ (no default)
**	in bytes                                See below.
**
**  -S	sub-block size	cachep->subblocksize	J (default: 0 (no sub-blocks))
**	in bytes                                See below.
**
**  -u  unified cache size cachep->Dsize	J * blocksize (default: 0
**	in bytes				(no unifed cache))
**  
**  -d  data cache size	cachep->Dsize		J * blocksize (default: 0
**	in bytes				(no data cache))
**  
**  -i  instruction cache size	cachep->Isize	J * blocksize (default: 0
**	in bytes				(no instruction cache))
**						Note either -u or both
**						-i and -d must be positive.
**  
**  -a  associativity	cachep->assoc		J+ (default: 1 (direct-mapped))
**
**-----------------------------------------------------------------------
**
**  -r	replacement	policyp->replacement	LRU* ('l'),
**						FIFO ('f'),
**						RANDOM ('r').
**
**  -f	fetch		policyp->fetch		DEMAND* ('d'),
** 						ALWAYSPREFETCH ('a'),
** 						MISSPREFETCH ('m'),
** 						TAGGEDPREFETCH ('t'),
** 						LOADFORWARDPREFETCH ('l'),
**					(don't prefetch into next block)
** 						SUBBLOCKPREFETCH ('S'),
**					(wrap around w/i current block)
**
**
**  -p prefetch distance policyp->prefetchdist	J+ (default: 1 (sub)-block)
**     in (sub)-blocks
**
**  -P abort prefetch 	policyp->abortprefetchpercent  0<= J <= 100
**     percent					(default: 0, no prefetches
**						aborted)
**
**  -w	write		policyp->write		COPYBACK* ('c'),
**						WRITETHROUGH ('w').
**
**  -A	writeallocate	policyp->writeallocate	WRITEALLOCATE* ('w'),
**						NOWRITEALLOCATE ('n').
**
**-----------------------------------------------------------------------
**
**  -D	debug flag	ctrlp->debug		0 ==> NODEBUG*,
**						1 ==> DEBUG1 (dumps stacks),
**						2 ==> DEBUG2 (dumps addrs, 
**						      stacks, and metrics).
**
**  -o 	output length	ctrlp->output		0 ==> TERSE*,
**						1 ==> VERBOSE (dumps stats
**						      every 0.5M addrs), 
**						2 ==> BUS (see below for
**						documentation on bus records).
**						3 ==> BUS_SNOOP (see below for
**						documentation on bus records).
**						4 ==> BUS_TRANSP (see below for
**						documentation on bus records).
**
**  -Z	skip count	ctrlp->skipcount	J (default: 0 ==> do not skip
**						  		any addresses)
**
**  -z	maximum count	ctrlp->maxcount		J+ (default: 10**7)
**
**  -Q	mean inter-task	ctrlp->Q		J (default: 0 ==> no flushing)
** 	switch time for flush
**
**  -O filename         ctrlp->ofp              output into filename,
**                                              if undefined, to stdout.
**                                              Must be used when using -o4.
**
**
**  Note: the maximum number of subblocks is 32.  There are combinations
**        of block and sub-block sizes (e.g. blocks of size 128 and subblock
**        of size 2) that cause dinero to produce no results at all
**        without crashing).  Also, both blocks and subblocks must be
**        at least as large as the word size (4 bytes).
**
** ******************************************************************* */


/* ******************************************************************* **
**
**  BUS RECORD format generated by output option 2 or 3 (-o2 or -o3) 
**  used by packbus.c and other programs (ascii, all numbers in hex).
**
**	BUS2 Access Size Address Rcount Icount 
**
**	where:
**
**	BUS2 	is a literal character string; all lines that do not
**		start with BUS2 are ignored.
**
**	Access	is a access code (r==BUSREAD, w==BUSWRITE, p==BUSPREFETCH,
**		and s==BUSSNOOP (-o3 only)).
**
**		In COPYBACK, misses can cause dirty blocks to be written
**		back. When this happens, a bus read followed by a bus
**		write is reported (as if the cache has write buffers). 
**		The bus write follows the bus read with a Rcount and 
**		an Icount of 0.
**
**	Address	(hex) is address of bus transfer (0 <= Address <= 0xffffffff).
**
**	Size	(decimal) is the transfer size in bytes.
**
**	Rcount	(decimal) is count of all references since the last miss
**		including the current reference. E.g., the second of
**		two consecutive misses will have an Rcount of 1.
**
**	Icount	(decimal) is count of instruction fetches since the last miss
**		including the current reference. E.g., the second of
**		two consecutive instruction fetch misses will have an 
**		Icount of 1.
**
**  BUS RECORD format generated by output option -o4 is identical to
**  the input format (i.e. one LABEL and ADDRESS (hex) per line).  This mode
**  should be used only if piping the output into another process.
**  Escape records at input (XMISC, XFLUSH, XSWITCH) are forwarded to
**  the output unchanged.  With option -o4, (sub-)block loads (or flushes)
**  generate all the bus accesses to "fill" the block (i.e. accesses start
**  at the location that missed then wrap around to the address of the first
**  word of the block).
**
** ******************************************************************* */



/* END OF USER DOCUMENTATION */



/* *******************************************************************

File Hierarchy:


      doc.h			main.c			global.h
				  |
				  |
	+---------------+---------+---------+---------------+
	|		|	 	    |		    |
     init.c	     fetch.c		 state.c	 output.c
	|
	|
    cmdargs.c



******************************************************************* */



/* *******************************************************************

Cross Reference:


doc.h		Global documentation

global.h	Type and constant definitions 

main.c		Executive and main loop
		main, mainloop

init.c		Initialize
		init, initmisc, initstacks, initmetric

cmdargs.c	Process command line
		getcmdargs, errormessage, echocmdargs

fetch.c		Get next addresses; organize prefetch
		fetch, prefetch, readfrominputstream, breakupaddr, flushcache,
		init_addrstack, push_addrstack, pop_addrstack, print_addrstack,
		sscanxx

state.c		Manage priority stacks
		update, stackupdate, push, remove, movetotop, swap, find, 
		findnth, putonfreelist, makenode, copybackstack, busupdate,
		replacesubblocks, bustraffic, transpbustraffic

output.c	All output routines
		outputmetric, dumpaddr, dumpstate, dumpmetric

******************************************************************* */

/* *******************************************************************

Organizational Chart:

                               main
                                |
           +--------------------+----+-----------------+
           |                         |                 |
         init                     mainloop        outputmetric
           |                         |
      +----+-+---+    +-------+------+---+-----------------+
      |      |   |    |       |          |                 |
echocmdargs, |   |  fetch     |       update            dumpaddr,
initmetric,  |   |    |       |          |              dumpstate
initstacks   |   |   ++-+--+  |          |
             |   |   |  |  |  |          +-------+----+-----------+----+
      getcmdargs |   |  | flushcache     |       |    |           |    |
             |   |   |  |       |    stackupdate | prefetch  findnth   |
    errormessage |   |  |      [1]       |       |      |              |
                 |   |  |            makenode,   | push_addrstack     find
           initmisc  | breakupaddr   movetotop,  |                     |
            |    |   |               push,swap   |           +-[1]-+   |
init_addrstack   |   |                           |           |     |   |
                 |   |                    busupdate  copybackstack |   |
    readfrominputstream                    |     |       |     |   |   |
          |        |                       |  replacesubblocks |   |   |
  pop_addrstack  sscanxx                   |         |         |   |   |
					   +---------+---------+   |   |
						     |             |   |
						bustraffic    putonfreelist
                                             transpbustraffic

******************************************************************* */

/* *******************************************************************

Porting Information:

DineroIII runs on Unix 4.2 BSD systems.  I have used it on the
VAX and SUN-1 architectures.

DineroIII uses a fast C-function called "sscanxx" to interpret
input characters instead of the library function "sscanf"
because a profile showed that dineroIII was spending 35% to 50%
of this time in sscanf.  The function "sscanxx" runs about 6
times faster than "sscanf."  The function "sscanxx" is selected
by the compile-time flag FAST_BUT_DANGEROUS_INPUT in global.h.
DineroIII with sscanxx should run in 60 to 70% of the time of
dineroIII with sscanf.

******************************************************************* */
