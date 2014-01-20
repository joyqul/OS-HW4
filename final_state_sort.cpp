#include <iostream>
#include <map>
#include <fstream>

using namespace std;

int main () {
    map<string, int> account_balance;
    ifstream file("final_state_answer.txt");
    string s;
    while (file >> s) {
        int money;
        file >> money;
        account_balance[s] = money;
    }
    for (map<string, int>::iterator it = account_balance.begin(); it != account_balance.end(); ++it) 
        cout << it->first << " " << it->second << endl;
    return 0;
}
