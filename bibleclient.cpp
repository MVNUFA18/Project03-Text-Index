/* bibleclient.cpp
 * Bible full-text search client:
 * receives an Ajax request from web client
 * extracts search string, and sends request to bibleserver
 */
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
// Stuff for Ajax
#include "/home/class/csc3004/cgicc/Cgicc.h"
#include "/home/class/csc3004/cgicc/HTTPHTMLHeader.h"
#include "/home/class/csc3004/cgicc/HTMLClasses.h"
using namespace cgicc;

//Stuff for Pipes   
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
	

  Cgicc cgi; 

  // Collect input data 
  form_iterator searchType = cgi.getElement("search_type");
  form_iterator version = cgi.getElement("version");
  form_iterator book = cgi.getElement("book");
  form_iterator chapter = cgi.getElement("chapter");
  form_iterator verse = cgi.getElement("verse");
  form_iterator numberOfVerses = cgi.getElement("numberOfVerses");

  // Passes the required response header with content type to the client web page.

  cout << "Content-Type: text/plain\n\n" << endl;
  
  //Check input data
  bool validInput = true;
  
  //Chapter Check
  if (chapter != cgi.getElements().end()) {
	 int chapterNum = chapter->getIntegerValue();
	 if (chapterNum > 150) {
		 cout << "<p>Chapter number (" << chapterNum << ") is too high.</p>" << endl;
		 validInput = false;
	 }
	 else if (chapterNum <= 0) {
		 cout << "<p>Only numeric chapters greater than zero are allowed!</p>" << endl;
		 validInput = false;
	 }
  }
  
  //Verse Check - if we pass the chapter check
  if (validInput) {
	 int verseNum = verse->getIntegerValue();
	 if (verseNum <= 0) {
		 cout << "<p>Only numeric verses greater than zero are allowed!</p>" << endl;
		 validInput = false;
	 }
  }
  
  //Check Number of Verses, check for validity
  int numberOfValidVerses;
  if (numberOfVerses->getValue() == "") {
	  numberOfValidVerses = 1;
  }
  else if(validInput) {
	  int numVerses = numberOfVerses->getIntegerValue();
	  if(numVerses<=0)
	  {
		  cout << "<p>You must input a positive number of verses!</p>"<<endl;
		  validInput = false;
	  }
	  else 
	  numberOfValidVerses = numVerses;	  
  }
  
  /* SEND BACK THE RESULTS
   * We do this by building a result string and sending it to the web page.
   * Needs HTML formatting
   */

  if (validInput) {
	cout << "Search Type: <b>" << **searchType << "</b>" << endl;
	
	//use convertInt to make sure we're sending correct data type 
	string searchString = convertInt(book->getIntegerValue())
					+":"+convertInt(chapter->getIntegerValue())
					+":"+convertInt(verse->getIntegerValue())
					+":"+convertInt(numberOfValidVerses)
					+":"+version->getValue();
  int length = searchString.length();
	
	//Wrap everything in a try so we can debug any error messages that may come

   try{
				  Fifo recfifo(receiver);
				  Fifo sendfifo(sender);

				#ifdef logging
				  logFile.open(logFilename.c_str(),ios::out);
				#endif


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
