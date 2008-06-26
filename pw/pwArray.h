/*
 *    pwArray.h - Header file for the array template.
 *
 *    Copyright (C) 2008, Richard Pennington.
 */

#ifndef pwArray_h
#define pwArray_h

namespace pw {

template<class Type> class array {         	// The Array class.
public:
    array();                                  // Basic constructor.
    ~array();                                 // Destructor.
    array(const array& value);              // Copy constructor.
    array<Type>& operator+=(const Type& value);     // Append an element to an array.
    array<Type>& operator+=(const array& value);  // Append an array to an array.
    array<Type>& operator=(const array& value);   // Assign to an array.
    array<Type>& truncate();                  // Truncate an array.
    array<Type>& remove(int count);           // Remove elements from an array.
    void add(const Type& value)                 // Add an array element.
        { *this += value; }
    Type& operator[](int element);              // Array reference.
    Type& operator[](int element) const;        // Array reference.
    int size() const                            // Return the size of an array.
      { return open - start; }
    array<Type> slice(int from, int to) const;// Return a slice of an array.

private:
    enum {
        ALLOC = 32
    };                                          // Array allocation unit.
    void init();                                // Array initialization function.
    array<Type>& create(int element);         // Create an element.
    Type* start;                                // Array buffer.
    Type* end;                                  // End of array buffer.
    Type* open;                                 // Next open array slot.
};


template<class Type> void array<Type>::init()
{
    start = end = open = NULL;
}

template<class Type> array<Type>::array()
{
    init();
}

template<class Type> array<Type>::array(const array& value)
{
    init();
    if (value.size()) {
        start = new Type[value.size()];
        for (int i = 0; i < value.size(); ++i) {
            start[i] = value.start[i];
        }
        open = start + (value.open - value.start);
        end = start + value.size();
    }
}

template<class Type> array<Type>::~array()
{
    if (start) {
	delete[] start;
    }
}

//
// [] - Access an Array as an lvalue.
//
template<class Type> Type& array<Type>::operator[](int element)
{
    if (element >= size()) {
        // Accessing a new element, create it.
        create(element);
        open = start + element + 1;     // The next element will be one past this one.
    }
    return start[element];
}

//
// [] - Access an Array as an rvalue.
//
template<class Type> Type& array<Type>::operator[](int element) const
{
    // RICH: Check for array out of bounds... Exception?
    return start[element];
}

//
// += - Append an element to an array.
//
template<class Type> array<Type>& array<Type>::operator+=(const Type& value)
{
    if (!open || open >= end - 1) {
        // Allocate space for the next element.
        create(size());
    }

    *open++ = value;
    return *this;
}

//
// += - Append an array to an array.
//
template<class Type> array<Type>& array<Type>::operator+=(const array<Type>& value)
{
    for (int i = 0; i < value.size(); ++i) {
        *this += value[i];
    }
    
    return *this;
}

//
// = - Assign an array to an array.
//
template<class Type> array<Type>& array<Type>::operator=(const array<Type>& value)
{
    truncate();
    *this += value;
    return *this;
}

//
// remove - Remove elements from an array.
//
template<class Type> array<Type>& array<Type>::remove(int count)
{
    if (!start) {
        return *this;
    }

    if (open - start < count) {
        count = open - start;
    }

    open -= count;
    return *this;
}

//
// slice - return a range from an array.
//
template<class Type> array<Type> array<Type>::slice(int from, int to) const
{
    array result;

    for(int i = from; i < size() && i < to; ++i) {
        result += start[i];
    }
    return result;
}

//
// truncate - Truncate an array.
//
template<class Type> array<Type>& array<Type>::truncate()
{
    open = start;
    return *this;
}

//
// create - Create a non-existant element.
//
template<class Type> array<Type>& array<Type>::create(int element)
{
    Type* newp = new Type[element + ALLOC];

    // Copy data.
    if (start) {
        for (int i = 0; i < size(); ++i) {
            newp[i] = start[i];
        }
    }
 
    // Adjust pointers.
    open = newp + (open - start);
    end = newp + (end - start);
    delete[] start;
    start = newp;

    end += ALLOC;
    return *this;
}

};

#endif
