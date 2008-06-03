/*
 *    pwTable.h - Header file for the table template.
 *
 *    Copyright (C) 2008, Richard Pennington.
 */

#ifndef pwTable_h
#define pwTable_h

#include <string>
#include "pwArray.h"

namespace pw {

template<class Type> class Table { 
public:
    Table();                                  // Basic constructor.
    ~Table();                                 // Destructor.
    Table(const Table& value);              // Copy constructor.
    Table<Type>& operator+=(const Type& value);     // Append an element to a table.
    Table<Type>& operator+=(const Table& value);  // Append a table to a table.
    Table<Type>& operator=(const Table& value);   // Assign to a table.
    Type& operator[](int element);              // Array reference.
    Type& operator[](int element) const;        // Array reference.
    int size() const                            // Return the size of a table.
      { return contents.size(); }
    Table<Type> slice(int from, int to) const;// Return a slice of a table.
    void truncate();                            // Truncate a table.
    void remove(int count);                     // Remove elements from a table.
    void add(const Type& value)                 // Add a table element.
        { *this += value; }
    Type lookup(const std::string& name);
    Type lookup(const char* name);
    void lookup(const std::string& name, Type& result);
    void lookup(const char* name, Type& result);
    void lookup(const std::string& name, array<Type>& result);
    void lookup(const char* name, array<Type>& result);
    void reHash();

private:
    enum {
        HASHSIZE = 1024,                        // Size of hash table.
        THRESHOLD = 30,                         // Minimum contents threshold before the hash table is created.
    };
    void init();                                // Table initialization function.
    void checkHash();                           // Check whether a hash table is needed, create it if so.
    struct element {
        Type value;                             // Entry value.
        int next;                               // Next element in the hash bucket.
    };
    void insert(int index);                     // Insert an entry in the hash table.
    static unsigned int hash(const std::string& name); // Compute the hash value of a name.
    static unsigned int hash(const char* name); // Compute the hash value of a name.
    array<element> contents;
    int* hashTable;                             // Hash table for contents.
};


//
// init - Initialize a table.
//
template<class Type> void Table<Type>::init()
{
    hashTable = NULL;
}

//
// Table - Construct a table.
//
template<class Type> Table<Type>::Table()
{
    init();
}

//
// Table - Construct a table (copy).
//
template<class Type> Table<Type>::Table(const Table& value)
{
    init();
    if (value.size()) {
        for (int i = 0; i < value.size(); ++i) {
            contents.array[i] = value.contents[i];
        }
    }
}

//
// ~Table - Destruct a table.
//
template<class Type> Table<Type>::~Table()
{
    delete[] hashTable;
}

//
// checkHash - Check to see if a hash table is needed.
//
template<class Type> void Table<Type>::checkHash()
{
    if (hashTable != NULL || contents.size() < THRESHOLD) {
        // The hash table already exists, or the threshold hasn't been hit.
        return;
    }

    hashTable = new int[HASHSIZE];
    for (int i = 0; i < HASHSIZE; ++i) {
        hashTable[i] = -1;                      // All buckets are empty.
    }

    // Insert entries already present in order of their definition..
    for (int i = 0; i < contents.size(); ++i) {
        insert(i);
    }
}

//
// insert - Insert an entry in the hash table.
//
template<class Type> void Table<Type>::insert(int index)
{
    if (hashTable == NULL) {
        // The hash table doesn't exist, yet.
        return;
    }

    if (contents[index].value == NULL) {
        // Ignore NULL entries.
        return;
    }

    unsigned int value = hash(contents[index].value->name());
    contents[index].next = hashTable[value];
    hashTable[value] = index;
}

//
// hash - compute the hash value for a string.
//
template<class Type> unsigned int Table<Type>::hash(const std::string& name)
{
    unsigned int value = 0;

    for (size_t i = 0; i < name.length(); ++i) {
        value += name[i];
    }

    return value % HASHSIZE;
}

//
// hash - compute the hash value for a char*.
//
template<class Type> unsigned int Table<Type>::hash(const char* name)
{
    unsigned int value = 0;

    for ( ; *name; ++name) {
        value += *name;
    }

    return value % HASHSIZE;
}

//
// [] - Access a Table as an lvalue.
//
template<class Type> Type& Table<Type>::operator[](int element)
{
    reHash();
    return contents[element].value;
}

//
// [] - Access a Table as an rvalue.
//
template<class Type> Type& Table<Type>::operator[](int element) const
{
    // RICH: Check for array out of bounds... Exception?
    return contents[element].value;
}

//
// += - Append an element to a table.
//
template<class Type> Table<Type>& Table<Type>::operator+=(const Type& value)
{
    int size = contents.size();

    contents[size].value = value;
    contents[size].next = -1;
    insert(size);
    return *this;
}

//
// += - Append a table to a table.
//
template<class Type> Table<Type>& Table<Type>::operator+=(const Table<Type>& value)
{
    for (int i = 0; i < value.size(); ++i) {
        *this += value[i];
    }
    
    return *this;
}

//
// = - Assign a table to a table.
//
template<class Type> Table<Type>& Table<Type>::operator=(const Table<Type>& value)
{
    truncate();
    *this += value;
    return *this;
}

//
// slice - return a range from a table.
//
template<class Type> Table<Type> Table<Type>::slice(int from, int to) const
{
    Table result;

    for(int i = from; i < size() && i < to; ++i) {
        result += contents[i];
    }
    return result;
}

//
// truncate - Truncate a table.
//
template<class Type> void Table<Type>::truncate()
{
    contents.truncate();
    reHash();
}

//
// remove - Remove elements from a table.
//
template<class Type> void Table<Type>::remove(int count)
{
    contents.remove(count);
    reHash();
}

//
// lookup - Look up a name in a table. Find the most recent occurance.
//
template<class Type> Type Table<Type>::lookup(const std::string& name)
{
    Type result;
    lookup(name, result);
    return result;
}

//
// lookup - Look up a name in a table. Find the most recent occurance.
//
template<class Type> Type Table<Type>::lookup(const char* name)
{
    Type result;
    lookup(name, result);
    return result;
}

//
// lookup - Look up a name in a table. Find the most recent occurance.
//
template<class Type> void Table<Type>::lookup(const std::string& name, Type& result)
{
    // Find the most recently added entry first.
    // Do we need a hashTable?
    checkHash();

    if (hashTable) {
        // Find an entry in the hash table.

        unsigned int value = hash(name);

        for (int i = hashTable[value]; i != -1; i = contents[i].next) {
            if (contents[i].value->name() == name) {
                result = contents[i].value;
                return;
            }
        }
    } else {
        // Do a linear search of the table.

        for (int i = contents.size() - 1; i >= 0; --i) {
            if (contents[i].value && contents[i].value->name() == name) {
                result = contents[i].value;
                return;
            }
        }
    }

    result = NULL;
}

//
// lookup - Look up a name in a table. Find the most recent occurance.
//
template<class Type> void Table<Type>::lookup(const char* name, Type& result)
{
    // Find the most recently added entry first.
    // Do we need a hashTable?
    checkHash();

    if (hashTable) {
        // Find an entry in the hash table.

        unsigned int value = hash(name);

        for (int i = hashTable[value]; i != -1; i = contents[i].next) {
            if (contents[i].value->name() == name) {
                result = contents[i].value;
                return;
            }
        }
    } else {
        // Do a linear search of the table.

        for (int i = contents.size() - 1; i >= 0; --i) {
            if (contents[i].value && contents[i].value->name() == name) {
                result = contents[i].value;
                return;
            }
        }
    }

    result = NULL;
}

//
// lookup - Look up a name in a table. Find all occurances.
//
template<class Type> void Table<Type>::lookup(const std::string& name, array<Type>& result)
{
    // Find the most recently added entry first.
    // Do we need a hashTable?
    checkHash();

    if (hashTable) {
        // Find an entry in the hash table.

        unsigned int value = hash(name);

        for (int i = hashTable[value]; i != -1; i = contents[i].next) {
            if (contents[i].value->name() == name) {
                result += contents[i].value;
            }
        }
    } else {
        // Do a linear search of the table.

        for (int i = contents.size() - 1; i >= 0; --i) {
            if (contents[i].value && contents[i].value->name() == name) {
                result += contents[i].value;
            }
        }
    }
}

//
// lookup - Look up a name in a table. Find all occurances.
//
template<class Type> void Table<Type>::lookup(const char* name, array<Type>& result)
{
    // Find the most recently added entry first.
    // Do we need a hashTable?
    checkHash();

    if (hashTable) {
        // Find an entry in the hash table.

        unsigned int value = hash(name);

        for (int i = hashTable[value]; i != -1; i = contents[i].next) {
            if (contents[i].value->name() == name) {
                result += contents[i].value;
            }
        }
    } else {
        // Do a linear search of the table.

        for (int i = contents.size() - 1; i >= 0; --i) {
            if (contents[i].value && contents[i].value->name() == name) {
                result += contents[i].value;
            }
        }
    }
}

//
// reHash - Force a rebuild of the hash table.
//
template<class Type> void Table<Type>::reHash()
{
    // Delete any current table.
    delete[] hashTable;
    hashTable = NULL;
    // This will cause a rebuild, if necessary, in the next lookup().
}

};

#endif
