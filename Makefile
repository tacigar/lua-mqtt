LUA     = 5.1
INCDIR  = -Isrc/ -I/usr/include/lua$(LUA) -I/usr/include

CFLAGS  = -O2 -fPIC $(INCDIR)
LIBS    = -lpaho-mqtt3c $(shell pkg-config --libs lua$(LUA))

PREFIX  = /usr/local
LIBDIR  = $(PREFIX)/lib/lua/$(LUA)
DATADIR = $(PREFIX)/share/lua/$(LUA)

SRCS = src/client_base.c src/token.c src/message.c

all: mqtt/core/ClientBase.so mqtt/Token.so mqtt/core.so

mqtt/core/ClientBase.so: ${SRCS:.c=.o}
	mkdir -p mqtt/core
	$(CC) $(CFLAGS) -o $@ -shared ${SRCS:.c=.o} $(LIBS)
 
mqtt/Token.so:
	$(CC) $(CFLAGS) -o $@ -shared src/token.o $(LIBS)
 
mqtt/core.so: src/mqtt.o
	$(CC) $(CFLAGS) -shared -o $@ src/mqtt.o $(LIBS)

clean:
	rm -rf mqtt/
	rm -rf src/*.o

install: all
	install -d $(DESTDIR)$(LIBDIR)/mqtt/core/ $(DESTDIR)$(DATADIR)/mqtt
	install -m 755 mqtt/core/ClientBase.so $(DESTDIR)$(LIBDIR)/mqtt/core/
	install -m 755 mqtt/Token.so $(DESTDIR)$(LIBDIR)/mqtt/
	install -m 755 mqtt/core.so $(DESTDIR)$(LIBDIR)/mqtt/
	
	install -m 755 src/init.lua $(DESTDIR)$(DATADIR)/mqtt/
	install -m 755 src/client_base.lua $(DESTDIR)$(DATADIR)/mqtt/ClientBase.lua
	install -m 755 src/client.lua $(DESTDIR)$(DATADIR)/mqtt/Client.lua
	install -m 755 src/async_client.lua $(DESTDIR)$(DATADIR)/mqtt/AsyncClient.lua
 
uninstall:
	rm -rf $(DESTDIR)$(LIBDIR)/mqtt/
	rm -rf $(DESTDIR)$(DATADIR)/mqtt

.PHONY: all