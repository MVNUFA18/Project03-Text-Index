// Class Verse
// Computer Science, MVNU
//
// A Verse object represents a complete Bible verse, including its reference
//    * Ref ref          - the reference (e.g., 1:1:1)
//    * string verseText - the actual verse text (without reference)
//

#ifndef Verse_H
#define Verse_H
#include <string> 
#include <stdlib.h>
#include "Ref.h"
using namespace std;

class Verse {
 private:
   Ref verseRef;        // The reference for this verse.
   string verseText;  	// actual verse text (without reference)

 public:
   Verse();   	// Default constructor
   Verse(const string s); // Parse constructor
		// pass entire line from file including reference and text

   // Accessors
   string getVerse();
   Ref getRef();

   // Display Verse on cout
   void display(); // Display ref & verse [with line breaks].
        // the reference should be displayed by calling verseref.display().
};

#endif //Verse_H
