
CC=g++
CFLAGS=-std=c++11

#LIBS=-I./include -L./lib
#LIBS=-F/Library/Frameworks -framework myo

SRC_DIR=./src
BIN_DIR=./bin
INC_DIR=./include
LIB_DIR=./lib

ifeq ($(OS),Windows_NT)
	LIBS=-I$(INC_DIR) -L$(LIB_DIR)
else
   UNAME_S := $(shell uname -s)
   ifeq ($(UNAME_S),Darwin)
		LIBS=-F/Library/Frameworks -framework myo -I$(INC_DIR) -L$(LIB_DIR) -lcurl
   endif
endif


emg-upload: $(SRC_DIR)/emg-upload.cpp
	$(CC) $(CFLAGS) -o $(BIN_DIR)/emg-upload $(SRC_DIR)/*.cpp $(LIBS)

clean:
	rm -rf $(BIN_DIR)/*




