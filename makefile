#
# Makefile for MangaMe
#

# DEFAULT SETTINGS
CC = clang++
STD_VERSION = -std=c++11
OTHERFLAGS = -pthread
INCLUDES = -I /usr/local/Cellar/openssl/1.0.2e/include
LINKS = -L /usr/local/Cellar/openssl/1.0.2e/lib

# GUI INCLUDES
GUIINCLUDES = -I /usr/local/Cellar/wxmac/3.0.2/include/wx-3.0/wx
GUILINKDS = -L /usr/local/Cellar/wxmac/3.0.2/lib

# LIBS
BOOST_LIBS = -lboost_system-mt -lboost_thread-mt -lboost_filesystem-mt
CPPNETLIB_LIBS = -lcppnetlib-client-connections -lcppnetlib-uri -lcppnetlib-server-parsers
OPENSSL_LIBS = -lssl -lcrypto
ALL_LIBS = $(BOOST_LIBS) $(CPPNETLIB_LIBS) $(OPENSSL_LIBS)

# RECIPES
default : 
	$(CC) $(STD_VERSION) $(OTHERFLAGS) $(INCLUDES) mangaMe.cpp -o MangaMe $(LINKS) $(ALL_LIBS)

gui :
	$(CC) $(STD_VERSION) $(GUIINCLUDES) gui.cpp -o GUI

.PHONY : clean
clean :
	rm mangaMe
