#include <exception>
#include "megasum.h"
#include "intlist.h"

IntList buildIntList(const std::string s) {
    IntList l;
    for(int i = 0; i < s.size(); ++i) {
        char c = s[i];
        int val = c - '0';
        if(val < 0 || val > 9) {
            throw std::invalid_argument("failed building list from string that was not numeric!");
        }
        l.append(val);
    }
    return l;
}

std::string megasum(const std::string& s1, const std::string& s2) {

    IntList l1 = buildIntList(s1);
    IntList l2 = buildIntList(s2);

    l1.reverse();
    l2.reverse();

    Node* n1 = l1.at(0);
    Node* n2 = l2.at(0);

    IntList l3;

    int carry = 0;
    while(true) {
        int v1 = n1 != nullptr ? n1->val : 0;
        int v2 = n2 != nullptr ? n2->val : 0;

        int sum = v1 + v2 + carry;
        if(sum == 0) {
            break;
        }

        if(sum > 9) {
            carry = 1;
            sum -= 10;
        } else {
            carry = 0;
        }

        l3.insert(sum, 0);

        n1 = n1 != nullptr ? n1->next : nullptr;
        n2 = n2 != nullptr ? n2->next : nullptr;
    }

    if(l3.count() == 0) {
        return "0";
    }

    std::string result;

    l3.forEach([&result](Node* node) {
            result += '0' + (char)node->val;
            });


    return result;
}
