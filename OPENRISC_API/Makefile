CC= g++
CFLAGS= -O3 -Wall
OBJECTS= SMIMS_VEXT2.o Baton_API.o main.o
TARGET=./openrisc_api

all: $(OBJECTS) $(TARGET)

$(TARGET): 
	@$(CC) -o $(TARGET) $(OBJECTS) -lpthread -ldl
	
%.o: %.cpp
	@$(CC) $(CFLAGS) -c $<
	
clean:
	@rm -f $(OBJECTS) $(TARGET)

