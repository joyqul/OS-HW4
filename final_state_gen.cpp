#include <iostream>
#include <map>
#include <fstream>

using namespace std;

typedef pair<string, int> PAIR;

ostream& operator<<(ostream& out, const PAIR& p) {
    return out << p.first << ' ' << p.second;
}

int main () {
    map<string, int> account_balance;
    ifstream file("transactions.txt");
    int account_num;
    file >> account_num;
    string base_id;
    while (account_num--) {
        file >> base_id;
        int money;
        file >> money;
        account_balance[base_id] += money;
        int transaction;
        file >> transaction;
        while (transaction--) {
            string tran_id;
            char tran_type;
            file >> tran_id >> tran_type;
            if (tran_type == 'T') {
                string forwoard_id;
                file >> forwoard_id >> money;
                account_balance[base_id] -= money;
                account_balance[forwoard_id] += money;
            }
        }
    }
    for (map<string, int>::iterator it = account_balance.begin(); it != account_balance.end(); ++it) 
        cout << *it << endl;

    return 0;
}
