.PHONY: all, clean

# Disable implicit rules
.SUFFIXES:

# Keep intermediate files
#.PRECIOUS: %.o

CC = gcc
CFLAGS = -Wall -Werror
LDFLAGS =

# Note: -lnsl does not seem to work on Mac OS but will
# probably be necessary on Solaris for linking network-related functions 
#LIBS += -lsocket -lnsl -lrt
LIBS += -lpthread

INCLUDE = ./csapp.h
OBJS = ./csapp.o
INCLDIR = -I.

PROGS = SerialServer

all: $(PROGS)

%.o: %.c %.h $(INCLUDE)
	$(CC) $(CFLAGS) $(INCLDIR) -c -o $@ $<

%: %.o $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^ $(LIBS)
#	$(CC) -o $@ $(LDFLAGS) $(LIBS) $^

install_service: SerialServer.conf
	sudo cp SerialServer.conf /etc/systemd/system/SerialServer.service
	sudo cp ./SerialServer /bin/SerialServer

run_service: install_service
	systemctl start SerialServer
	systemctl daemon-reload

auto_run_service:
	systemctl enable SerialServer

stop_service:
	systemctl stop  SerialServer

disable_service:
	systemctl disable SerialServer


clean:
	rm -f $(PROGS) *.o output.txt *~
