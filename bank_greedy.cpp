#include <iostream>
#include <map>
#include <vector>
#include <fstream>

using namespace std;

typedef pair<string, int> PAIR;
typedef map<string, char> MAP_CH;
typedef map<string, bool> MAP_BO;
typedef map<string, string> MAP_STR;
typedef map<string, int> MAP_INT;
typedef map<int, PAIR> MAP_PA;
typedef map<int, vector<string> > MAP_VEC;
typedef vector<PAIR> VEC_PA;

void bank(const int sec_num, int now_tran[], int last_tran[], const int now_stamp,
        fstream& file, MAP_INT& account_balance, MAP_VEC sec_tran, MAP_PA tran_sec,
        MAP_STR tran_forward_id, MAP_INT tran_forward_money, MAP_CH tran_type,
        MAP_INT tran_sec_id) {
    // Lock
    bool locked[sec_num];
    for (int i = 0; i < sec_num; ++i)
        locked[i] = false;


    // Find B and lock first
    for (int now_sec_id = 0; now_sec_id < sec_num; ++now_sec_id) {
        if (now_tran[now_sec_id] == last_tran[now_sec_id]) continue;
        string now_tran_id = sec_tran[now_sec_id][now_tran[now_sec_id]];
        if (tran_type[now_tran_id] == 'B') {
            locked[now_sec_id] = true;
            ++now_tran[now_sec_id];
            string base_id = tran_sec[now_sec_id].first;

            file.open("transaction_log.txt", fstream::app);
            file << now_tran_id << " " << now_stamp << " " << account_balance[base_id] << endl;
            cout << now_tran_id << " " << now_stamp << " " << account_balance[base_id] << endl;
            file.close();
        }
    }


    // Do one by one
    for (int now_sec_id = 0; now_sec_id < sec_num; ++now_sec_id) {
        if (locked[now_sec_id]) continue;
        if (now_tran[now_sec_id] == last_tran[now_sec_id]) continue;

        string now_tran_id = sec_tran[now_sec_id][now_tran[now_sec_id]];
        string base_id = tran_sec[now_sec_id].first;
        string forward_id = tran_forward_id[now_tran_id];
        if (locked[tran_sec_id[forward_id]]) continue;
        
        int tran_money = tran_forward_money[now_tran_id];

        if (now_tran[now_sec_id] == 0) {
            int money = tran_sec[now_sec_id].second;
            account_balance[base_id] += money;

            if (account_balance[base_id] < tran_money) {
                account_balance[base_id] -= money;
                continue;
            }
            else {
                account_balance[base_id] -= tran_money;
                account_balance[forward_id] += tran_money;

                file.open("transaction_log.txt", fstream::app);
                file << now_tran_id << " " << now_stamp << endl;
                cout << now_tran_id << " " << now_stamp << endl;
                file.close();

                ++now_tran[now_sec_id];
                locked[now_sec_id] = true;
                locked[tran_sec_id[forward_id]] = true;
            }
        }
        else if (account_balance[base_id] > tran_money) {
            account_balance[base_id] -= tran_money;
            account_balance[forward_id] += tran_money;

            file.open("transaction_log.txt", fstream::app);
            file << now_tran_id << " " << now_stamp << endl;
            cout << now_tran_id << " " << now_stamp << endl;
            file.close();

            ++now_tran[now_sec_id];
            locked[now_sec_id] = true;
            locked[tran_sec_id[forward_id]] = true;
        }
    }

}

bool finished(const int sec_num, int last_tran[], int now_tran[]) {
    for (int now_sec_id = 0; now_sec_id < sec_num; ++now_sec_id) {
        if (last_tran[now_sec_id] != now_tran[now_sec_id]) {
            return false;
        }
    }
    return true;
}

void schedule(const int sec_num, int last_tran[], fstream& file,
        MAP_VEC sec_tran, MAP_PA tran_sec, MAP_STR tran_forward_id, 
        MAP_INT tran_forward_money, MAP_CH tran_type,
        MAP_INT tran_sec_id) {

    int now_stamp = 1;
    int now_tran[sec_num];
    for (int i = 0; i < sec_num; ++i)
        now_tran[i] = 0;

    MAP_INT account_balance;

    while (!finished(sec_num, last_tran, now_tran)) {
        bank(sec_num, now_tran, last_tran, now_stamp, file, account_balance, sec_tran, tran_sec,
                tran_forward_id, tran_forward_money, tran_type, tran_sec_id);
        ++now_stamp;
    }
}

int main () {
    MAP_CH tran_type; // T_id, B or T
    MAP_STR tran_forward_id; // T_id, forward_id
    MAP_INT tran_forward_money; // T_id, forward_money
    MAP_INT tran_sec_id; // T_id, sec_id
    MAP_PA tran_sec; // sec_id, <account, money>
    MAP_VEC sec_tran; // one scetion's transactions

    // read the transaction.txt
    fstream file("transactions.txt", fstream::in);

    int sec_num;;
    file >> sec_num;

    string base_id;
    int last_tran[sec_num];
    for (int now_sec_id = 0; now_sec_id < sec_num; ++now_sec_id) {

        file >> base_id;
        int money;
        file >> money;

        tran_sec[now_sec_id] = make_pair(base_id, money);

        int transaction;
        file >> transaction;
        last_tran[now_sec_id] = transaction;
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
    file.close();

    
    file.open("transaction_log.txt", fstream::out);
    file.close();

    schedule(sec_num, last_tran, file, sec_tran, tran_sec, tran_forward_id, 
            tran_forward_money, tran_type, tran_sec_id);

    return 0;
}
