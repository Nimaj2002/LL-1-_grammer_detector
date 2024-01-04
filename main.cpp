#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <algorithm>
#include <map>
#include <set>

using namespace std;

map<char, vector<int>> NoneTerminals;
map<int, vector<char>> Rules;
set<char> terminals;
int ruleNumber = 1;

map<char, set<char>> First;
map<char, set<char>> Follow;

void read_grammar(string filePath);
void printGrammer();
bool isLetter(char ch);
bool isTerminal(char ch);
void generateError(int errorType);
void calculateFirst(char NT);
void printFirst();
set<char> firstof(char item, vector<char> Rule, int i);

int main(int argc, char *argv[])
{
    string filePath;
    filePath = argv[1];

    read_grammar(filePath);
    printGrammer();
    for (pair NT : NoneTerminals)
    {
        calculateFirst(NT.first);
    }
    printFirst();
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
                    Rules[ruleNumber] = items;
                    First[noneTerminal] = emptySet;
                    Follow[noneTerminal] = emptySet;
                    switch (ruleNumber)
                    {
                    case 1: // adding $ to the starting NoneTerminal
                        Follow[noneTerminal].insert('$');
                        break;
                    default:
                        break;
                    }
                    ruleNumber++;
                    items.clear();
                }
                else // there is a rule starting with same NoneTerminal
                {
                    find->second.push_back(ruleNumber);
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
            continue;
        }
        for (int i = 0; i < rule.size(); i++)
        {
            if (isTerminal(rule[i]))
            {
                firstNT.insert(rule[i]);
                break;
            }

            calculateFirst(rule[i]);
            auto it = find(First[rule[i]].begin(), First[rule[i]].end(), '&');
            if (it != First[rule[i]].end())
            {
                set<char> add = First[rule[i]];
                add.erase('&');
                firstNT.insert(add.begin(), add.end());
                continue;
            }
            else
            {
                firstNT.insert(First[rule[i]].begin(), First[rule[i]].end());
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
        if (rule.size() == i){
            firstNT.insert('&');
        }
        
    }
    First[NT].insert(firstNT.begin(), firstNT.end());
}

void printFirst()
{
    for (pair NoneTerminal : First)
    {
        cout << "FIRST(" << NoneTerminal.first << "): ";
        for (char terminal : NoneTerminal.second)
        {
            cout << terminal << ", ";
        }
        cout << endl;
    }
}