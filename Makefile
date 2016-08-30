

CC=gcc
CFLAGS=-Wall -O2 -g

VERSION=0.1
BINARIES=dump_6010 dump_maxvals dump_baro download_flights dump_baromem \
	set_rtc
all: $(BINARIES)

clean:
	rm -f $(BINARIES)

tar: clean
	mkdir -p flytec-linux-$(VERSION)
	rsync -avp * flytec-linux-$(VERSION) --exclude \*barobin --exclude \*minmaxbin --exclude \*~ --exclude Schnittstelle\* --exclude \*.tar.gz --exclude baromem --exclude bla\* --exclude goed --exclude fout --exclude \*dump --exclude bad\* --exclude ee_dump\* --exclude \*.hd 
	rmdir flytec-linux-$(VERSION)/flytec-linux-$(VERSION)
	tar cvfz flytec-linux-$(VERSION).tar.gz flytec-linux-$(VERSION)
	rm -rf flytec-linux-$(VERSION)

