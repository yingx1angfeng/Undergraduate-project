#include <iostream>
#include <vector>
#include <sstream>
#include <set>
#include <algorithm>
#include <string>
using namespace std;

//对集合的解析
set<string> parseset(const string& s) {
	set<string> result;
	string content = s.substr(1, s.length() - 2);
	stringstream ss(content);
	string element;

	while (getline(ss, element, ',')) {
		result.insert(element);
	}
	return result;
}

//对二元关系解析
set<pair<string, string>> parsereliation(const string& s) {
	set<pair<string, string>> result;
	string content = s.substr(1, s.length() - 2);
	stringstream ss(content);
	string element;

	while (getline(ss, element, '>')) {
		if (element[0] == ',') element = element.substr(1);
		if (element[0] == '<') element = element.substr(1);
		int a = element.find(',');
		string first = element.substr(0, a);
		string second = element.substr(a+1);
		result.insert({ first,second });
	}
	return result;
}

//格式化输出
string formatrelation(const set<pair<string, string>>& r) {
	stringstream ss;
	ss << "{";
	bool a1 = true;
	for (const auto& p : r) {
		if (!a1) {
			ss << ",";
		}
		ss << "<" << p.first << "," << p.second << ">";
		a1 = false;
	}
	ss << "}";
	return ss.str();
}

//自反闭包
set<pair<string, string>> rclosure(const set<string>& s,const set<pair<string, string>>& r) {
	set<pair<string, string>> result = r;
	for (const auto& p : s) {
		result.insert({ p,p });
	}
	return result;
}

//对称闭包
set<pair<string, string>> sclosure(const set<string>& s, const set<pair<string, string>>& r) {
	set<pair<string, string>> result = r;
	for (const auto& p : r) {
		result.insert({ p.second,p.first });
	}
	return result;
}

//传递闭包
set<pair<string, string>> tclosure(const set<string>& s,const set<pair<string, string>>& r) {
	set<pair<string, string>> result = r;
	bool changed = true;
	while (changed) {
		changed = false;
		set<pair<string, string>> newPairs;
		for (const auto& p1 : result) {
			const string& a = p1.first;
			const string& b = p1.second;
			for (const auto& p2 : result) {
				const string& c = p2.first;
				const string& d = p2.second;
				if (b == c && !result.count({ a, d })) {
					newPairs.insert({ a, d });
					changed = true;
				}
			}
		}

		result.insert(newPairs.begin(), newPairs.end());
	}

	return result;
}

int main() {
	string sStr, rStr, cType;
	getline(cin, sStr);
	getline(cin, rStr);
	getline(cin, cType);

	set<string> s = parseset(sStr);
	set<pair<string, string>> r = parsereliation(rStr);

	set<pair<string, string>> result;
	if (cType == "r") {
		result = rclosure(s, r);
	}
	else if (cType == "s") {
		result = sclosure(s,r);
	}
	else if (cType == "t") {
		result = tclosure(s,r);
	}
	cout << formatrelation(result) << endl;
	return 0;
}