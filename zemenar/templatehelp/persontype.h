#ifndef _PERSONTYPE_H
#define _PERSONTYPE_H

// Mackend_CS325_ch3ex9.cpp : Defines the entry point for the console application.
//

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

#endif 
