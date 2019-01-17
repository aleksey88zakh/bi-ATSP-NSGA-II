#pragma once

#include <vector>
#include <windows.h>
#include <queue>
#include <set>
#include <map>
#include <list>
#include <string>
#include <direct.h>

using namespace std;
using namespace System;
using namespace System::IO;


//чтение множества Парето из файла
vector<vector<int>> read_Pareto_set_from_file(String^ file_name_source_str, String^ problem_name_str, int num_criteria);
void time_format(unsigned long long result_time, String^ title, StreamWriter^ sw);
vector<int> shaking_Kopt(vector<int> s, int k);
//unsigned hiper_volume(vector<int> r, vector<vector<int>> f);

////////////////////////////////////////////////////////////////////////////////
//Отношение Парето
////////////////////////////////////////////////////////////////////////////////
template <typename T>
bool Pareto_pref(const vector<T> a, const vector<T> b)
{
	if (a.size() != b.size())
		return false;

	int counter_greater = 0;
	for (int i = 0; i < a.size(); i++)
		if (a[i] <= b[i]) counter_greater++;

	if ((counter_greater == a.size()) && (a != b))
		return true;
	else
		return false;
}




