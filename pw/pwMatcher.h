/*
 *    pwMatcher.h - Match strings and regular expressions.
 *
 *    Copyright (C) 2008, Richard Pennington.
 */

#ifndef pwMatcher_h
#define pwMatcher_h

#include <limits.h>
#include <string>
#include "pwArray.h"

namespace pw {

class MatchNode;

class Matcher {
public:
    typedef int Input;                          // State machine input type.
    enum { INPUTMAX = INT_MAX,                  // Largest input word.
           CHARSIZE = SCHAR_MAX                 // Number of characters in character set.
    };

    Matcher(const std::string& name, int maxinput,
                   const char* (*inputname)(int, void*), const char* (*valuename)(int, void*),
                   int value);
    ~Matcher();

    // Add elements to the state machine.
    bool addWord(const char* word, int value);
    bool addWord(const std::string& word, int value);
    bool addTree(const MatchNode* tree, int value);
    // Match input to the state machine.
    int matchWord(const char* word);
    int matchWord(const std::string& word);
    int matchStream(int current,                       // Current input.
                    int (*next)(void*),                // Get next input.
                    void (*save)(void*, int),          // Save matching input.
                    void (*backup)(void*, int, int),   // Reuse unmatching input.
                    void* context);                    // Input context.

    void print(FILE* fp, void* context);
    int getMaxvalue() { return maxvalue; }
    int getInputsize() { return inputsize; }
    std::string& getName() { return name; }

    struct State;

    struct Machines {                           // A state machine list.
        void add(Matcher* p);
        pw::array<Matcher*> list;
    };

    struct States {                             // A state list.
        void clear();
        int add(State* p, int min);
        void append(const States* from);
        pw::array<State*> list;
    };

    struct Entry {
        Entry()
            { value = -1; machine = NULL; }

        int value;                              // Value of state if matched.
        States next;                            // Next state(s), if any.
        
        Matcher* machine;                // State machine to generate, if any.

        struct AVPair {                         // An action/value pair.
            AVPair() { value = -1; }
            int value;                          // Value of state if matched.
            States next;                        // Next state(s), if any.
        };
        pw::array<AVPair> av;                     // Actions and/or values associated with this entry.
    };

    struct State {                              // State machine state.
        State()
            { next = NULL; number = 0; depth = 0; index = 0; states = NULL; }
        State* next;                            // Next state in machine.
        Machines machines;                      // External state machine pointers.
        int number;                             // State number.
        int depth;                              // Depth into state machine.
        int index;                              // Parameter index, if any.
        Entry* states;                          // Per-input states.
    };

    bool addTree(State** root, States& rootlist, const MatchNode* tree,
                 int value, const States *next, int depth);

private:
    static bool hasTarget(State* sp, int target);
    State** setRoot(State** root, States& list, int depth, int index);
    bool setValue(Entry* entry, int value, const States* next = NULL);
    void statePrint(FILE* fp, State* sp, void* context);
    void reversePrint(FILE* fp, State* sp, void* context);
    int addWord(State** root, const char* word, int value, int depth);
    int addWord(State** root, const std::string& word, int value, int depth);
    int addSentence(State** root, const Input* sentence, int value, int depth);
    int checkWord(const char* word);
    int checkWord(const std::string& word, size_t index);
    int checkSentence(const Input* sentence);

    std::string name;                              // Name of the state machine.
    const char* (*inputname)(int, void*);       // Input name display function.
    const char* (*valuename)(int, void*);       // Value name display function.
    State* states;                              // List of states in this machine.
    int inputsize;                              // # of distinct inputs.
    int nextnumber;                             // Next state number to use.
    int maxvalue;                               // Maximum value returned by this state machine.
    States start;                               // Starting nodes.
    States traverse;                            // State traversal pointers.
    int value;                                  // Value of nested state machine.
    bool traversing;                            // True if traversing this machine.
};

class MatchNode {                             // A state tree node.
public:
    enum Type {                                 // State tree node types.
        NONE,                                   // No node.
        INPUT,                                  // A state machine input.
        RANGE,                                  // A range of inputs.
        CONCAT,                                 // Concatenate operands.
        OR,                                     // Operand choice.
        SET,                                    // A set of inputs.
        NOTSET,                                 // A set of excluded inputs.
        ZEROORONE,                              // Zero or one occurances.
        ZEROORMORE,                             // Zero or more occurances.
        ONEORMORE,                              // One or more occurances.
        UNKNOWN,                                // A state machine unknown value.
    };

    // Constructors.
    // An input node.
    MatchNode(int index, Matcher::Input input, Matcher* machine);
    // A range node.
    MatchNode(int index, Matcher::Input left, Matcher::Input right);
    // A unary operator node.
    MatchNode(int index, Type op, MatchNode* node);
    // A binary operator node.
    MatchNode(int index, Type op, MatchNode* left, MatchNode* right);
    // A user defined node.
    MatchNode(int index, void* value, void (*free)(void*), std::string (*name)(void*));
    // A regular expression.
    MatchNode(const std::string& input);
    static void freeTree(MatchNode* tree);

    void print(FILE* fp, const char* (*inputname)(int, void*), void* context);

    Type type;                                  // Type of the node.
    union {                                     // Node operands.
        struct {                                // An input operator.
                                                // INPUT
            Matcher::Input input;
            Matcher* machine;            // State machine that produces this input.
        } i;
        struct {                                // Range operator operands.
                                                // RANGE, 
            Matcher::Input left;
            Matcher::Input right;
        } r;
        struct {                                // Binary operator operands:
                                                // CONCAT, OR.
            MatchNode* left;      
            MatchNode* right;      
        } b;
        struct MatchNode* node;               // Unary operator operand:
                                                // ZEROORONE, ZEROORMORE, ONEORMORE,
                                                // SET, NOTSET.
        struct {                                // Unknown operand.
                                                // UNKNOWN.
            void* value;                        // Unknown value.
            void (*free)(void *);               // Free function.
            std::string (*name)(void *);           // Name function.
        } u;
    } u;

    int index;                                  // Index when used as a action parameter value.
private:
    void treePrint(FILE* fp, const char* (*inputname)(int, void*), void* context, int prec);
};

extern const char* stateCharName(int value, void* context);
extern const char* stateInputName(int value, void* context);
extern const char* stateValueName(int value, void* context);

};

#endif
