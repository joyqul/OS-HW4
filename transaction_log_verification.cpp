#include <iostream>
#include <map>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <vector>

using namespace std;

typedef pair<string, int> PAIR;
typedef map<string, char> MAP_CH;
typedef map<string, bool> MAP_BO;
typedef map<string, string> MAP_STR;
typedef map<string, int> MAP_INT;
typedef map<int, PAIR> MAP_PA;
typedef map<int, vector<string> > MAP_VEC;
typedef vector<PAIR> VEC_PA;


ostream& operator<<(ostream& out, const PAIR& p) {
    return out << p.first << ' ' << p.second;
}

bool cmp_by_value(const PAIR& lhs, const PAIR& rhs) {
    return lhs.second < rhs.second;
}

bool mark(int sections[], VEC_PA::iterator& simulate_log, 
        const VEC_PA::iterator simulate_end, MAP_INT tran_sec_id,
        MAP_BO tran_done, const int now_sec_id, const int section_num,
        MAP_INT& account_balance, MAP_PA tran_sec, MAP_STR tran_forward_id, 
        MAP_INT tran_forward_money, MAP_VEC sec_tran, MAP_CH tran_type,
        MAP_INT log_account_balance) {

    bool mutual[section_num];
    for (int i = 0; i < section_num; ++i)
        mutual[i] = false;

    for (; simulate_log != simulate_end && now_sec_id == simulate_log->second; 
            ++simulate_log) {

        // to see if whether the string is the first one of each section
        string now_tran = simulate_log->first;  // T_id
        int orig_sec = tran_sec_id[now_tran]; // origin sec
        if ((sec_tran[orig_sec])[sections[orig_sec]] != now_tran) {
            cout << "Invalid answer: inner section order wrong" << endl;
            cout << "transaction: " << now_tran << " occure before " 
                << (sec_tran[orig_sec])[sections[orig_sec]] << endl;
            return false;
        }

        // to see whether each section has one transaction
        if (mutual[orig_sec]) {
            cout << "Invalid answer: more than one transaction in one section" << endl;
            cout << "transaction: " << now_tran << " in section" << orig_sec << endl;
            return false;
        }
        else {
            mutual[orig_sec] = true;
            if (!sections[orig_sec]) {
                account_balance[tran_sec[orig_sec].first] += tran_sec[orig_sec].second;
            }
            if (tran_type[now_tran] == 'T') {
                mutual[tran_sec_id[tran_forward_id[now_tran]]] = true;
            }
        }

        // do the bank!
        int tran_money = tran_forward_money[now_tran];
        string base_id = tran_sec[orig_sec].first; // log sec
        if (tran_money > account_balance[base_id]) {
            cout << "Invalid answer: negtive account" <<endl;
            cout << "transaction: " << now_tran << endl;
            cout << "account: " << base_id << " will be negtive" << endl;
            return false;
        }
        else {
            account_balance[tran_forward_id[now_tran]] += tran_money;
            account_balance[base_id] -= tran_money;
        }

        // Check the account
        if (tran_type[now_tran] == 'B') {
            if (account_balance[base_id] != log_account_balance[now_tran]) {
                cout << "Wrong account balance" << endl;
                cout << "transaction: " << now_tran << endl;
                cout << "your balance: " << log_account_balance[now_tran] 
                    << " correct balance: " << account_balance[base_id] << endl;
                return false;
            }
        }
        
        ++sections[orig_sec];
        tran_done[now_tran] = true;
    }

    return true;
}

void verify(VEC_PA log_transaction, MAP_BO tran_done, const int section_num, 
        MAP_INT tran_sec_id, MAP_PA tran_sec, MAP_STR tran_forward_id, 
        MAP_INT tran_forward_money, MAP_VEC sec_tran, MAP_CH tran_type, 
        const MAP_INT log_account_balance) {

    VEC_PA::iterator simulate_log = log_transaction.begin();
    VEC_PA::iterator simulate_end = log_transaction.end();

    int sections[section_num];
    for (int i = 0; i < section_num; ++i) {
        sections[i] = 0;
    }

    MAP_INT account_balance;
    int max_timestamp = (--log_transaction.end())->second;
    for (int i = 0; i <= max_timestamp; ++i) {
        if (!mark(sections, simulate_log, simulate_end, tran_sec_id, tran_done, 
                i, section_num, account_balance, tran_sec, tran_forward_id, 
                tran_forward_money, sec_tran, tran_type, log_account_balance))
            return;
    }

    cout << "Correct!" << endl;
}

int main () {
    MAP_CH tran_type; // T_id, B or T
    MAP_STR tran_forward_id; // T_id, forward_id
    MAP_INT tran_forward_money; // T_id, forward_money
    MAP_INT tran_sec_id; // T_id, sec_id
    MAP_PA tran_sec; // sec_id, <account, money>
    MAP_VEC sec_tran; // one scetion's transactions

    // read the transaction.txt
    ifstream file("transactions.txt");

    int section_num;;
    file >> section_num;
    
    string base_id;
    for (int now_sec_id = 0; now_sec_id < section_num; ++now_sec_id) {

        file >> base_id;
        int money;
        file >> money;

        tran_sec[now_sec_id] = make_pair(base_id, money);

        int transaction;
        file >> transaction;
        vector<string> tmp_sec_tran;
        
        while (transaction--) {
            string tran_id;
            char now_tran_type;
            file >> tran_id >> now_tran_type;
            tran_sec_id[tran_id] = now_sec_id;
            tran_type[tran_id] = now_tran_type;
            tmp_sec_tran.push_back(tran_id);
            if (now_tran_type == 'T') {
                string forward_id;
                file >> forward_id >> money;
                tran_forward_id[tran_id] = forward_id;
                tran_forward_money[tran_id] = money;
            }
        }
        sec_tran[now_sec_id] = tmp_sec_tran;
        tmp_sec_tran.clear();
    }

    // sort the log transaction
    MAP_INT log_account_balance; // T_id, balance when B
    MAP_INT log_transaction; // T_id, timestamp
    MAP_BO tran_done; // sec_id, true for done
    ifstream logfile("transaction_log.txt");

    string tran_id;
    int timestamp;
    while (logfile >> tran_id) {
        logfile >> timestamp;
        log_transaction[tran_id] = timestamp - 1;
        tran_done[tran_id] = false;
        if (tran_type[tran_id] == 'B') {
            int balance;
            logfile >> balance;
            log_account_balance[tran_id] = balance;
        }
    }

    // sort log transactions by timestamp
    VEC_PA log_transaction_vec(log_transaction.begin(), log_transaction.end());
    sort(log_transaction_vec.begin(), log_transaction_vec.end(), cmp_by_value);

    verify(log_transaction_vec, tran_done, section_num, tran_sec_id, 
            tran_sec, tran_forward_id, tran_forward_money, sec_tran, tran_type, log_account_balance);

    return 0;
}
