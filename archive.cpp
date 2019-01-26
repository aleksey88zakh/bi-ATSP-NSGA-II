//#include "stdafx.h"
#include "archive.h"
#include "common.h"

Archive::Archive()
{
}

Archive::Archive(GA_path* p_ga)
{
	//? использовать переменную i_start_last_front_exceeded
	// переделать с учетом:
	//	1) заполнения ga.phi;
	//	2) если все особи ранга 1
	archive.push_back(p_ga->pop[0]);
	val_crit_archive.push_back(p_ga->multi_phitness(p_ga->pop[0]));
	ar_index_not_cons_lst.push_back(0);

	for (int i = 1; i < p_ga->get_N(); i++)
	{
		if (p_ga->i_rank[i] == 2)
			break;
		for (int j = 0; j < archive.size(); j++)
		{
			if (p_ga->multi_phitness(p_ga->pop[i]) == val_crit_archive[j])
				break;
			else
				if (j == archive.size() - 1)
				{
					ar_index_not_cons_lst.push_back(archive.size()); // индексация 0, 1, 2, ... (определяется не i)
					archive.push_back(p_ga->pop[i]);
					val_crit_archive.push_back(p_ga->multi_phitness(p_ga->pop[i]));
				}
		}
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

int Archive::check_new(vector<int> val_crit_new)
{
	list<unsigned>::const_iterator it_lst;

	// пробегаемся по индексам элементов архива, кот. не просмотрены
	for (it_lst = ar_index_not_cons_lst.cbegin(); it_lst != ar_index_not_cons_lst.cend(); )
	{
		//новая особь доминирует особь в позиции *it_lst
		if (Pareto_pref(val_crit_new, val_crit_archive[*it_lst]))
		{
			ar_index_no_lst.push_back(*it_lst);
			it_lst = ar_index_not_cons_lst.erase(it_lst); // уже указывает на следующий элемент
			continue; 
		}
		else
			if ( Pareto_pref(val_crit_archive[*it_lst], val_crit_new) || val_crit_archive[*it_lst] == val_crit_new )
				return 0;
		it_lst++;
	}


	// пробегаемся по индексам элементов архива, кот. просмотрены
	for (it_lst = ar_index_cons_lst.cbegin(); it_lst != ar_index_cons_lst.cend(); )
	{
		//новая особь доминирует особь в позиции *it_lst
		if (Pareto_pref(val_crit_new, val_crit_archive[*it_lst]))
		{
			ar_index_no_lst.push_back(*it_lst);
			it_lst = ar_index_cons_lst.erase(it_lst); // уже указывает на следующий элемент
			continue; 
		}
		else
			if ( Pareto_pref(val_crit_archive[*it_lst], val_crit_new) || val_crit_archive[*it_lst] == val_crit_new )
				return 0;
		it_lst++;
	}
	
	return 1;
}

void Archive::arch_modify(vector<int> pop_new, vector<int> val_crit_new)
{
	// новая особь не была продоминирована
	// добавляем ее в архив
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
		ar_index_not_cons_lst.push_back(archive.size()); // с учетом того, что новую особь в архив не добавили

		//добавляем новую особь в популяцию (архив)
		archive.push_back(pop_new);
		val_crit_archive.push_back(val_crit_new);
	}
}


//обновление архива на основе элементов другого архива (объединение архивов)
// this = (this U new_arch)
// возвращает true, если текущий архив продоминирован добавляемым
bool Archive::elite_modify(Archive arch_to_add)
{
	list<unsigned>::const_iterator it_lst;
	//флаг доминирования текущего архива добавляемым архивом
	bool this_arch_is_improved = false;

	// пробегаемся по индексам элементов добавляемого архива
	// если архив построен, то все особи в нем просмотрены
	for (it_lst = arch_to_add.ar_index_cons_lst.cbegin(); it_lst != arch_to_add.ar_index_cons_lst.cend(); it_lst++)
	{
		if (this->check_new(arch_to_add.val_crit_archive[*it_lst]))
		{
			//добавляем новую особь в элиту
			this->ar_index_not_cons_lst.push_back(this->archive.size());
			this->archive.push_back(arch_to_add.archive[*it_lst]);
			this->val_crit_archive.push_back(arch_to_add.val_crit_archive[*it_lst]);
			this_arch_is_improved = true;
		}
	}

	return this_arch_is_improved;
}

//обновление архива на основе элементов другого архива (объединение архивов)
// this = (this U new_arch)
// возвращает true, если текущий архив продоминирован добавляемым
//bool Archive::elite_modify(Archive arch_to_add)
//{
//	list<unsigned>::const_iterator it_lst1;
//	list<unsigned>::const_iterator it_lst2;
//	//фиксируем конец элиты
//	list<unsigned>::const_iterator it_lst2_end = this->ar_index_not_cons_lst.cend();
//	//флаг доминирования текущего архива добавляемым архивом
//	bool this_arch_is_improved = false;
//	// текущий элемент архива arch_to_add уже добавлен
//	bool s_is_added = false;
//
//	// пробегаемся по индексам элементов добавляемого архива
//	// если архив построен, то все особи в нем просмотрены
//	for (it_lst1 = arch_to_add.ar_index_cons_lst.cbegin(); it_lst1 != arch_to_add.ar_index_cons_lst.cend(); it_lst1++)
//	{
//		s_is_added = false;
//		for (it_lst2 = this->ar_index_not_cons_lst.cbegin(); it_lst2 != it_lst2_end; )
//		{
//			//если текущий элемент архива arch_to_add уже добавлен
//			if (s_is_added)
//			{
//				if (Pareto_pref(arch_to_add.val_crit_archive[*it_lst1], this->val_crit_archive[*it_lst2]))
//				{
////					this->ar_index_no_lst.push_back(*it_lst2);
//					it_lst2 = this->ar_index_not_cons_lst.erase(it_lst2);
//				}
//				else
//					it_lst2++;
//			}
//			else
//			{
//				if (Pareto_pref(arch_to_add.val_crit_archive[*it_lst1], this->val_crit_archive[*it_lst2]))
//				{
//					//добавляем новую особь в элиту
//					this->ar_index_not_cons_lst.push_back(archive.size());
//					this->archive.push_back(arch_to_add.archive[*it_lst1]);
//					this->val_crit_archive.push_back(arch_to_add.val_crit_archive[*it_lst1]);
//
//					this_arch_is_improved = true;
//					s_is_added = true;
////					this->ar_index_no_lst.push_back(*it_lst2);
//					it_lst2 = this->ar_index_not_cons_lst.erase(it_lst2);
//				}
//				else
//				{
//					if (Pareto_pref(this->val_crit_archive[*it_lst2], arch_to_add.val_crit_archive[*it_lst1]) || arch_to_add.val_crit_archive[*it_lst1] == this->val_crit_archive[*it_lst2])
//						break;
//					else
//					{
//						//добавляем новую особь в элиту
//						this->ar_index_not_cons_lst.push_back(archive.size());
//						this->archive.push_back(arch_to_add.archive[*it_lst1]);
//						this->val_crit_archive.push_back(arch_to_add.val_crit_archive[*it_lst1]);
//						
//						this_arch_is_improved = true;
//						s_is_added = true;
//						it_lst2++;
//					}
//				}
//			}	
//		} // элита
//	} // текущий архив
//
//	return this_arch_is_improved;
//}

//bool Archive::elite_modify(Archive arch_to_add)
//{
//	list<unsigned>::const_iterator it_lst1;
//	list<unsigned>::const_iterator it_lst2;
//	//фиксируем конец элиты
//	list<unsigned>::const_iterator it_lst2_end = this->ar_index_not_cons_lst.cend();
//	//флаг доминирования текущего архива добавляемым архивом
//	bool this_arch_is_improved = false;
//	// текущий элемент архива arch_to_add уже добавлен
//	bool s_is_added = false;
//
//	// пробегаемся по индексам элементов добавляемого архива
//	// если архив построен, то все особи в нем просмотрены
//	for (it_lst1 = arch_to_add.ar_index_cons_lst.cbegin(); it_lst1 != arch_to_add.ar_index_cons_lst.cend(); it_lst1++)
//	{
//		s_is_added = false;
//		for (it_lst2 = this->ar_index_not_cons_lst.cbegin(); it_lst2 != this->ar_index_not_cons_lst.cend(); )
//		{
//			if (Pareto_pref(arch_to_add.val_crit_archive[*it_lst1], this->val_crit_archive[*it_lst2]))
//			{
//				//если текущий элемент архива arch_to_add еще не добавлен
//				if (!s_is_added)
//				{
//					this->arch_modify(arch_to_add.archive[*it_lst1], arch_to_add.val_crit_archive[*it_lst1]);
//					this_arch_is_improved = true;
//					s_is_added = true;
//					this->ar_index_no_lst.push_back(*it_lst2);
//					it_lst2 = this->ar_index_not_cons_lst.erase(it_lst2);
//					
//				}
//				else // только удаляем доминируемый элемент из элиты
//				{
//					this->ar_index_no_lst.push_back(*it_lst2);
//					it_lst2 = this->ar_index_not_cons_lst.erase(it_lst2);
//				}
//
//
//			}
//			else
//				if (!s_is_added) // нужно идти только если текущий элемент архива arch_to_add еще не добавили
//				{
//					if (Pareto_pref(this->val_crit_archive[*it_lst2], arch_to_add.val_crit_archive[*it_lst1]) || arch_to_add.val_crit_archive[*it_lst1] == this->val_crit_archive[*it_lst2])
//						break;
//					else
//					{
//						this->arch_modify(arch_to_add.archive[*it_lst1], arch_to_add.val_crit_archive[*it_lst1]);
//						this_arch_is_improved = true;
//						s_is_added = true;
//						it_lst2++;
//					}
//				}
//				else // переходим к следующему элементу элиты
//					it_lst2++;
//		}
//	}
//
//	return this_arch_is_improved;
//}