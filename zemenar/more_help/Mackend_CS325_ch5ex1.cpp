// Mackend_CS325_ch5ex1.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <string>
#include <fstream>
#include "persontypeL.h"
#include <stdlib.h>
#include "adressBookTypeL.h"
#include "persontypeL.h"
#include "linkedListIterator.h"
#include "linkedListType.h"
#include "unorderedLinkedList.h"
#include "nodeType.h"


int main()
{
	string lastName;
	string relStatus;
	string inpLast1, inpLast2;
	int relSelect;
	int birthMonthSelect;
	addressBookType entries;

	/*entries.massOutput();  // Outputs the entire directory
	
	cout << "\nEnter a last name to search for: ";      // last name search
	cin >> lastName;
	cout << endl;
	entries.seqSearch(lastName);        // runs a search and prints the result

	cin.get();
	cin.get();
	system("CLS");             // clears the screen

	cout << "\nFind all people with relationship status... \n\n1) Family\n2) Friend\n3) Business Associate\n\n---->:";   // Outputs all people under chosen relationship status
	cin >> relSelect;
	if (relSelect == 1)
	{
		relStatus = "Family";
	}
	else if (relSelect == 2)
	{
		relStatus = "Friend";
	}
	else if (relSelect == 3)
	{
		relStatus = "BusinessAssoc";
	}
	else
	{
		cout << "Invalid entry..." << endl;
	}
	entries.relSearch(relStatus);

	cin.get();
	cin.get(); 

	system("CLS");               // clears the screen
	entries.massOutput();  // Outputs the entire directory

	cout << "\n\nEnter the last name of first person: ";     // Outputs all people in directory between the two names selected. 
	cin >> inpLast1;
	cout << "Enter the last name of second person: ";
	cin >> inpLast2;
	entries.blnSearch(inpLast1, inpLast2);

	cin.get();
	cin.get();
	system("CLS");               // clears the screen

	entries.massOutput();  // Outputs the entire directory

	cout << "See all people born in which month...?" << endl;
	cout << "1) January\n2) February\n3) March\n4) April\n5) May\n6) June\n7) July\n8) August\n9) September\n10) October\n11) November\n12) December\n---->:";
	cin >> birthMonthSelect;
	entries.bMonthSearch(birthMonthSelect);       // Outputs all people in directory sharing chosen birth month.
	*/

	cin.get();
	cin.get();
    return 0;
}

