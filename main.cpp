#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <algorithm>
#include <map>
#include <set>
#include <stack>
#include <iomanip>

using namespace std;

map<char, vector<int>> NoneTerminals; // storing rules for each NoneTerminal
map<int, char> RuleNT;                // storing rule number poinint to NoneTerminal
map<int, vector<char>> Rules;         // storing state of each rule
set<char> terminals;                  // set of terminals
int ruleNumber = 1;

map<char, set<char>> First;    // storing First of each NoneTerminal
map<int, set<char>> ruleFirst; // storing First of each rule
map<char, set<char>> Follow;   // storing Follow of each NoneTerminal

map<char, map<char, int>> M; // Parsing Tree

stack<char> STACK;
string W;

void read_grammar(string filePath);
bool isLetter(char ch);
bool isTerminal(char ch);
void generateError(int errorType);
void calculateFirst(char NT);
void calculateFollow();
void generateParsingTable();
void validateInput();

void printGrammer();
void printFirst();
void printFollow();
void printFirstFollow();
void printParsingTable();
void printRule(int rule);

int main(int argc, char *argv[])
{
    string filePath;
    filePath = argv[1];

    read_grammar(filePath);
    for (pair NT : NoneTerminals)
    {
        calculateFirst(NT.first);
    }
    calculateFollow();
    cout << '\n'
         << "First Follow Table:" << endl;
    printFirstFollow();
    generateParsingTable();
    cout << '\n'
         << "Parsing Table:" << endl;
    printParsingTable();

    while (true)
    {
        while (!STACK.empty())
        {
            STACK.pop();
        }
        STACK.push(RuleNT[1]);
        cout << endl
             << "Enter the input:" << endl;
        W.clear();
        cin >> W;
        W += "$";
        validateInput();
    }
}

void read_grammar(string filePath)
{
    ifstream myfile;
    myfile.open(filePath);
    while (!myfile.eof())
    {
        string line;
        getline(myfile, line);
        if (('/' == line[0]) || (' ' == line[0]) || (line.empty())) // handeling comments
        {
            continue;
        }
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        line += "|";
        char noneTerminal = line[0];

        if (isTerminal(noneTerminal))
        {
            generateError(1);
        }

        vector<char> items;
        set<char> emptySet;
        for (int i = 3; i < line.size(); i++)
        {
            if (('|' == line[i]))
            {
                auto find = NoneTerminals.find(noneTerminal);
                if (find == NoneTerminals.end()) // no same NoneTerminal in map
                {
                    NoneTerminals[noneTerminal] = {ruleNumber};
                    RuleNT[ruleNumber] = noneTerminal;
                    Rules[ruleNumber] = items;
                    First[noneTerminal] = emptySet;
                    Follow[noneTerminal] = emptySet;
                    switch (ruleNumber)
                    {
                    case 1: // adding $ to the starting NoneTerminal
                    {
                        Follow[noneTerminal].insert('$');
                        break;
                    }
                    default:
                        break;
                    }
                    ruleNumber++;
                    items.clear();
                }
                else // there is a rule starting with same NoneTerminal
                {
                    find->second.push_back(ruleNumber);
                    RuleNT[ruleNumber] = noneTerminal;
                    Rules[ruleNumber] = items;
                    ruleNumber++;
                    items.clear();
                }
            }
            else
            {
                if (isTerminal(line[i]))
                {
                    terminals.insert(line[i]);
                }
                items.push_back(line[i]);
            }
        }
    }
}

bool isLetter(char ch)
{
    // checks if the character is not a punctuation mark
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool isTerminal(char ch)
{
    if (!isLetter(ch))
    {
        return true;
    }
    else if (islower(ch))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void generateError(int errorType)
{
    switch (errorType)
    {
    case 0:
        break;
    case 1:
        cout << "ERROR" << endl;
        exit(EXIT_FAILURE);
    case 2:
        cout << "Error" << endl;
        exit(EXIT_FAILURE);
    default:
        break;
    }
}

void calculateFirst(char NT)
{
    set<char> firstNT;
    for (int r : NoneTerminals[NT])
    {
        vector<char> rule = Rules[r];
        if (isTerminal(rule[0]))
        {
            firstNT.insert(rule[0]);
            ruleFirst[r].insert(rule[0]);
            continue;
        }
        for (int i = 0; i < rule.size(); i++)
        {
            if (isTerminal(rule[i]))
            {
                firstNT.insert(rule[i]);
                ruleFirst[r].insert(rule[i]);
                break;
            }

            calculateFirst(rule[i]);
            auto it = find(First[rule[i]].begin(), First[rule[i]].end(), '&');
            if (it != First[rule[i]].end())
            {
                set<char> add = First[rule[i]];
                add.erase('&');
                firstNT.insert(add.begin(), add.end());
                ruleFirst[r].insert(add.begin(), add.end());
                continue;
            }
            else
            {
                firstNT.insert(First[rule[i]].begin(), First[rule[i]].end());
                ruleFirst[r].insert(First[rule[i]].begin(), First[rule[i]].end());
                break;
            }
        }

        int i = 0;
        for (auto rl : rule)
        {
            if (isTerminal(rl))
            {
                break;
            }

            auto it = find(First[rl].begin(), First[rl].end(), '&');
            if (it != First[rl].end())
            {
                i++;
            }
            else
            {
                continue;
            }
        }
        if (rule.size() == i)
        {
            firstNT.insert('&');
            ruleFirst[r].insert('&');
        }
    }
    First[NT].insert(firstNT.begin(), firstNT.end());
}

void calculateFollow()
{
    int n = ruleNumber;
    int t = n;
    while (--t)
    {

        int r = (int)'A';

        for (; r <= (int)'Z'; r++)
        {

            int i = (int)'A';
            for (; i <= (int)'Z'; i++)
            {
                if (NoneTerminals[(char)i].empty())
                    continue;

                int q = NoneTerminals[(char)i].size(); // vector of ints , rule numbers
                for (int j = 0; j < q; j++)
                {                                      // iterate through rule numbers in vector
                    int p = NoneTerminals[(char)i][j]; // number of rule to analyze
                    if(p == 1)
                        Follow[(char)i].insert('$');

                    for (int k = 0; k < Rules[p].size(); k++)
                    { // go through rule number p
                        if (Rules[p][k] == (char)r)
                        {

                            if (k + 1 == Rules[p].size())
                            {                                                                           // if r is the last character of the production. (ex. "B" in "A-> bcB")
                                Follow[(char)r].insert(Follow[(char)i].begin(), Follow[(char)i].end()); // add Follow(A) to Follow(B)
                            }
                            else
                                // if we find nonterminal r  (ex. "B")
                                while (++k < Rules[p].size())
                                { // if not the last character of production
                                    if (isTerminal(Rules[p][k]))
                                    {                                        // if the character after is a terminal (ex. Bb)
                                        Follow[(char)r].insert(Rules[p][k]); // add b to follow(B)
                                        break;
                                    }
                                    else
                                    {

                                        Follow[(char)r].insert(First[Rules[p][k]].begin(), First[Rules[p][k]].end()); // if the char after nonterminal is another nonterminal (ex BC)
                                        auto pos = First[Rules[p][k]].find('&');
                                        if (distance(First[Rules[p][k]].begin(), pos) < First[Rules[p][k]].size() && k == Rules[p].size() - 1)
                                        {                                                                           // if there's epsilon in first C
                                            Follow[(char)r].insert(Follow[(char)i].begin(), Follow[(char)i].end()); // add Follow of "A" in "A-> BC" to Follow of B
                                        }
                                        else if (distance(First[Rules[p][k]].begin(), pos) == First[Rules[p][k]].size())
                                        {                                                                         // no epsilon in first of nonterminal
                                            Follow[(char)r].insert(First[(char)i].begin(), First[(char)i].end()); // if no epsilon in first C, add First(C) to Follow(B)
                                            break;
                                        }
                                    }
                                }
                        }
                        Follow[(char)r].erase('&');
                    }
                }
            }
        }
    }
}

void generateParsingTable()
{
    for (pair NT : NoneTerminals)
    {
        for (int ruleN : NT.second)
        {
            set<char> rf = ruleFirst[ruleN];
            auto it = find(rf.begin(), rf.end(), '&');
            if (it != rf.end()) // rule has &
            {
                set<char> ts = Follow[NT.first];
                for (char t : ts)
                {
                    if (0 != M[NT.first][t])
                    {
                        cout << "Grammar is not LL (1)" << endl;
                        exit(EXIT_FAILURE);
                    }
                    M[NT.first][t] = ruleN;
                }
            }

            rf.erase('&');
            for (char f : rf)
            {
                if (0 != M[NT.first][f])
                {
                    cout << "Grammar is not LL (1)" << endl;
                    exit(EXIT_FAILURE);
                }

                M[NT.first][f] = ruleN;
            }
        }
    }

    for (pair NT : NoneTerminals)
    {
        if (NT.second.empty())
        {
            continue;
        }

        set<char> follow = Follow[NT.first];
        for (char t : follow)
        {
            if (0 != M[NT.first][t])
            {
                continue;
            }
            M[NT.first][t] = 'S';
        }
    }
}

void validateInput()
{
    int i = 0;
    char a = W[i];
    char X = STACK.top();
    while (!STACK.empty())
    {
        if (X == a)
        {
            STACK.pop();
            i++;
            a = W[i];
        }
        else if (isTerminal(X))
        {
            generateError(2);
        }
        else if (0 == M[X][a])
        {
            generateError(2);
        }
        else if (int rule = M[X][a])
        {
            printRule(rule);
            STACK.pop();
            vector<char> rules = Rules[rule];
            for (auto it = rules.rbegin(); it != rules.rend(); ++it)
            {
                if ('&' == *it)
                {
                    continue;
                }
                STACK.push(*it);
            }
        }
        if (STACK.empty())
        {
            break;
        }

        X = STACK.top();
    }
    cout << "ACCEPT" << endl;
    return;
}

void printGrammer()
{
    for (auto it : NoneTerminals)
    {
        cout << "Key: " << it.first << endl;
        for (auto j : it.second)
        {
            std::string myString;
            for (char c : Rules[j])
            {
                myString += c;
            }
            cout << "ruleNO:" << j << " Values: " << myString << endl;
        }
    }
    for (char element : terminals)
    {
        std::cout << element << " ";
    }
    cout << endl;
}

void printFirst()
{
    for (pair NoneTerminal : First)
    {
        cout << "FIRST(" << NoneTerminal.first << "): ";
        for (char terminal : NoneTerminal.second)
        {
            cout << terminal << " ";
        }
        cout << endl;
    }
}

void printFollow()
{

    int a = (int)'A';
    for (; a <= (int)'Z'; a++)
    {

        if (NoneTerminals[(char)a].empty())
            continue;
        auto it = Follow[(char)a].begin();
        cout << "Follow(" << (char)a << "): ";
        for (; it != Follow[(char)a].end(); it++)
            cout << *it << " ";
        cout << endl;
    }
}

void printFirstFollow() {
    cout << setw(20) << "First" << setw(20) << "Follow" << endl;

    for (auto NT : First) {
        cout << NT.first << setw(10);

        for (char terminal : NT.second) {
            cout << terminal << " ";
        }
        cout << setw(20 - NT.second.size() * 2);

        for (char terminal : Follow[NT.first]) {
            cout << terminal << " ";
        }
        cout << endl;
    }
}

void printParsingTable()
{
    set<char> finalTerminals = terminals;
    terminals.erase('&');
    terminals.insert('$');

    cout << "\t";
    for (char terminal : terminals)
    {
        cout << terminal << " ";
    }
    cout << endl;

    for (pair NT : NoneTerminals)
    {
        if (NT.second.empty())
        {
            continue;
        }

        cout << NT.first << "\t";
        for (char terminal : terminals)
        {
            if ('S' == M[NT.first][terminal])
            {
                cout << 'S' << " ";
            }
            else
            {
                cout << M[NT.first][terminal] << " ";
            }
        }
        cout << endl;
    }
}

void printRule(int rule)
{
    cout << RuleNT[rule] << "\t->\t";
    for (char c : Rules[rule])
    {
        cout << c;
    }
    cout << endl;
}
