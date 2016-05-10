
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

json-test: $(SRC_DIR)/JsonTest/json-test.cpp
	$(CC) $(CFLAGS) -o $(BIN_DIR)/json-test $(SRC_DIR)/JsonTest/*.cpp $(LIBS)

training-game: $(SRC_DIR)/TrainingGame/TGMain.cpp
	$(CC)	$(CFLAGS) -o $(BIN_DIR)/training-game $(SRC_DIR)/TrainingGame/*.cpp $(LIBS)

hello-myo: $(SRC_DIR)/hello-myo.cpp
	$(CC) $(CFLAGS) -o $(BIN_DIR)/hello-myo $(SRC_DIR)/hello-myo.cpp $(LIBS)

clean:
	rm -rf $(BIN_DIR)/*




