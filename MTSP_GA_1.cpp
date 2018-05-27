// MTSP_GA_1.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include "GA_for_MTSP_source.h"

using namespace std;
using namespace System;
using namespace System::IO;



int main(int argc, char* argv[])
{
	//число городов
	int num_n = 12;

	//число особей
	int num_N = 50;

	//число итерация
	int num_iter = 200;

	//кол-во запусков на одной задаче
	int num_runs = 1;

	//имя файла, с которого считываем
	String^ file_name_rd_str = "Example_MTSP_m2_n20_N50_12.txt";//ПОМЕНЯТЬ!!!

	//имя задачи в файле
	String^ problem_name_str;	//ПОМЕНЯТЬ!!!

	//чтение множества Парето из файла
	String^ file_name_source_Pareto_set_str;

	//оператор рекомбинации
	recomb_oper rec_oper = recomb_oper::DEC_new;

	//тип 1-го критерия
	en_type_criterion type_1nd_criterion = en_type_criterion::SUM;

	//тип 2-го критерия
	en_type_criterion type_2nd_criterion = en_type_criterion::SUM;

	//"кванты информации"
	unsigned quantum_inf = _1ST_2ND_ + _2ND_1ST_;

	//общее время выполнения всех задач
	unsigned long long total_time = 0;
	//для инициализации рандомизатора
	unsigned long long temp_time;
	//среднее время выполнения задачи
	double aver_time = 0;

	//для статистики по метрике
	double total_dist_begin_1 = 0;
	double total_dist_begin_2 = 0;
	double total_dist_end_1 = 0;
	double total_dist_end_2 = 0;

	double max_dist_begin_1 = 0;
	double max_dist_begin_2 = 0;
	double max_dist_end_1 = 0;
	double max_dist_end_2 = 0;

	double min_dist_begin_1 = 0;
	double min_dist_begin_2 = 0;
	double min_dist_end_1 = 0;
	double min_dist_end_2 = 0;

	//для статистики аппроксимации мн-ва Парето
	double total_num_approx_P_begin = 0;
	double total_num_approx_P_end = 0;

	int max_num_approx_P_begin = 0;
	int max_num_approx_P_end = 0;

	int min_num_approx_P_begin = 0;
	int min_num_approx_P_end = 0;

	double total_num_approx_in_P_begin = 0;
	double total_num_approx_in_P_end = 0;

	int max_num_approx_in_P_begin = 0;
	int max_num_approx_in_P_end = 0;

	int min_num_approx_in_P_begin = 0;
	int min_num_approx_in_P_end = 0;

	//статистика эксперимента по сужению мн-ва Парето
	//контейнеры для относительных показателей сужения мн-ва Парето
	//доля "отброшенных" точек (в %)
	vector<vector<double>> index_reduced_1_2; //1-ый критерий важнее 2-го
	vector<vector<double>> index_reduced_2_1; //2-ой критерий важнее 1-го
	vector<double> index_reduced_both(SIZE_EXCL_TETA+19); //1-ый важнее 2-го + 2-ой важнее 1-го (таблица, разложенная по строке)
	vector<double> index_reduced_tmp(SIZE_EXCL_TETA+19); //хранение текущих значения (два "кванта информации")


	//контейнер числа точек в аппроксимации мн-ва Парето 
	vector<int> total_num_P_approx;


	//флаг вычисления сужения
	bool reduction = false;

	//инициализация параметров
	string str_temp;
	for (int i = 1; i < argc; i++)
	{
		str_temp = argv[i];
		if (str_temp == "\\n")
		{
			num_n = stoi(argv[i + 1]);
			continue;
		}

		if (str_temp == "\\N")
		{
			num_N = stoi(argv[i + 1]);
			continue;
		}

		if (str_temp == "\\MAX_1")
			type_1nd_criterion = en_type_criterion::MAX;

		if (str_temp == "\\MAX_2")
			type_2nd_criterion = en_type_criterion::MAX;

		if (str_temp == "\\ITER")
		{
			num_iter = stoi(argv[i + 1]);
			continue;
		}

		if (str_temp == "\\RUNS")
		{
			num_runs = stoi(argv[i + 1]);
			continue;
		}

		if (str_temp == "\\INPUT")
		{
			file_name_rd_str = gcnew String(argv[i + 1]);
			continue;
		}

		if (str_temp == "\\FILE_PARETO")
		{
			file_name_source_Pareto_set_str = gcnew String(argv[i + 1]);
			continue;
		}

		if (str_temp == "\\PROBLEM_NAME")
		{
			problem_name_str = gcnew String(argv[i + 1]);
			continue;
		}

		if (str_temp == "\\REDUCE")
			reduction = true;
		
		if (str_temp == "\\DPX")
			rec_oper = recomb_oper::DPX;

		if (str_temp == "\\DEC_one")
			rec_oper = recomb_oper::DEC_old;
	}
	
	//открываем для считывания
	StreamReader^ sr = gcnew StreamReader(file_name_rd_str);
	/*
	TCHAR buffer[MAX_PATH];
	GetCurrentDirectory(sizeof(buffer), buffer);
	String^ str_temp_1 = gcnew String(buffer);
	str_temp.copy
	str_temp = "";
	str_temp += "\\results";

	_mkdir(str_temp.c_str());
	*/

	//короткий файл для записи
	String^ file_name_1_st = "results/Example_MTSP_m2_n" + num_n + "_N" + num_N + "_" + rec_oper.ToString("g") + "_results.csv";
	StreamWriter^ sw_1 = gcnew StreamWriter(file_name_1_st);

	//файл для записи эксперимента по сужению мн-ва Парето
	String^ file_name_red_P_set_str = "results/Example_MTSP_m2_n" + num_n + "_N" + num_N + "_" + rec_oper.ToString("g") + "_red_P_set.csv";
	StreamWriter^ sw_3 = gcnew StreamWriter(file_name_red_P_set_str);

	//короткий файл для записи эксперимента по сужению мн-ва Парето
	String^ file_name_short_red_P_set_str = "results/Example_MTSP_m2_n" + num_n + "_N" + num_N + "_" + rec_oper.ToString("g") + "_red_P_set_short.csv";
	StreamWriter^ sw_4 = gcnew StreamWriter(file_name_short_red_P_set_str);
	sw_4->WriteLine("Reduction of the Pareto set");
	sw_4->WriteLine();

	String^ cur_line_str = sr->ReadLine();

	//количества задач
	int num_prbl_val = 0;
	//флаг, указано ли название задачи
	bool is_problem_specified;

	//если указано название задачи
	if (problem_name_str)
	{
		num_prbl_val = 1;
		is_problem_specified = true;
		while (cur_line_str != problem_name_str)
			cur_line_str = sr->ReadLine();
	}
	//если не указано название задачи
	//определение количества задач
	else
	{
		is_problem_specified = false;
		while (cur_line_str != "example_number=")
			cur_line_str = sr->ReadLine();

		cur_line_str = sr->ReadLine();
		str_temp = "";
		str_temp += cur_line_str[0];
		num_prbl_val = stoi(str_temp);
	}

	


	//ЗАДАЧИ
	//цикл по всем задачам
	for (int iter_prbl = 0; iter_prbl < num_prbl_val; iter_prbl++) 
	{
		//если не указано название задачи
		if (!is_problem_specified)
		{
			while (cur_line_str != "")
				cur_line_str = sr->ReadLine();

			problem_name_str = sr->ReadLine();
		}
		
		

//to do: проверить считывание размера задачи из файлы
//!!! добавить обработку, если в файле отсутствует строка "n="
//?   нужно ли задавать n из командной строки?
//		(для каждой задачи свой размер)
		while (cur_line_str != "n=")
			cur_line_str = sr->ReadLine();
		cur_line_str = sr->ReadLine();
		str_temp = "";
		int k = 0;
		while ( k < cur_line_str->Length )
			str_temp += cur_line_str[k++];
		num_n = stoi(str_temp);

		//значения аргументов конструктора ГА (по порядку)
		//число городов
		//число особей
		//число критериев
		//длина максимального пути (?)
		//тип 1-го критерия (сумма или максимум)
		//тип 2-го критерия (сумма или максимум)
		GA_path ga(num_n, num_N, 2, 100, type_1nd_criterion, type_2nd_criterion);


		
		//функция reserve() почему-то не работает
		//s_temp.reserve(10);
		//for (int i = 0; i < 10; i++)
		//	s_temp[i].reserve(10);

		
		int var_rand = 10;
		

		String^ file_name_st;
		if (MODIF)
			file_name_st = "results/Example_MTSP_m2_n" + ga.get_n() + "_N" + ga.get_N() + "_" + problem_name_str + "_" + rec_oper.ToString("g") + "_modif_temp.csv";
		else
			file_name_st = "results/Example_MTSP_m2_n" + ga.get_n() + "_N" + ga.get_N() + "_" + problem_name_str + "_" + rec_oper.ToString("g") + "_temp.csv";
		StreamWriter^ sw = gcnew StreamWriter(file_name_st);

		//String^ file_name_1_st = "results/Example_MTSP_m2_n12_N50_" + problem_name_str + ".csv";
		//StreamWriter^ sw_1 = gcnew StreamWriter(file_name_1_st);


		//чтение множества Парето из файла (если присвоена строка с именем)
		//String^ file_name_source_Pareto_set_str = "Example_MTSP_m2_n20_N50_1_10_Temp.csv";
		vector<vector<int>> phi_Pareto_set;
		if (file_name_source_Pareto_set_str)
			phi_Pareto_set = ga.read_Pareto_set_from_file(file_name_source_Pareto_set_str, problem_name_str);
		
		//счетчик числа совпадений аппроксимации и мн-ва Парето
		ga.count_P_eq_approx = 0;


		sw->WriteLine(problem_name_str);
		sw->WriteLine("n; {0}", ga.get_n());
		sw->WriteLine("m; {0}", ga.get_m());
		sw->WriteLine();

		sw_1->WriteLine(problem_name_str);
		//printf(problem_name_str);
		sw_1->WriteLine("n; {0}", ga.get_n());
		sw_1->WriteLine("m; {0}", ga.get_m());
		sw_1->WriteLine("N; {0}", ga.get_N());
		sw_1->WriteLine("iter; {0}", num_iter);
		sw_1->WriteLine("run; {0}", num_runs);

		//файл для записи эксперимента по сужению мн-ва Парето
		if (reduction)
		{
			sw_3->WriteLine(problem_name_str);
			sw_3->WriteLine("n; {0}", ga.get_n());
			sw_3->WriteLine("m; {0}", ga.get_m());
			sw_3->WriteLine("N; {0}", ga.get_N());
			sw_3->WriteLine();

		}


		//заполнение матриц критериев
		ga.set_matrices(sr);

		sw->WriteLine();


//		ga.init();
//<<<
		//сбрасываем значения перед циклом по всем запускам
		total_dist_begin_1 = 0;
		total_dist_begin_2 = 0;
		total_dist_end_1 = 0;
		total_dist_end_2 = 0;

		max_dist_begin_1 = 0;
		max_dist_begin_2 = 0;
		max_dist_end_1 = 0;
		max_dist_end_2 = 0;

		min_dist_begin_1 = 0;
		min_dist_begin_2 = 0;
		min_dist_end_1 = 0;
		min_dist_end_2 = 0;

		total_num_approx_P_begin = 0;
		total_num_approx_P_end = 0;

		max_num_approx_P_begin = 0;
		max_num_approx_P_end = 0;

		min_num_approx_P_begin = 0;
		min_num_approx_P_end = 0;

		total_num_approx_in_P_begin = 0;
		total_num_approx_in_P_end = 0;

		max_num_approx_in_P_begin = 0;
		max_num_approx_in_P_end = 0;

		min_num_approx_in_P_begin = 0;
		min_num_approx_in_P_end = 0;

		//для сужения делаем только один запуск
		if (reduction)
			num_runs = 1;
///>>>

		//засекаем время начала работы алгоритма для данной задачи
		//считаем общее время для одной задачи по всем запускам
		unsigned long long start_time = ::GetTickCount();

		//ЗАПУСКИ
		//цикл по запускаем (на одной задаче)
		for (int index_run = 0; index_run < num_runs; index_run++)
		{
			sw->WriteLine("Problem{0} Run {1}", iter_prbl + 1, index_run + 1);
			//для рандомизатора
			temp_time = ::GetTickCount();
			srand(temp_time);

			//родители нач популяции
			vector<int> p1, p2, p3, p4;
			p1 = ga.patching_algorithm(ga.s_m[0], ga.c_max[0], false);
			p2 = ga.patching_algorithm(ga.s_m[0], ga.c_max[0], true);
			p3 = ga.patching_algorithm(ga.s_m[1], ga.c_max[1], false);
			p4 = ga.patching_algorithm(ga.s_m[1], ga.c_max[1], true);

			//sw->WriteLine("p1; p2; p3; p4");
			//printf("\np1\t p2\t p3\t p4\n");

			vector<int> p1_temp(ga.get_n());
			vector<int> p2_temp(ga.get_n());
			vector<int> p3_temp(ga.get_n());
			vector<int> p4_temp(ga.get_n());

			p1_temp = p1;
			p2_temp = p2;
			p3_temp = p3;
			p4_temp = p4;

			p1_temp[0] = 1;
			p2_temp[0] = 1;
			p3_temp[0] = 1;
			p4_temp[0] = 1;
			//sw->Write("{0}; {1}; {2}; {3};", p1[0], p2[0], p3[0], p4[0]);
			//printf("%d\t%d\t%d\t%d\t", p1[0], p2[0], p3[0], p4[0]);

			int i_temp = 0;
			for (int i = 1; i < ga.get_n(); ++i)
			{
				p1_temp[i] = p1[i_temp] + 1;
				i_temp = p1[i_temp];
			}
			i_temp = 0;
			for (int i = 1; i < ga.get_n(); ++i)
			{
				p2_temp[i] = p2[i_temp] + 1;
				i_temp = p2[i_temp];
			}
			i_temp = 0;
			for (int i = 1; i < ga.get_n(); ++i)
			{
				p3_temp[i] = p3[i_temp] + 1;
				i_temp = p3[i_temp];
			}
			i_temp = 0;
			for (int i = 1; i < ga.get_n(); ++i)
			{
				p4_temp[i] = p4[i_temp] + 1;
				i_temp = p4[i_temp];
			}

			p1 = p1_temp;
			p2 = p2_temp;
			p3 = p3_temp;
			p4 = p4_temp;

			/*
			for (int i = 0; i < ga.get_n(); i++)
			{
				sw->Write("{0};", p1[i]);
				printf("%d\t", p1[i]);

				sw->Write("{0};", p2[i]);
				printf("%d\t", p2[i]);

				sw->Write("{0};", p3[i]);
				printf("%d\t", p3[i]);

				sw->Write("{0};", p4[i]);
				printf("%d\t", p4[i]);

				sw->WriteLine();
				printf("\n");
			}

			sw->WriteLine();
			printf("\n");
			*/


			//0. начальная популяция
			temp_time = ::GetTickCount();
			ga.init_pop(ga.s_m, ga.c_max[0], temp_time, p1, p2, p3, p4);

			sw->WriteLine("Initial population");
			//sw_1->WriteLine("Initial population");
			printf("Initial population (problem %d):\n", iter_prbl+1);
			for (int j = 0; j < ga.get_n(); j++)
			{
				for (int i = 0; i < ga.get_N(); i++)
				{
					sw->Write("{0};", ga.pop[i][j]);
					printf("%d\t", ga.pop[i][j]);
				}
				sw->WriteLine();
			}

			sw->WriteLine();
			printf("\n");



			//заполняем значение векторного критерия по популяции
			//phi[i][j], i - индекс критерия, j - индекс особи

			sw->WriteLine("Values of vector criterion (initial population)");
			//sw_1->WriteLine("Values of vector criterion");
			printf("Values of vector criterion (initial population, problem %d):\n", iter_prbl+1);
			//vector<int> phi_min, phi_max;
			//for (int j = 0; j < ga.get_N(); j++)
			//	for (int i = 0; i < ga.get_m(); i++)
			//		ga.phi[i][j] = ga.multi_phitness(ga.pop[j])[i];

			for (int i = 0; i < ga.get_m(); i++)
			{
				for (int j = 0; j < ga.get_N(); j++)
				{
					ga.phi[i][j] = ga.multi_phitness(ga.pop[j])[i];
					sw->Write("{0};", ga.phi[i][j]);
					//sw_1->Write("{0};", ga.phi[i][j]);
					printf("%d\t", ga.phi[i][j]);
				}
				sw->WriteLine();
				//sw_1->WriteLine();
				/*
				phi_min.push_back(*min_element(ga.phi[i].cbegin(), ga.phi[i].cend()));
				printf("min = %d\t", phi_min[i]);
				phi_max.push_back(*max_element(ga.phi[i].cbegin(), ga.phi[i].cend()));
				printf("max = %d\t\n", phi_max[i]);
				*/
			}

			sw->WriteLine();
			//sw_1->WriteLine();
			printf("\n");

			//построение фронтов
			sw->WriteLine("Ranks of poulation (initial population)");
			//sw_1->WriteLine("Ranks of poulation");
			printf("Ranks of poulation (initial population, problem %d):\n", iter_prbl+1);
			//false - популяция в явном виде не отсорирована по фронтам, ранги фронтов заданы в векторе i_rank
			ga.i_rank = ga.range_front(ga.pop, true);
			for (int i = 0; i < ga.get_N(); i++)
			{
				sw->Write("{0};", ga.i_rank[i]);
				//sw_1->Write("{0};", ga.i_rank[i]);
				printf("%d\t", ga.i_rank[i]);
			}
			sw->WriteLine();
			//sw_1->WriteLine();
			printf("\n");

			/*
			sw->WriteLine("Sorted initial population");
			printf("Sorted initial population:\n");
			for (int j = 0; j < ga.get_n(); j++)
			{
				for (int i = 0; i < ga.get_N(); i++)
				{
					sw->Write("{0};", ga.pop[i][j]);
					printf("%d\t", ga.pop[i][j]);
				}
				sw->WriteLine();
			}

			sw->WriteLine();
			printf("\n");


			sw->WriteLine("Values of vector criterion");
			printf("Values of vector criterion:\n");
			for (int i = 0; i < ga.get_m(); i++)
			{
				for (int j = 0; j < ga.get_N(); j++)
				{
					ga.phi[i][j] = ga.multi_phitness(ga.pop[j])[i];
					sw->Write("{0};", ga.phi[i][j]);
					printf("%d\t", ga.phi[i][j]);
				}
				sw->WriteLine();
			}

			sw->WriteLine();
			printf("\n");
			*/

			//построение расстояний в каждом фронте
			ga.crowd_dist_new(ga.pop, true);

			sw->WriteLine("Crowding distances of population (initial population)");
			printf("Crowding distances of population (initial population, problem %d):\n", iter_prbl+1);
			for (int i = 0; i < ga.get_N(); i++)
			{
				sw->Write("{0:F2};", ga.i_dist[i]);
				printf("%.2f\n", ga.i_dist[i]);
			}
			sw->WriteLine();

			//вывод значений векторного критерия аппроксимации мн-ва Парето для НАЧАЛЬНОЙ популяции
			ga.phi_P_approx.clear();
			ga.build_phi_P_approx();

			sw->WriteLine("Approximation of the Pareto set (initial population)");
			//sw_1->WriteLine("Approximation of the Pareto set({0} iterations)", iter);
			for (int i = 0; i < ga.get_m(); i++)
			{
				for (int j = 0; j < ga.phi_P_approx.size(); j++)
				{
					sw->Write("{0};", ga.phi_P_approx[j][i]);
					//sw_1->Write("{0};", ga.phi_P_approx[j][i]);
				}
				sw->WriteLine();
				//sw_1->WriteLine();
			}

			sw->WriteLine();
			//sw_1->WriteLine();

			//вычисление метрики для НАЧАЛЬНОЙ популяции - аппроксимации множества Парето (если выше присвоено имя файла)
			if (file_name_source_Pareto_set_str)
			{
				//заголовок
				if (index_run == 0)
				{
					sw_1->WriteLine("N_PS; {0}", phi_Pareto_set.size());
					sw_1->WriteLine();

					sw_1->Write("Begin; ; ; ;");
					sw_1->WriteLine("End;");

					sw_1->Write("Metric 'Convergence approximation to Pareto set';");
					sw_1->Write("Metric 'Convergence Pareto set to approximation';");
					sw_1->Write("Points in approx to Pareto set;");
					sw_1->Write("Points from Pareto set;");

					sw_1->Write("Metric 'Convergence approximation to Pareto set';");
					sw_1->Write("Metric 'Convergence Pareto set to approximation';");
					sw_1->Write("Points in approx to Pareto set;");
					sw_1->Write("Points from Pareto set;");
					sw_1->WriteLine("Run");
				}

				//вычисляем метрики
				ga.count_P_eq_approx = 0;
				ga.evaluate_metric_of_approx(sw_1, phi_Pareto_set, true);
				ga.evaluate_metric_of_approx(sw, phi_Pareto_set, false);
				sw->WriteLine();
				sw_1->Write("{0};", ga.count_P_eq_approx);

				//для средней метрики по всем запускам
				total_dist_begin_1 += ga.dist_conver_approx_to_P_set_val;
				total_dist_begin_2 += ga.dist_conver_P_set_to_approx_val;

				total_num_approx_P_begin += ga.phi_P_approx.size();
				total_num_approx_in_P_begin += ga.count_P_eq_approx;

				if (0 == index_run)
				{
					max_dist_begin_1 = min_dist_begin_1 = ga.dist_conver_approx_to_P_set_val;
					max_dist_begin_2 = min_dist_begin_2 = ga.dist_conver_P_set_to_approx_val;
					max_num_approx_P_begin = min_num_approx_P_begin = ga.phi_P_approx.size();
					max_num_approx_in_P_begin = min_num_approx_in_P_begin = ga.count_P_eq_approx;
				}

				//определение максимума метрики по всем запускам
				if (max_dist_begin_1 < ga.dist_conver_approx_to_P_set_val)
					max_dist_begin_1 = ga.dist_conver_approx_to_P_set_val;
				if (max_dist_begin_2 < ga.dist_conver_P_set_to_approx_val)
					max_dist_begin_2 = ga.dist_conver_P_set_to_approx_val;

				if (max_num_approx_P_begin < ga.phi_P_approx.size())
					max_num_approx_P_begin = ga.phi_P_approx.size();
				if (max_num_approx_in_P_begin < ga.count_P_eq_approx)
					max_num_approx_in_P_begin = ga.count_P_eq_approx;

				//определение минимума метрики по всем запускам
				if (min_dist_begin_1 > ga.dist_conver_approx_to_P_set_val)
					min_dist_begin_1 = ga.dist_conver_approx_to_P_set_val;
				if (min_dist_begin_2 > ga.dist_conver_P_set_to_approx_val)
					min_dist_begin_2 = ga.dist_conver_P_set_to_approx_val;

				if (min_num_approx_P_begin > ga.phi_P_approx.size())
					min_num_approx_P_begin = ga.phi_P_approx.size();
				if (min_num_approx_in_P_begin > ga.count_P_eq_approx)
					min_num_approx_in_P_begin = ga.count_P_eq_approx;
			}


			int i_p1, i_p2;
			p1_temp = p1;
			p2_temp = p2;

			//родители + потомки (R_t = P_t + Q_t) - сделан членом класса GA_path
			//vector<vector<int>> pop_R_t(2 * ga.get_N());
			//текущий потомок
			vector<int> child(ga.get_n());
			//вектор рангов фронтов для R_t - сделан членом класса GA_path
			//vector<int> i_rank_R_t(2 * ga.get_N());


			//ОСНОВНОЙ ЦИКЛ ПО ИТЕРАЦИЯМ
			int iter = 0;
			while (++iter <= num_iter)
			{
				ga.pop_R_t = ga.pop; //R_t = P_t
				for (int i = 0; i < ga.get_N(); i++)
				{
					//1. выбираем родительские решения турнирной селекцией
					
					//для рандомизатора
					temp_time = ::GetTickCount();
					srand(temp_time);
					i_p1 = ga.tourn_selection();
					i_p2 = ga.tourn_selection();

					//2. мутация

					//для рандомизатора
					temp_time = ::GetTickCount();
					srand(temp_time);
					ga.mutation(i_p1, i_p2, p1_temp, p2_temp);

					//printf("After mutation:\n");
					//printf("p1_temp\t p2_temp\n");
					//for (int i = 0; i < ga.get_n(); i++)
					//{
					//	printf("%d\t", p1_temp[i]);
					//	printf("%d\t\n", p2_temp[i]);
					//}
					//printf("\n");

					//записываем родителей в нужном виде для рекомбинации
					//первый родитель
					p1[0] = 1;
					int i_temp = 0;
					for (int i = 1; i < ga.get_n(); ++i)
					{
						p1[i] = p1_temp[i_temp] + 1;
						i_temp = p1_temp[i_temp];
					}
					//второй родитель
					p2[0] = 1;
					i_temp = 0;
					for (int i = 1; i < ga.get_n(); ++i)
					{
						p2[i] = p2_temp[i_temp] + 1;
						i_temp = p2_temp[i_temp];
					}

					//3. оптимальная рекомбинация

					//для рандомизатора
					temp_time = ::GetTickCount();
					srand(temp_time);

					switch (rec_oper)
					{
						case recomb_oper::DEC_new:
							child = ga.DEC_new(ga.s_m, p1, p2);
							break;

						case recomb_oper::DEC_old:
							child = ga.DEC_old(ga.s_m[0], p1, p2);
							break;

						case recomb_oper::DPX:
							child = ga.DPX(ga.s_m, p1, p2);
							break;
					}
						

					//мутируем потомка, у кототорого значения критерия совпадает хотя бы с одним из родителей
					//Юля
					if (MODIF)
					{
						//для рандомизатора
						temp_time = ::GetTickCount();
						srand(temp_time);
						
						vector<int> child_new(ga.get_n());
						if ((ga.multi_phitness(child) == ga.multi_phitness(ga.pop[i_p1])) ||
							(ga.multi_phitness(child) == ga.multi_phitness(ga.pop[i_p2])))
						{
							double rand_mut = (double)rand() / RAND_MAX;
							if (rand_mut < 0.5)
								child_new = ga.exchange_mutation(ga.pop[i_p1]);
							else
								child_new = ga.exchange_mutation(ga.pop[i_p2]);
							child = child_new;
						}
					}

					ga.pop_R_t.push_back(child);//R_t = P_t + child
				}


				//R_t = P_t + Q_t

				//ФОРМИРОВАНИЕ НОВОЙ ПОПУЛЯЦИИ

				//вычисление ранга особей
				//printf("Ranks of poulation:\n");
				//true - популяция в явном виде отсорирована про фронтам, i_rank_R_t - такой же порядок (ранги фронтов)
				ga.i_rank_R_t = ga.range_front(ga.pop_R_t, true);
				//for (int i = 0; i < ga.get_N(); i++)
				//	printf("%d\t", ga.i_rank[i]);
				//printf("\n");

				//ранг последенго фронта, который целиком войдет в новую популяцию
				int index_rank_last = ga.i_rank_R_t[ga.get_N()] - 1;

				//для проверки, что расстояния сортируются, если все ранги = 1
				int temp = 0;
				if ((index_rank_last == 0) && (ga.i_rank_R_t[2 * ga.get_N() - 1] == 1))
				{
					temp++; //поставить точку останова
				}

				//обновляем популяцию "цельными" фронтами
				ga.pop.clear();
				ga.i_rank.clear();
				int i_pop_last_rank = 0;
				for (int i = 0; i < ga.get_N(); i++)
				{
					if (ga.i_rank_R_t[i] == index_rank_last + 1)
					{
						i_pop_last_rank = i;
						break;
					}
					ga.pop.push_back(ga.pop_R_t[i]);
					ga.i_rank.push_back(ga.i_rank_R_t[i]);
				}

				//true - пирамидальная сортировка, false - быстрая сортировка
				ga.crowd_dist_new(ga.pop_R_t, true);
				

				//ищем границы последнего фронта
				int i_start_last_front_exceeded = 0;
				int i_stop_last_front_exceeded = 0;
				bool is_seen_i_start = false;
				bool is_seen_i_stop = false;
				for (int s = 0; s < 2 * ga.get_N(); s++)
				{
					if ((ga.i_rank_R_t[s] == index_rank_last + 1) && (!is_seen_i_start))
					{
						i_start_last_front_exceeded = s;
						is_seen_i_start = true;
					}
					if ((ga.i_rank_R_t[s] == index_rank_last + 2) && (!is_seen_i_stop))
					{
						i_stop_last_front_exceeded = s - 1;
						is_seen_i_stop = true;
					}
				}

				//если у всех особей один ранг
				//if ( (!is_seen_i_start) && (!is_seen_i_stop))


				//если фронт, который частично вошел в новую популяцию,
				//оказался последним в расширенной популяции (родители+потомки)
				if (!is_seen_i_stop)
					i_stop_last_front_exceeded = 2 * ga.get_N() - 1;

				//если последний фронт закончился индексом > N-1 (полностью не войдет в новую популяцию)
				//то добавляем особи из следующего фронта 
				if (ga.i_rank_R_t[ga.get_N() - 1] != index_rank_last)
				{
					//дополяняем до N из фронта, не попавшего полностью (сортировка по i_dist)
					vector<int> index_front_temp;
					for (int s = 0; s <= i_stop_last_front_exceeded - i_start_last_front_exceeded; s++)
						index_front_temp.push_back(i_start_last_front_exceeded + s);
					ga.heap_sort(index_front_temp, i_start_last_front_exceeded, i_stop_last_front_exceeded);

					for (int s = 0; s <= ga.get_N() - i_start_last_front_exceeded - 1; s++)
					{
						//ga.i_dist[i_start_last_front_exceeded + s] = ga.i_dist[index_front_temp[s]];
						ga.pop.push_back(ga.pop_R_t[index_front_temp[s]]);
						ga.i_rank.push_back(ga.i_rank_R_t[index_front_temp[s]]);
					}
					ga.i_dist.resize(ga.get_N());
				}
				//если последний фронт закончился индексом N-1 (полностью войдет в новую популяцию)
				//то особи из следующих фронтов не добавляем

				//ЛОКАЛЬНЫЙ ПОИСК (не сделан)
				//for (int i = 0; i < ga.get_n(); i++)
				//{
					//ga.local_search(ga.pop[i], ga.s_m[0]); //что писать вторым аргументом?
				//}

				//формируем значения векторного критерия аппроксимации мн-ва Парето (без повторов)
				ga.phi_P_approx.clear();
				ga.build_phi_P_approx();
				
				
				//выводим популяцию, ранги, расстояния и аппрокимацию мн-ва Парето (векторный критерий) без повторов
				if (  ( ((iter % FREQ_SHOW_COMP) == 0) && file_name_source_Pareto_set_str ) || 
					( ((iter % FREQ_SHOW_RED ) == 0) && reduction )  )
				{
					//str_temp = problem_name_str->ToString + "\n";
					printf("Problem %d\n", iter_prbl+1);
					sw->WriteLine("Run {0}, Iteration {1}", index_run+1, iter);
					//sw_1->WriteLine("Iteration {0}", iter);
					printf("----Run %d, Iteration %d------\n", index_run+1, iter);
					for (int j = 0; j < ga.get_n(); j++)
					{
						for (int i = 0; i < ga.get_N(); i++)
						{
							sw->Write("{0};", ga.pop[i][j]);
							printf("%d\t", ga.pop[i][j]);
						}
						sw->WriteLine();
					}

					sw->WriteLine();
					printf("\n");

					sw->WriteLine("Ranks of poulation (problem {0}, run {1}, iteration {2})", iter_prbl+1, index_run+1, iter);
					printf("Ranks of poulation (problem %d, run %d, iteration %d):\n", iter_prbl+1, index_run+1, iter);
					printf("Start index of last front: %d\n", i_start_last_front_exceeded);
					printf("Final index of last front: %d\n", i_stop_last_front_exceeded);
					for (int i = 0; i < ga.get_N(); i++)
					{
						sw->Write("{0};", ga.i_rank[i]);
						printf("%d\t", ga.i_rank[i]);
					}

					sw->WriteLine();
					printf("\n");

					//расстояния популяции
					sw->WriteLine("Crowding distances of population (problem {0}, run {1}, iteration {2})", iter_prbl+1, index_run+1, iter);
					printf("Crowding distances of population (problem %d, run %d, iteration %d):\n", iter_prbl+1, index_run+1, iter);
					for (int i = 0; i < ga.get_N(); i++)
					{
						sw->Write("{0:F2};", ga.i_dist[i]);
						printf("%.2f\n", ga.i_dist[i]);
					}

					sw->WriteLine();
					printf("\n");

					//значания критериев
					sw->WriteLine("Values of vector criterion (problem {0}, run {1}, iteration {2})", iter_prbl+1, index_run+1, iter);
					printf("Values of vector criterion (problem %d, run %d, iteration %d):\n", iter_prbl+1, index_run+1, iter);
					for (int i = 0; i < ga.get_m(); i++)
					{
						for (int j = 0; j < ga.get_N(); j++)
						{
							ga.phi[i][j] = ga.multi_phitness(ga.pop[j])[i];
							sw->Write("{0};", ga.phi[i][j]);
							printf("%d\t", ga.phi[i][j]);
						}
						sw->WriteLine();
					}

					sw->WriteLine();
					printf("\n");

					
					sw->WriteLine("Approximation of the Pareto set (problem {0}, run {1}, iteration {2})", iter_prbl+1, index_run+1, iter);
					//sw_1->WriteLine("Approximation of the Pareto set({0} iterations)", iter);
					for (int i = 0; i < ga.get_m(); i++)
					{
						for (int j = 0; j < ga.phi_P_approx.size(); j++)
						{
							sw->Write("{0};", ga.phi_P_approx[j][i]);
							//sw_1->Write("{0};", ga.phi_P_approx[j][i]);
						}
						sw->WriteLine();
						//sw_1->WriteLine();
					}

					sw->WriteLine();
					//sw_1->WriteLine();

					//вычисление метрики - аппроксимации множества Парето (если выше присвоено имя файла)
					if (file_name_source_Pareto_set_str)
					{
						ga.evaluate_metric_of_approx(sw, phi_Pareto_set, false);
						sw->WriteLine();
					}

				}

				//sw->WriteLine("Crowding distances of population");
				//printf("Crowding distances of population:\n");
				//for (int i = 0; i < ga.get_N(); i++)
				//{
				//	sw->Write("{0:F2};", ga.i_dist[i]);
					//printf("%.2f\n", ga.i_dist[i]);
				//}


				ga.pop_R_t.clear();
				//ga.i_dist_R_t.clear();
				ga.i_rank_R_t.clear();
			}//основной цикл итераций

			/*
			//расстояния конечной популяции
			sw->WriteLine("Crowding distances of population");
			printf("Crowding distances of population:\n");
			for (int i = 0; i < ga.get_N(); i++)
			{
				sw->Write("{0:F2};", ga.i_dist[i]);
				printf("%.2f\n", ga.i_dist[i]);
			}

			//значания критериев конечной популяции
			sw->WriteLine("Values of vector criterion");
			printf("Values of vector criterion:\n");
			for (int i = 0; i < ga.get_m(); i++)
			{
				for (int j = 0; j < ga.get_N(); j++)
				{
					ga.phi[i][j] = ga.multi_phitness(ga.pop[j])[i];
					sw->Write("{0};", ga.phi[i][j]);
					printf("%d\t", ga.phi[i][j]);
				}
				sw->WriteLine();
			}

			sw->WriteLine();
			printf("\n");

			//построение аппроксимации мн-ва Парето
			ga.build_phi_P_approx();
			//вывод аппроксимации мн-ва Парето (векторный критерий)
			sw->WriteLine("Approximation of the Pareto set");
			for (int i = 0; i < ga.get_m(); i++)
			{
				for (int j = 0; j < ga.phi_P_approx.size(); j++)
				{
					sw->Write("{0};", ga.phi_P_approx[i][j]);
				}
				sw->WriteLine();
			}

			sw->WriteLine();
			*/

			/*		sw_1->WriteLine("Approximation of the Pareto set");
			for (int i = 0; i < ga.get_m(); i++)
			{
				for (int j = 0; j < ga.phi_P_approx.size(); j++)
				{
					sw_1->Write("{0};", ga.phi_P_approx[j][i]);
				}
				sw_1->WriteLine();
			}

			sw_1->WriteLine();
			*/

			//после основного цикла итераций
			//вычисление метрики - аппроксимации множества Парето (если выше присвоено имя файла)
			if (file_name_source_Pareto_set_str)
			{
				//в файл sw выводим в цикле итераций
				ga.count_P_eq_approx = 0;
				ga.evaluate_metric_of_approx(sw_1, phi_Pareto_set, true);
				sw->WriteLine();
				sw_1->Write("{0};", ga.count_P_eq_approx);
				sw_1->WriteLine("{0};", index_run + 1);

				//для средней метрики по всем запускам
				total_dist_end_1 += ga.dist_conver_approx_to_P_set_val;
				total_dist_end_2 += ga.dist_conver_P_set_to_approx_val;
				
				//для среднего числа точек в аппрокс мн-ва Парето по всем запускам
				total_num_approx_P_end += ga.phi_P_approx.size();
				total_num_approx_in_P_end += ga.count_P_eq_approx;

				if (0 == index_run)
				{
					max_dist_end_1 = min_dist_end_1 = ga.dist_conver_approx_to_P_set_val;
					max_dist_end_2 = min_dist_end_2 = ga.dist_conver_P_set_to_approx_val;
					max_num_approx_P_end = min_num_approx_P_end = ga.phi_P_approx.size();
					max_num_approx_in_P_end = min_num_approx_in_P_end = ga.count_P_eq_approx;
				}

				//определение максимума метрики по всем запускам
				if (max_dist_end_1 < ga.dist_conver_approx_to_P_set_val)
					max_dist_end_1 = ga.dist_conver_approx_to_P_set_val;
				if (max_dist_end_2 < ga.dist_conver_P_set_to_approx_val)
					max_dist_end_2 = ga.dist_conver_P_set_to_approx_val;

				if (max_num_approx_P_end < ga.phi_P_approx.size())
					max_num_approx_P_end = ga.phi_P_approx.size();

				if (max_num_approx_in_P_end < ga.count_P_eq_approx)
					max_num_approx_in_P_end = ga.count_P_eq_approx;

				//определение минимума метрики по всем запускам
				if (min_dist_end_1 > ga.dist_conver_approx_to_P_set_val)
					min_dist_end_1 = ga.dist_conver_approx_to_P_set_val;
				if (min_dist_end_2 > ga.dist_conver_P_set_to_approx_val)
					min_dist_end_2 = ga.dist_conver_P_set_to_approx_val;

				if (min_num_approx_P_end > ga.phi_P_approx.size())
					min_num_approx_P_end = ga.phi_P_approx.size();

				if (min_num_approx_in_P_end > ga.count_P_eq_approx)
					min_num_approx_in_P_end = ga.count_P_eq_approx;
				
			
				//множество Парето (векторный критерий)
				sw->WriteLine("The Pareto set");
				for (int i = 0; i < ga.get_m(); i++)
				{
					for (int j = 0; j < phi_Pareto_set.size(); j++)
					{
						sw->Write("{0};", phi_Pareto_set[j][i]);
					}
					sw->WriteLine();
				}
				sw->WriteLine();
			}

			//очищаем популяцию и ранги
			ga.pop.clear();
			for (int l = 0; l < ga.get_N(); l++)
				ga.i_rank[l] = 0;

		}//цикл запусков


		//время окончания решения одной задачи (все запуски)
		unsigned long long stop_time = ::GetTickCount();



		//------------------------------------------------------------------------------------
		//----------ЭКСПЕРИМЕНТ ПО СУЖЕНИЮ МН-ВА ПАРЕТО (если установлен флаг)

		if (reduction)
		{
			sw_3->WriteLine("Approximation of the Pareto set");
			for (int i = 0; i < ga.get_m(); i++)
			{
				for (int j = 0; j < ga.phi_P_approx.size(); j++)
				{
					sw_3->Write("{0};", ga.phi_P_approx[j][i]);
				}
				sw_3->WriteLine();
			}

			sw_3->WriteLine("Points in approximation");
			sw_3->WriteLine("{0}", ga.phi_P_approx.size());
			sw_3->WriteLine();


			//определение минимума, максимума и среднего числа точек в мн-ве Парето по всем запускам
			total_num_P_approx.push_back(ga.phi_P_approx.size());
			
			/*
			if (0 == iter_prbl)
				min_num_P_approx = max_num_P_approx = ga.phi_P_approx.size();

			if (min_num_P_approx > ga.phi_P_approx.size())
				min_num_P_approx = ga.phi_P_approx.size();
			if (max_num_P_approx < ga.phi_P_approx.size())
				max_num_P_approx = ga.phi_P_approx.size();
			*/
			
			if (quantum_inf == _1ST_2ND_)
			{ //задан один "квант информации"

				//1-ый критерий важнее 2-го
				sw_3->WriteLine("Reduction of the Pareto set");
				sw_3->WriteLine("1st is more important than 2nd");
				index_reduced_1_2.push_back(ga.experiment_reduction(sw_3, problem_name_str, 0.1, NUM_STEPS_TETA, 0, _1ST_2ND_));
				sw_3->WriteLine();
			}
			if (quantum_inf == _2ND_1ST_ )
			{ //задан один "квант информации"

				//2-ой критерий важнее 1-го
				sw_3->WriteLine("Reduction of the Pareto set");
				sw_3->WriteLine("2nd is more important than 1st");
				index_reduced_2_1.push_back(ga.experiment_reduction(sw_3, problem_name_str, 0.1, NUM_STEPS_TETA, 0, _2ND_1ST_));
				sw_3->WriteLine();
			}
			if (quantum_inf == _1ST_2ND_+_2ND_1ST_ )
			{ //задано два "кванта информации"

				//1-ый критерий важнее 2-го + 2-ой критерий важнее 1-го
				sw_3->WriteLine("Reduction of the Pareto set");
				sw_3->WriteLine("1st -> 2nd and 2nd -> 1st");
				index_reduced_tmp = ga.experiment_reduction(sw_3, problem_name_str, 0.1, NUM_STEPS_TETA+1, -0.1, _1ST_2ND_ + _2ND_1ST_);
				//накапливаем сумму по всем задачам, чтобы потом вычислить среднее
//!!! инициализация index_reduced_both и index_reduced_tmp
				for (int i = 0; i < index_reduced_tmp.size(); i++)
					index_reduced_both[i] += index_reduced_tmp[i];
				sw_3->WriteLine();
				//вывод таблицы процента сужения
				print_exp_red_two(sw_4, problem_name_str, index_reduced_tmp, NUM_STEPS_TETA+1, total_num_P_approx[iter_prbl]);


				//таблица (в случае двух "квантов информации")

			}


	
		}//эксперимент по сужению мн-ва Парето


		//------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------


		

		 //статистика по метрике после всех запусков
		if (file_name_source_Pareto_set_str)
		{
			total_dist_begin_1 = total_dist_begin_1 / num_runs;
			total_dist_end_1 = total_dist_end_1 / num_runs;
			total_dist_begin_2 = total_dist_begin_2 / num_runs;
			total_dist_end_2 = total_dist_end_2 / num_runs;

			total_num_approx_P_begin = total_num_approx_P_begin / num_runs;
			total_num_approx_P_end = total_num_approx_P_end / num_runs;

			total_num_approx_in_P_begin = total_num_approx_in_P_begin / num_runs;
			total_num_approx_in_P_end = total_num_approx_in_P_end / num_runs;
						
			sw->Write("{0:F4};", total_dist_begin_1);
			sw->Write("{0:F4};", total_dist_begin_2);
			sw->Write("{0:F2};", total_num_approx_P_begin);
			sw->Write("{0:F2};", total_num_approx_in_P_begin);
			sw->Write("{0:F4};", total_dist_end_1);
			sw->Write("{0:F4};", total_dist_end_2);
			sw->Write("{0:F2};", total_num_approx_P_end);
			sw->Write("{0:F2};", total_num_approx_in_P_end);
			sw->WriteLine("Average");

			sw->Write("{0:F4};", max_dist_begin_1);
			sw->Write("{0:F4};", max_dist_begin_2);
			sw->Write("{0};", max_num_approx_P_begin);
			sw->Write("{0};", max_num_approx_in_P_begin);
			sw->Write("{0:F4};", max_dist_end_1);
			sw->Write("{0:F4};", max_dist_end_2);
			sw->Write("{0};", max_num_approx_P_end);
			sw->Write("{0};", max_num_approx_in_P_end);
			sw->WriteLine("Max");

			sw->Write("{0:F4};", min_dist_begin_1);
			sw->Write("{0:F4};", min_dist_begin_2);
			sw->Write("{0};", min_num_approx_P_begin);
			sw->Write("{0};", min_num_approx_in_P_begin);
			sw->Write("{0:F4};", min_dist_end_1);
			sw->Write("{0:F4};", min_dist_end_2);
			sw->Write("{0};", min_num_approx_P_end);
			sw->Write("{0};", min_num_approx_in_P_end);
			sw->WriteLine("Min");
			sw->WriteLine();

			sw_1->Write("{0:F4};", total_dist_begin_1);
			sw_1->Write("{0:F4};", total_dist_begin_2);
			sw_1->Write("{0};", total_num_approx_P_begin);
			sw_1->Write("{0};", total_num_approx_in_P_begin);
			sw_1->Write("{0:F4};", total_dist_end_1);
			sw_1->Write("{0:F4};", total_dist_end_2);
			sw_1->Write("{0};", total_num_approx_P_end);
			sw_1->Write("{0};", total_num_approx_in_P_end);
			sw_1->WriteLine("Average");
			
			sw_1->Write("{0:F4};", max_dist_begin_1);
			sw_1->Write("{0:F4};", max_dist_begin_2);
			sw_1->Write("{0};", max_num_approx_P_begin);
			sw_1->Write("{0};", max_num_approx_in_P_begin);
			sw_1->Write("{0:F4};", max_dist_end_1);
			sw_1->Write("{0:F4};", max_dist_end_2);
			sw_1->Write("{0};", max_num_approx_P_end);
			sw_1->Write("{0};", max_num_approx_in_P_end);
			sw_1->WriteLine("Max");

			sw_1->Write("{0:F4};", min_dist_begin_1);
			sw_1->Write("{0:F4};", min_dist_begin_2);
			sw_1->Write("{0:F2};", min_num_approx_P_begin);
			sw_1->Write("{0:F2};", min_num_approx_in_P_begin);
			sw_1->Write("{0:F4};", min_dist_end_1);
			sw_1->Write("{0:F4};", min_dist_end_2);
			sw_1->Write("{0:F2};", min_num_approx_P_end);
			sw_1->Write("{0:F2};", min_num_approx_in_P_end);
			sw_1->WriteLine("Min");
			sw_1->WriteLine();
		}


		//вычисляем время решения одной задачи (все запуски)
		unsigned long long result_time = stop_time - start_time;
		total_time += result_time; //общее время решения всех задач
		//printf("Time: %d\n", result_time);
		time_format(result_time, "Time (one problem)", sw);
		time_format(result_time, "Time (one problem)", sw_1);
		time_format(result_time, "Time (one problem)", sw_3);
		sw_3->WriteLine();
		sw_3->WriteLine();

		sw_4->WriteLine(problem_name_str);
		time_format(result_time, "Time (one problem)", sw_4);
		sw_4->WriteLine();


		if (num_runs > 1)
		{
			aver_time = result_time / num_runs;
			time_format(aver_time, "Average time of all runs (one problem)", sw);
			time_format(aver_time, "Average time of all runs (one problem)", sw_1);
		}


		sw->WriteLine();
		sw->Close();

		sw_1->WriteLine();
		sw_1->WriteLine();

		ga.~GA_path();

	}//цикл по задачам


	 //вывод 2-х таблиц: 1) 1-ый важнее 2-го; 2) 2-ой важнее 1-го
	 //вывод статистики по сужению по всем задачам
	if ( reduction )
	{
		if (quantum_inf < _1ST_2ND_ + _2ND_1ST_)
		{
			//ТАБЛ. 1) 1-ый важнее 2-го
			print_exp_red(sw_4, "1st criterion is more important then the 2nd", index_reduced_1_2, total_num_P_approx);

			//ТАБЛ. 2) 2-ой важнее 1-го
			print_exp_red(sw_4, "2nd criterion is more important then the 1st", index_reduced_2_1, total_num_P_approx);
		}
		else
		{
			for (int i = 0; i < index_reduced_tmp.size(); i++)
				index_reduced_both[i] = index_reduced_both[i]/ num_prbl_val;
			double aver_num = 0;
			for (int i = 0; i < total_num_P_approx.size(); i++)
				aver_num += total_num_P_approx[i];
			print_exp_red_two(sw_4, "average statistics", index_reduced_both, NUM_STEPS_TETA+1, aver_num / total_num_P_approx.size());
		}

		/*

		//ТАБЛ. 1) 1-ый важнее 2-го
		//заголовок
		sw_4->WriteLine("1st criterion is more important then the 2nd");
		sw_4->Write(";Points in approx of Pareto set;");
		for (int j = 1; j <= index_reduced_1_2[0].size(); j++)
			sw_4->Write("{0:F1};", 0.1*j);
		sw_4->WriteLine();

		//процент "отброшенных" точек
		for (int i = 0; i < index_reduced_1_2.size(); i++)
		{
			//sw_4->Write(vec_problem_name_str[i]);
			sw_4->Write(";{0};", total_num_P_approx[i]);
			for (int j = 0; j < index_reduced_1_2[0].size(); j++)
				sw_4->Write("{0:F2};", index_reduced_1_2[i][j]);
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
		for (int j = 0; j < index_reduced_1_2[0].size(); j++)
		{
			aver = 0;
			for (int i = 0; i < index_reduced_1_2.size(); i++)
				aver += index_reduced_1_2[i][j];
			aver = aver / index_reduced_1_2.size();
			sw_4->Write("{0:F2};", aver);
		}
		sw_4->WriteLine();

		//минимум
		sw_4->Write("Min;");
		sw_4->Write("{0};", min_num);

		double min;
		for (int j = 0; j < index_reduced_1_2[0].size(); j++)
		{
			min = index_reduced_1_2[0][j];
			for (int i = 0; i < index_reduced_1_2.size(); i++)
			{
				if (min > index_reduced_1_2[i][j])
					min = index_reduced_1_2[i][j];
			}
			sw_4->Write("{0:F2};", min);
		}
		sw_4->WriteLine();

		//максимум
		sw_4->Write("Max;");
		sw_4->Write("{0};", max_num);

		double max;
		for (int j = 0; j < index_reduced_1_2[0].size(); j++)
		{
			max = index_reduced_1_2[0][j];
			for (int i = 0; i < index_reduced_1_2.size(); i++)
			{
				if (max < index_reduced_1_2[i][j])
					max = index_reduced_1_2[i][j];
			}
			sw_4->Write("{0:F2};", max);
		}
		sw_4->WriteLine();
		sw_4->WriteLine();


		//ТАБЛ. 2) 2-ой важнее 1-го
		//заголовок
		sw_4->WriteLine("2nd criterion is more important then the 1st");
		sw_4->Write(";Points in approx of Pareto set;");
		for (int j = 1; j <= index_reduced_2_1[0].size(); j++)
			sw_4->Write("{0:F1};", 0.1*j);
		sw_4->WriteLine();

		//процент "отброшенных" точек
		for (int i = 0; i < index_reduced_2_1.size(); i++)
		{
			//sw_4->Write(vec_problem_name_str[i]);
			sw_4->Write(";{0};", total_num_P_approx[i]);
			for (int j = 0; j < index_reduced_2_1[0].size(); j++)
				sw_4->Write("{0:F2};", index_reduced_2_1[i][j]);
			sw_4->WriteLine();
		}

		//статистика
		//среднее
		sw_4->Write("Aver;");
		sw_4->Write("{0:F2};", aver_num);

		for (int j = 0; j < index_reduced_2_1[0].size(); j++)
		{
			aver = 0;
			for (int i = 0; i < index_reduced_2_1.size(); i++)
				aver += index_reduced_2_1[i][j];
			aver = aver / index_reduced_2_1.size();
			sw_4->Write("{0:F2};", aver);
		}
		sw_4->WriteLine();

		//минимум
		sw_4->Write("Min;");
		sw_4->Write("{0};", min_num);

		for (int j = 0; j < index_reduced_2_1[0].size(); j++)
		{
			min = index_reduced_2_1[0][j];
			for (int i = 0; i < index_reduced_2_1.size(); i++)
			{
				if (min > index_reduced_2_1[i][j])
					min = index_reduced_2_1[i][j];
			}
			sw_4->Write("{0:F2};", min);
		}
		sw_4->WriteLine();

		//максимум
		sw_4->Write("Max;");
		sw_4->Write("{0};", max_num);

		for (int j = 0; j < index_reduced_2_1[0].size(); j++)
		{
			max = index_reduced_2_1[0][j];
			for (int i = 0; i < index_reduced_2_1.size(); i++)
			{
				if (max < index_reduced_2_1[i][j])
					max = index_reduced_2_1[i][j];
			}
			sw_4->Write("{0:F2};", max);
		}
		sw_4->WriteLine();
		sw_4->WriteLine();

		*/

	}

	//общее время по всем задачам
	time_format(total_time, "Total time of all problems", sw_1);
	time_format(total_time, "Total time of all problems", sw_3);
	time_format(total_time, "Total time of all problems", sw_4);


	sw_1->Close();
	sw_3->Close();
	sw_4->Close();
	sr->Close();

	printf("\n");
	//system("pause");

	return 0;
}

