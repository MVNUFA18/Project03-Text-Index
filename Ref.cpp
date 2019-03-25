// Ref class function definitions
// Computer Science, MVNU

#include "Ref.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

// GetNextToken - general routine for getting next token from a string
// Parameters
//    str - string to search. search is destructive, i.e., token is removed.
//    delimiters - string, a list of characters to use a delimiters between tokens
// Returns: string token (removed from str)

string GetNextToken(string& str, const string& delimiters = " ") {
  // Skip delimiters at beginning                                          
  string::size_type startPos = str.find_first_not_of(delimiters, 0);
  // Find position of delimiter at end of token                                           
  string::size_type endPos = str.find_first_of(delimiters, startPos);

  // Found a token, remove it from string, and return it                       
  string next = str.substr(startPos, endPos - startPos);
  string rest = str.substr(endPos - startPos + 1,string::npos);
  str = rest;
  return(next);
}

// Ref member functions
class Bible;

Ref::Ref() {book = 0; chap = 0; verse = 0;}  					// Default constructor, Initialize book, chapter, verse

Ref::Ref(const string s) {										// Parse constructor - receives a line "34:5:7 text"
	string rtext = s;											// make local copy of string to avoid modifying parameter

	if (rtext.length() >= 5 ) {									// "1 1 1" (5 characters including spaces) would be the smallest acceptable input
		// Grab book (first number)
		string strbook = GetNextToken(rtext,":");
		book = atoi(strbook.c_str());
		// Grab chapter (number after first semicolon)
		string strchap = GetNextToken(rtext,":");
		chap = atoi(strchap.c_str());
		// Grab verse (number after second semicolon)
		string strverse = GetNextToken(rtext," ");
		verse = atoi(strverse.c_str());
	}
}

Ref::Ref(const int b,const int c,const int v){ 	// Construct Ref from three integers
	book = b;
	chap = c;
	verse = v;
	}

   // Accessors
   int Ref::getBook() const {return book;};		// Access book number
   int Ref::getChap() const {return chap;};		// Access chapter number
   int Ref::getVerse() const {return verse;};	// Access verse number

   bool Ref::operator==(const Ref r) {	// Comparison: determine if two references are equal
      return ( (r.book == book) &&
               (r.chap == chap) &&
               (r.verse == verse) );
   }

   void Ref::display() const{			// Display the reference
	   string bookName = getBookName();
	   cout << bookName << " " << chap << ":" << verse;
   }

   string Ref::getBookName() const {
	   ifstream bibleFile("bibleBookNames.txt");
	   string currentLine;
	   //Loop through bible file until I reach the line with the book name assocaited with my book number.
	   for (int i = 0; i < book; i++) {
		   getline(bibleFile,currentLine);
	   }
		
	   return GetNextToken(currentLine, "\r");
   }

   bool Ref::operator<(const Ref r) const {
	   if (getBook() == r.getBook()) {
		   if (getChap() == r.getChap()) {
			   return getVerse() < r.getVerse();
		   }
		   else return getChap() < r.getChap();
	   }
	   else return getBook() < r.getBook();
   }
