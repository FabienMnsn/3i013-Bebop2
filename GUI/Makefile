
GUI : 
	make clean
	make interface
	./ihm

interface : 
	gcc ihm-v2.c -o ihm `pkg-config --cflags --libs gtk+-3.0`

listbox :
	gcc list_box_test.c -o test_list_box `pkg-config --cflags --libs gtk+-3.0`

clean :
	rm -f ihm test_list_box