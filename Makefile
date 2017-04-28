CC=gcc
CFLAGS= -pthread -lcrypto -lssl

CONF_SERVER = server.conf

LOCAL_SRC_FILES := \
		src/config.c\
		src/logger.c\
		src/ssl.c\
		src/threadpool.c\
		src/snail-server.c

EXEC_FILE = snail-httpserver

all: clean $(EXEC_FILE)

$(EXEC_FILE): 
	$(CC)  $(LOCAL_SRC_FILES) $(CFLAGS)  -o $(EXEC_FILE)
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
	