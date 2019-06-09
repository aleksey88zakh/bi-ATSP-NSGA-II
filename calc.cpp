//
// Файл содержит:
// рассчет значений критерия
// рассчет аппрокисмаций
// рассчет метрик
// построение мн-ва Парето перебором
// 
//#include "stdafx.h"
#include "GA_for_MTSP_source.h"

////////////////////////////////////////////////////////////////////////////////    
//Вычисление значения особи по некоторому критерию
////////////////////////////////////////////////////////////////////////////////
int GA_path::phitness(vector<vector<int>> s, vector<int> p)
{
	int phi_c = 0;
	for (int i = 0; i < size(p) - 1; ++i)
		phi_c = phi_c + s[p[i] - 1][p[i + 1] - 1];

	phi_c = phi_c + s[p[size(p) - 1] - 1][p[0] - 1];
	return phi_c;
}


///////////////////////////////////////////////////////////////////////////////
//Значение векторного критерия для особи
///////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::multi_phitness(vector<int> p)
{
	vector<int> temp;
	for (int i = 0; i<s_m.size(); i++)
		temp.push_back(phitness(s_m[i], p));

	return temp;
}

//заполнение значений критерия
void  GA_path::calc_phi()
{
	for (int i = 0; i <this->get_N(); i++)
		this->phi[i] = this->multi_phitness(this->pop[i]);
}

//заполнение значений критерия расширенной популяции
void  GA_path::calc_ext_phi()
{
	for (int i = 0; i < this->get_ext_N(); i++)
		this->phi[i] = this->multi_phitness(this->pop_R_t[i]);
}


////////////////////////////////////////////////////////////////////////////////
//Построение аппроксимации мн-ва Парето (значения векторного критерия, без повторов)
////////////////////////////////////////////////////////////////////////////////
void GA_path::build_phi_P_approx()
{
	this->phi_P_approx.push_back(this->multi_phitness(this->pop[0]));
	for (int i = 1; i < this->pop.size(); i++)
	{
		if (i_rank[i] == 1)
		{
			for (int k = 0; k < this->phi_P_approx.size(); k++)
			{
				if (this->multi_phitness(this->pop[i]) == this->phi_P_approx[k])
					break;
				else
					if (k == this->phi_P_approx.size() - 1)
						this->phi_P_approx.push_back(this->multi_phitness(this->pop[i]));
			}
		}
		else
			break;
	}
}


////////////////////////////////////////////////////////////////////////////////
//Расстояние от особи (из аппроксимации) до ближайшего вектора в мн-ве Парето
////////////////////////////////////////////////////////////////////////////////
double GA_path::dist_p_to_nearest_set(vector<int> phi_p, vector<vector<int>> phi_set)
{
	double dist_temp = 0;
	double dist_min = 0;

	for (int j = 0; j < this->get_m(); j++)
		dist_temp += (phi_set[0][j] - phi_p[j])*(phi_set[0][j] - phi_p[j]);
	dist_min = sqrt(dist_temp);

	for (int i = 1; i < phi_set.size(); i++)
	{
		dist_temp = 0;
		for (int j = 0; j < this->get_m(); j++)
		{
			dist_temp += (phi_set[i][j] - phi_p[j])*(phi_set[i][j] - phi_p[j]);
		}
		dist_temp = sqrt(dist_temp);
		if (dist_min > dist_temp)
			dist_min = dist_temp;
	}

	return dist_min;
}

////////////////////////////////////////////////////////////////////////////////
//Вычисление метрики - сходимость первого мн-ва ко второму мн-ву
////////////////////////////////////////////////////////////////////////////////
double GA_path::dist_convergence(vector<vector<int>> first_set, vector<vector<int>> second_set, bool flag_count)
{
	double dist_conver = 0;
	double dist_temp = 0;
	for (int i = 0; i < first_set.size(); i++)
	{
		dist_temp = this->dist_p_to_nearest_set(first_set[i], second_set)*this->dist_p_to_nearest_set(first_set[i], second_set);
		dist_conver += dist_temp;

		if ((flag_count) && (0 == dist_temp))
			this->count_P_eq_approx = this->count_P_eq_approx + 1;
	}

	dist_conver = sqrt(dist_conver) / first_set.size();

	return dist_conver;
}

////////////////////////////////////////////////////////////////////////////////
//Cреднеквадратическое отклонение метрики "сходимость аппроксимации к мн-ву Парето"
////////////////////////////////////////////////////////////////////////////////
double GA_path::standard_devation_conver_approx_to_P_set(vector<vector<int>> P_set, double dist_conver)
{
	double stand_dev = 0;
	for (int i = 0; i < this->phi_P_approx.size(); i++)
		stand_dev += (this->dist_p_to_nearest_set(this->phi_P_approx[i], P_set) - dist_conver)*(this->dist_p_to_nearest_set(this->phi_P_approx[i], P_set) - dist_conver);
	stand_dev = sqrt(stand_dev / this->phi_P_approx.size());

	return stand_dev;
}

////////////////////////////////////////////////////////////////////////////////
//Вычисление метрики - аппроксимации множества Парето
////////////////////////////////////////////////////////////////////////////////
void GA_path::evaluate_metric_of_approx(StreamWriter^ sw, vector<vector<int>> phi_Pareto_set, bool flag_count)
{
	//метрика - сходимость аппрокимации к мн-ву Парето
	this->dist_conver_approx_to_P_set_val = this->dist_convergence(this->phi_P_approx, phi_Pareto_set, flag_count);


	printf("Metric 'Convergence appoximation to Pareto set':\n");
	//вывод первой метрики
	sw->Write("{0:F4};", dist_conver_approx_to_P_set_val);
	//sw_short->WriteLine("{0:F4}", dist_conver_approx_to_P_set_val);
	printf("%.4f\n", dist_conver_approx_to_P_set_val);

	//метрика - сходимость мн-ва Парето к его аппрокимации
	this->dist_conver_P_set_to_approx_val = this->dist_convergence(phi_Pareto_set, this->phi_P_approx, false);


	printf("Metric 'Convergence Pareto set to appoximation':\n");
	//вывод второй метрики и кол-ва точек в мн-ве Парето
	sw->Write("{0:F4};{1};", dist_conver_P_set_to_approx_val, this->phi_P_approx.size());
	//sw_short->WriteLine("{0:F4}", dist_conver_P_set_to_approx_val);
	printf("%.4f\n", dist_conver_P_set_to_approx_val);

	//sw->WriteLine();
	//sw_short->WriteLine();
}

////////////////////////////////////////////////////////////////////////////////
//построение мн-ва Парето перебором
////////////////////////////////////////////////////////////////////////////////
vector<vector<int>> GA_path::build_phi_P_enum(vector<vector<double>> init_set, vector<vector<int>> phi_P_approx)
{
	vector<vector<int>> res_set;

	bool* flag_s = new bool[init_set.size()];
	for (int i = 0; i < init_set.size(); i++)
		flag_s[i] = false;

	for (int i = 0; i < init_set.size() - 1; i++)
	{
		if (flag_s[i])
			continue;

		for (int j = i + 1; j < init_set.size(); j++)
		{
			if (flag_s[j])
				continue;

			if (Pareto_pref(init_set[i], init_set[j]))
				flag_s[j] = true;
			if (Pareto_pref(init_set[j], init_set[i]))
			{
				flag_s[i] = true;
				break;
			}
		}

	}

	for (int i = 0; i < phi_P_approx.size(); i++)
	{
		if (!flag_s[i])
			res_set.push_back(phi_P_approx[i]);
	}

	return res_set;
}

//гиперобъем
unsigned GA_path::hyper_volume(vector<int> r, vector<vector<int>> f)
{
	unsigned vol_res = 0;

	//предсортировка по 1-му критерию
	//? учесть равенство по 1-му критерию
	//вектор фронтов (используется индекс особи в популяции pop)
	vector<int> index_front_temp;
	for (int i = 0; i < f.size(); i++)
		index_front_temp.push_back(i);
	heap_sort(f, index_front_temp, -1, 0, index_front_temp.size() - 1);

	for (int i = 0; i < index_front_temp.size(); i++)
	{
		unsigned vol_tmp = 1;
		for (int j = 0; j < f[0].size(); j++)
			vol_tmp *= (r[j] - f[index_front_temp[i]][j]);
		vol_res += vol_tmp;
		//передвигаем точку r по первой координате
		r[1] = f[index_front_temp[i]][1];

	}

	return vol_res;
}

//C-metric
float С_metric(vector<vector<int>> P_set_1, vector<vector<int>> P_set_2)
{
	float cnt_improved = 0;

	// пробегаемся по элементам 1-го множества
	for (int i = 0; i < P_set_2.size(); i++)
	{
		// пробегаемся по индексам элементов 2-го множества
		for (int j = 0; j < P_set_1.size(); j++)
		{
			if (Pareto_pref(P_set_1[j], P_set_2[i]) || P_set_1[j] == P_set_2[i])
			{
				cnt_improved++;
				break;
			}
			/*else
				if (Pareto_pref(P_set_2[j], P_set_1[i]))
					return 0;*/
		}
	}
		
		
	return cnt_improved/ P_set_2.size();
}