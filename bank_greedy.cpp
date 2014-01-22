#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <fstream>
#define MAX 20

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
MAP_INT account_balance;
MAP_PA tran_sec; // sec_id, <account, money>
MAP_VEC sec_tran; // one scetion's transactions

void bank(int now_tran[], int last_tran[], const int now_stamp, 
        fstream& file, bool finsh[]) {
    int now_test = 0;

    // Lock
    bool locked[sec_num];
    for (int i = 0; i < sec_num; ++i)
        locked[i] = false;


    // Find B and lock first
    for (int now_sec_id = 0; now_sec_id < sec_num; ++now_sec_id) {
        if (now_test == MAX) return;
        if (finsh[now_sec_id]) continue;
        if (now_tran[now_sec_id] == last_tran[now_sec_id]) continue;
        string now_tran_id = sec_tran[now_sec_id][now_tran[now_sec_id]];
        if (tran_type[now_tran_id] == 'B') {
            locked[now_sec_id] = true;
            ++now_tran[now_sec_id];
            string base_id = tran_sec[now_sec_id].first;

            file << now_tran_id << " " << now_stamp << " " << account_balance[base_id] << endl;
            ++now_test;
        }
    }


    // Do one by one
    for (int now_sec_id = 0; now_sec_id < sec_num; ++now_sec_id) {
        if (now_test == MAX) return;
        if (finsh[now_sec_id]) continue;
        if (locked[now_sec_id]) continue;
        if (now_tran[now_sec_id] == last_tran[now_sec_id]) continue;

        ++now_test;
        string now_tran_id = sec_tran[now_sec_id][now_tran[now_sec_id]];
        string base_id = tran_sec[now_sec_id].first;
        string forward_id = tran_forward_id[now_tran_id];
        if (locked[tran_sec_id[forward_id]]) continue;
        

        int tran_money = tran_forward_money[now_tran_id];

        if (account_balance[base_id] >= tran_money) {
            account_balance[base_id] -= tran_money;
            account_balance[forward_id] += tran_money;

            file << now_tran_id << " " << now_stamp << endl;

            ++now_tran[now_sec_id];
            locked[now_sec_id] = true;
            locked[tran_sec_id[forward_id]] = true;
        }
    }

}

bool finished(int last_tran[], int now_tran[], bool finish[]) {
    for (int now_sec_id = 0; now_sec_id < sec_num; ++now_sec_id) {
        if (finish[now_sec_id]) continue;
        else if (last_tran[now_sec_id] == now_tran[now_sec_id]) {
            finish[now_sec_id] = true;
        }
        else return false;
    }
    return true;
}

void schedule(int last_tran[]) {

    int now_stamp = 1;
    int now_tran[sec_num];
    bool finish[sec_num];
    for (int i = 0; i < sec_num; ++i) {
        now_tran[i] = 0;
        finish[i] = false;
    }

    fstream file;
    file.open("transaction_log.txt", fstream::out);

    while (!finished(last_tran, now_tran, finish)) {
        bank(now_tran, last_tran, now_stamp, file, finish);
        ++now_stamp;
    }
    file.close();

    file.open
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

        account_balance[base_id] = money;
        tran_sec[now_sec_id] = make_pair(base_id, money);
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

    

    schedule(last_tran);

    return 0;
}
