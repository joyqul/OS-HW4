#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <fstream>

using namespace std;

typedef pair<string, int> PAIR;
typedef unordered_map<string, char> MAP_CH;
typedef unordered_map<string, bool> MAP_BO;
typedef unordered_map<string, string> MAP_STR;
typedef unordered_map<string, int> MAP_INT;
typedef unordered_map<int, PAIR> MAP_PA;
typedef unordered_map<int, vector<string> > MAP_VEC;
typedef vector<PAIR> VEC_PA;

int sec_num;
MAP_CH tran_type; // T_id, B or T
MAP_STR tran_forward_id; // T_id, forward_id
MAP_INT tran_forward_money; // T_id, forward_money
MAP_INT tran_sec_id; // T_id, sec_id
MAP_PA tran_sec; // sec_id, <account, money>
MAP_VEC sec_tran; // one scetion's transactions
MAP_INT account_balance;

void bank(int now_tran[], int last_tran[], int now_sec_id, int& now_stamp, 
        fstream& file, bool finish[]) {

    // Do one by one
    while (1) {

        if (now_tran[now_sec_id] == last_tran[now_sec_id]) {
            finish[now_sec_id] = true;
            return;
        }
        string now_tran_id = sec_tran[now_sec_id][now_tran[now_sec_id]];
        string base_id = tran_sec[now_sec_id].first;
        string forward_id = tran_forward_id[now_tran_id];

        int tran_money = tran_forward_money[now_tran_id];

        if (tran_type[now_tran_id] == 'B') {
            if (now_tran[now_sec_id] == 0) {
                int money = tran_sec[now_sec_id].second;
                account_balance[base_id] += money;
            }

            file << now_tran_id << " " << now_stamp << " " << account_balance[base_id]<< endl;

            ++now_tran[now_sec_id];
            ++now_stamp;
            continue;
        }

        if (now_tran[now_sec_id] == 0) {
            int money = tran_sec[now_sec_id].second;
            account_balance[base_id] += money;

            if (account_balance[base_id] < tran_money) {
                account_balance[base_id] -= money;
                return;
            }
            else {
                account_balance[base_id] -= tran_money;
                account_balance[forward_id] += tran_money;

                file << now_tran_id << " " << now_stamp << endl;

                ++now_tran[now_sec_id];
                ++now_stamp;
            }
        }
        else if (account_balance[base_id] >= tran_money) {
            account_balance[base_id] -= tran_money;
            account_balance[forward_id] += tran_money;

            file << now_tran_id << " " << now_stamp << endl;

            ++now_tran[now_sec_id];
            ++now_stamp;
        }
        else return;
    }
}

bool finished(int now_tran[], int last_tran[], bool finsh[]) {
    for (int now_sec_id = 0; now_sec_id < sec_num; ++now_sec_id) {
        if (finsh[now_sec_id]) continue;
        else if (last_tran[now_sec_id] == now_tran[now_sec_id]) {
            finsh[now_sec_id] = true;
            continue;
        }
        else return false;
    }
    return true;
}

void schedule(int last_tran[]) {

    fstream file;
    file.open("transaction_log.txt", fstream::out);

    int now_stamp = 1;
    int now_tran[sec_num];
    bool finsh[sec_num];
    for (int i = 0; i < sec_num; ++i) {
        now_tran[i] = 0;
        finsh[i] = false;
    }


    while (!finished(now_tran, last_tran, finsh)) {
        for (int now_sec_id = 0; now_sec_id < sec_num; ++now_sec_id) {
            if (finsh[now_sec_id]) continue;
            bank(now_tran, last_tran, now_sec_id, now_stamp, file, finsh);
        }
    }
    file.close();
    
    file.open("final_state.txt", fstream::out);
    for (int i = 0; i < sec_num; ++i)
        file << tran_sec[i].first << " " << account_balance[tran_sec[i].first] << endl;
    file.close();
}

int main () {

    // read the transaction.txt
    fstream file;
    file.open("transactions.txt", fstream::in);

    file >> sec_num;

    string base_id;
    int last_tran[sec_num];
    for (int now_sec_id = 0; now_sec_id < sec_num; ++now_sec_id) {

        file >> base_id;
        int money;
        file >> money;

        int transaction;
        file >> transaction;
        last_tran[now_sec_id] = transaction;
        vector<string> tmp_sec_tran;

        // 0 transaction
        if (!transaction) {
            account_balance[base_id] += money;
            continue;
        }

        tran_sec[now_sec_id] = make_pair(base_id, money);

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


    schedule(last_tran);

    return 0;
}
