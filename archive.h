#pragma once

#include <vector>
#include <windows.h>
#include <queue>
#include <set>
#include <map>
#include <list>
#include <string>
#include <direct.h>
#include "GA_for_MTSP_source.h"

using namespace std;
using namespace System;
using namespace System::IO;


class Archive
{
public:
	Archive();
	Archive(GA_path ga); // на основе данных генетического алгоритма
	~Archive();

	vector<vector<int>> archive;
	vector<vector<int>> val_crit_archive;
	list<unsigned> ar_index_cons_lst;
	list<unsigned> ar_index_not_cons_lst;
	list<unsigned> ar_index_no_lst;

	int check_new(vector<int> pop_new, vector<int> val_crit_new);
};



inline unsigned get_status_lst(unsigned x)
{
	return x >> 31;
};

inline void set_status_lst(unsigned *x, unsigned short st)
{
	unsigned temp = (unsigned)st << 31;
	*x = (((*x) << 1) >> 1) | temp; //сдвиг для надежности
};

inline unsigned get_index_lst(unsigned x)
{
	return x & 0x7FFFFFFF;
};

inline unsigned set_index_lst(unsigned index, unsigned short st = 0)
{
	if (st)
		return index | 0x80000000;
	return  ((index << 1) >> 1);
};