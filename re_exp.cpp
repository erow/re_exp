// re_exp.cpp : 定义控制台应用程序的入口点。
//
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <fstream>
#include <map>
#include <tuple>
using namespace std;

typedef int  stateT;
typedef int  classifyT;

vector<int> state_type{0};
map<stateT, set<stateT> > e_transform_table;
map<stateT, map<classifyT, stateT>> transform_table;

map<stateT,classifyT> classify;

int gen_unique_state() {
	static int s= 1;
	state_type.push_back(1);
	return s++;
}
//生成NFA
tuple<int,int> gen_table(int start_state, string input) {
	int pre_state = start_state;

	for (int i = 0; i < input.size();i++) {
		char c = input[i];
		if (c == '*') {
			c = input[++i];
			int Si = gen_unique_state(), Sj = gen_unique_state(), Sq = gen_unique_state();
			e_transform_table[pre_state].insert(Si);
			e_transform_table[Sj].insert(Sq);
			e_transform_table[Sj].insert(Si);
			e_transform_table[pre_state].insert(Sq);
			transform_table[Si][c] = Sj;
			pre_state = Sq;
		}
		else if (c == '|') {
			int Sm = gen_unique_state(), Sn = gen_unique_state(), Sk = gen_unique_state();
			auto t = gen_table(Sk, input.substr(i+1));
			e_transform_table[Sm].insert(start_state);
			e_transform_table[Sm].insert(std::get<0>(t));
			e_transform_table[pre_state].insert(Sn);
			e_transform_table[std::get<1>(t)].insert(Sn);
			return make_tuple(Sm, Sn);
		}
		else {
			int NewS = gen_unique_state();
			transform_table[pre_state][c] = NewS;
			classify[c] = c;
			pre_state = NewS;
		}
	}
	return make_tuple(start_state,pre_state);
}

void PrintState() {
	vector<string> res;
	for (int i = 0; i < state_type.size(); i++) {
		for (auto t : transform_table[i]) {
			char str[100];
			sprintf(str, "%d-%c>%d", i, t.first, t.second);
			res.push_back(str);
		}
		for (auto t : e_transform_table[i]) {
			char str[100];
			sprintf(str, "%d-@>%d", i, t);
			res.push_back(str);
		}
	}
	sort(res.begin(), res.end());

	for (auto s : res) {
		cout << s << endl;
	}
}

set<stateT> get_closure(stateT s) {
	set<stateT> states;
	states.insert(s);
	vector<stateT> work;
	work.push_back(s);
	while (!work.empty())
	{
		s = *(work.end() - 1);
		work.pop_back();
		for (auto i : e_transform_table[s])
			states.insert(i);
	}
	return states;
}

map<stateT, set<stateT>> closure_set;
map<stateT, map<classifyT,stateT>> transform_map;
map<char, set<stateT>> get_transform(stateT s) {
	map<char, set<stateT>> transfom;
	for (auto s : closure_set[s]) {
		for (auto t : transform_table[s]) {
			for(auto S: get_closure(t.second))
				transfom[t.first].insert(S);
		}
	}
	return transfom;
}
void PrintState1() {
	for(auto s :transform_map)
	for (auto t : s.second) {
		char str[100];
		sprintf(str, "%d-%c>%d", s.first, t.first, t.second);
		cout << str << endl;
	}
}


bool re_match(string str, stateT s) {
	
	for (auto c : str) {
		if (transform_map[s][c])
			s = transform_map[s][c];
		else
			return false;
	}
	for (auto t : closure_set[s])
	{
		if (state_type[t] == -1)
			return true;
	}
	return false;
}
int main()
{
	string str;
	cin >> str;
	
		auto result = gen_table(0, str);
		state_type[get<1>(result)] = -1;
		stateT s0 = get<0>(result);
		PrintState();
		//化为dfa 未化简
		auto NewS0 = gen_unique_state();
		closure_set[NewS0] = get_closure(s0);
		vector<stateT> worklist;
		set<set<stateT>> added;
		added.insert(closure_set[NewS0]);
		worklist.push_back(NewS0);
		while (!worklist.empty()) {
			stateT s = *(worklist.end() - 1);
			worklist.pop_back();
			for (auto t : get_transform(s)) {
				if (added.find(t.second) == added.end()) {
					auto NewS = gen_unique_state();
					added.insert(t.second);
					worklist.push_back(NewS);
					closure_set[NewS] = t.second;
					transform_map[s][t.first] = NewS;
				}
				else
				{
					for(auto p:closure_set)
						if (p.second == t.second)
						{
							transform_map[s][t.first] = p.first;
							break;
						}
				}
			}
		}
		cout << "-------" << endl;
		PrintState1();
		cout << "-------" << endl;
	
	while (cin >> str)
	{
		if (re_match(str, NewS0))
			cout << "match" << endl;
		else
			cout << "not match" << endl;
	}
    return 0;
}

