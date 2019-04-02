/***************************************************************************
 * bibleserver.cpp  -  Program to serve Shakespeare search results
 *
 * copyright : (C) 2009 by Jim Skon
 * Dante Cherubini
 * March 19 2019
 ***************************************************************************/
 
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "Bible.h"
#include "Verse.h"
#include "Ref.h"
#include "fifo.h"

using namespace std;

/* error code info
VALUES:
0-SUCCESS
1-NO BOOK
2-NO CHAPTER
3-NO VERSE
4-NO VERSION AVAILABLE
5-OTHER
*/
int LookupResultCodes[] = {0,1,2,3,4,5};
const int NO_VERSION = 4;
const string DELIMITER = "$";

string receive_pipe = "SSrequest";
string send_pipe = "SSreply";

string convertInt(int number)
{
  stringstream ss;//create a stringstream
  ss << number;//add number to the stream
  return ss.str();//return a string with the contents of the stream
}


// Return a int lookup error code
const int errorCode(LookupResult result){
	return LookupResultCodes[result];
};	


int main() {
	list<int> result;
	list<int>::iterator it;
	int position;

	ifstream textFile;
	string currentLine;  // A line from the text file
	string message;

	/* Build the bible indexes */
	Bible kjv(1);
	Bible dby(2);
	Bible web(3);
	Bible webster(4);
	Bible ylt(5);
	
	/* Create the communication fifos */
	Fifo recfifo(receive_pipe);
	Fifo sendfifo(send_pipe);

	while (1) { // "infinite loop" for server: repeat each time a request is received

		recfifo.openread();

		/* Get a message from a client */
		string lookupMessage = recfifo.recv();

		//format is book:chapter:verse


		cout << "Message: |" << lookupMessage << "|" << endl;
		sendfifo.openwrite();

		/* Send back the search term message */
		
		
		
		if (lookupMessage.length() > 1) {
		
			string delimiterSend = ":";
			
			//parse out request info
			int book, chap, verse, numberOfVerses, version;
			istringstream(GetNextToken(lookupMessage, delimiterSend)) >> book;
			istringstream(GetNextToken(lookupMessage, delimiterSend)) >> chap;
			istringstream(GetNextToken(lookupMessage, delimiterSend)) >> verse;
			istringstream(GetNextToken(lookupMessage, delimiterSend)) >> numberOfVerses;			
			istringstream(GetNextToken(lookupMessage, delimiterSend)) >> version;
			cout << "version: " << BookVersionString[version] << endl;
			
			
			//variables for lookup: ref, v, result
			Ref ref = Ref(book, chap, verse);			
			bool version_found = true;
			vector<Verse> v;
			LookupResult result;
			
			if(version==1)
				v = kjv.lookup(numberOfVerses, ref, result);
			else if (version == 2)
				v = dby.lookup(numberOfVerses, ref, result);
			else if (version == 3)
				v = web.lookup(numberOfVerses, ref, result);
			else if(version==4)
				v = webster.lookup(numberOfVerses, ref, result);
			else if(version==5)
				v = ylt.lookup(numberOfVerses, ref, result);
			//if we don't have the given version, throw NO_VERSION error
			else {
				version_found = false;
				message = convertInt(NO_VERSION)+DELIMITER;		
				sendfifo.send(message);				
			}
			
			//if the version exists, look up verse in that version
			if(version_found){
			
				message = convertInt(errorCode(result))+DELIMITER;
				sendfifo.send(message);
				cout << "Message sent: "<< message<< endl;

				for (int i = 0; i < v.size(); i++) {
					message = "";
					//if we're in a new chapter or book, then print the header
					if (i > 0 && v[i].getRef().getBook() != v[i - 1].getRef().getBook() || v[i].getRef().getChap() != v[i - 1].getRef().getChap())
						message += "<b>" + v[i].getRef().getBookName() + " " + convertInt(v[i].getRef().getChap()) + "</b><br />";
					message += convertInt(v[i].getRef().getVerse()) + " " + v[i].getVerse() + "<br />";
					sendfifo.send(message);
					cout << "Message sent: "<< message<< endl;
				}			
			}			
			message = "$end";
			sendfifo.send(message);
			sendfifo.fifoclose();
			recfifo.fifoclose();
		}
	}

}
