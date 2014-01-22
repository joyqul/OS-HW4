#include <iostream>
#include <unordered_map>
#include <queue>
#include <vector>
#include <fstream>

using namespace std;

struct ACCOUNT {
    int balance;
    string account_id;
    queue<int> trans;
    ACCOUNT(string s, int b):balance(b), account_id(s){}
};

struct TRANS {
    string tran_id;
    char tran_type;
    string forward_id;
    int forward_money, timestamp;
    TRANS(string tid, char t, string foid, int m):
        tran_id(tid), tran_type(t), forward_id(foid), forward_money(m){}
};
int account_num;
int now_stamp = 1;

vector<ACCOUNT> account_vec;
vector<TRANS> tran_vec;
unordered_map<string, int> ac_to_acint;
vector<bool> locked;
vector<int> alive;

void intput() {
    ifstream file("transactions.txt");
    file >> account_num;

    int tid = 0;

    for (int i = 0; i < account_num; ++i) {

        string account_id;
        int account_balance, tran_num;

        file >> account_id >> account_balance >> tran_num;
        account_vec.push_back(ACCOUNT(account_id, account_balance));
        ac_to_acint[account_id] = i;
        
        for (int j = 0; j < tran_num; ++j) {
            string tran_id, forward_id = "";
            char tran_type;
            int forward_money = 0;

            file >> tran_id >> tran_type;

            if (tran_type == 'T')
                file >> forward_id >> forward_money;
            
            tran_vec.push_back(TRANS(tran_id, tran_type, forward_id, forward_money));
            account_vec[i].trans.push(tid);
            ++tid;
        }
    }

    file.close();
}

void solve() {
    for (int i = 0; i < account_num; ++i) {
        locked.push_back(false);
        if (!account_vec[i].trans.empty())
            alive.push_back(i);
    }

    while (alive.size() > 0) {
        for (int i = 0; i < alive.size(); ++i) {
            int now_sec = alive[i];
            if (locked[now_sec]) continue;
            int tid = account_vec[now_sec].trans.front();

            if (tran_vec[tid].tran_type == 'B') {
                locked[now_sec] = true;
                tran_vec[tid].timestamp = now_stamp;
                account_vec[now_sec].trans.pop();
                if (account_vec[now_sec].trans.empty()) {
                    alive[i] = alive.back();
                    alive.pop_back();
                    --i;
                }
            }
            else { // type == 'T'
                int fo_ac = ac_to_acint[tran_vec[tid].forward_id];
                if (locked[fo_ac]) continue;
                int fo_m = tran_vec[tid].forward_money;
                if (account_vec[now_sec].balance < fo_m) continue;
                locked[now_sec] = true;
                locked[fo_ac] = true;
                tran_vec[tid].timestamp = now_stamp;
                account_vec[now_sec].trans.pop();
                account_vec[now_sec].balance -= fo_m;
                account_vec[fo_ac].balance += fo_m;
                if (account_vec[now_sec].trans.empty()) {
                    alive[i] = alive.back();
                    alive.pop_back();
                    --i;
                }
            }
        }

        for (int i = 0; i < account_num; ++i)
            locked[i] = false;
        ++now_stamp;
    }
}

void write() {
    ofstream file;
    file.open("transaction_log.txt");
    for (int i = 0, j = tran_vec.size(); i < j; ++i)
        file << tran_vec[i].tran_id << " " << tran_vec[i].timestamp << endl;
    file.close();
    
    file.open("final_state.txt");
    for (int i = 0; i < account_num; ++i)
        file << account_vec[i].account_id << " " << account_vec[i].balance << endl;
    file.close();

    cout << "Max Time Stamp: " << now_stamp - 1 << endl;
}

int main () {
    intput();
    solve();
    write();
}
