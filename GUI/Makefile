GUI : 
	make clean
	make interface
	./ihm

interface : 
	gcc ihm-v2.c -o ihm `pkg-config --cflags --libs gtk+-3.0`

clean :
	rm -f ihm