#
# Makefile for MangaMe
#

# DEFAULT SETTINGS
CC = clang++
STD_VERSION = -std=c++14
OTHERFLAGS = -pthread
INCLUDES = -I /usr/local/Cellar/openssl/1.0.2e/include
LINKS = -L /usr/local/Cellar/openssl/1.0.2e/lib

# LIBS
BOOST_LIBS = -lboost_system-mt -lboost_thread-mt -lboost_filesystem-mt -pthread
CPPNETLIB_LIBS = -lcppnetlib-client-connections -lcppnetlib-uri -lcppnetlib-server-parsers
OPENSSL_LIBS = -lssl -lcrypto
ALL_LIBS = $(BOOST_LIBS) $(CPPNETLIB_LIBS) $(OPENSSL_LIBS)

# RECIPES
default : 
	$(CC) $(STD_VERSION) $(OTHERFLAGS) $(INCLUDES) mangaMe.cpp -o MangaMe $(LINKS) $(ALL_LIBS)

test : 
	$(CC) $(STD_VERSION) $(OTHERFLAGS) $(INCLUDES) test.cpp $(LINKS) $(ALL_LIBS)

.PHONY : clean
clean :
	rm mangaMe
