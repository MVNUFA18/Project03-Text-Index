# bibleclient program
# Feb 2010, James Skon
# August 2016, Bob Kasper
# February 2019, Dante Cherubini
# updated February 2018 to use paths on cs2.mvnu.edu

# This is the Makefile for the Bible system demo program
# Copy this directory to a location within your home directory. 
# Change the USER name value below to your own user name.
# Then use "make" to build the server program,
# and deploy it to the live web server directory.
# To test the program, go to http://cs2.mvnu.edu/class/csc3004/dancherubini/
# and click on the bibleclient.html link.
CC= g++
CFLAGS= -g
USER= dancherubini
all: bibleserver bibleclient.cgi PutCGI PutHTML biblereader

#Bible Client Object
bibleclient.o:	bibleclient.cpp fifo.h Ref.h Bible.h
			g++ -c bibleclient.cpp

#Bible Reader Object	
biblereader.o : Ref.h Verse.h Bible.h biblereader.h biblereader.cpp
	$(CC) $(CFLAGS) -c biblereader.cpp

#Bible Server Object
bibleserver.o : fifo.h Ref.h Bible.h bibleserver.cpp
	$(CC) $(CFLAGS) -c bibleserver.cpp

#Fifo
fifo.o : fifo.cpp fifo.h
	$(CC) $(CFLAGS) -c fifo.cpp

# Ref Object
Ref.o : Ref.h Ref.cpp
	$(CC) $(CFLAGS) -c Ref.cpp

# Verse Object
Verse.o : Ref.h Verse.h Verse.cpp
	$(CC) $(CFLAGS) -c Verse.cpp

# Bible Object
Bible.o : Ref.h Verse.h Bible.h Bible.cpp
	$(CC) $(CFLAGS) -c Bible.cpp

biblereader: Ref.o Verse.o Bible.o biblereader.o
	$(CC) $(CFLAGS) -o biblereader Ref.o Verse.o Bible.o biblereader.o

bibleserver: fifo.o Ref.o Verse.o Bible.o bibleserver.o
	$(CC) $(CFLAGS) -o bibleserver Ref.o Verse.o Bible.o fifo.o bibleserver.o


# TO DO: add classes from Project 1 to be linked
# in the executable for bibleclient.cgi
bibleclient.cgi:	bibleclient.o Ref.o Verse.o Bible.o fifo.o 
			g++ -o bibleclient.cgi bibleclient.o Ref.o Verse.o Bible.o fifo.o  -lcgicc
			# -l option links with cgicc library
			
PutCGI:	bibleclient.cgi
		chmod 755 bibleclient.cgi
		cp bibleclient.cgi /var/www/html/class/csc3004/$(USER)/cgi-bin
		cp bibleBookNames.txt /var/www/html/class/csc3004/$(USER)/cgi-bin

		echo "Current contents of your cgi-bin directory: "
		ls -l /var/www/html/class/csc3004/$(USER)/cgi-bin/

PutHTML:
		cp bibleclient.html /var/www/html/class/csc3004/$(USER)

		echo "Current contents of your HTML directory: "
		ls -l /var/www/html/class/csc3004/$(USER)

clean:		
		rm *.o core bibleclient.cgi
