// Bible class function definitions
// Computer Science, MVNU

#include "Ref.h"
#include "Verse.h"
#include "Bible.h" 
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <algorithm>


using namespace std;

//Bible::Bible() { infile = "/home/class/csc3004/Bibles/kjv-complete"; }	// Default constructor
//Bible::Bible(const string s) { infile = s; }						    // Constructor – pass bible filename

// Default constructor
Bible::Bible(){
	infile = "/home/class/csc3004/Bibles/kjv-complete";
	buildTextIndex();
}
//Constructor - passes bible filename
Bible::Bible(const string s) { 
	infile = s; 
	buildTextIndex();
}	

Bible::Bible(int i) {
	infile = BookVersionString[i];
	buildTextIndex();
}

//Main lookup function - will forward along all other overloads to this function using dummy data
vector<Verse> Bible::lookup(const int numberOfVerses, Ref ref, LookupResult &status, Ref& prev) {
	//initialize status to 'OTHER'
	status = OTHER;	

	//Create iterator
	map<Ref, int>::iterator iterator;
	iterator = index.find(ref);
	map<Ref, int>::iterator previousIterator = index.find(ref);

	//Check to see if the previous iterator is the beginning
	if (previousIterator != index.begin()) {
		--previousIterator;
		prev = (previousIterator->first);
	}

	//Check to see if the iterator has reached the end
	if (iterator == index.end()) {

		//Check for book
		Ref book = Ref(ref.getBook(), 1, 1);
		map<Ref, int>::iterator bookIterator;
		bookIterator = index.find(book);
		if (bookIterator == index.end()) {
			status = NO_BOOK;
			return vector<Verse>();
		}

		//Check for chapter
		Ref chapter = Ref(ref.getBook(), ref.getChap() , 1);
		map<Ref, int>::iterator chapterIterator;
		chapterIterator = index.find(chapter);

		if (chapterIterator == index.end()) {
			status = NO_CHAPTER;
			return vector<Verse>();
		}

		//if there's an issue, but it's not with the book or chapter, it's with the verse

		status = NO_VERSE;
		return vector<Verse>();
	}

	else {
		status = SUCCESS;

		ifstream file(infile.c_str());
		string currentLine;
		file.seekg(iterator->second);
		vector<Verse> vectorOfVerses;

		for (int i = 0; i < numberOfVerses&&getline(file, currentLine); i++) {
			Verse v = Verse(currentLine);
			vectorOfVerses.push_back(v);
		}
		return vectorOfVerses;
	}
}


//If only one verse is needed, pass along to overloaded lookup function with a 1 for the 'numberOfVerses' parameter
vector<Verse> Bible::lookup(Ref ref, LookupResult& status) { 
	return lookup(1,ref, status);
}

//If multiple verses are needed, but not the 'prev' reference, passes along a Ref 'r' to fulfill this prev parameter.
vector<Verse> Bible::lookup(const int numberOfVerses, Ref ref, LookupResult& status)
{
	Ref r;
	return lookup(numberOfVerses, ref, status, r);
}


const Ref Bible::next(const Ref ref, LookupResult& status){
	//Gets a verse and its next, but only returns the next verse
	vector<Verse> tempVerse = lookup(2, ref, status);
	Verse nextVerse = tempVerse[1];
	Ref r = nextVerse.getRef();
	return r;
};

    //Gets a verse and its previous, but only returns the previous verse
const Ref Bible::prev(const Ref ref, LookupResult& status) {
	Ref previous;
	vector<Verse> verse = lookup(1,ref,status,previous);
	return previous;
};

void Bible::buildTextIndex() {

	ifstream file(infile.c_str());			// input file descriptor
	string line;							//line in file

	//Process the file
	bool done = false;
	while (!done) {
		//Gets file position at beginning of the line
		int position = file.tellg();
		done = !getline(file, line);
		//Uppercase
		//transform(line.begin(), line.end(), line.begin(), ::toupper);

		if (line != "") {
			//For every ref in the line, add  the ref to the map
			Ref currentRef = Ref(line);
			index[currentRef] = position;
		}
	}
}

// Return an error message string to describe status
const string Bible::error(LookupResult status){
	return "Error - " + LookupResultStrings[status] + "\n";
};
	
void Bible::display() {
	cout << "Bible file: " << infile << endl;
}