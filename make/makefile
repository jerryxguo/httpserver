
ifdef DEBUG
DEFINES += -DDEBUG_VERSION
endif

SRC		= ../src
INCLUDE = ../include

CC      = gcc
OUT_EXE = httpserver
CFLAGS  = -Wall
LDFLAGS = -pthread -lrt 


SOURCES	= $(addprefix $(SRC)/,$(notdir $(wildcard $(SRC)/*.c)))

build: 
	$(CC) $(SOURCES) -o $(OUT_EXE) $(CFLAGS) $(LDFLAGS) -I$(INCLUDE) $(DEFINES)

clean:
	rm $(OUT_EXE) 

rebuild: clean build
