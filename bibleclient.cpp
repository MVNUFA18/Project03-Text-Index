/* bibleclient.cpp
 * Bible full-text search client:
 * receives an Ajax request from web client
 * extracts search string, and sends request to bibleserver
 */

 //Stuff for Pipes  
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <list>
#include "Ref.h"
#include "fifo.h"

using namespace std;

// Stuff for Ajax
#include "/home/class/csc3004/cgicc/Cgicc.h"
#include "/home/class/csc3004/cgicc/HTTPHTMLHeader.h"
#include "/home/class/csc3004/cgicc/HTMLClasses.h"
using namespace cgicc;


#define logging


// Pipes for communication                                                      
string receiver = "SSreply";
string sender = "SSrequest";

string convertInt(int number)
{
  stringstream ss;//create a stringstream
  ss << number;   //add number to the stream
  return ss.str();//return a string with the contents of the stream
}
//Logfile
#ifdef logging
const string logFilename =  "/tmp/dancherubini-logFile.txt";
ofstream logFile;
string timestamp() {
  time_t _tm = time( NULL );
  struct tm * curtime = localtime ( &_tm );
  string t = asctime(curtime);
  t.erase(t.end() - 1);
  return t;
}
#define log(message) logFile << timestamp() << ": " << message << endl
#else
#define log(message)
#endif

int main() {

cout << "Content-Type: text/plain\n\n" << endl;

#ifdef logging
logFile.open(logFilename.c_str(), ios::out);
#endif

  Cgicc cgi; 

  // Collect input data 
  form_iterator stIterator = cgi.getElement("search_type");
  form_iterator bookIterator = cgi.getElement("book");
  form_iterator chapterIterator = cgi.getElement("chapter");
  form_iterator verseIterator = cgi.getElement("verse");
  form_iterator nvIterator = cgi.getElement("numberOfVerses");
  form_iterator versionIterator = cgi.getElement("version");

  // Passes the required response header with content type to the client web page.

  log("Started Client");
  //Check input data
  bool validInput = false;
  
  //Valid input check
  if (chapterIterator != cgi.getElements().end() && verseIterator != cgi.getElements().end() && nvIterator != cgi.getElements().end()) {
	  int chapter = chapterIterator->getIntegerValue();
	  int verse = verseIterator->getIntegerValue();
	  int verseCount = nvIterator->getIntegerValue();
	  if (chapter <= 0) {
		  cout << "<p> No negative or zero chapters! </p>" << endl;
	  }
	  else if (verse <= 0) {
		  cout << "<p> No negative or zero verse! </p>" << endl;
	  }
	  else if (verseCount < 0) {
		  cout << "<p> No negative verse count! </p>" << endl;
	  }
	  else
		  validInput = true;
  }
  else {
	  cout << "<p> Missing parameter </p>" << endl;
  }

  
  /* SEND BACK THE RESULTS
   * We do this by building a result string and sending it to the web page.
   * Needs HTML formatting
   */

  if (validInput) {
	cout << "Search Type: <b>" << **stIterator << "</b>" << endl;
	cout << "<p>";
	
	int chapter = chapterIterator->getIntegerValue();
	int verse = verseIterator->getIntegerValue();
	int verseCount = nvIterator->getIntegerValue();
	int book = bookIterator->getIntegerValue();
	int version = versionIterator->getIntegerValue();
	int numberOfVerses = nvIterator->getIntegerValue();

	if (verseCount == 0)
		verseCount = 1;
	
	//use convertInt to make sure we're sending correct data type 
	string searchString = convertInt(book)
					+":"+convertInt(chapter)
					+":"+convertInt(verse)
					+":"+convertInt(numberOfVerses)
					+":"+convertInt(version);

  int length = searchString.length();
	
	//Wrap everything in a try so we can debug any error messages that may come

   try{
				  Fifo recfifo(receiver);
				  Fifo sendfifo(sender);

				//#ifdef logging
				//  logFile.open(logFilename.c_str(),ios::out);
				//#endif


				  sendfifo.openwrite();
				  log("Open Request Pipe");

				  // Server call for results                                             
				  sendfifo.send(searchString);
				  log("Request: "+searchString);

				  recfifo.openread();
				  log("open reply fifo");

				  // Post response to page
				  string results = "";
				  const string delimiter = "$";
				  int times = 0; // Counter for header lines
				  
				  //Gather all results until we reach the end
				  string fifoResults = "";
				  while (results != "$end") {					  
					fifoResults += results;
					results = recfifo.recv();					
					log("Reply: "+results);
				  }
				  
				  recfifo.fifoclose();
				  log("close reply fifo");
				  sendfifo.fifoclose();
				  log("close request fifo");
				  
				//Retrieve status
				string status = GetNextToken(fifoResults,delimiter);
				
				//Default to success
				int statusInt = 0;
				istringstream(status) >> statusInt;
				
				/* error code info
						VALUES:
						0-SUCCESS
						1-NO BOOK
						2-NO CHAPTER
						3-NO VERSE
						4-NO VERSION AVAILABLE
						5-OTHER
				*/
							
				cout << "<br />" << endl;
							
				switch(statusInt) {
					case(0):
						cout << fifoResults << endl;
						break;
					case(1):
						cout << "The book was not found!" << endl;
						break;
					case(2):
						cout << "The chapter was not found!" << endl;
						break;
					case(3):
						cout << "The verse was not found!" << endl;
						break;
					case(4):
						cout << "The bible version was not found!" << endl;
						break;	
					case(5):
						cout << "An unknown error occurred, check your inputs" << endl;
						break;							
				}	
  
   }
	catch(const exception &exc)
	   {
		   cout << exc.what();
	   }   
			 
	  }
	  
  return 0;
}
