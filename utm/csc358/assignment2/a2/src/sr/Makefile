OBJ += main.o
OBJ += arp.o
OBJ += ip_link.o
OBJ += interface.o

FLAGS = -Wall
CC = gcc

../../sr:$(OBJ)  
	$(CC) $(OBJ) -o $@ $(FLAGS) -lpthread
%.o:%.c
	$(CC) -c $^ -o $@ $(FLAGS)
.PHONY:clean
clean:
	rm ../../sr *.o -rfv	
