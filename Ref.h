// Class Ref
// Computer Science, MVNU
//
// Ref is a class for representing a Bible reference consisting of
//    * integer book   - the book from 1 (Genesis) to 66 (Revalation)
//    * integer chap   - the chapter number >= 1
//    * integer verse  - the verse number >= 1
//

#ifndef Ref_H
#define Ref_H
#include <string> 
#include <stdlib.h>
using namespace std;

string GetNextToken(string&, const string&);

class Ref {
private:
   short book, chap, verse;							// Reference information
public:
   Ref();  											// Default constructor
   Ref(string s); 									// Parse constructor - example parameter "43:3:16"
   Ref(const int, const int, const int); 			// Construct from three integers;
   // Accessors
   string getBookName() const;						//Retrieves book name using book number
   int getBook() const;								//Access Book Number
   int getChap() const;								//Access Chapter Number
   int getVerse() const;							//Access Verse Number			
   bool operator==(const Ref);						//Comparison: determine if two references are equal
   bool operator<(const Ref r) const;				//Comparison for project 3
   void display() const; 							// Display the reference on cout, example output: John 3:16
												    // Your version of display must show the book name
													// corresponding to the stored book number.
};

#endif //Ref_H
