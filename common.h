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

void time_format(unsigned long long result_time, String^ title, StreamWriter^ sw);

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




