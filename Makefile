CC=gcc
CFLAGS= -pthread

CONF_SERVER = server.conf

LOCAL_SRC_FILES := \
		src/config.c\
		src/logger.c\
		src/threadpool.c\
		src/snail-server.c

EXEC_FILE = snail-httpserver

all: clean $(EXEC_FILE)

$(EXEC_FILE): 
	$(CC) $(CFLAGS) $(LOCAL_SRC_FILES)  -o $(EXEC_FILE)
clean:
	rm -rf $(EXEC_FILE)
	rm -rf snail-httpServer-package.deb

	
deb: all checkdirectory
	cp $(EXEC_FILE) snail-httpServer-package/DEBIAN/usr/bin
	cp $(CONF_SERVER) snail-httpServer-package/DEBIAN/etc/snail-httpserver
	sudo dpkg-deb --build snail-httpServer-package

checkdirectory:
	mkdir -p snail-httpServer-package/DEBIAN/usr/bin
	mkdir -p snail-httpServer-package/DEBIAN/etc/snail-httpserver
	