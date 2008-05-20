/*
 *    EllpStateMachine.cc - Creation and traversing of state machines.
 *
 *    Copyright (C) 2008, Richard Pennington.
 */

#include <stdlib.h>
#include "EllpStateMachine.h"

static const EllpStateMachine::States null;       // A null state list.

//
// clear - Clear state list pointers.
//
void EllpStateMachine::States::clear()
{
    for (int i = 0; i < list.size(); i++) {
        list[i] = NULL;
    }
}

//
// add - Add a pointer to a state list.
//
int EllpStateMachine::States::add(State* p, int min)
{
    for (int i = min; i < list.size(); i++) {
        if (list[i] == NULL || list[i] == p) {
            list[i] = p;
            return i;
        }
    }

    // Not added, allocate space for a new one.
    list[list.size()] = p;
    return list.size() - 1;
}

//
// append - Append a list to a state list.
//
void EllpStateMachine::States::append(const States* from)
{
    for (int i = 0; i < from->list.size(); i++) {
        add(from->list[i], 0);
    }
}

//
// add - Add a pointer to a machine list.
//
void EllpStateMachine::Machines::add(EllpStateMachine* p)
{
    for (int i = 0; i < list.size(); i++) {
        // Don't duplicate.
        if (list[i] == p) {
            return;
        }
    }

    // Not added, allocate space for a new one.
    list[list.size()] = p;
}

//
// EllpStateMachine - Construct a new state machine.
//
EllpStateMachine::EllpStateMachine(const std::string& name, int maxinput,
                               const char* (*inputname)(int, void*), const char* (*valuename)(int, void*),
                               int value)
{
    this->name = name;                          // Name of the state machine.
    this->inputname = inputname;                // Function to display input names.
    this->valuename = valuename;                // Function to display value names.
    states = NULL;                              // No states, yet.
    inputsize = maxinput + 1;                   // Size of input set.
    nextnumber = 0;                             // Next state number.
    maxvalue = 0;                               // Maximum value seen.
    this->value = value;                        // State machine's nested value.
    traversing = false;                         // Clear the traversing flag.
    // Reserve space for state 0.
    start.list[0] = NULL;
}

//
// ~EllpStateMachine - Destruct a state machine.
//
EllpStateMachine::~EllpStateMachine()
{
}

//
// setRoot - Set up initial state machine, if necessary.
//
EllpStateMachine::State** EllpStateMachine::setRoot(State** root, States& list, int depth, int index)
{
    if (root == NULL || list.list.size() == 0 || (root && *root && (*root)->index != index)) {
        // This needs a new starting state.

        if (root) {
            // Check for a pre-existing matching entry.
            root = NULL;
            for (int i = 0; i < list.list.size(); ++i) {
                State* sp = list.list[i];
                if (sp && sp->index == index) {
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
        (*root)->index = index;
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
bool EllpStateMachine::setValue(Entry* entry, int value)
{
    if (value == -1) {
        return true;                            // Not a stop state.
    }

    if (entry->value != -1) {
        // RICH: ambiguous.
        return false;
    }

    entry->value = value;
    if (value > maxvalue) {
        maxvalue = value;                       // Remember the largest value.
    }
    return true;
}

//
// addWord - Add a character string word to a state machine.
//
int EllpStateMachine::addWord(State** root, const char* word, int value, int depth)
{
    root = setRoot(root, start, depth, 0);
    if (*(word + 1)) {
        // More items in the word.
        if ((*root)->states[*word].next.list.size() == 0) {
            (*root)->states[*word].next.add(NULL, 0);
        }
        return addWord(&(*root)->states[*word].next.list[0], word + 1, value, depth + 1);
    } else {
        return setValue(&(*root)->states[*word], value);
    }
}

//
// addWord - Add a character string word to a state machine.
//
bool EllpStateMachine::addWord(const char* word, int value)
{
    State **root;

    root = setRoot(start.list.size() ? &start.list[0] : NULL, start, 0, 0);
    return addWord(root, word, value, 0);
}

//
// addWord - Add a string word to a state machine.
//
int EllpStateMachine::addWord(State** root, const std::string& word, int value, int depth)
{
    root = setRoot(root, start, depth, 0);
    if (word[depth + 1]) {
        // More items in the word.
        if ((*root)->states[word[depth]].next.list.size() == 0) {
            (*root)->states[word[depth]].next.add(NULL, 0);
        }

        return addWord(&(*root)->states[word[depth]].next.list[0], word, value, depth + 1);
    } else {
        return setValue(&(*root)->states[word[depth]], value);
    }
}

//
// addWord - Add a string word to a state machine.
//
bool EllpStateMachine::addWord(const std::string& word, int value)
{
    State **root;

    root = setRoot(start.list.size() ? &start.list[0] : NULL, start, 0, 0);
    return addWord(root, word, value, 0);
}

//
// checkWord - Check for a character string word in a state machine.
//
int EllpStateMachine::checkWord(const char* word)
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
                    traverse.add(sp->list[j], j == 0 ? i : last);
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
int EllpStateMachine::matchWord(const char* word)
{
    traverse.clear();                           // Clear the traversal pointers.
    for (int i = 0; i < start.list.size(); ++i) {
        // Set up traversal pointers.
        if (start.list[i]) {
            traverse.add(start.list[i], 0);
        }
    }
    return checkWord(word);
}

//
// checkWord - Check for a std::string word in a state machine.
//
int EllpStateMachine::checkWord(const std::string& word, int index)
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
                    traverse.add(sp->list[j], j == 0 ? i : last);
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
int EllpStateMachine::matchWord(const std::string& word)
{
    traverse.clear();                           // Clear the traversal pointers.
    for (int i = 0; i < start.list.size(); ++i) {
        // Set up traversal pointers.
        if (start.list[i]) {
            traverse.add(start.list[i], 0);
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
int EllpStateMachine::matchStream(int current,                       // Current input.
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
    for (i = 0; i < start.list.size(); ++i) {
        // Set up traversal pointers.
        if (start.list[i]) {
            traverse.add(start.list[i], 0);
        }
    }

    count = 1;
    good = 0;
    for (;;) {
        save(context, current);
        for (i = 0; i < traverse.list.size(); ++i) {
            if (traverse.list[i] && traverse.list[i]->states[current].value >= 0) {
                // Remember the last successful input.
                good = count;
                token = traverse.list[i]->states[current].value;
                break;
            }
        }

        // Set up the next state pointers.
        allnull = true;
        last = traverse.list.size();
        for (i = 0; i < last; ++i) {
            if (traverse.list[i]) {
                States *sp;

                sp = &traverse.list[i]->states[current].next;
                traverse.list[i] = NULL;
                for (j = 0; j < sp->list.size(); ++j) {
                    if (sp->list[j]) {
                        // Have a non-NULL pointer.
                        allnull = false;
                        traverse.add(sp->list[j], j == 0 ? i : last);
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
// EllpStateNode - Create an input node.
//
EllpStateNode::EllpStateNode(int index, EllpStateMachine::Input input, EllpStateMachine* machine)
{
    type = INPUT;
    u.i.input = input;
    u.i.machine = machine;
    this->index = index;
}

//
// EllpStateNode - Create a range node.
//
EllpStateNode::EllpStateNode(int index, EllpStateMachine::Input left, EllpStateMachine::Input right)
{
    type = RANGE;
    u.r.left = left;
    u.r.right = right;
    this->index = index;
}

//
// EllpStateNode - Create a unary node.
//
EllpStateNode::EllpStateNode(int index, EllpStateNode::Type op, EllpStateNode* node)
{
    type = op;
    u.node = node;
    this->index = index;
}

//
// EllpStateNode - Create a binary node.
//
EllpStateNode::EllpStateNode(int index, EllpStateNode::Type op, EllpStateNode* left, EllpStateNode* right)
{
    type = op;
    u.b.left = left;
    u.b.right = right;
    this->index = index;
}

//
// EllpStateNode - Create an unknown node.
//
EllpStateNode::EllpStateNode(int index, void* value, void (*free)(void*), std::string (*name)(void*))
{
    type = UNKNOWN;
    u.u.value = value;
    u.u.free = free;
    u.u.name = name;
    this->index = index;
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

static EllpStateNode* orop(const std::string& input, int& index, int& ch);

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
static EllpStateNode* getRanges(const std::string& input, int from, int to)
{
    EllpStateNode *left;
    EllpStateNode *right;

    if (from >= to) {
        return NULL;
    }

    if (from < (to - 2) && input[from + 1] == '-') {
        // The head is a range.
        left = new EllpStateNode(0, input[from], input[from + 2]);
        from += 3;
    } else {
        // The head is a single character.
        left = new EllpStateNode(0, input[from], (EllpStateMachine*)NULL);
        from += 1;
    }

    right = getRanges(input, from, to);
    if (right) {
        // Add the right side.
        left = new EllpStateNode(0, EllpStateNode::OR, left, right);
    }

    return left;
}

//
// buildSet - Create a set tree.
//
static EllpStateNode* buildSet(EllpStateNode::Type type, const std::string& input, int from, int to)
{
    EllpStateNode *np = getRanges(input, from, to);

    return new EllpStateNode(0, type, np, NULL);
}

//
// primary - Get a regular expression primary.
//
static EllpStateNode* primary(const std::string& input, int& index, int& ch)
{
    EllpStateNode *node = NULL;

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
        node = new EllpStateNode(0, ch, (EllpStateMachine*)NULL);
        getinput(input, index, ch);
    } else if (ch == DOT) {
        // Match any input.
        node = new EllpStateNode(0, 0, EllpStateMachine::INPUTMAX);
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

        node = buildSet(negate ? EllpStateNode::NOTSET : EllpStateNode::SET, input, index, current);
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
static EllpStateNode* unary(const std::string& input, int& index, int& ch)
{
    EllpStateNode::Type type = EllpStateNode::NONE;
    EllpStateNode *node;

    node = primary(input, index, ch);

    while (ch == QUESTION || ch == STAR || ch == PLUS) {
        if (ch == QUESTION) {
            if (type != EllpStateNode::NONE) {
                if (type != EllpStateNode::ZEROORONE) {
                    // *? or +? become *. ?? remains ?.
                    node->type = EllpStateNode::ZEROORMORE;
                }
                getinput(input, index, ch);
                continue;
            }
            type = EllpStateNode::ZEROORONE;
        } else if (ch == STAR) {
            if (type != EllpStateNode::NONE) {
                // ?*, **, +* become *.
                node->type = EllpStateNode::ZEROORMORE;
                getinput(input, index, ch);
                continue;
            }
            type = EllpStateNode::ZEROORMORE;
        } else {
            if (type != EllpStateNode::NONE) {
                if (type != EllpStateNode::ONEORMORE) {
                    // *+, ?+, become *. ++ remains +.
                    node->type = EllpStateNode::ZEROORMORE;
                }
                getinput(input, index, ch);
                continue;
            }
            type = EllpStateNode::ONEORMORE;
        }
        node = new EllpStateNode(0, type, node);
        getinput(input, index, ch);
    }

    return node;
}

//
// concat - Adjacent characters are concatenated.
//
static EllpStateNode* concat(const std::string& input, int& index, int& ch)
{
    EllpStateNode *left, *right;

    left = unary(input, index, ch);
    while (ch != EOS && ch != BAR && ch != RPAREN && ch != ']') {
        // Concatenated.
        right = unary(input, index, ch);
        left = new EllpStateNode(0, EllpStateNode::CONCAT, left, right);
    }
    return left;
}

//
// orop - Parse the or operator.
//
static EllpStateNode* orop(const std::string& input, int& index, int& ch)
{
    EllpStateNode *left, *right;
    
    left = concat(input, index, ch);
    while (ch == BAR) {
        // OR operator.
        getinput(input, index, ch);
        right = concat(input, index, ch);
        left = new EllpStateNode(0, EllpStateNode::OR, left, right);
    }

    return left;
}

//
// EllpStateNode - Convert a regular expression string into a tree.
//
EllpStateNode::EllpStateNode(const std::string& input)
{
    int pos = 0;
    int ch;
    getinput(input, pos, ch);
    *this = *orop(input, pos, ch);              // Build the regular expression tree.
}

//
// freeTree - Deconstruct a state tree.
//
void EllpStateNode::freeTree(EllpStateNode* tree)
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
void EllpStateNode::treePrint(FILE* fp, const char* (*inputname)(int, void*), void* context, int prec)
{

    if (index) {
        fprintf(fp, "$%d = ", index);
    }

    switch (type) {
    case UNKNOWN:
        if (u.u.name) {
            std::string name = u.u.name(u.u.value);
            fprintf(fp, "@s ", &name);
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
            if (u.r.right == EllpStateMachine::INPUTMAX) {
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
void EllpStateNode::print(FILE* fp, const char* (*inputname)(int, void*), void* context)
{
    treePrint(fp, inputname, context, PRECLOW);
}

//
// statePrint - Print out a state.
//
void EllpStateMachine::statePrint(FILE* fp, State* sp, void* context)
{
    fprintf(fp, "    %d (%d", sp->number, sp->depth);
    if (sp->index) {
        fprintf(fp, " $%d", sp->index);
    }
    fprintf(fp, "):\n", sp->number, sp->depth);
    if (sp->machines.list.size()) {
        // This state has pre-state machines.
        fprintf(fp, "      (");
        for (int i = 0; i < sp->machines.list.size(); ++i) {
            fprintf(fp, " @s", &sp->machines.list[i]->name);
        }
        fprintf(fp, " )\n");
    }
    for (int i = 0; i < inputsize; ++i) {
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
    }
}

//
// reversePrint - Print the state list in definition order.
//
void EllpStateMachine::reversePrint(FILE* fp, State* sp, void* context)
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
void EllpStateMachine::print(FILE* fp, void* context)
{
    fprintf(fp, "State machine: @s ", &name);
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
