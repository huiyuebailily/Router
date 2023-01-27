OBJ += endsys.o

FLAGS =
CC = gcc

../../endsys:$(OBJ)  
	$(CC) $(OBJ) -o $@ $(FLAGS) -lpthread
%.o:%.c
	$(CC) -c $^ -o $@ $(FLAGS)
