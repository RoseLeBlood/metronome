PROMPT := " >> "
PIOFLAGS := -f -c vim
PIORUN = @echo $(PROMPT) PIO BUILD"    " $ && pio $(PIOFLAGS) run
PIOCFG = @echo $(PROMPT) PIO CONFIG"   " $ && pio $(PIOFLAGS) run -t menuconfig
PIOCLN = @echo $(PROMPT) PIO CLEAN"    " $ && pio $(PIOFLAGS) run --target clean
PIOUPL = @echo $(PROMPT) PIO UPLOAD"   " $ && pio $(PIOFLAGS) run --target upload
PIOUAT = @echo $(PROMPT) PIO MONITOR"  " $ && pio $(PIOFLAGS) device monitor
RM     = @echo $(PROMPT) RM"  	       " $ $< && rm

all:
	$(PIORUN)
clean:
	$(PIOCLN)
	$(RM) platformio-device-monitor-*.log
upload:
	$(PIOUPL)
monitor:
	$(PIOUAT)
config:
	$(PIOCFG)
