#include "account.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

struct deleter {
    template <class T> void operator()(T* p) { delete p; }
};

// 读取并执行历史命令
void loadCommands(vector<Account*>& accounts, Date& date) {
    ifstream cmdFile("commands.txt");
    if (!cmdFile.is_open()) return; // 文件不存在或无法打开则跳过

    string line;
    while (getline(cmdFile, line)) { // 逐行读取历史命令
        istringstream iss(line);
        char cmd;
        iss >> cmd; // 提取命令类型

        switch (cmd) {
        case 'a': { // 添加账户
            char type;
            string id;
            double rate, credit, fee;
            iss >> type >> id;
            if (type == 's') {
                iss >> rate;
                Account* acc = new SavingsAccount(date, id, rate);
                accounts.push_back(acc);
            }
            else {
                iss >> credit >> rate >> fee;
                Account* acc = new CreditAccount(date, id, credit, rate, fee);
                accounts.push_back(acc);
            }
            break;
        }
        case 'd': { // 存款
            size_t index;
            double amount;
            string desc;
            iss >> index >> amount;
            getline(iss, desc); // 读取带空格的描述
            if (index >= 0 && index < accounts.size()) {
                accounts[index]->deposit(date, amount, desc);
            }
            break;
        }
        case 'w': { // 取款
            size_t index;
            double amount;
            string desc;
            iss >> index >> amount;
            getline(iss, desc); // 读取带空格的描述
            if (index >= 0 && index < accounts.size()) {
                accounts[index]->withdraw(date, amount, desc);
            }
            break;
        }
        case 'c': { // 改变日期
            int day;
            iss >> day;
            // 验证日期合法性
            if (day < date.getDay()) {
                cerr << "Error: Cannot specify a previous day" << endl;
                continue;
            }
            else if (day > date.getMaxDay()) {
                cerr << "Error: Invalid day" << endl;
                continue;
            }
            date = Date(date.getYear(), date.getMonth(), day);
            break;
        }
        case 'n': { // 进入下个月
            // 原日期处理逻辑
            if (date.getMonth() == 12) {
                date = Date(date.getYear() + 1, 1, 1);
            }
            else {
                date = Date(date.getYear(), date.getMonth() + 1, 1);
            }
            // 执行结算
            for (auto& acc : accounts) {
                acc->settle(date);
            }
            break;
        }
                // 其他命令（如查询q）无需执行，因为不修改状态
        }
    }
    cmdFile.close();
}

// 保存当前命令到文件（覆盖模式，避免重复）
void saveCommand(const string& cmd) {
    ofstream outFile("commands.txt", ios::trunc); // 覆盖模式，每次保存清空文件
    if (outFile.is_open()) {
        outFile << cmd << endl; // 仅保存最新一条有效命令
        outFile.close();
    }
}

int main() {
    Date date(2008, 11, 1); // 起始日期
    vector<Account*> accounts; // 账户数组

    // 启动时加载历史命令（首次运行时文件为空，后续保存的命令会被覆盖加载）
    loadCommands(accounts, date);

    cout << "(a)add account (d)deposit (w)withdraw (s)show (c)change day (n)next month (q)query (e)exit" << endl;
    char cmd;

    do {
        // 显示当前状态
        date.show();
        cout << "\tTotal: " << Account::getTotal() << "\tcommand> ";

        string cmdLine;
        getline(cin, cmdLine); // 读取完整命令行
        if (cmdLine.empty()) continue; // 跳过空行

        istringstream iss(cmdLine);
        iss >> cmd; // 提取命令类型

        // 保存当前命令到文件（非查询/退出命令时覆盖保存）
        if (cmd != 'q' && cmd != 'e') {
            saveCommand(cmdLine);
        }

        // 命令处理逻辑
        size_t index, day;
        double amount, credit, rate, fee;
        string id, desc;
        Account* account;
        Date date1, date2;

        switch (cmd) {
        case 'a': { // 添加账户
            char type;
            string id;
            iss >> type >> id;
            if (type == 's') {
                iss >> rate;
                account = new SavingsAccount(date, id, rate);
            }
            else {
                iss >> credit >> rate >> fee;
                account = new CreditAccount(date, id, credit, rate, fee);
            }
            accounts.push_back(account);
            break;
        }
        case 'd': { // 存款
            iss >> index >> amount;
            getline(iss, desc); // 处理带空格的描述
            if (index >= 0 && index < accounts.size()) {
                accounts[index]->deposit(date, amount, desc);
            }
            break;
        }
        case 'w': { // 取款
            iss >> index >> amount;
            getline(iss, desc); // 处理带空格的描述
            if (index >= 0 && index < accounts.size()) {
                accounts[index]->withdraw(date, amount, desc);
            }
            break;
        }
        case 's': { // 显示账户信息
            for (size_t i = 0; i < accounts.size(); i++) {
                cout << "[" << i << "] ";
                accounts[i]->show();
                cout << endl;
            }
            break;
        }
        case 'c': { // 改变日期
            iss >> day;
            if (day < date.getDay()) {
                cout << "You cannot specify a previous day" << endl;
            }
            else if (day > date.getMaxDay()) {
                cout << "Invalid day" << endl;
            }
            else {
                date = Date(date.getYear(), date.getMonth(), day);
            }
            break;
        }
        case 'n': { // 进入下个月
            if (date.getMonth() == 12) {
                date = Date(date.getYear() + 1, 1, 1);
            }
            else {
                date = Date(date.getYear(), date.getMonth() + 1, 1);
            }
            for (auto& acc : accounts) {
                acc->settle(date);
            }
            break;
        }
        case 'q': { // 查询账目
            date1 = Date::read();
            date2 = Date::read();
            Account::query(date1, date2);
            break;
        }
        case 'e': { // 退出
            break;
        }
        default: {
            cout << "Unknown command" << endl;
            break;
        }
        }
    } while (cmd != 'e'); // 退出命令为'e'

    // 清理内存
    for_each(accounts.begin(), accounts.end(), deleter());
    return 0;
}