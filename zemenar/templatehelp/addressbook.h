#ifndef _ADDRESSBOOK_H
#define _ADDRESSBOOK_H

#include <iostream>
#include <string>
#include "arrayListType.h"
#include <fstream>

using namespace std;


class addressBookType : public arrayListType<extPersonType>
{
public:
	int searchByName() const {
		string keyName, name, temp;

		int count = 0;

		// prompt and read the person name to search

		cout << "\n\n\tEnter the last name to search : ";

		cin >> keyName;

		// go through the address book

		for (int i = 0; i < ind; i++)

		{

			// get last name from address-book

			name = persons[i].getLastName();

			// compare the names

			if (keyName.compare(name) == 0)

				return i;
		}

		return -1;

	}   // end fucntion searchByName

	void printAPerson() const
	{

		int result = searchByName();

		if (result == -1)

			cout << "\n\tNo person found as specified by you.";

		else

		{

			// print the name upon match

			cout << "\n\t";

			persons[result].printName();

		}   // end else-if

	}   // end function printAPerson

	addressBookType()
	{
		string STRING;
		ifstream theFile("people.txt");

		string fFirstName;
		string fLastName;
		string fAddress;
		string fAddress2;
		string fCity, fState, fZip;
		int fRelation;
		string fPhone;
		int i = 0;

		while (theFile >> fFirstName >> fLastName >> fAddress >> fAddress2 >> fCity >> fState >> fZip >> fRelation >> fPhone)
		{
			persons[i].setName(fFirstName, fLastName);
			persons[i].setAddress(fAddress, fAddress2, fCity, fState, fZip);
			persons[i].setRelation(fRelation);
			persons[i].setPhone(fPhone);
			i++;
		}
	} // end of constructor

	void massOutput()
	{

		for (int j = 0; j < 4; j++)
		{
			persons[j].printName();
			persons[j].printAddress();
			persons[j].printRelation();
			persons[j].printPhone();
		}
	}

// (MEM) all inhereted virtual methods must be implemented when they are pure.

    virtual void insertAt(int location, const extPersonType& insertItem) {

    }

    virtual void insertEnd(const extPersonType& insertItem) {

    }

    virtual void replaceAt(int location, const extPersonType& repItem) {

    }

    virtual int seqSearch(const extPersonType& searchItem) const {
        return 0;
    }

    virtual void remove(const extPersonType& removeItem) {

    }

private:
	int ind;
	extPersonType persons[500];
};

#endif

