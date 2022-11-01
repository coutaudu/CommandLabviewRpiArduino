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

INCLUDE = csapp.h
OBJS = csapp.o CommunicationMicrocontrollers-Serial.o CommunicationClients-UDP.o Protocol.o Logs.o
INCLDIR = -I.

PROGS = TemperatureControlRouter

all: $(PROGS)

%.o: %.c %.h $(INCLUDE)
	$(CC) $(CFLAGS) $(INCLDIR) -c -o $@ $<

%: %.o $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^ $(LIBS)

install_service: TemperatureControlRouter.conf
	sudo cp TemperatureControlRouter.conf /etc/systemd/system/TemperatureControlRouter.service
	sudo cp ./TemperatureControlRouter /bin/TemperatureControlRouter

run_service: install_service
	systemctl start TemperatureControlRouter
	systemctl daemon-reload

auto_run_service:
	systemctl enable TemperatureControlRouter

stop_service:
	systemctl stop  TemperatureControlRouter

disable_service:
	systemctl disable TemperatureControlRouter


clean:
	rm -f $(PROGS) *.o output.txt *~
