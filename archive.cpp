#include "archive.h"
#include "common.h"

Archive::Archive()
{
}

Archive::Archive(GA_path ga)
{
	//? ������������ ���������� i_start_last_front_exceeded
	// ���������� � ������:
	//	1) ���������� ga.phi;
	//	2) ���� ��� ����� ����� 1
	for (int i = 0; i < ga.get_N(); i++)
	{
		if (ga.i_rank[i] == 2)
			break;
		archive.push_back(ga.pop[i]);
		val_crit_archive.push_back(ga.multi_phitness(ga.pop[i]));
		ar_index_not_cons_lst.push_back(0);
	}

}

Archive::~Archive()
{
	archive.clear();
	ar_index_cons_lst.clear();
	ar_index_not_cons_lst.clear();
	ar_index_no_lst.clear();
	val_crit_archive.clear();
}

int Archive::check_new(vector<int> pop_new, vector<int> val_crit_new)
{
	list<unsigned>::const_iterator it_lst;

	// ����������� �� �������� ��������� ������, ���. �� �����������
	for (it_lst = ar_index_not_cons_lst.cbegin(); it_lst != ar_index_not_cons_lst.cend(); ++it_lst)
	{
		//����� ����� ���������� ����� � ������� *it_lst
		if (Pareto_pref(val_crit_new, val_crit_archive[*it_lst]))
		{
			ar_index_no_lst.push_back(*it_lst);
			it_lst = ar_index_not_cons_lst.erase(it_lst); // ���������� it_lst?
													  //continue; 
		}

		if (Pareto_pref(val_crit_archive[*it_lst], val_crit_new))
			return 0;
	}


	// ����������� �� �������� ��������� ������, ���. �����������
	for (it_lst = ar_index_cons_lst.cbegin(); it_lst != ar_index_cons_lst.cend(); ++it_lst)
	{
		//����� ����� ���������� ����� � ������� *it_lst
		if (Pareto_pref(val_crit_new, val_crit_archive[*it_lst]))
		{
			ar_index_no_lst.push_back(*it_lst);
			it_lst = ar_index_cons_lst.erase(it_lst); // ���������� it_lst?
													  //continue; 
		}

		if (Pareto_pref(val_crit_archive[*it_lst], val_crit_new))
			return 0;
	}


	// ����� ����� �� ���� ���������������
	// ��������� �� � �����
	if (!ar_index_no_lst.empty())
	{
		int i_tmp = ar_index_no_lst.front();
		ar_index_not_cons_lst.push_back(i_tmp);
		archive[i_tmp] = pop_new;
		val_crit_archive[i_tmp] = val_crit_new;
		ar_index_no_lst.pop_front();
	}
	else
	{
		if (!ar_index_not_cons_lst.empty())
			ar_index_not_cons_lst.push_back(archive.size()); // � ������ ����, ��� ����� ����� � ����� �� ��������
		else
			ar_index_not_cons_lst.push_back(0); // ������ � 0

														   //��������� ����� ����� � ��������� (�����)
		archive.push_back(pop_new);
		val_crit_archive.push_back(val_crit_new);
	}
		

	return 1;
}