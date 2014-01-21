#include <iostream>
#include <map>
#include <fstream>
#include <algorithm>
#include <vector>

using namespace std;

typedef pair<string, int> PAIR;

ostream& operator<<(ostream& out, const PAIR& p) {
    return out << p.first << '\t' << p.second;
}

bool cmp_by_value(const PAIR& lhs, const PAIR& rhs) {
    return lhs.second < rhs.second;
}

int main () {
    map<string, char> tran_type; // T_id, B or T
    map<string, string> tran_forwoard_id; // T_id, forward_id
    map<string, int> tran_forwoard_money; // T_id, forward_money
    map<string, int> tran_sec_id; // T_id, sec_id
    map<int, PAIR> tran_sec; // sec_id, <account, money>

    // read the transaction.txt
    ifstream file("transactions.txt");

    int account_num;
    file >> account_num;
    
    string base_id;
    int now_sec_id = 0;
    while (account_num--){

        file >> base_id;
        int money;
        file >> money;

        tran_sec[now_sec_id] = make_pair(base_id, money);

        int transaction;
        file >> transaction;
        while (transaction--) {
            string tran_id;
            char now_tran_type;
            file >> tran_id >> now_tran_type;
            tran_sec_id[tran_id] = now_sec_id;
            tran_type[tran_id] = now_tran_type;
            if (now_tran_type == 'T') {
                string forwoard_id;
                file >> forwoard_id >> money;
                tran_forwoard_id[base_id] = forwoard_id;
                tran_forwoard_money[base_id] = money;
            }
        }
        ++now_sec_id;
    }

    // sort the log transaction
    map<string, int> account_balance;
    map<string, int> log_transaction;
    map<string, int> log_balance;
    ifstream logfile("transaction_log.txt");

    string tran_id;
    while (logfile >> tran_id) {
        
    }

    vector<PAIR> log_transaction_vec(log_transaction.begin(), log_transaction.end());
    sort(log_transaction_vec.begin(), log_transaction_vec.end(), cmp_by_value);

    return 0;
}
