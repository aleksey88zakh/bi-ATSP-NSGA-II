//
// Файл содержит функции по самому эксперименту
//

#include "GA_for_MTSP_source.h"

////////////////////////////////////////////////////////////////////////////////
//эксперимент по сужению мн-ва Парето
////////////////////////////////////////////////////////////////////////////////
vector<double> GA_path::experiment_reduction(StreamWriter^ sw, String^ problem_name_str,
	double h, int num_steps, double teta_start, unsigned quantum_inf)
{
	//коэффициенты относительной важности
	double teta_1 = teta_start;
	double teta_2 = teta_start; //используется, если задано два кванта
								//число строк в "таблице"
								//1 - один "квант информации"
								//num_steps - два "кванта информации"
	int num_rows = 1;

	vector<vector<double>> new_phi_P_approx;
	vector<vector<int>> phi_P_approx_reduced;
	vector<double> index_reduced_temp;

	/*
	if (0 == iter_prbl)
	{
	sw_short->Write(";Points in approx of Pareto set;");
	for (int i = 1; i <= num_steps; i++)
	sw_short->Write("{0:F1};", h*i);
	sw_short->WriteLine();
	}
	*/

	//sw_short->Write(problem_name_str + ";");
	//sw_short->Write("{0};", this->phi_P_approx.size());

	//вывод сужения множества Парето
	//если num_rows = num_steps (два "кванта информации"), то каждый вывод для пары (teta_1, teta_2)
	if (quantum_inf == (_1ST_2ND_ + _2ND_1ST_))
		num_rows = num_steps;

	//если задан один "квант информации", то внешний цикл выполнятеся только один раз
	for (int j = 1; j <= num_rows; j++)
	{
		teta_1 = teta_start;
		teta_2 += h;
		for (int i = 1; i <= num_steps; i++)
		{
			teta_1 += h;

			//проверка на непротиворечивость информации
			if ((((double)((int)(teta_1 * 10 + 0.5)) / 10) + ((double)((int)(teta_2 * 10 + 0.5)) / 10) >= 1) && (quantum_inf == _1ST_2ND_ + _2ND_1ST_))
				break;

			//пересчитываем векторы в новом вектороном критерии (значения вещественные, т.к. teta вещественные)
			new_phi_P_approx = this->build_new_phi_P_approx(this->phi_P_approx, teta_1, teta_2, quantum_inf);
			//находим сужение мн-ва Парето в исходном критеритериальном пр-ве (относительно 2-го аргумента)
			phi_P_approx_reduced = this->build_phi_P_enum(new_phi_P_approx, this->phi_P_approx);

			if (quantum_inf == (_1ST_2ND_ + _2ND_1ST_))
				sw->WriteLine("teta_12; {0:F1}; teta_21; {0:F1}", teta_1, teta_2);
			else
				sw->WriteLine("teta; {0:F1}", teta_1);


			for (int i = 0; i < this->get_m(); i++)
			{
				for (int j = 0; j < phi_P_approx_reduced.size(); j++)
				{
					sw->Write("{0:F3};", phi_P_approx_reduced[j][i]);
				}
				sw->WriteLine();
			}


			sw->WriteLine("Number of vectors in the reduced Pareto set");
			sw->WriteLine("{0}", phi_P_approx_reduced.size());
			sw->WriteLine();

			//выводим процент отброшенных точек
			index_reduced_temp.push_back(((double)(this->phi_P_approx.size() - phi_P_approx_reduced.size()) / this->phi_P_approx.size()) * 100);
			//sw_short->Write("{0:F2};", reduced_index);

			//aver_reduced_index[i - 1] += reduced_index;

			//if (min_reduced_index[i - 1] > reduced_index)
			//	min_reduced_index[i - 1] = reduced_index;
			//if (max_reduced_index[i - 1] < reduced_index)
			//	max_reduced_index[i - 1] = reduced_index;
		}
	}

	return index_reduced_temp;
}


////////////////////////////////////////////////////////////////////////////////
//Аппрокимации мн-ва Парето (без повтроов) относительного "нового" критерия
////////////////////////////////////////////////////////////////////////////////
vector<vector<double>> GA_path::build_new_phi_P_approx(vector<vector<int>> init_phi_P_approx, double teta_1, double teta_2, unsigned quantum_inf)
{
	vector<vector<double>> new_phi_temp(init_phi_P_approx.size(), vector<double>(this->get_m(), 0));

	for (int i = 0; i < init_phi_P_approx.size(); i++)
	{
		switch (quantum_inf)
		{
			//1-ый критерий важнее 2-го с коффициентом teta_1
		case _1ST_2ND_:
			new_phi_temp[i][0] = init_phi_P_approx[i][0];
			new_phi_temp[i][1] = teta_1 * init_phi_P_approx[i][0] + (1 - teta_1)*init_phi_P_approx[i][1];
			break;
			//2-ой критерий важнее 1-го с коэффициентом teta_1
		case _2ND_1ST_:
			new_phi_temp[i][0] = (1 - teta_1)*init_phi_P_approx[i][0] + teta_1 * init_phi_P_approx[i][1];
			new_phi_temp[i][1] = init_phi_P_approx[i][1];
			break;
			//1-ый критерий важнее 2-го с коэффициентом teta_1 + 2-ой критерий важнее 1-го с коэффициентом teta_2
		case _1ST_2ND_ + _2ND_1ST_:
			new_phi_temp[i][0] = (1 - teta_2)*init_phi_P_approx[i][0] + teta_2 * init_phi_P_approx[i][1];
			new_phi_temp[i][1] = teta_1 * init_phi_P_approx[i][0] + (1 - teta_1)*init_phi_P_approx[i][1];
		}

	}
	return new_phi_temp;
}




////////////////////////////////////////////////////////////////////////////////
//Вывод результатов эксперимента по сужению множества Парето
////////////////////////////////////////////////////////////////////////////////
void print_exp_red(StreamWriter^ sw_4, String^ quantum_inf_name_str, vector<vector<double>> index_reduced, vector<int> total_num_P_approx)
{
	//ТАБЛИЦА
	//заголовок
	sw_4->WriteLine(quantum_inf_name_str);
	sw_4->Write(";Points in approx of Pareto set;");
	for (int j = 1; j <= index_reduced[0].size(); j++)
		sw_4->Write("{0:F1};", 0.1*j);
	sw_4->WriteLine();

	//процент "отброшенных" точек
	for (int i = 0; i < index_reduced.size(); i++)
	{
		//sw_4->Write(vec_problem_name_str[i]);
		sw_4->Write(";{0};", total_num_P_approx[i]);
		for (int j = 0; j < index_reduced[0].size(); j++)
			sw_4->Write("{0:F2};", index_reduced[i][j]);
		sw_4->WriteLine();
	}

	//статистика
	//среднее
	sw_4->Write("Aver;");
	double aver_num = 0;
	int max_num = total_num_P_approx[0];
	int min_num = total_num_P_approx[0];
	for (int i = 0; i < total_num_P_approx.size(); i++)
	{
		aver_num += total_num_P_approx[i];
		if (min_num > total_num_P_approx[i])
			min_num = total_num_P_approx[i];
		if (max_num < total_num_P_approx[i])
			max_num = total_num_P_approx[i];
	}
	aver_num = aver_num / total_num_P_approx.size();
	sw_4->Write("{0:F2};", aver_num);

	double aver = 0;
	for (int j = 0; j < index_reduced[0].size(); j++)
	{
		aver = 0;
		for (int i = 0; i < index_reduced.size(); i++)
			aver += index_reduced[i][j];
		aver = aver / index_reduced.size();
		sw_4->Write("{0:F2};", aver);
	}
	sw_4->WriteLine();

	//минимум
	sw_4->Write("Min;");
	sw_4->Write("{0};", min_num);

	double min;
	for (int j = 0; j < index_reduced[0].size(); j++)
	{
		min = index_reduced[0][j];
		for (int i = 0; i < index_reduced.size(); i++)
		{
			if (min > index_reduced[i][j])
				min = index_reduced[i][j];
		}
		sw_4->Write("{0:F2};", min);
	}
	sw_4->WriteLine();

	//максимум
	sw_4->Write("Max;");
	sw_4->Write("{0};", max_num);

	double max;
	for (int j = 0; j < index_reduced[0].size(); j++)
	{
		max = index_reduced[0][j];
		for (int i = 0; i < index_reduced.size(); i++)
		{
			if (max < index_reduced[i][j])
				max = index_reduced[i][j];
		}
		sw_4->Write("{0:F2};", max);
	}
	sw_4->WriteLine();
	sw_4->WriteLine();

}

////////////////////////////////////////////////////////////////////////////////
//Вывод результатов эксперимента по сужению множества Парето (два "кванта информации")
////////////////////////////////////////////////////////////////////////////////
void print_exp_red_two(StreamWriter^ sw_4, String^ problem_name_str, vector<double> index_reduced, int num_steps, double total_num_P_approx)
{
	//ТАБЛИЦА
	//заголовок
	sw_4->WriteLine(problem_name_str);
	sw_4->WriteLine(";Points in approx of Pareto set; {0}", total_num_P_approx);
	sw_4->WriteLine("2nd-1st; 1st-2nd");
	sw_4->Write(";");
	for (int i = 0; i < num_steps; i++)
		sw_4->Write("{0:F1};", i*0.1);
	sw_4->WriteLine();


	//индекс контейнера процент "отброшенных" точек (index_reduced)
	int k = 0;
	//процент "отброшенных" точек
	for (int i = 0; i < num_steps; i++)
	{
		sw_4->Write("{0:F1};", i*0.1);
		for (int j = 0; j < num_steps; j++)
		{
			if (i + j > num_steps - 1)
				sw_4->Write(";");
			else
			{
				sw_4->Write("{0:F2};", index_reduced[k]);
				k++;
			}
		}
		sw_4->WriteLine();
	}

	/*
	//статистика
	//среднее
	sw_4->Write("Aver;");
	double aver_num = 0;
	int max_num = total_num_P_approx[0];
	int min_num = total_num_P_approx[0];
	for (int i = 0; i < total_num_P_approx.size(); i++)
	{
	aver_num += total_num_P_approx[i];
	if (min_num > total_num_P_approx[i])
	min_num = total_num_P_approx[i];
	if (max_num < total_num_P_approx[i])
	max_num = total_num_P_approx[i];
	}
	aver_num = aver_num / total_num_P_approx.size();
	sw_4->Write("{0:F2};", aver_num);

	double aver = 0;
	for (int j = 0; j < index_reduced[0].size(); j++)
	{
	aver = 0;
	for (int i = 0; i < index_reduced.size(); i++)
	aver += index_reduced[i][j];
	aver = aver / index_reduced.size();
	sw_4->Write("{0:F2};", aver);
	}
	sw_4->WriteLine();

	//минимум
	sw_4->Write("Min;");
	sw_4->Write("{0};", min_num);

	double min;
	for (int j = 0; j < index_reduced[0].size(); j++)
	{
	min = index_reduced[0][j];
	for (int i = 0; i < index_reduced.size(); i++)
	{
	if (min > index_reduced[i][j])
	min = index_reduced[i][j];
	}
	sw_4->Write("{0:F2};", min);
	}
	sw_4->WriteLine();

	//максимум
	sw_4->Write("Max;");
	sw_4->Write("{0};", max_num);

	double max;
	for (int j = 0; j < index_reduced[0].size(); j++)
	{
	max = index_reduced[0][j];
	for (int i = 0; i < index_reduced.size(); i++)
	{
	if (max < index_reduced[i][j])
	max = index_reduced[i][j];
	}
	sw_4->Write("{0:F2};", max);
	}
	*/

	sw_4->WriteLine();
	sw_4->WriteLine();

}
