#
# Makefile for MangaMe
#

# DEFAULT SETTINGS
CC = clang++
STD_VERSION = -std=c++14
INCLUDES = -I /usr/local/Cellar/openssl/1.0.2e/include
LINKS = -L /usr/local/Cellar/openssl/1.0.2e/lib

# LIBS
BOOST_LIBS = -lboost_system -lboost_thread-mt -pthread
CPPNETLIB_LIBS = -lcppnetlib-client-connections -lcppnetlib-uri -lcppnetlib-server-parsers
OPENSSL_LIBS = -lssl -lcrypto
OTHER_LIBS = -pthread
ALL_LIBS = $(BOOST_LIBS) $(CPPNETLIB_LIBS) $(OPENSSL_LIBS) $(OTHER_LIBS)

#OBJECTS = mangaMe.o

# RECIPES
default : 
	$(CC) $(STD_VERSION) $(INCLUDES) mangaMe.cpp -o MangaMe $(LINKS) $(ALL_LIBS) 

.PHONY : clean
clean :
	rm mangaMe
