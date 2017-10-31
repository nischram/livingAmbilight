CXX ?=g++
CC ?=gcc
ROOT_WD=watchdog
ROOT_LM=livingAmbilight
ROOT_START=start
ROOT_STOP=stop

all: $(ROOT_WD) $(ROOT_LM) $(ROOT_START) $(ROOT_STOP)

$(ROOT_WD): cleanWD
	$(CXX) -O1 Watchdog.cpp -o $@
$(ROOT_LM): cleanLA
	$(CC) -O1 livingAmbilight.c ../rpi_ws281x/mailbox.c ../rpi_ws281x/ws2811.c ../rpi_ws281x/pwm.c ../rpi_ws281x/pcm.c ../rpi_ws281x/dma.c ../rpi_ws281x/rpihw.c -o $@ -lwiringPi
$(ROOT_START): cleanSTART
	$(CC) -O1 prog/start.c -o $@
$(ROOT_STOP): cleanSTOP
	$(CC) -O1 prog/stop.c -o $@

cleanWD:
	-rm $(ROOT_WD) $(VECTOR)
cleanLA:
	-rm $(ROOT_LM) $(VECTOR)
cleanSTART:
	-rm $(ROOT_START) $(VECTOR)
cleanSTOP:
	-rm $(ROOT_STOP) $(VECTOR)
