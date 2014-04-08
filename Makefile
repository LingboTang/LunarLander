#name:               Xuping Fang
#ONE Card number:    1365512
#Unix id:            xuping
#lecture section:    A2
#instructor's name:  Davood Rafiei
#lab section:        D06
#TA's name:          Ali Yaclollahi
lander: lander.o functions.o
	gcc -Wall -std=c99 -o lander lander.o functions.o -lm -l curses

lander.o: lander.c lander.h
	gcc -Wall -std=c99 -c lander.c

functions.o: functions.c lander.h
	gcc -Wall -std=c99 -c functions.c

clean:
	-rm -f *.o lander core

tar:
	tar -cvf submit.tar lander.c lander.h functions.c Makefile README landscape.txt
