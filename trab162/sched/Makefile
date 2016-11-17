DEST	      = ${HOME}/bin

EXTHDRS	      = /usr/include/stdio.h

HDRS	      = decls.h

LDFLAGS	      =

LIBS	      =

LINKER	      = gcc

CC            = gcc

CCFLAGS       = -O3 -c 

MAKEFILE      = Makefile

OBJS	      = main.o cmdargs.o pipes.o

PRINT	      = lpr -Plpmh

PROGRAM	      = sched

SRCS	      = main.c cmdargs.c pipes.c

$(PROGRAM):     $(OBJS) $(LIBS)
		@echo -n "Loading $(PROGRAM) ... "
		@$(LINKER) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)
		@echo "done"

$(OBJS):	$(SRCS) $(LIBS)

clean:;		rm -f $(OBJS) tags core  ${PROGRAM}

# depend:;	@mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

index:;		ctags -wt $(HDRS) $(SRCS)

install:	$(PROGRAM)
		@echo Installing $(PROGRAM) in $(DEST)
		@install -s $(PROGRAM) $(DEST)

print:;		$(PRINT) $(HDRS) $(SRCS)
		lpq -Plpmh

backup:;	@for file in $(SRCS) $(HDRS) Makefile ; \
		do \
			compress -v $${file} ; \
			cp $${file}.Z ./bak ; \
			uncompress $${file}.Z ; \
		done

program:        $(PROGRAM)

tags:           $(HDRS) $(SRCS); @ctags -t $(HDRS) $(SRCS)

update:		$(DEST)/$(PROGRAM)

$(DEST)/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
		@make -f $(MAKEFILE) DEST=$(DEST) install

main.o: decls.h /usr/include/stdio.h
	$(CC) $(CCFLAGS) main.c

cmdargs.o: decls.h /usr/include/stdio.h
	$(CC) $(CCFLAGS) cmdargs.c

pipes.o: decls.h /usr/include/stdio.h
	$(CC) $(CCFLAGS) pipes.c

# init.o: global.h /usr/include/stdio.h
# 	$(CC) $(CCFLAGS) init.c
# 
# output.o: global.h /usr/include/stdio.h
# 	$(CC) $(CCFLAGS) output.c
# 
# ptable.o: global.h /usr/include/stdio.h
# 	$(CC) $(CCFLAGS) ptable.c
# 
# state.o: global.h /usr/include/stdio.h
# 	$(CC) $(CCFLAGS) state.c
# 
