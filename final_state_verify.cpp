#include <iostream>
#include <map>
#include <fstream>

using namespace std;

typedef map<string, int> MAP_INT;
typedef pair<string, int> PAIR;

MAP_INT account_balance;

ostream& operator<<(ostream& out, const PAIR& p) {
    return out << p.first << '\t' << p.second;
}

int main () {
    fstream file;
    file.open("transactions.txt", fstream::in);
    int sec_num;
    file >> sec_num;
    while (sec_num--) {
        string account;
        int init_money, transactions;
        file >> account >> init_money >> transactions;
        account_balance[account] += init_money;
        while (transactions--) {
            string T_id;
            char T_type;
            file >> T_id >> T_type;
            if (T_type == 'T') {
                string forward_id;
                int forward_money;
                file >> forward_id >> forward_money;
                account_balance[forward_id] += forward_money;
                account_balance[account] -= forward_money;
            }
        }
    }
    file.close();

    for (MAP_INT::iterator it = account_balance.begin(); it != account_balance.end(); ++it)
        cout << *it << endl;
    return 0;
}
