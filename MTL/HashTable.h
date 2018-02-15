//
// Created by michael on 2/10/18.
//

#ifndef MICHAELMACKEND_HASHTABLE_H
#define MICHAELMACKEND_HASHTABLE_H

namespace michaelmackend {

	template<class TKey>
    bool DefaultEqualityComp(const TKey& a, const TKey& b)
    {
        return a == b;
    }

	template<class TKey>
	long Jenkins1MemoryHash(const TKey& key) {
		unsigned long i = 0;
		unsigned long length = sizeof(TKey);
		unsigned int hash = 0;
		const char* mem = (const char*)&key;
		while (i != length) {
			hash += mem[i++];
			hash += hash << 10;
			hash ^= hash >> 6;
		}
		hash += hash << 3;
		hash ^= hash >> 11;
		hash += hash << 15;
		return hash;
	}

    template<class TKey, class TVal, bool (*KeyComp)(const TKey&, const TKey&) = DefaultEqualityComp, long(*HashFunc)(const TKey&) = Jenkins1MemoryHash, unsigned long TableSize = 283>
    class HashTable {
    public:

        HashTable();

        TVal& operator[](const TKey &key);
        bool Erase(const TKey& key);
        bool Empty() const;

    private:
        struct KeyValuePair {
            KeyValuePair(const TKey& key, const TVal& val) : _key(key), _val(val), _next(nullptr) {}
            TKey _key;
            TVal _val;
            KeyValuePair* _next;
            KeyValuePair* _prev;
        };
        
    private:
        KeyValuePair* findKey(KeyValuePair* head, const TKey& key) {
            if(head == nullptr) {
                return nullptr;
            }
            while(head->_key != key && head->_next != nullptr) {
                head = head->_next;
            }
            return head;
        }

    private:
        KeyValuePair* _table[TableSize];
        unsigned int _count;
    };

    template<class TKey, class TVal, bool (*KeyComp)(const TKey&, const TKey&), long (*HashFunc)(const TKey &), unsigned long TableSize>
    HashTable<TKey, TVal, KeyComp, HashFunc, TableSize>::HashTable() : _count(0), _table({nullptr}) {}


    template<class TKey, class TVal, bool (*KeyComp)(const TKey&, const TKey&), long(*HashFunc)(const TKey&), unsigned long TableSize>
    TVal& HashTable<TKey, TVal, KeyComp, HashFunc, TableSize>::operator[](const TKey &key) {

        const long hashVal = HashFunc(key);
        const long index = hashVal % TableSize;
		KeyValuePair* kvp = findKey(_table[index],key);
		if (kvp == nullptr) {
            if(_table[index] == nullptr) {
                _table[index] = kvp = new KeyValuePair(key, TVal());
            }
            else {
                kvp = _table[index];
                while (kvp->_next != nullptr) {
                    kvp = kvp->_next;
                }
                kvp->_next = new KeyValuePair(key, TVal());
                kvp->_next->_prev = kvp;
                kvp = kvp->_next;
            }
		}
		return kvp->_val;
    }

    template<class TKey, class TVal, bool (*KeyComp)(const TKey&, const TKey&), long (*HashFunc)(const TKey &), unsigned long TableSize>
    bool HashTable<TKey, TVal, KeyComp, HashFunc, TableSize>::Erase(const TKey &key) {

        const long hashVal = HashFunc(key);
        const long index = hashVal % TableSize;
        KeyValuePair* kvp = findKey(_table[index],key);
        if(kvp == nullptr) {
            return false;
        }
        kvp->_prev = kvp->_prev != nullptr ? kvp->_prev->_prev : nullptr;
        kvp->_next = kvp->_next != nullptr ? kvp->_next->_next : nullptr;
        if(kvp == _table[index]) {
            _table[index] = nullptr;
        }
        delete kvp;
        return true;
    }

    template<class TKey, class TVal, bool (*KeyComp)(const TKey&, const TKey&), long (*HashFunc)(const TKey &), unsigned long TableSize>
    bool HashTable<TKey, TVal, KeyComp, HashFunc, TableSize>::Empty() const {
        return _count == 0;
    }

}



#endif //MICHAELMACKEND_HASHTABLE_H
