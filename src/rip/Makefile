OBJ += main.o
OBJ += arp.o
OBJ += ip_link.o
OBJ += interface.o
OBJ += rip.o

FLAGS = -Wall
CC = gcc

../../rip:$(OBJ)  
	$(CC) $(OBJ) -o $@ $(FLAGS) -lpthread
%.o:%.c
	$(CC) -c $^ -o $@ $(FLAGS)
.PHONY:clean
clean:
	rm multirouter *.o -rfv
