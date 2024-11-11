Objects = main.o house.o room.o hunter.o ghost.o evidence.o utils.o logger.o


all:	main house room hunter ghost evidence utils logger
			gcc -o FP ${Objects}

main:	main.c defs.h
			gcc -c main.c

house:	house.c defs.h
			gcc -c house.c

room:	room.c defs.h
			gcc -c room.c

hunter:	hunter.c defs.h
			gcc -c hunter.c

ghost:	ghost.c defs.h
			gcc -c ghost.c

evidence:	evidence.c defs.h
				gcc -c evidence.c

utils: utils.c defs.h
			gcc -c utils.c

logger: logger.c defs.h
			gcc -c logger.c

clean:
			rm -f ${Objects} FP