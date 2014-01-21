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
typedef vector<PAIR> VEC_PA;


ostream& operator<<(ostream& out, const PAIR& p) {
    return out << p.first << ' ' << p.second;
}

bool cmp_by_value(const PAIR& lhs, const PAIR& rhs) {
    return lhs.second < rhs.second;
}

void find_sec_begin(VEC_PA::iterator sections[], 
        VEC_PA tran_sec_id, const int section_num) {
    int pre_sec = 0;
    sections[0] = tran_sec_id.begin();

    int i = 1;
    for (VEC_PA::iterator it = tran_sec_id.begin(); 
            it != tran_sec_id.end() && i < section_num; ++it) {
        if (it->second != pre_sec) {
            sections[i] = it;
            ++i;
            ++pre_sec;
        }
    }
    return;
}

bool mark(VEC_PA::iterator sections[], VEC_PA::iterator& simulate_log, 
        const VEC_PA::iterator simulate_end, MAP_INT tran_sec_id,
        MAP_BO tran_done, const int now_sec_id, const int section_num,
        bool section_beginning[], MAP_INT account_balance, MAP_PA tran_sec,
        MAP_STR tran_forwoard_id, MAP_INT tran_forwoard_money) {

    bool mutual[section_num];
    for (int i = 0; i < section_num; ++i)
        mutual[i] = false;

    for (; simulate_log != simulate_end && now_sec_id == simulate_log->second; 
            ++simulate_log) {
        // to see if whether the string is the first one of each section
        string now_tran = simulate_log->first;  // T_id
        int now_sec = tran_sec_id[now_tran];
        if (sections[now_sec]->first != now_tran) {
            cout << "Invalid answer: inner section order wrong" << endl;
            cout << "transaction: " << now_tran << " occure before " 
                << sections[now_sec]->first << endl;
            return false;
        }
        else {
            ++sections[now_sec];
        }

        // to see whether each section has one transaction
        if (mutual[now_sec]) {
            cout << "Invalid answer: more than one transaction in one section" << endl;
            cout << "transaction: " << now_tran << " in section" << now_sec << endl;
            return false;
        }
        else {
            mutual[now_sec] = true;
            if (section_beginning[now_sec]) {
                account_balance[tran_sec[now_sec].first] += tran_sec[now_sec].second;
                section_beginning[now_sec] = false;
            }
        }

        // do the bank!
        int tran_money = tran_forwoard_money[now_tran];
        string base_id = sections[now_sec]->first;
        if (tran_money > account_balance[base_id]) {
            cout << "Invalid answer: negtive account" <<endl;
            cout << "transaction: " << now_tran << endl;
            cout << "account: " << base_id << " will be negtive" << endl;
            return false;
        }
        else {
            account_balance[tran_forwoard_id[now_tran]] += tran_money;
            account_balance[base_id] -= tran_money;
        }

        tran_done[simulate_log->first] = true;
    }
    --simulate_log;
    return true;
}

void verify(VEC_PA log_transaction, VEC_PA tran_sec_id, MAP_BO tran_done, 
        const int section_num, MAP_INT tran_sec_id_map, MAP_PA tran_sec,
        MAP_STR tran_forwoard_id, MAP_INT tran_forwoard_money) {
    VEC_PA::iterator simulate_log = log_transaction.begin();
    VEC_PA::iterator simulate_end = log_transaction.end();
    VEC_PA::iterator* sections = new VEC_PA::iterator[section_num];
    find_sec_begin(sections, tran_sec_id, section_num); // cut the sections
    
    bool section_beginning[section_num];
    for (int i = 0; i < section_num; ++i)
        section_beginning[i] = true;

    MAP_INT account_balance;
    int max_timestamp = (--log_transaction.end())->second;
    for (int i = 0; i < max_timestamp; ++i) {
        if (!mark(sections, simulate_log, simulate_end, tran_sec_id_map, tran_done, 
                i, section_num, section_beginning, account_balance, tran_sec,
                tran_forwoard_id, tran_forwoard_money))
            return;
    }
}

int main () {
    MAP_CH tran_type; // T_id, B or T
    MAP_STR tran_forwoard_id; // T_id, forward_id
    MAP_INT tran_forwoard_money; // T_id, forward_money
    MAP_INT tran_sec_id; // T_id, sec_id
    MAP_PA tran_sec; // sec_id, <account, money>

    // read the transaction.txt
    ifstream file("transactions.txt");

    int section_num;;
    file >> section_num;
    
    string base_id;
    int now_sec_id = 0;
    for (int i = 0; i < section_num; ++i) {

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
    MAP_INT log_account_balance; // T_id, balance when B
    MAP_INT log_transaction; // T_id, timestamp
    MAP_BO tran_done; // sec_id, true for done
    ifstream logfile("transaction_log.txt");

    string tran_id;
    int timestamp;
    while (logfile >> tran_id) {
        logfile >> timestamp;
        log_transaction[tran_id] = timestamp;
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

    // sort tran_id by sec_id
    VEC_PA tran_sec_id_vec(tran_sec_id.begin(), tran_sec_id.end());
    sort(tran_sec_id_vec.begin(), tran_sec_id_vec.end(), cmp_by_value);

    //verify();
    verify(log_transaction_vec, tran_sec_id_vec, tran_done, section_num, 
            tran_sec_id, tran_sec, tran_forwoard_id, tran_forwoard_money);

    return 0;
}
