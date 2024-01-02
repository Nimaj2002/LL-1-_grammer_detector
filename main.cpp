#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <algorithm>
#include <map>
#include <set>

using namespace std;

map<char, vector<int>> listOfRules;
map<int, vector<char>> Rules;
set<char> terminals;
int ruleNumber = 1;

void read_grammar(string filePath);
void printGrammer();
bool isLetter(char ch);
bool isTerminal(char ch);
void generateError(int errorType);

int main(int argc, char *argv[])
{
    string filePath;
    filePath = argv[1];

    read_grammar(filePath);
    printGrammer();
}

void read_grammar(string filePath)
{
    ifstream myfile;
    myfile.open(filePath);
    while (!myfile.eof())
    {
        string line;
        getline(myfile, line);
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        line += "|";
        char noneTerminal = line[0];

        if (islower(noneTerminal))
        {
            generateError(1);
        }

        vector<char> items;
        for (int i = 3; i < line.size(); i++)
        {
            if (('|' == line[i]))
            {
                auto find = listOfRules.find(noneTerminal);
                if (find == listOfRules.end()) // no same NoneTerminal in map
                {
                    listOfRules[noneTerminal] = {ruleNumber};
                    Rules[ruleNumber] = items;

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
    for (auto it : listOfRules)
    {
        cout << "Key: " << it.first << endl;
        for (auto j : it.second)
        {
            std::string myString;
            for (char c : Rules[j])
            {
                myString += c;
            }
            cout << "Values: " << myString << endl;
        }
    }
    for (char element : terminals)
    {
        std::cout << element << " ";
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
    default:
        break;
    }
}