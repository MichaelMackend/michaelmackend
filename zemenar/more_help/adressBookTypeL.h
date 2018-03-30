#ifndef _ADDRESSBOOKL_H
#define _ADDRESSBOOKL_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "persontypeL.h"
#include "linkedListIterator.h"
#include "linkedListType.h"
#include "unorderedLinkedList.h"
#include "nodeType.h"

using namespace std;


class addressBookType : public unorderedLinkedList<extPersonType>
{
public:

	addressBookType() :unorderedLinkedList()
	{
		string STRING;
		ifstream theFile("people.txt");

		string fFirstName;
		string fLastName;
		string fAddress;
		string fAddress2;
		string fCity, fState, fZip;
		string fRelation;
		string fPhone;
		int fMonth, fDay, fYear;
		

		while (theFile >> fFirstName >> fLastName >> fAddress >> fAddress2 >> fCity >> fState >> fZip >> fRelation >> fPhone >> fMonth >> fDay >> fYear)
		{
			extPersonType person;
			person.setName(fFirstName, fLastName);
			person.setAddress(fAddress, fAddress2, fCity, fState, fZip);
			person.setRelation(fRelation);
			person.setPhone(fPhone);
			person.setDate(fMonth, fDay, fYear);
			this->insertLast(person);
		}

	} // end of constructor

	/*void massOutput()
	{
		cout << "------------------------- Address book directory -------------------------\n" << endl;
		print();
		cout << "--------------------------------------------------------------------------" << endl;
	}



	void printSelectedPerson(int p)
	{
		cout << persons[p].printPerson() << "   DOB: ";
		persons[p].printDate();
		cout << endl;
	}

	int seqSearch(string inputLast)
	{     

		string tempLast;
		cout << "\n====================== Last Name Search Function ======================" << endl;
		for (int i = 0; i < persons.size(); i++)
		{
			tempLast = persons[i].getLastName();
			if (tempLast == inputLast)
			{
				printSelectedPerson(i);
			}
		}
		return 0;
	}

	virtual int relSearch(string inputRelation)
	{         

		cout << "====================== Relationship Status Search Function ======================" << endl;
		string tempRel;
		for (int i = 0; i < persons.size(); i++)
		{
			tempRel = persons[i].getRelation();
			if (tempRel == inputRelation)
			{
				printSelectedPerson(i);
			}
		}
		return 0;
	}

	virtual int blnSearch(string inputLN1, string inputLN2)
	{   

		string tempLast1, tempLast2;
		int j, k;
		cout << "\n====================== Between Names Search Function ======================" << endl;
		for (int i = 0; i < persons.size(); i++)
		{
			tempLast1 = persons[i].getLastName();
			if (tempLast1 == inputLN1)
			{
				j = i + 1;
			}
		}

		for (int i = 0; i < persons.size(); i++)
		{
			tempLast2 = persons[i].getLastName();
			if (tempLast2 == inputLN2)
			{
				k = i;
			}
		}

		for (int i = j; i < k; i++)
		{
			printSelectedPerson(i);
		}

		return 0;
	}

	virtual int bMonthSearch(int inputBirthMonth) 
	{
		cout << "====================== Birth Month Search Function ======================" << endl;
		int tempMonth;
		for (int i = 0; i < persons.size(); i++)
		{
			tempMonth = persons[i].getMonth();
			if (tempMonth == inputBirthMonth)
			{
				printSelectedPerson(i);
			}
		}
		return 0;
	}

	

private:
	vector<extPersonType> persons;
	*/
//(MEM) you had commented out the closing brace on accident
};

#endif

