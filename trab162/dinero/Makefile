DEST	      = ${HOME}/bin

EXTHDRS	      = /usr/include/stdio.h

HDRS	      = doc.h global.h

LDFLAGS	      =

LIBS	      =

LINKER	      = gcc

CC            = gcc

CCFLAGS       = -O3 -c

MAKEFILE      = Makefile

OBJS	      = main.o cmdargs.o fetch.o init.o output.o state.o

PRINT	      = lpr -Plpmh

PROGRAM	      = dinero

SRCS	      = main.c cmdargs.c fetch.c init.c output.c state.c

$(PROGRAM):     $(OBJS) $(LIBS)
		@echo -n "Loading $(PROGRAM) ... "
		@$(LINKER) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)
		@echo "done"

clean:;		rm -f $(OBJS) tags core ${PROGRAM}

# depend:;	@mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

index:;		ctags -wt $(HDRS) $(SRCS)

install:	$(PROGRAM)
		@echo Installing $(PROGRAM) in $(DEST)
		@install -s $(PROGRAM) $(DEST)
		rm ./$(PROGRAM)

print:;		$(PRINT) $(HDRS) $(SRCS)
		lpq -Plpmh

backup:;	@for file in $(SRCS) $(HDRS) Makefile ; \
		do \
			compress -v $${file} ; \
			cp $${file}.Z ./bak ; \
			uncompress $${file}.Z ; \
		done

lint:;		lint $(SRCS)

program:        $(PROGRAM)

tags:           $(HDRS) $(SRCS); @ctags -t $(HDRS) $(SRCS)

update:		$(DEST)/$(PROGRAM)

$(DEST)/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
		@make -f $(MAKEFILE) DEST=$(DEST) install
###
main.o:	main.c global.h /usr/include/stdio.h
	$(CC) $(CCFLAGS) main.c

cmdargs.o:	cmdargs.c global.h /usr/include/stdio.h
	$(CC) $(CCFLAGS) cmdargs.c

fetch.o:	fetch.c global.h /usr/include/stdio.h
	$(CC) $(CCFLAGS) fetch.c

init.o:	init.c global.h /usr/include/stdio.h
	$(CC) $(CCFLAGS) init.c

output.o:	output.c global.h /usr/include/stdio.h
	$(CC) $(CCFLAGS) output.c

state.o:	state.c global.h /usr/include/stdio.h
	$(CC) $(CCFLAGS) state.c
