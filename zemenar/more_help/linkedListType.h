// Abstract class to specify the members to implement the basic properties of a linked list. We can't instantiate an object of this class.
//(MEM) you were missing "#pragma once".  this is equivalent to codeguards ie. #ifndef __MARKSLINKEDLISTTYPEHEADER_H #define __MARKSLINKEDLISTTYPEHEADER_H google it :)
#pragma once

template <class Type>
class linkedListType
{
public:
	const linkedListType<Type>& operator=(const linkedListType<Type>&);
	//Overload the assignment operator

	void initializeList();
	//Initialize the list to an empty state.
	//Postcondition: first = NULL, last = NULL, count = 0;

	bool isEmptyList() const;
	//Function to determine whether the list is empty.
	//Postcondition: Returns true if the list is empty, otherwise it returns false.

	void print() const;
	//Function to output the data contained in each node. 
	//Postcondition: none

	int length() const;
	//Function to return the number of nodes in the list. 
	//Postcondition: The value of count is returned. 

	void destroyList();
	//Function to delete all the nodes from the list.
	//Postcondition: first = NULL, last = NULL, count = 0;

	Type front() const;
	//Function to return the first element of the list. 
	//Precondition: The list must exist and must not be empty
	//Postcondition: If the list is empty, the program terminates;
	//       otherwise, the first element of the list is returned.

	Type back() const;
	//Function to return the last element of the list
	//Precondition: the list must exist and must not be empty
	//Postcondition: If the list is empty, the program terminates; otherwise
	//       the last elemetn of the list is returned. 

	virtual bool search(const Type& searchItem) const = 0;
	//Fucntion to determine whether searchItem is in the list.
	//Postcondition: Returns true if searchItem is in the list,
	//         otherwise it returns false

	virtual void insertFirst(const Type& newItem) = 0;
	//Function to insert newItem at the beginning of the list.
	//Postcondition: first points to the new list, newItem is inserted
	//    at the beginning of the list, last points to the last node
	//     in the list, and count is incremented by 1.

	virtual void insertLast(const Type& newItem) = 0;
	//Function to insert newItem at the end of the list.
	//Postcondition: first points to the new list, newItem is inserted
	//    at the end of the list, last points to the last node
	//     in the list, and count is incremented by 1.

	virtual void deleteNode(const Type& deleteItem) = 0;
	//Function to delete deleteItem from the list. 
	//Postcondition: If found, the node containing deleteItem is deleted 
	//     from the list. first points ot the first node, last points
	//     to the last node of the updated list, and count is decremented by 1. 

	linkedListIterator<Type> begin();
	//Function to return an iterator at the beginning of the linked list. 
	//Postcondition: Returns an iterator such that current is set to first.

	linkedListIterator<Type> end();
	//Function to return an iterator one element past the last element of the linked list.
	//Postcondition: Returns an iterator such that current is set to NULL.

	linkedListType();
	//default constructor
	//Initializes the list to an empty state.
	//Postcondition: first = NULL, last = NULL, count = 0;

	linkedListType(const linkedListType<Type>& otherList);
	//copy constructor

	~linkedListType();
	//destructor
	//Deletes all the nodes from the list
	//Postcondition: The list object is obliterated.

protected:
	int count; //Variable to store the number of list elements
	nodeType<Type> *first; //pointer to the first node of the list.
	nodeType<Type> *last; //pointer to the last node of the list. 

private: 
	void copyList(const linkedListType<Type>& otherList);
	//Function to make a copy of otherList
	//Postcondition: A copy of otherList is created and assigned to this list. 

};

template<class Type>
linkedListType<Type>::linkedListType() //Default constructor
{
	this->first = NULL;//(MEM) mis-spelled NULL as Null.  
	this->last = NULL;
	this->count = 0;
}

template <class Type>
void linkedListType<Type>::destroyList()
{
	nodeType<Type> *temp; //pointer to deallocate the memory occupied by the node

	while (first != NULL) //while there are no nodes in the list....
	{
		temp = first;              //set temp to the current node
		first = first->link;       //advance  first to the next node
		delete temp;               //deallocate the memory occupied by temp
	}

	last = NULL;        //initialize last to NULL; first has already been set to NULL by the while loop
	count = 0;
}

template <class Type>
void linkedListType<Type>::initializeList()
{
	destroyList(); //if the list has any nodes, delete them
}

template <class Type>
void linkedListType<Type>::print() const
{
	nodeType<Type> *current; //pointer to traverse the list

	current = first; //set current point to the first node
	while (current != NULL)  //while more data to print
	{
		cout << current->info << " ";
		current = current->link;
	}
}  // end print

template <class Type> 
int linkedListType<Type>::length() const
{
	return count;
}

template <class Type>
Type linkedListType<Type>::front() const
{
	assert(first != NULL);

	return first->info; //return the info of the first node
}

template <class Type>
Type linkedListType<Type>::back() const
{
	assert(last != NULL);

	return last->info; //return the info of the last node
} 

template <class Type> 
linkedListIterator<Type> linkedListType<Type>::begin()
{
	linkedListIterator<Type> temp(first);

	return temp;
}

template <class Type>
linkedListIterator<Type> linkedListType<Type>::end()
{
	linkedListIterator<Type> temp(NULL);

	return temp;
}

template <class Type>
void linkedListType<Type>::copyList(const linkedListType<Type>& otherList)
{
	nodeType<Type> *newNode; //pointer to create a node
	nodeType<Type> *current; //pointer to traverse the list

	if (first != NULL) //if the list is nonempty, make it empty
		destroyList();

	if (otherList.first == NULL) //otherList is empty
	{
		first = NULL;
		last = NULL; 
		count = 0;
	}
	else
	{
		current = otherList.first; //current points to the list to be copied

		count = otherList.count;

		//copy the first node:
		first = new nodeType<Type>; //create the new node
		first->info = current->info //copy the info
		first->link == NULL; //set the link field of the node to NULL
		last = first;                   //make last point to the first node
		current = current->link; //make current point to the next node

		//copy the remaining list:
		while (current != NULL)
		{
			newNode = new nodeType<Type>; //create a node
			newNode->info = current->info; //copy the info
			newNode->link = NULL; //set the link of newNode to NULL
			last->link = newNode; //attach newNode after last
			last = newNode; //make last point to the actual last node
			current = current->link; //make current point to the next node
		}
	}
}

template <class Type>
linkedListType<Type>::~linkedListType() // destructor
{
	destroyList();
}

template <class Type>
linkedListType<Type>::linkedListType(const linkedListType<Type>& otherList)
{
	first = NULL;
	copyList(otherList);
}

//overload the assignment operator
template <class Type>
const linkedListType<Type>& linkedListType<Type>::operator=(const linkedListType<Type>& otherList)
{
	if (this != &otherList) //avoid self copy
	{
		copyList(otherList);
	}

	return *this;
}

