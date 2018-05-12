#pragma once
#include <iostream>
#include <vector>

struct Node
{
    int val = 0;
    Node* next = nullptr;
};

typedef bool (*IntListDelegate)(Node* node);

class IntList
{
    public:
    void append(int val);
    void insert(int val, int pos);
    int count() const;
    Node* at(int index);
    std::vector<int> toArray() const;

    template<typename T>
    void removeAllWhere(int fromIndex, T isTrue) {
        if(fromIndex < 0) {
            return;
        }

        Node* prev = nullptr;
        Node* node = nullptr;

        if(fromIndex == 0) {
            prev = nullptr;
            node = head;
        }
        else {
            prev = at(fromIndex - 1);
            if(prev == nullptr || prev->next == nullptr) {
                return;
            }
            node = prev->next;
        }

        while(node != nullptr)
        {
            if(isTrue(node)) {
                if(node == head) {
                    head = node->next;
                    delete node;
                    node = head;
                }
                else {
                    prev->next = node->next;
                    delete node;
                    node = prev->next;
                }
            }
            else {
                prev = node;
                node = node->next;
            }
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const IntList& list);
    private:
    Node* head = nullptr;
};


