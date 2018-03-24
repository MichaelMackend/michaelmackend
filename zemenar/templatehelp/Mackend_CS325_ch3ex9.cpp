// Mackend_CS325_ch3ex9.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include "arrayListType.h"
#include <fstream>

using namespace std;

class extPersonType
{
public: 

	void printName() const
	{
		cout << firstName << " " << lastName;
	}
	
	void printAddress() const
	{
		cout << dAddress << " " << dAddress2 << " " << dCity << " " << dState << " " << dZip << endl;
	}

	void setName(string first, string last)
	{
		firstName = first;
		lastName = last;
	}

	string getFirstName() const
	{
		return firstName;
	}

	string getLastName() const
	{
		return lastName;
	}

		//Default constructor
	extPersonType()
	{
		firstName = "";
		lastName = "";
	}

	extPersonType(string first, string last)
	{
		firstName = first;
		lastName = last;
	}

	void setAddress(string address, string address2, string city, string state, string zip)
	{
		dAddress = address;
		dAddress2 = address2;
		dCity = city;
		dState = state;
		dZip = zip;
	}

	string getAddress()
	{
		return dAddress;
	}

	string getCity()
	{
		return dCity;
	}

	string getState()
	{
		return dState;
	}

	string getZip()
	{
		return dZip;
	}

public:
	void setDate(int month, int day, int year)
	{
		if (year >= 0)
		{
			if (month >= 1 && month <= 12)
			{
				if (month == 1)
				{
					if (day >= 1 && day <= 31)
					{
						dMonth = month;
						dDay = day;
						dYear = year;
					}
					else
					{
						cout << "Invalid date." << endl;
					}
				}
				else if (month == 2 && year % 4 == 0)
				{
					if (day >= 1 && day <= 29)
					{
						dMonth = month;
						dDay = day;
						dYear = year;
					}
					else
					{
						cout << "Invalid date." << endl;
					}
				}
				else if (month == 2 && year % 4 >= 1)
				{
					if (day >= 1 && day <= 28)
					{
						dMonth = month;
						dDay = day;
						dYear = year;
					}
					else
					{
						cout << "Invalid date." << endl;
					}
				}
				else if (month == 3)
				{
					if (day >= 1 && day <= 31)
					{
						dMonth = month;
						dDay = day;
						dYear = year;
					}
					else
					{
						cout << "Invalid date." << endl;
					}
				}
				else if (month == 4)
				{
					if (day >= 1 && day <= 30)
					{
						dMonth = month;
						dDay = day;
						dYear = year;
					}
					else
					{
						cout << "Invalid date." << endl;
					}
				}
				else if (month == 5)
				{
					if (day >= 1 && day <= 31)
					{
						dMonth = month;
						dDay = day;
						dYear = year;
					}
					else
					{
						cout << "Invalid date." << endl;
					}
				}
				else if (month == 6)
				{
					if (day >= 1 && day <= 30)
					{
						dMonth = month;
						dDay = day;
						dYear = year;
					}
					else
					{
						cout << "Invalid date." << endl;
					}
				}
				else if (month == 7)
				{
					if (day >= 1 && day <= 31)
					{
						dMonth = month;
						dDay = day;
						dYear = year;
					}
					else
					{
						cout << "Invalid date." << endl;
					}
				}
				else if (month == 8)
				{
					if (day >= 1 && day <= 31)
					{
						dMonth = month;
						dDay = day;
						dYear = year;
					}
					else
					{
						cout << "Invalid date." << endl;
					}
				}
				else if (month == 9)
				{
					if (day >= 1 && day <= 30)
					{
						dMonth = month;
						dDay = day;
						dYear = year;
					}
					else
					{
						cout << "Invalid date." << endl;
					}
				}
				else if (month == 10)
				{
					if (day >= 1 && day <= 31)
					{
						dMonth = month;
						dDay = day;
						dYear = year;
					}
					else
					{
						cout << "Invalid date." << endl;
					}
				}
				else if (month == 11)
				{
					if (day >= 1 && day <= 30)
					{
						dMonth = month;
						dDay = day;
						dYear = year;
					}
					else
					{
						cout << "Invalid date." << endl;
					}
				}
				else if (month == 12)
				{
					if (day >= 1 && day <= 31)
					{
						dMonth = month;
						dDay = day;
						dYear = year;
					}
					else
					{
						cout << "Invalid date." << endl;
					}
				}
			}
			else
			{
				cout << "Invalid date." << endl;
			}
		}
		else
		{
			cout << "Invalid date." << endl;
		}
	}

	int getDay() const
	{
		return dDay;
	}

	int getMonth() const
	{
		return dMonth;
	}

	int getYear() const
	{
		return dYear;
	}

	void printDate() const
	{
		cout << dMonth << "-" << dDay << "-" << dYear;
	}

	void dateType(int month, int day, int year)
	{
		setDate(month, day, year);
	}

	void isLeapYear(int year)
	{
		if (year % 4 == 0)
		{
			cout << "\n\nIt is a leap year!" << endl;
		}
		else
		{
			cout << "\n\nIt isn't a leap year..." << endl;
		}
	}

	void setRelation(int relation) // relation input must be 1, 2, or 3 respectively. 
	{
		if (relation == 1)
		{
			dRelation == "Family Member";
		}
		else if (relation == 2)
		{
			dRelation == "Friend";
		}
		else if (relation == 3)
		{
			dRelation == "Business Associate";
		}
		else
		{
			dRelation == "Unknown Relation";
		}
	}

	void printRelation() const
	{
		cout << dRelation << endl;
	}

	string getRelation() const
	{
		return dRelation;
	}

	void setPhone(string phone)
	{
		dPhone = phone;
	}

	string getPhone() const
	{
		return dPhone;
	}

	void printPhone() const
	{
		cout << dPhone << endl;
	}

private:
	string firstName;
	string lastName;
	string dAddress;
	string dAddress2;
	string dCity;
	string dState;
	string dZip;
	int dMonth; // variable to store the month
	int dDay; // variable to store the day
	int dYear; // variable to store the year
	string dRelation;
	string dPhone;
};

class addressBookType : public arrayListType<extPersonType>
{
public:
	int searchByName() const
	{
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




private:
	int ind;
	extPersonType persons[500];
};



int main()
{
	addressBookType<extPersonType> entries[500];
	

	cin.get();
	cin.get();
    return 0;
}

