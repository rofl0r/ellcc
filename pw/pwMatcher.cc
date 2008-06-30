/*
 *    pwMatcher.cc - Match strings and regular expressions.
 *
 *    Copyright (C) 2008, Richard Pennington.
 */

#include <stdlib.h>
#include "pwMatcher.h"

namespace pw {

static const Matcher::States null;       // A null state list.

//
// clear - Clear state list pointers.
//
void Matcher::States::clear()
{
    for (int i = 0; i < list.size(); i++) {
        list[i] = NULL;
    }
}

//
// add - Add a pointer to a state list.
//
int Matcher::States::add(State* p, int first)
{
    int last = list.size();
    for (int i = first; i < last; i++) {
        if (list[i] == NULL || list[i] == p) {
            list[i] = p;
            return i;
        }
    }

    // Not added, allocate space for a new one.
    list[last] = p;
    return last;
}

//
// append - Append a list to a state list.
//
void Matcher::States::append(const States* from)
{
    int last = from->list.size();
    int next = 0;
    for (int i = 0; i < last; i++) {
        next = add(from->list[i], next);
    }
}

//
// add - Add a pointer to a machine list.
//
void Matcher::Machines::add(Matcher* p)
{
    int last = list.size();
    for (int i = 0; i < last; i++) {
        // Don't duplicate.
        if (list[i] == p) {
            return;
        }
    }

    // Not added, allocate space for a new one.
    list[last] = p;
}

//
// Matcher - Construct a new state machine.
//
Matcher::Matcher(const std::string& name, int maxinput,
                               const char* (*inputname)(int, void*), const char* (*valuename)(int, void*))
{
    this->name = name;                          // Name of the state machine.
    this->inputname = inputname;                // Function to display input names.
    this->valuename = valuename;                // Function to display value names.
    states = NULL;                              // No states, yet.
    inputsize = maxinput + 1;                   // Size of input set.
    nextnumber = 0;                             // Next state number.
    maxvalue = 0;                               // Maximum value seen.
    traversing = false;                         // Clear the traversing flag.
    // Reserve space for state 0.
    start.list[0] = NULL;
}

//
// ~Matcher - Destruct a state machine.
//
Matcher::~Matcher()
{
}

/* Set up initial the state machine, if necessary.
 */
Matcher::State** Matcher::setRoot(State** root, States& list, int depth)
{
    if (root == NULL || list.list.size() == 0) {
        // This needs a new starting state.

        if (root) {
            // Check for a pre-existing matching entry.
            root = NULL;
            for (int i = 0; i < list.list.size(); ++i) {
                State* sp = list.list[i];
                if (sp) {
                    root = &list.list[i];
                    break;
                }
            }
        }

        if (root == NULL) {
            // None found, make a new entry.
            list.list[list.list.size()] = NULL;
            root = &list.list[list.list.size() - 1];
        }
    }

    if (*root == NULL) {
        int i;
        // Allocate a new state array.
        *root = new State;
        (*root)->states = new Entry[inputsize];
        (*root)->number = nextnumber++;
        (*root)->depth = depth;
        // Create a linked list of all created states.
        (*root)->next = states;
        states = *root;
        i = start.list.size() - 1;        
        if (start.list[i] == NULL) {
            start.list[i] = *root;         // Remember the starting state.
        }
    }

    return root;
}

//
// setValue - Set a termination state value.
//  This version resolves ambiguity by choosing the earliest value.
//
bool Matcher::setValue(Entry* entry, int value, const States* next)
{
    if (next == NULL) {
        if (value == -1) {
            return true;                            // Not a stop state.
        }
        if (entry->value != -1) {
            // RICH: ambiguous.
            return false;
        }
        entry->value = value;
    } else {
        if (value != -1 && entry->value == -1) {
            // A stop state.
            entry->value = value;
        }
    }

    if (next) {
        entry->next.append(next);
    }
    if (value > maxvalue) {
        maxvalue = value;                       // Remember the largest value.
    }

    return true;
}

//
// addWord - Add a character string word to a state machine.
//
int Matcher::addWord(State** root, const char* word, int value, int depth)
{
    root = setRoot(root, start, depth);
    if (*(word + 1)) {
        // More items in the word.
        if ((*root)->states[*word].next.list.size() == 0) {
            (*root)->states[*word].next.add(NULL);
        }
        return addWord(&(*root)->states[*word].next.list[0], word + 1, value, depth + 1);
    } else {
        return setValue(&(*root)->states[*word], value);
    }
}

//
// addWord - Add a character string word to a state machine.
//
bool Matcher::addWord(const char* word, int value)
{
    State **root;

    root = setRoot(start.list.size() ? &start.list[0] : NULL, start, 0);
    return addWord(root, word, value, 0);
}

//
// addWord - Add a string word to a state machine.
//
int Matcher::addWord(State** root, const std::string& word, int value, int depth)
{
    root = setRoot(root, start, depth);
    if (word[depth + 1]) {
        // More items in the word.
        if ((*root)->states[word[depth]].next.list.size() == 0) {
            (*root)->states[word[depth]].next.add(NULL);
        }

        return addWord(&(*root)->states[word[depth]].next.list[0], word, value, depth + 1);
    } else {
        return setValue(&(*root)->states[word[depth]], value);
    }
}

//
// addWord - Add a string word to a state machine.
//
bool Matcher::addWord(const std::string& word, int value)
{
    State **root;

    root = setRoot(start.list.size() ? &start.list[0] : NULL, start, 0);
    return addWord(root, word, value, 0);
}

//
// addTree - Add a tree to a state machine.
//
bool Matcher::addTree(State** root, States& rootlist, const MatchNode* tree,
                             int value, const States *next, int depth)
{
    Input first, last, temp;
    State* start;
    States list;

    if (!tree) {
        return false;
    }

    root = setRoot(root, rootlist, depth);   // Set up the initial state machine.
    switch (tree->type) {
    case MatchNode::UNKNOWN:
        // Ignore. RICH: should be an error.
        break;

    case MatchNode::INPUT:
        // Add a single input.
        // Set the value.
        setValue(&(*root)->states[tree->u.i.input], value, next);
        break;

    case MatchNode::RANGE:
        // Add a range of inputs.
        first = tree->u.r.left;
        last = tree->u.r.right;
        if (first > last) {
            // If first is bigger, swap.
            temp = first;
            first = last;
            last = temp;
        }

        // Fill in values, if necessary.
        if (last >= inputsize) {
            // Max out at the size of the input set.
            last = inputsize - 1;
        }

        for (int i = first; i <= last; ++i) {
            // Set the values.
            setValue(&(*root)->states[i], value, next);
        }

        break;

    case MatchNode::CONCAT:
        // Do the right side.
        // Preserve the original start state.
        start = *root;
        *root = NULL;
        root = setRoot(root, rootlist, depth + 1);
        list.add(*root);
        bool rightOptional;
        bool leftOptional;
        rightOptional = addTree(root, list, tree->u.b.right, value, next, depth + 1);
        *root = start;
        leftOptional = addTree(root, rootlist, tree->u.b.left, rightOptional ? value : -1, &list, depth);
        return rightOptional && leftOptional;   // Return true if both sides are optional.

    case MatchNode::OR:
        // If in a set, add all trees to the same start state with the same follow state.
        addTree(root, rootlist, tree->u.b.left, value, next, depth);
        addTree(root, rootlist, tree->u.b.right, value, next, depth);
        break;

    case MatchNode::NOTSET:
        // Add the set negated.
        addTree(root, rootlist, tree->u.node, -2, &null, depth);
        // Negate the set: false becomes true, true, false.
        for (int i = 0; i < inputsize; ++i) {
            if ((*root)->states[i].value == -1 && (*root)->states[i].next.list.size() == 0) {
                // No state, use the target state.
                setValue(&(*root)->states[i], value, next);
            } else if ((*root)->states[i].value == -2) {
                // Undefine this one.
                (*root)->states[i].value = -1;
                (*root)->states[i].next.list.truncate();
            }
        }
        break;

    case MatchNode::SET:
        // Add this set to the root.
        addTree(root, rootlist, tree->u.node, value, next, depth);
        break;

    case MatchNode::ZEROORONE:
    case MatchNode::ZEROORMORE:
    case MatchNode::ONEORMORE:
        // Add the tree twice, second occurance loops.
        // Create a new state.
        if (tree->type == MatchNode::ZEROORMORE || tree->type == MatchNode::ONEORMORE) {
            root = setRoot(root, rootlist, depth);
            list.add(*root);
            list.append(next);
            addTree(root, rootlist, tree->u.node, value, &list, depth);
            if (next->list.size()) {
                // Allow the first state to be nul.
                for (int i = 0; i < inputsize; ++i) {
                    for (int j = 0; j < next->list.size(); ++j) {
                        root = setRoot(root, rootlist, depth);
                        setValue(&(*root)->states[i], next->list[j]->states[i].value,
                                 &next->list[j]->states[i].next);
                    }
                }
            }
        } else {
            addTree(root, rootlist, tree->u.node, value, next, depth);
        }

        if (tree->type == MatchNode::ZEROORMORE || tree->type == MatchNode::ZEROORONE) {
            // A potentially empty state.
            if (next->list.size()) {
                // Allow the first state to be nul.
                for (int i = 0; i < inputsize; ++i) {
                    for (int j = 0; j < next->list.size(); ++j) {
                        root = setRoot(root, rootlist, depth);
                        setValue(&(*root)->states[i], next->list[j]->states[i].value,
                                 &next->list[j]->states[i].next);
                    }
                }
            }
            return true;        // This state may have zero members.
        }
        break;
    case MatchNode::NONE:
        // Ignore. RICH: should be an error.
        break;
    }

    return false;               // Not zero or more.
}

//
// addTree - Add a tree to a state machine.
//
bool Matcher::addTree(const MatchNode* tree, int value)
{
    int result;

    if (!tree) {
        return false;
    }

    if (   tree->type == MatchNode::INPUT
        || (   tree->type == MatchNode::CONCAT
            && tree->u.b.left && tree->u.b.left->type == MatchNode::INPUT)) {
        State **root;
        // Add simple starting trees to state 0.

        root = setRoot(start.list.size() ? &start.list[0] : NULL, start, 0);
        result = addTree(root, start, tree, value, &null, 0);
    } else {
        result = addTree(NULL, start, tree, value, &null, 0);
    }
    return result;
}

//
// checkWord - Check for a character string word in a state machine.
//
int Matcher::checkWord(const char* word)
{
    int first, i, j, last;
    Input input;

    // Check for a non-null next state pointer.
    for (first = 0; first < traverse.list.size(); ++first) {
        if (traverse.list[first]) {
            break;
        }
    }

    if (first >= traverse.list.size()) {
        // No non-null pointer.
        return -1;                              // Failed.
    }

    input = *word;
    if (*(word + 1)) {
        // Check the rest of the word.

        // Set up the next state pointers.
        last = traverse.list.size();
        for (i = first; i < last; ++i) {
            if (traverse.list[i]) {
                States *sp;

                sp = &traverse.list[i]->states[input].next;
                traverse.list[i] = NULL;
                for (j = 0; j < sp->list.size(); ++j) {
                    traverse.add(sp->list[j]);
                }
            }
        }
        return checkWord(word + 1);
    }

    // Return the first match or -1.
    for ( ; first < traverse.list.size(); ++first) {
        if (traverse.list[first] == NULL) {
            continue;
        }
        if (traverse.list[first]->states[input].value != -1) {
            return traverse.list[first]->states[input].value;   // Matched.
        }
    }
    return -1;
}

//
// matchWord - Match a null terminated word in a state machine.
//
// This function does not handle nested state machines.
//
//
int Matcher::matchWord(const char* word)
{
    traverse.clear();                           // Clear the traversal pointers.
    for (int i = 0; i < start.list.size(); ++i) {
        // Set up traversal pointers.
        if (start.list[i]) {
            traverse.add(start.list[i]);
        }
    }
    return checkWord(word);
}

//
// checkWord - Check for a std::string word in a state machine.
//
int Matcher::checkWord(const std::string& word, size_t index)
{
    int first, i, j, last;
    Input input;

    // Check for a non-null next state pointer.
    for (first = 0; first < traverse.list.size(); ++first) {
        if (traverse.list[first]) {
            break;
        }
    }

    if (first >= traverse.list.size()) {
        // No non-null pointer.
        return -1;                              // Failed.
    }

    input = word[index];
    if (index + 1 < word.length()) {
        // Check the rest of the word.

        // Set up the next state pointers.
        last = traverse.list.size();
        for (i = first; i < last; ++i) {
            if (traverse.list[i]) {
                States *sp;

                sp = &traverse.list[i]->states[input].next;
                traverse.list[i] = NULL;
                for (j = 0; j < sp->list.size(); ++j) {
                    traverse.add(sp->list[j]);
                }
            }
        }
        return checkWord(word, index + 1);
    }

    // Return the first match or -1.
    for ( ; first < traverse.list.size(); ++first) {
        if (traverse.list[first] == NULL) {
            continue;
        }
        if (traverse.list[first]->states[input].value != -1) {
            return traverse.list[first]->states[input].value;   // Matched.
        }
    }
    return -1;
}

//
// matchWord - Match a std::string word in a state machine.
//
// This function does not handle nested state machines.
//
//
int Matcher::matchWord(const std::string& word)
{
    traverse.clear();                           // Clear the traversal pointers.
    for (int i = 0; i < start.list.size(); ++i) {
        // Set up traversal pointers.
        if (start.list[i]) {
            traverse.add(start.list[i]);
        }
    }
    return checkWord(word, 0);
}

//
// matchStream - Match the longest matching sentence in a state machine.
//
// This function does not handle nested state machines.
//
//
int Matcher::matchStream(int current,                       // Current input.
                         int (*next)(void*),                // Get next input.
                         void (*save)(void*, int),          // Save matching input.
                         void (*backup)(void*, int, int),   // Reuse unmatching input.
                         void* context)                     // Input context.
{
    int i, j, last;
    int count;
    int good;
    bool allnull;
    int token = -1;

    if (current < 0) {
        // End of input.
        return -1;
    }

    traverse.clear();                          // Clear the traversal pointers.
    last = start.list.size();
    int nxt = 0;
    for (i = 0; i < last; ++i) {
        // Set up traversal pointers.
        if (start.list[i]) {
            nxt = traverse.add(start.list[i], nxt);
        }
    }

    count = 1;
    good = 0;
    int newnxt = nxt;
    for (;;) {
        save(context, current);
        // last = traverse.list.size();
        last = newnxt + 1;
        for (i = 0; i < last; ++i) {
            if (traverse.list[i] && traverse.list[i]->states[current].value >= 0) {
                // Remember the last successful input.
                good = count;
                token = traverse.list[i]->states[current].value;
                break;
            }
        }

        // Set up the next state pointers.
        allnull = true;
        newnxt = 0;
        for (i = 0; i < last; ++i) {
            if (traverse.list[i]) {
                States *sp = &traverse.list[i]->states[current].next;
                nxt = i;
                traverse.list[i] = NULL;
                int states = sp->list.size();
                for (j = 0; j < states; ++j) {
                    if (sp->list[j]) {
                        // Have a non-NULL pointer.
                        allnull = false;
                        nxt = traverse.add(sp->list[j], nxt);
                        if (nxt > newnxt) {
                            newnxt = nxt;
                        }
                    }
                }
            }
        }

        current = next(context);        // Get next input.

        if (allnull) {
            // No more states.
            break;
        }

        if (current < 0) {
            // EOF.
            break;
        }

        ++count;                // Count all buffered characters.
    }

    // At the end. Check for a match.
    if (good == count) {
        return token;       // The whole sentence matched.
    }

    // Restore uneeded input.
    backup(context, good, count);
    return token;
}

//
//
// MatchNode - Create an input node.
//
MatchNode::MatchNode(Matcher::Input input, Matcher* machine)
{
    type = INPUT;
    u.i.input = input;
    u.i.machine = machine;
}

//
// MatchNode - Create a range node.
//
MatchNode::MatchNode(Matcher::Input left, Matcher::Input right)
{
    type = RANGE;
    u.r.left = left;
    u.r.right = right;
}

//
// MatchNode - Create a unary node.
//
MatchNode::MatchNode(MatchNode::Type op, MatchNode* node)
{
    type = op;
    u.node = node;
}

//
// MatchNode - Create a binary node.
//
MatchNode::MatchNode(MatchNode::Type op, MatchNode* left, MatchNode* right)
{
    type = op;
    u.b.left = left;
    u.b.right = right;
}

//
// MatchNode - Create an unknown node.
//
MatchNode::MatchNode(void* value, void (*free)(void*), std::string (*name)(void*))
{
    type = UNKNOWN;
    u.u.value = value;
    u.u.free = free;
    u.u.name = name;
}

//
// The regular expression parser.
// The following are all used by the regular expression tree constructor.
//

enum {              // Operators. These values are all less than 0.
    LPAREN = -9,
    RPAREN,
    QUESTION,
    STAR,
    PLUS,
    LBRACKET,
    BAR,
    DOT,
    EOS,
};

static MatchNode* orop(const std::string& input, int& index, int& ch);

//
// getinput - Get the next character or operator.
//
static void getinput(const std::string& input, int& index, int& ch)
{
    if (input[index] == '\\') {
        ++index;
        // An escaped character.
        ch = input[index++];
        return;
    } else {
        // Convert a character into an operator.
        switch (input[index]) {
        case '(':
            ++index;
            ch = LPAREN;
            return;
        case ')':
            ++index;
            ch = RPAREN;
            return;
        case '?':
            ++index;
            ch = QUESTION;
            return;
        case '*':
            ++index;
            ch = STAR;
            return;
        case '+':
            ++index;
            ch = PLUS;
            return;
        case '[':
            ++index;
            ch = LBRACKET;
            return;
        case '|':
            ++index;
            ch = BAR;
            return;
        case '.':
            ++index;
            ch = DOT;
            return;
        case '\0':  // End of string.
            ch = EOS;
            return;

        default:
            // No translation.
            ch = input[index++];
            return;
        }
    }
}

//
// getRanges - Get set elements.
//
static MatchNode* getRanges(const std::string& input, int from, int to)
{
    MatchNode *left;
    MatchNode *right;

    if (from >= to) {
        return NULL;
    }

    if (from < (to - 2) && input[from + 1] == '-') {
        // The head is a range.
        left = new MatchNode(input[from], input[from + 2]);
        from += 3;
    } else {
        // The head is a single character.
        left = new MatchNode(input[from], (Matcher*)NULL);
        from += 1;
    }

    right = getRanges(input, from, to);
    if (right) {
        // Add the right side.
        left = new MatchNode(MatchNode::OR, left, right);
    }

    return left;
}

//
// buildSet - Create a set tree.
//
static MatchNode* buildSet(MatchNode::Type type, const std::string& input, int from, int to)
{
    MatchNode *np = getRanges(input, from, to);

    return new MatchNode(type, np, NULL);
}

//
// primary - Get a regular expression primary.
//
static MatchNode* primary(const std::string& input, int& index, int& ch)
{
    MatchNode *node = NULL;

    if (ch == LPAREN) {
        // Parenthesized expression.
        getinput(input, index, ch);
        node = orop(input, index, ch);
        if (ch != RPAREN) {
            // RICH: Unmatched parenthesis.
            printf("MISSINGRPAREN\n");
        } else {
            getinput(input, index, ch);
        }
    } else if (ch >= 0) {
        // A specific input.
        node = new MatchNode(ch, (Matcher*)NULL);
        getinput(input, index, ch);
    } else if (ch == DOT) {
        // Match any input.
        node = new MatchNode(0, Matcher::INPUTMAX);
        getinput(input, index, ch);
    } else if (ch == LBRACKET) {
        // A regular expression set.
        bool negate = false;
        int current = index;
        
        // Gather a set.
        for (;;) {
            if (input[index] == '\0') {
                break;
            }
            if (current != index) {
                if (input[current] == ']') {
                    // End of set.
                    break;
                }
            } else if (!negate && input[current] == '^') {
                // Negate the matching set.
                negate = true;
                // Move the set starting point.
                ++index;
                ++current;
                continue;
            }

            // Add this character to the set.
            ++current;
        }

        // The set lies from index to current - 1.

        node = buildSet(negate ? MatchNode::NOTSET : MatchNode::SET, input, index, current);
        if (input[current] == '\0') {
            // RICH: Missing ']'
            printf("MISSINGRBRACKET\n");
            index = current;
        } else {
            index = current + 1;
        }

        // Get the next input character.
        getinput(input, index, ch);
    }

    return node;
}

//
// unary - Parse the unary operators.
//
static MatchNode* unary(const std::string& input, int& index, int& ch)
{
    MatchNode::Type type = MatchNode::NONE;
    MatchNode *node;

    node = primary(input, index, ch);

    while (ch == QUESTION || ch == STAR || ch == PLUS) {
        if (ch == QUESTION) {
            if (type != MatchNode::NONE) {
                if (type != MatchNode::ZEROORONE) {
                    // *? or +? become *. ?? remains ?.
                    node->type = MatchNode::ZEROORMORE;
                }
                getinput(input, index, ch);
                continue;
            }
            type = MatchNode::ZEROORONE;
        } else if (ch == STAR) {
            if (type != MatchNode::NONE) {
                // ?*, **, +* become *.
                node->type = MatchNode::ZEROORMORE;
                getinput(input, index, ch);
                continue;
            }
            type = MatchNode::ZEROORMORE;
        } else {
            if (type != MatchNode::NONE) {
                if (type != MatchNode::ONEORMORE) {
                    // *+, ?+, become *. ++ remains +.
                    node->type = MatchNode::ZEROORMORE;
                }
                getinput(input, index, ch);
                continue;
            }
            type = MatchNode::ONEORMORE;
        }
        node = new MatchNode(type, node);
        getinput(input, index, ch);
    }

    return node;
}

//
// concat - Adjacent characters are concatenated.
//
static MatchNode* concat(const std::string& input, int& index, int& ch)
{
    MatchNode *left, *right;

    left = unary(input, index, ch);
    while (ch != EOS && ch != BAR && ch != RPAREN && ch != ']') {
        // Concatenated.
        right = unary(input, index, ch);
        left = new MatchNode(MatchNode::CONCAT, left, right);
    }
    return left;
}

//
// orop - Parse the or operator.
//
static MatchNode* orop(const std::string& input, int& index, int& ch)
{
    MatchNode *left, *right;
    
    left = concat(input, index, ch);
    while (ch == BAR) {
        // OR operator.
        getinput(input, index, ch);
        right = concat(input, index, ch);
        left = new MatchNode(MatchNode::OR, left, right);
    }

    return left;
}

//
// MatchNode - Convert a regular expression string into a tree.
//
MatchNode::MatchNode(const std::string& input)
{
    int pos = 0;
    int ch;
    getinput(input, pos, ch);
    *this = *orop(input, pos, ch);              // Build the regular expression tree.
}

//
// freeTree - Deconstruct a state tree.
//
void MatchNode::freeTree(MatchNode* tree)
{
    if (tree == NULL) {
        return;
    }

    switch (tree->type) {
    case UNKNOWN:
        if (tree->u.u.free) {
            tree->u.u.free(tree->u.u.value);
        }
        // Fall through.
    case RANGE:
    case INPUT:
        break;

    case CONCAT:
    case OR:
        freeTree(tree->u.b.left);
        freeTree(tree->u.b.right);
        break;

    case SET:
    case NOTSET:
    case ZEROORONE:
    case ZEROORMORE:
    case ONEORMORE:
        freeTree(tree->u.node);
        break;

    case NONE:
        // Ignore. RICH: should be an error.
        break;
    }

    delete tree;
}

enum {
    PRECLOW, PRECOR, PRECAND, PRECREPEAT, PRECHIGH,
};

//
// treePrint - Print a state tree.
//
void MatchNode::treePrint(FILE* fp, const char* (*inputname)(int, void*), void* context, int prec)
{
    switch (type) {
    case UNKNOWN:
        if (u.u.name) {
            std::string name = u.u.name(u.u.value);
            fprintf(fp, "%s ", name.c_str());
        } else {
            fprintf(fp, "UNKNOWN ");
        }
        break;

    case INPUT:
        if (inputname) {
            fprintf(fp, "%s ", inputname(u.i.input, context));
        } else {
            fprintf(fp, "(%u) ", u.i.input);
        }
        break;

    case RANGE:
        if (inputname) {
            fprintf(fp, "%s-%s ", inputname(u.r.left, context), inputname(u.r.right, context));
        } else {
            fprintf(fp, "(%u-", u.r.left);
            if (u.r.right == Matcher::INPUTMAX) {
                fprintf(fp, "<INF>) ");
            } else {
                fprintf(fp, "%u) ", u.r.right);
            }
        }
        break;

    case CONCAT:
        if (1 || prec > PRECAND) {    // RICH: Noisy output.
            fprintf(fp, "( ");
        }
        u.b.left->treePrint(fp, inputname, context, PRECAND);
        fprintf(fp, "& ");
        u.b.right->treePrint(fp, inputname, context, PRECAND);
        if (1 || prec > PRECAND) {    // RICH: Noisy output.
            fprintf(fp, ") ");
        }
        break;

    case OR:
        if (1 || prec > PRECOR) { // RICH: Noisy output.
            fprintf(fp, "( ");
        }
        u.b.left->treePrint(fp, inputname, context, PRECOR);
        fprintf(fp, "| ");
        u.b.right->treePrint(fp, inputname, context, PRECOR);
        if (1 || prec > PRECOR) {  // RICH: Noisy output.
            fprintf(fp, ") ");
        }
        break;

    case NOTSET:
    case SET:
        fprintf(fp, "[%s ", type == NOTSET ? "^" : "");
        u.node->treePrint(fp, inputname, context, PRECLOW);
        fprintf(fp, "] ");
        break;

    case ZEROORONE:
    case ZEROORMORE:
    case ONEORMORE:
        fprintf(fp, "( ");    // RICH: Noisy output.
        u.node->treePrint(fp, inputname, context, PRECREPEAT);
        fprintf(fp, ") ");    // RICH: Noisy output.
        if (type == ZEROORONE) {
            fprintf(fp, "? ");
        } else if (type == ZEROORMORE) {
            fprintf(fp, "* ");
        } else {
            fprintf(fp, "+ ");
        }
        break;

    case NONE:
        // Ignore. RICH: should be an error.
        break;
    }
}

//
// print - Print a state tree.
//
void MatchNode::print(FILE* fp, const char* (*inputname)(int, void*), void* context)
{
    treePrint(fp, inputname, context, PRECLOW);
}

//
// statePrint - Print out a state.
//
void Matcher::statePrint(FILE* fp, State* sp, void* context)
{
    fprintf(fp, "    %d (%d):\n", sp->number, sp->depth);
    for (int i = 0; i < inputsize; ++i) {
#if RICH
        char multi = sp->states[i].av.size() > 1 ? '*' : ' ';
        for (int j = 0; j < sp->states[i].av.size(); ++j) {
            if (sp->states[i].av[j].value != -1 || sp->states[i].next.list.size()) {
                // This state is active.
                if (inputname) {
                    fprintf(fp, "%c     %s: ", multi, inputname(i, context));
                } else {
                    fprintf(fp, "%c     %d: ", multi, i);
                }

                if (sp->states[i].av[j].value != -1) {
                    if (valuename) {
                        fprintf(fp, "value %s ", valuename(sp->states[i].av[j].value, context));
                    } else {
                        fprintf(fp, "value %d ", sp->states[i].av[j].value);
                    }
                } else {
                    fprintf(fp, "no value ");
                }

                if (sp->states[i].av[j].next.list.size()) {
                    fprintf(fp, "next(");
                    for (int k = 0; k < sp->states[i].av[j].next.list.size(); ++k) {
                        fprintf(fp, " %d ", sp->states[i].av[j].next.list[k]->number);
                    }
                    fprintf(fp, ")");
                }

                fprintf(fp, "\n");
            }
        }
#endif
    }
}

//
// reversePrint - Print the state list in definition order.
//
void Matcher::reversePrint(FILE* fp, State* sp, void* context)
{
    if (sp == NULL) {
        return;
    }

    reversePrint(fp, sp->next, context);
    statePrint(fp, sp, context);
}

//
// print - Display a state machine.
//
void Matcher::print(FILE* fp, void* context)
{
    fprintf(fp, "State machine: %s ", name.c_str());
    fprintf(fp, "starts(");
    for (int i = 0; i < start.list.size(); ++i) {
        if (start.list[i]) {
            fprintf(fp, " %d ", start.list[i]->number);
        } else {
            fprintf(fp, " <NULL> ");
        }
    }
    fprintf(fp, ")\n");
    // In reverse definition order.
    reversePrint(fp, states, context);
}

//
// stateCharName - Display a character in a standard way.
//
const char* stateCharName(int value, void* context)
{
    static char buffer[10];
    char *p;

    buffer[0] = '\'';
    p = buffer + 1;
    if (value < ' ' || value > 126 || value == '\\' || value == '\'') {
        *p++ = '\\';
        switch (value) {
        default:
            sprintf(p, "x%X'", value);
            return buffer;
        case '\\':
            *p++ = '\\';
            break;
        case '\'':
            *p++ = '\'';
            break;
        case '\a':
            *p++ = 'a';
            break;
        case '\b':
            *p++ = 'b';
            break;
        case '\f':
            *p++ = 'f';
            break;
        case '\n':
            *p++ = 'n';
            break;
        case '\r':
            *p++ = 'r';
            break;
        case '\t':
            *p++ = 't';
            break;
        case '\v':
            *p++ = 'v';
            break;
        }
    } else {
        *p++ = value;
    }

    *p++ = '\'';
    *p++ = '\0';
    return buffer;
}

//
// stateInputName - Display an input in a standard way.
//
const char* stateInputName(int value, void* context)
{
    static char buffer[20];

    sprintf(buffer, "[%u]", value);
    return buffer;
}

//
// stateValueName - Display a value in a standard way.
//
const char* stateValueName(int value, void* context)
{
    static char buffer[20];

    sprintf(buffer, "%d", value);
    return buffer;
}

};
