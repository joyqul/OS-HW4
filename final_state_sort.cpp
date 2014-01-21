#include <iostream>
#include <map>
#include <fstream>

using namespace std;

typedef pair<string, int> PAIR;

ostream& operator<<(ostream& out, const PAIR& p) {
    return out << p.first << '\t' << p.second;
}

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
        cout << *it << endl;
    return 0;
}
