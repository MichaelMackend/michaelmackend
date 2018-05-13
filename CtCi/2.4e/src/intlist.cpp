#include "intlist.h"
#include <algorithm>//<math.h>

Node* getTail(Node* node) {
    while(node->next != nullptr) {
        node = node->next;
    }
    return node;
}


void IntList::append(int val) {

    Node* node = new Node();
    node->val = val;

    if(head == nullptr) {
        head = node;
    } else {
        getTail(head)->next = node;
    }
}


void IntList::insert(int val, int pos) {


    Node* node = new Node();
    node->val = val;

    if(pos == 0) {
        node->next = head;
        head = node;
    } else {
        pos = std::min(pos, count() - 1);
        if(pos < 0) {
            return;
        }
        Node* prev = at(pos - 1);
        node->next = prev->next;
        prev->next = node;
    }
}

int IntList::count() const {
    int count = 0;
    Node* counter = head;
    while(counter != nullptr) {
        count++;
        counter = counter->next;
    }
    return count;
}


std::vector<int> IntList::toArray() const {
    std::vector<int> returnArray;
    Node* node = head;
    while(node) {
        returnArray.push_back(node->val);
        node = node->next;
    }
    return returnArray;
}

Node* IntList::at(int index) {

    Node* node = head;
    int current = 0;
    while(node != nullptr) {
        if(current == index) {
            return node;
        }
        node = node->next;
        ++current;
    }

    return nullptr;
}

void IntList::reverse() {

    //initialize some values
    Node* current = head;
    head = nullptr;
    Node* prev = nullptr;
    Node* next = nullptr;

    //process the reversals
    while(current != nullptr) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    //point to the new head
    head = prev;
}


std::ostream& operator<<(std::ostream& os, const IntList& list) {
    Node* node = list.head;
    os << "[ ";

    while(node != nullptr) {
        os << node->val << " ";
        node = node->next;
    }

    os << "]";
    return os;
}

