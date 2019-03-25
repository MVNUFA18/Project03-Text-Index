// Verse class function definitions
// Computer Science, MVNU

#include "Verse.h"
#include <iostream>

using namespace std;

Verse::Verse() {  	// Default constructor
    verseText = "Uninitialized Verse!";
    verseRef = Ref();
} 

// Parse constructor - pass verse string from file
Verse::Verse(const string s) {
	verseText = s.substr(s.find_first_of(" ") + 1, s.length() - 1);
	verseRef = Ref(s);
}  	

// Accessors
string Verse::getVerse() {
	return verseText;
}

Ref Verse::getRef() {
	return verseRef;
}

// display reference and verse
void Verse::display() {
	cout << "<b>(" << verseRef.getVerse() << ")</b>    " << verseText << endl;
}
