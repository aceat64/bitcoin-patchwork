# Copyright (c) 2009 Satoshi Nakamoto
# Distributed under the MIT/X11 software license, see the accompanying
# file license.txt or http://www.opensource.org/licenses/mit-license.php.


ifneq "$(BUILD)" "debug"
ifneq "$(BUILD)" "release"
BUILD=debug
endif
endif
ifeq "$(BUILD)" "debug"
D=d
DEBUGFLAGS=-g -D__WXDEBUG__
endif



INCLUDEPATHS=-I"/usr/local/boost_1_37_0" \
             -I"/usr/local/db-4.7.25.NC/build_unix" \
             -I"/usr/local/ssl/include" \
             -I"/usr/local/include/wx-2.8" \
             -I"/usr/local/lib/wx/include/gtk2-ansi-debug-static-2.8"

LIBPATHS=-L"/usr/local/db-4.7.25.NC/build_unix" \
         -L"/usr/local/ssl/lib" \
         -L"/usr/local/lib"

LIBS= \
 -l db_cxx \
 -l crypto \
 -l wx_gtk2$(D)-2.8
WXDEFS=-D__WXGTK__ -DNOPCH
CFLAGS=-O0 -w -Wno-invalid-offsetof -Wformat $(DEBUGFLAGS) $(WXDEFS) $(INCLUDEPATHS)
HEADERS=headers.h util.h main.h serialize.h uint256.h key.h bignum.h script.h db.h base58.h



all: bitcoin.exe


headers.h.gch: headers.h $(HEADERS) net.h irc.h market.h uibase.h ui.h
	g++ -c $(CFLAGS) -o $@ $<

obj/util.o: util.cpp		    $(HEADERS)
	g++ -c $(CFLAGS) -o $@ $<

obj/script.o: script.cpp	    $(HEADERS)
	g++ -c $(CFLAGS) -o $@ $<

obj/db.o: db.cpp		    $(HEADERS) market.h
	g++ -c $(CFLAGS) -o $@ $<

obj/net.o: net.cpp		    $(HEADERS) net.h
	g++ -c $(CFLAGS) -o $@ $<

obj/main.o: main.cpp		    $(HEADERS) net.h market.h sha.h
	g++ -c $(CFLAGS) -o $@ $<

obj/market.o: market.cpp	    $(HEADERS) market.h
	g++ -c $(CFLAGS) -o $@ $<

obj/ui.o: ui.cpp		    $(HEADERS) net.h uibase.h ui.h market.h
	g++ -c $(CFLAGS) -o $@ $<

obj/uibase.o: uibase.cpp	    uibase.h
	g++ -c $(CFLAGS) -o $@ $<

obj/sha.o: sha.cpp		    sha.h
	g++ -c $(CFLAGS) -O3 -o $@ $<

obj/irc.o:  irc.cpp		    $(HEADERS)
	g++ -c $(CFLAGS) -o $@ $<




OBJS=obj/util.o obj/script.o obj/db.o obj/net.o obj/main.o obj/market.o \
	obj/ui.o obj/uibase.o obj/sha.o obj/irc.o 

bitcoin.exe: headers.h.gch $(OBJS)
	g++ $(CFLAGS) -o $@ $(LIBPATHS) $(OBJS) $(LIBS)

clean:
	-del /Q obj\*
	-del /Q headers.h.gch



