/** @file
 * Match strings and regular expressions.
 * @author Richard Pennington
 * @date June 30, 2008
 *
 * Copyright (C) 2008, Richard Pennington.
 */

#ifndef pwMatcher_h
#define pwMatcher_h

#include <limits.h>
#include <string>
#include "pwArray.h"

namespace pw {

class MatchNode;

/** Match a string or regular expression.
 */
class Matcher {
public:
    typedef int Input;                          ///< Matcher input type.
    static const int INPUTMAX = INT_MAX;        ///< Largest input word.
    static const int CHARSIZE = SCHAR_MAX;      ///< Number of characters in character set.
    /** Construct a matcher.
     * @param name The matcher's name.
     * @param maxinput The maximum input value.
     * @param inputname A function to return a string representing an input.
     * @param valuename A function to return a string representing a value.
     */
    Matcher(const std::string& name, int maxinput,
                   const char* (*inputname)(int, void*), const char* (*valuename)(int, void*));
    /** Destruct a matcher.
     */
    ~Matcher();

    /** Add a word to the matcher.
     * @param word The word to add.
     * @param value The token value.
     * @return true if the word is added unambiguously.
     */
    bool addWord(const char* word, int value);
    /** Add a word to the matcher.
     * @param word The word to add.
     * @param value The token value.
     * @return true if the word is added unambiguously.
     */
    bool addWord(const std::string& word, int value);
    /** Add a MatchNode tree to the matcher.
     * @param tree The tree to add.
     * @param value The token value.
     * @return true if the tree is added unambiguously.
     */
    bool addTree(const MatchNode* tree, int value);
    /** Match a word.
     * @param word The word to match.
     * @return The token value or -1 if the word is not found.
     */
    int matchWord(const char* word);
    /** Match a word.
     * @param word The word to match.
     * @return The token value or -1 if the word is not found.
     */
    int matchWord(const std::string& word);
    /** Match input from a stream.
     * @param current The current input.
     * @param next Get the next input.
     * @param save Save matching input.
     * @param backup Reuse unmatching input.
     * @param context The scanning context.
     * @return The token value or -1 if the word is not found.
     */
    int matchStream(int current,
                    int (*next)(void*),
                    void (*save)(void*, int),
                    void (*backup)(void*, int, int),
                    void* context);

    /** Output a matcher to a file in human readable form.
     * @param fp The output file.
     * @param context The scanning context.
     */
    void print(FILE* fp, void* context);
    /** Get the maximum value that the matcher can return.
     * @return The maximum value.
     */
    int getMaxvalue() { return maxvalue; }
    /** Get the maximum int value that the matcher can handle.
     * @return The maximum value.
     */
    int getInputsize() { return inputsize; }
    /** Get the name of the matcher.
     * @return The name.
     */
    std::string& getName() { return name; }

    struct State;

    /** A state machine list.
     */
    struct Machines {
        /** Add a matcher.
         * @param p The matcher.
         */
        void add(Matcher* p);
        pw::array<Matcher*> list;               ///< The matcher list.
    };

    /** A list of states.
     */
    struct States {
        void clear();                           ///< Clear the state list.
        /** Add a state to the list.
         * @param p The state to add.
         * @param first The first slot to try to use.
         * @return The slot that was used.
         */
        int add(State* p, int first = 0);
        /** Append a state list to the list.
         * @param from The state list to append.
         */
        void append(const States* from);
        pw::array<State*> list;                 ///< The state list.
    };

    /** A state machine entry.
     */
    struct Entry {
        /** The constructor.
         */
        Entry()
            { value = -1; }
        int value;                              ///< The value of state if matched.
        States next;                            ///< The next state(s), if any.
    };

    /** A state machine state.
     */
    struct State {
        /** The constructor.
         */
        State()
            { next = NULL; number = 0; depth = 0; states = NULL; }
        State* next;                            ///< Next state in machine.
        int number;                             ///< State number.
        int depth;                              ///< Depth into state machine.
        Entry* states;                          ///< Per-input states.
    };

    /** Add a tree to a state machine.
     * @param[in,out] root The root pointer.
     * @param rootlist The root states.
     * @param tree The tree to add.
     * @param value The value to return if matched.
     * @param next Next states.
     * @param depth The depth of the tree.
     * @return true if the tree was added unambiguously.
     */
    bool addTree(State** root, States& rootlist, const MatchNode* tree,
                 int value, const States *next, int depth);

private:
    /** Set up initial the state machine, if necessary.
     * @param[in,out] root The root pointer.
     * @param list The root states.
     * @param depth The depth of the tree.
     */
    State** setRoot(State** root, States& list, int depth);
    /** Set the value of an entry.
     * @param entry The entry to set.
     * @param value The value to give it.
     * @param next The states that may follow.
     */
    bool setValue(Entry* entry, int value, const States* next = NULL);
    /** Print a state to a file.
     * @param fp The output file.
     * @param sp The state to output.
     * @param context The machine context.
     */
    void statePrint(FILE* fp, State* sp, void* context);
    /** Print a state to a file in reverse order.
     * @param fp The output file.
     * @param sp The state to output.
     * @param context The machine context.
     */
    void reversePrint(FILE* fp, State* sp, void* context);
    int addWord(State** root, const char* word, int value, int depth);
    int addWord(State** root, const std::string& word, int value, int depth);
    int checkWord(const char* word);
    int checkWord(const std::string& word, size_t index);

    std::string name;                              // Name of the state machine.
    const char* (*inputname)(int, void*);       // Input name display function.
    const char* (*valuename)(int, void*);       // Value name display function.
    State* states;                              // List of states in this machine.
    int inputsize;                              // # of distinct inputs.
    int nextnumber;                             // Next state number to use.
    int maxvalue;                               // Maximum value returned by this state machine.
    States start;                               // Starting nodes.
    States traverse;                            // State traversal pointers.
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
    MatchNode(Matcher::Input input, Matcher* machine);
    // A range node.
    MatchNode(Matcher::Input left, Matcher::Input right);
    // A unary operator node.
    MatchNode(Type op, MatchNode* node);
    // A binary operator node.
    MatchNode(Type op, MatchNode* left, MatchNode* right);
    // A user defined node.
    MatchNode(void* value, void (*free)(void*), std::string (*name)(void*));
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

private:
    void treePrint(FILE* fp, const char* (*inputname)(int, void*), void* context, int prec);
};

extern const char* stateCharName(int value, void* context);
extern const char* stateInputName(int value, void* context);
extern const char* stateValueName(int value, void* context);

};

#endif
