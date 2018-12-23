// MTSP_GA_1.cpp: определяет точку входа для консольного приложения.
//

//#include "stdafx.h"
#include <iostream>
#include <string>
#include "GA_for_MTSP_source.h"
#include "archive.h"

//using namespace std;
//using namespace System;
//using namespace System::IO;


//
//int main_VNS(int argc, char* argv[])
//{
//	//число городов
//	int num_n = 12;
//	//флаг числа городов не по умолчанию
//	bool is_num_n_specified = false;
//
//	//число особей
//	int num_N = 50;
//
//	//число итерация
//	int num_iter = 200;
//
//	//кол-во запусков на одной задаче
//	int num_runs = 1;
//
//	//имя файла, с которого считываем
//	String^ file_name_rd_str = "Example_MTSP_m2_n20_N50_12.txt";//ПОМЕНЯТЬ!!!
//
//	//имя задачи в файле
//	String^ problem_name_str;	//ПОМЕНЯТЬ!!!
//
//	//чтение множества Парето из файла
//	String^ file_name_source_Pareto_set_str;
//
//	//оператор рекомбинации
//	//recomb_oper rec_oper = recomb_oper::DEC_new;
//
//	//"кванты информации"
//	unsigned quantum_inf = _1ST_2ND_ + _2ND_1ST_;
//
//	//общее время выполнения всех задач
//	unsigned long long total_time = 0;
//	//для инициализации рандомизатора
//	unsigned long long temp_time;
//	//среднее время выполнения задачи
//	double aver_time = 0;
//
//	//для статистики по метрике
//	double total_dist_begin_1 = 0;
//	double total_dist_begin_2 = 0;
//	double total_dist_end_1 = 0;
//	double total_dist_end_2 = 0;
//
//	double max_dist_begin_1 = 0;
//	double max_dist_begin_2 = 0;
//	double max_dist_end_1 = 0;
//	double max_dist_end_2 = 0;
//
//	double min_dist_begin_1 = 0;
//	double min_dist_begin_2 = 0;
//	double min_dist_end_1 = 0;
//	double min_dist_end_2 = 0;
//
//	//для статистики аппроксимации мн-ва Парето
//	double total_num_approx_P_begin = 0;
//	double total_num_approx_P_end = 0;
//
//	int max_num_approx_P_begin = 0;
//	int max_num_approx_P_end = 0;
//
//	int min_num_approx_P_begin = 0;
//	int min_num_approx_P_end = 0;
//
//	double total_num_approx_in_P_begin = 0;
//	double total_num_approx_in_P_end = 0;
//
//	int max_num_approx_in_P_begin = 0;
//	int max_num_approx_in_P_end = 0;
//
//	int min_num_approx_in_P_begin = 0;
//	int min_num_approx_in_P_end = 0;
//
//	//статистика эксперимента по сужению мн-ва Парето
//	//контейнеры для относительных показателей сужения мн-ва Парето
//	//доля "отброшенных" точек (в %)
//	vector<vector<double>> index_reduced_1_2; //1-ый критерий важнее 2-го
//	vector<vector<double>> index_reduced_2_1; //2-ой критерий важнее 1-го
//	vector<double> index_reduced_both(SIZE_EXCL_TETA+19); //1-ый важнее 2-го + 2-ой важнее 1-го (таблица, разложенная по строке)
//	vector<double> index_reduced_tmp(SIZE_EXCL_TETA+19); //хранение текущих значения (два "кванта информации")
//
//
//	//контейнер числа точек в аппроксимации мн-ва Парето 
//	vector<int> total_num_P_approx;
//
//
//	//флаг вычисления сужения
//	bool reduction = false;
//
//	//инициализация параметров
//	string str_temp;
//	for (int i = 1; i < argc; i++)
//	{
//		str_temp = argv[i];
//		if (str_temp == "\\n")
//		{
//			num_n = stoi(argv[i + 1]);
//			is_num_n_specified = true;
//			continue;
//		}
//
//		if (str_temp == "\\N")
//		{
//			num_N = stoi(argv[i + 1]);
//			continue;
//		}
//
//		if (str_temp == "\\ITER")
//		{
//			num_iter = stoi(argv[i + 1]);
//			continue;
//		}
//
//		if (str_temp == "\\RUNS")
//		{
//			num_runs = stoi(argv[i + 1]);
//			continue;
//		}
//
//		if (str_temp == "\\INPUT")
//		{
//			file_name_rd_str = gcnew String(argv[i + 1]);
//			continue;
//		}
//
//		if (str_temp == "\\FILE_PARETO")
//		{
//			file_name_source_Pareto_set_str = gcnew String(argv[i + 1]);
//			continue;
//		}
//
//		if (str_temp == "\\PROBLEM_NAME")
//		{
//			problem_name_str = gcnew String(argv[i + 1]);
//			continue;
//		}
//
//		if (str_temp == "\\REDUCE")
//			reduction = true;
//		
//		//if (str_temp == "\\DPX")
//		//	rec_oper = recomb_oper::DPX;
//	}
//	
//	//открываем для считывания
//	StreamReader^ sr = gcnew StreamReader(file_name_rd_str);
//
//	//короткий файл для записи
//	String^ file_name_1_st = "results/Example_MTSP_VNS_m2_n" + num_n + "_N" + num_N + "_results.csv";
//	StreamWriter^ sw_1 = gcnew StreamWriter(file_name_1_st);
//
//	//файл для записи эксперимента по сужению мн-ва Парето
//	String^ file_name_red_P_set_str = "results/Example_MTSP_VNS_m2_n" + num_n + "_N" + num_N + "_red_P_set.csv";
//	StreamWriter^ sw_3 = gcnew StreamWriter(file_name_red_P_set_str);
//
//	//короткий файл для записи эксперимента по сужению мн-ва Парето
//	String^ file_name_short_red_P_set_str = "results/Example_MTSP_VNS_m2_n" + num_n + "_N" + num_N + "_red_P_set_short.csv";
//	StreamWriter^ sw_4 = gcnew StreamWriter(file_name_short_red_P_set_str);
//	sw_4->WriteLine("Reduction of the Pareto set");
//	sw_4->WriteLine();
//
//	String^ cur_line_str = sr->ReadLine();
//
//	//количества задач
//	int num_prbl_val = 0;
//	//флаг, указано ли название задачи
//	bool is_problem_specified;
//
//	//если указано название задачи
//	if (problem_name_str)
//	{
//		num_prbl_val = 1;
//		is_problem_specified = true;
//		while (cur_line_str != problem_name_str)
//			cur_line_str = sr->ReadLine();
//	}
//	//если не указано название задачи
//	//определение количества задач
//	else
//	{
//		is_problem_specified = false;
//		while (cur_line_str != "example_number=")
//			cur_line_str = sr->ReadLine();
//
//		cur_line_str = sr->ReadLine();
//		str_temp = "";
//		str_temp += cur_line_str[0];
//		num_prbl_val = stoi(str_temp);
//	}
//
//	
//
//
//	//ЗАДАЧИ
//	//цикл по всем задачам
//	for (int iter_prbl = 0; iter_prbl < num_prbl_val; iter_prbl++) 
//	{
//		//если не указано название задачи
//		if (!is_problem_specified)
//		{
//			while (cur_line_str != "")
//				cur_line_str = sr->ReadLine();
//
//			problem_name_str = sr->ReadLine();
//		}
//		
//		
//
////to do: проверить считывание размера задачи из файлы
//        //если число городов не задано с командной строки
//		if (!is_num_n_specified)
//		{
//			while (cur_line_str != "n=")
//				cur_line_str = sr->ReadLine();
//			cur_line_str = sr->ReadLine();
//			str_temp = "";
//			int k = 0;
//			while (k < cur_line_str->Length)
//				str_temp += cur_line_str[k++];
//			num_n = stoi(str_temp);
//		}
//
//		//значения аргументов конструктора ГА (по порядку)
//		//число городов
//		//число особей (в нач. популяции)
//		//число критериев
//		GA_path vns(num_n, num_N, 2, 100);
//
//
//		
//		//функция reserve() почему-то не работает
//		//s_temp.reserve(10);
//		//for (int i = 0; i < 10; i++)
//		//	s_temp[i].reserve(10);
//
//		
//		int var_rand = 10;
//		
//
//		String^ file_name_st = "results/Example_MTSP_m2_n" + vns.get_n() + "_N" + vns.get_N() + "_" + problem_name_str + ".csv";
//		StreamWriter^ sw = gcnew StreamWriter(file_name_st);
//
//
//		//чтение множества Парето из файла (если присвоена строка с именем)
//		//String^ file_name_source_Pareto_set_str = "Example_MTSP_m2_n20_N50_1_10_Temp.csv";
//		vector<vector<int>> phi_Pareto_set;
//		if (file_name_source_Pareto_set_str)
//			phi_Pareto_set = vns.read_Pareto_set_from_file(file_name_source_Pareto_set_str, problem_name_str);
//		
//		//счетчик числа совпадений аппроксимации и мн-ва Парето
//		vns.count_P_eq_approx = 0;
//
//
//		sw->WriteLine(problem_name_str);
//		sw->WriteLine("n; {0}", vns.get_n());
//		sw->WriteLine("m; {0}", vns.get_m());
//		sw->WriteLine();
//
//		sw_1->WriteLine(problem_name_str);
//		//printf(problem_name_str);
//		sw_1->WriteLine("n; {0}", vns.get_n());
//		sw_1->WriteLine("m; {0}", vns.get_m());
//		sw_1->WriteLine("N; {0}", vns.get_N());
//		sw_1->WriteLine("iter; {0}", num_iter);
//		sw_1->WriteLine("run; {0}", num_runs);
//
//		//файл для записи эксперимента по сужению мн-ва Парето
//		if (reduction)
//		{
//			sw_3->WriteLine(problem_name_str);
//			sw_3->WriteLine("n; {0}", vns.get_n());
//			sw_3->WriteLine("m; {0}", vns.get_m());
//			sw_3->WriteLine("N; {0}", vns.get_N());
//			sw_3->WriteLine();
//
//		}
//
//
//		//заполнение матриц критериев
//		vns.set_matrices(sr);
//
//		sw->WriteLine();
//
//
////		ga.init();
////<<<
//		//сбрасываем значения перед циклом по всем запускам
//		total_dist_begin_1 = 0;
//		total_dist_begin_2 = 0;
//		total_dist_end_1 = 0;
//		total_dist_end_2 = 0;
//
//		max_dist_begin_1 = 0;
//		max_dist_begin_2 = 0;
//		max_dist_end_1 = 0;
//		max_dist_end_2 = 0;
//
//		min_dist_begin_1 = 0;
//		min_dist_begin_2 = 0;
//		min_dist_end_1 = 0;
//		min_dist_end_2 = 0;
//
//		total_num_approx_P_begin = 0;
//		total_num_approx_P_end = 0;
//
//		max_num_approx_P_begin = 0;
//		max_num_approx_P_end = 0;
//
//		min_num_approx_P_begin = 0;
//		min_num_approx_P_end = 0;
//
//		total_num_approx_in_P_begin = 0;
//		total_num_approx_in_P_end = 0;
//
//		max_num_approx_in_P_begin = 0;
//		max_num_approx_in_P_end = 0;
//
//		min_num_approx_in_P_begin = 0;
//		min_num_approx_in_P_end = 0;
//
//		//для сужения делаем только один запуск
//		if (reduction)
//			num_runs = 1;
/////>>>
//
//		//засекаем время начала работы алгоритма для данной задачи
//		//считаем общее время для одной задачи по всем запускам
//		unsigned long long start_time = ::GetTickCount();
//
//
////ПРЕПРОЦЕССИНГ для локального поиска
//		//построение перестановки pi (переменная index_pi)
//		//? определение fronts_struct_temp с размерностью
//		vector<vector<int>> tmp; //заглушка
//		vns.i_rank_pi = vns.range_front_J(vns.c_max, tmp, vns.index_pi, 0);
//		tmp.clear();
//		//ga.i_rank_pi = fronts_struct_temp.ranks;
//		//ga.index_pi = fronts_struct_temp.fronts_index;
//		sw->WriteLine("Permutation pi");
//		printf("Permutation pi\n");
//		for (int i = 0; i < vns.index_pi.size(); i++)
//		{
//			sw->Write("{0};", vns.index_pi[i]);
//			printf("%d\t", vns.index_pi[i]);
//		}
//		sw->WriteLine();
//		printf("\n");
//		sw->WriteLine("Ranks");
//		printf("Ranks\n");
//		for (int i = 0; i < vns.index_pi.size(); i++)
//		{
//			sw->Write("{0};", vns.i_rank_pi[i]);
//			printf("%d\t", vns.i_rank_pi[i]);
//		}
//		sw->WriteLine();
//		printf("\n");
//
//
//		//построение перестановок p_i (переменная index_p)
//		sw->WriteLine("Permutations p");
//		printf("Permutations p\n");
//		vector<vector<int>> c_max_temp;
//		for (int i = 0; i < vns.get_n(); i++)
//		{
//			c_max_temp.clear();
//			for (int j = 0; j < vns.get_n(); j++)
//				//сохраняем весь массив вектров (в том числе при i=j)
//				//чтобы использовать в ф-ции постр. рангов
//				c_max_temp.push_back({ vns.s_m[0][i][j], vns.s_m[1][i][j] });
//
//			vector<int> index_front_temp;
//			vector<vector<int>> tmp; //заглушка
//			vns.range_front_J(c_max_temp, tmp, index_front_temp, 0);
//			//первым всегда будет вектор (0, 0) (при i=j)
//			//поэтому начинаем с k=1
//			for (int k = 1; k < index_front_temp.size(); k++)
//				vns.index_p[i].push_back(index_front_temp[k]);
//			index_front_temp.clear();
//			tmp.clear();
//
//			//вывод
//			sw->WriteLine("i = {0}", i + 1);
//			printf("i = %d\n", i);
//			for (int j = 0; j < vns.index_p[i].size(); j++)
//			{
//				sw->Write("{0};", vns.index_p[i][j]);
//				printf("%d\t", vns.index_p[i][j]);
//			}
//			sw->WriteLine();
//			printf("\n");
//		}
//
//		sw->WriteLine();
//		printf("\n");
//
//		//засекаем время препроцессинга
//		long time_local_search_b_pre = ::GetTickCount() - start_time;
//		//время локального поиска (в начале)
//		long long time_local_search_b = 0;
//		//время локального поиска (в конце)
//		long long time_local_search_f = 0;
//
//////////////////////////////////////////////////////////////////////////
//
//		//ЗАПУСКИ
//		//цикл по запускам (на одной задаче)
//		for (int index_run = 0; index_run < num_runs; index_run++)
//		{
//			sw->WriteLine("Problem{0} Run {1}", iter_prbl + 1, index_run + 1);
//			//для рандомизатора
//			temp_time = ::GetTickCount();
//			srand(temp_time);
//
//			//родители нач популяции
//			//4 особи по Зангу
//			vector<int> p1, p2, p3, p4;
//			p1 = vns.patching_algorithm(vns.s_m[0], vns.c_max_all[0], false);
//			p2 = vns.patching_algorithm(vns.s_m[0], vns.c_max_all[0], true);
//			p3 = vns.patching_algorithm(vns.s_m[1], vns.c_max_all[1], false);
//			p4 = vns.patching_algorithm(vns.s_m[1], vns.c_max_all[1], true);
//
//			//sw->WriteLine("p1; p2; p3; p4");
//			//printf("\np1\t p2\t p3\t p4\n");
//
//			vector<int> p1_temp(vns.get_n());
//			vector<int> p2_temp(vns.get_n());
//			vector<int> p3_temp(vns.get_n());
//			vector<int> p4_temp(vns.get_n());
//
//			p1_temp = p1;
//			p2_temp = p2;
//			p3_temp = p3;
//			p4_temp = p4;
//
//			p1_temp[0] = 1;
//			p2_temp[0] = 1;
//			p3_temp[0] = 1;
//			p4_temp[0] = 1;
//			//sw->Write("{0}; {1}; {2}; {3};", p1[0], p2[0], p3[0], p4[0]);
//			//printf("%d\t%d\t%d\t%d\t", p1[0], p2[0], p3[0], p4[0]);
//
//			int i_temp = 0;
//			for (int i = 1; i < vns.get_n(); ++i)
//			{
//				p1_temp[i] = p1[i_temp] + 1;
//				i_temp = p1[i_temp];
//			}
//			i_temp = 0;
//			for (int i = 1; i < vns.get_n(); ++i)
//			{
//				p2_temp[i] = p2[i_temp] + 1;
//				i_temp = p2[i_temp];
//			}
//			i_temp = 0;
//			for (int i = 1; i < vns.get_n(); ++i)
//			{
//				p3_temp[i] = p3[i_temp] + 1;
//				i_temp = p3[i_temp];
//			}
//			i_temp = 0;
//			for (int i = 1; i < vns.get_n(); ++i)
//			{
//				p4_temp[i] = p4[i_temp] + 1;
//				i_temp = p4[i_temp];
//			}
//
//			p1 = p1_temp;
//			p2 = p2_temp;
//			p3 = p3_temp;
//			p4 = p4_temp;
//
//			/*
//			for (int i = 0; i < ga.get_n(); i++)
//			{
//				sw->Write("{0};", p1[i]);
//				printf("%d\t", p1[i]);
//
//				sw->Write("{0};", p2[i]);
//				printf("%d\t", p2[i]);
//
//				sw->Write("{0};", p3[i]);
//				printf("%d\t", p3[i]);
//
//				sw->Write("{0};", p4[i]);
//				printf("%d\t", p4[i]);
//
//				sw->WriteLine();
//				printf("\n");
//			}
//
//			sw->WriteLine();
//			printf("\n");
//			*/
//
//
//			//0. начальная популяция
//			temp_time = ::GetTickCount(); // для рандомизатора
//			vns.init_pop(vns.s_m, vns.c_max_all[0], temp_time, p1, p2, p3, p4);
//
//			sw->WriteLine("Initial population");
//			//sw_1->WriteLine("Initial population");
//			printf("Initial population (problem %d):\n", iter_prbl+1);
//			for (int j = 0; j < vns.get_n(); j++)
//			{
//				for (int i = 0; i < vns.get_N(); i++)
//				{
//					sw->Write("{0};", vns.pop[i][j]);
//					printf("%d\t", vns.pop[i][j]);
//				}
//				sw->WriteLine();
//			}
//
//			sw->WriteLine();
//			printf("\n");
//
//
//
//			//заполняем значение векторного критерия по популяции
//			//phi[i][j], i - индекс критерия, j - индекс особи
//
//			sw->WriteLine("Values of vector criterion (initial population)");
//			printf("Values of vector criterion (initial population, problem %d):\n", iter_prbl+1);
//			
//			//заполнение значений критерия
//			for (int i = 0; i < vns.get_N(); i++)
//				vns.phi[i] = vns.multi_phitness(vns.pop[i]);
//
//			//вывод значений критерия
//			for (int j = 0; j < vns.get_m(); j++)
//			{
//				for (int i = 0; i < vns.get_N(); i++)
//				{
//					sw->Write("{0};", vns.phi[i][j]);
//					printf("%d\t", vns.phi[i][j]);
//				}
//				sw->WriteLine();
//				printf("\n");
//			}
//			printf("\n");
//			sw->WriteLine();
//			
//
//
////-----------------------------------------------------------------------------------------------
////----------ЛОКАЛЬНЫЙ ПОИСК (нач. поп.)----------------------------------------------------------
//
//			// ПРЕПРОЦЕССИНГ см. выше
//			// выполняется один раз перед всеми запусками одной задачи
//
//
//			//ПРИМЕНЕНИЕ ЛОКАЛЬНОГО ПОИСКА К ОСОБЯМ ИЗ МАССИВА pop
//
//			//время локального поиска (в начале)
//			time_local_search_b -= ::GetTickCount();
//
//			vector<int> pop_new;
//			printf("Local search\n");
//			printf("\n");
//			sw->WriteLine("Local search");
//			sw->WriteLine();
//
//			for (int i = 0; i  < vns.get_N(); i++)
//				vns.pop[i] = vns.local_search(vns.pop[i], 0.5);
//
//			sw->WriteLine("after LS");
//			printf("afret LS:\n");
//			for (int j = 0; j < vns.get_n(); j++)
//			{
//				for (int i = 0; i < vns.get_N(); i++)
//				{
//					sw->Write("{0};", vns.pop[i][j]);
//					printf("%d\t", vns.pop[i][j]);
//				}
//				sw->WriteLine();
//
//			}
//
//			sw->WriteLine();
//			printf("\n");
//			//заполняем значение векторного критерия по популяции
//			sw->WriteLine("Values of vector criterion:");
//			printf("Values of vector criterion:\n");
//
//			for (int j = 0; j < vns.get_m(); j++)
//			{
//				for (int i = 0; i < vns.get_N(); i++)
//				{
//					sw->Write("{0};", vns.multi_phitness(vns.pop[i])[j]);
//					printf("%d\t", vns.multi_phitness(vns.pop[i])[j]);
//				}
//				sw->WriteLine();
//			}
//
//			sw->WriteLine();
//			printf("\n");
//
//			//засекаем время локального поиска (в начале)
//			time_local_search_b += ::GetTickCount();
//
////-----------------------------------------------------------------------------------------------
////-----------------------------------------------------------------------------------------------
//
//
//
//
//
////------------ПОСТРОЕНИЕ ФРОНТОВ
//			//true - популяция в явном виде отсорирована по фронтам + ранги фронтов заданы в векторе i_rank
//			//false - популяция в явном виде не отсорирована по фронтам, ранги фронтов заданы в векторе i_rank
//			vector<int> tmp2; //заглушка
//			vns.i_rank = vns.range_front_J(vns.phi, vns.pop, tmp2, 1);
//			tmp2.clear();
//			//значение векторного критерия отсортированной популяции
//			sw->WriteLine("Values of vector criterion (sorted):");
//			printf("Values of vector criterion (sorted):\n");
//
//			//заполнение значений критерия
//			for (int i = 0; i < vns.get_N(); i++)
//				vns.phi[i] = vns.multi_phitness(vns.pop[i]);
//
//			//вывод значений критерия
//			for (int j = 0; j < vns.get_m(); j++)
//			{
//				for (int i = 0; i < vns.get_N(); i++)
//				{
//					sw->Write("{0};", vns.phi[i][j]);
//					printf("%d\t", vns.phi[i][j]);
//				}
//				sw->WriteLine();
//				printf("\n");
//			}
//			printf("\n");
//			sw->WriteLine();
//
//
//
//			sw->WriteLine("Ranks of population (initial population)");
//			//sw_1->WriteLine("Ranks of poulation");
//			printf("Ranks of poulation (initial population, problem %d):\n", iter_prbl+1);
//			for (int i = 0; i < vns.get_N(); i++)
//			{
//				sw->Write("{0};", vns.i_rank[i]);
//				//sw_1->Write("{0};", ga.i_rank[i]);
//				printf("%d\t", vns.i_rank[i]);
//			}
//			sw->WriteLine();
//			//sw_1->WriteLine();
//			printf("\n");
//						
//
//			//формируем архив:
//			//случ. популяция + ЛП
//			Archive arch(vns);
//
//			//вывод значений векторного критерия аппроксимации мн-ва Парето для НАЧАЛЬНОЙ популяции
//			//vns.phi_P_approx.clear();
//			// архив = мн-во Парето
//			vns.phi_P_approx = arch.val_crit_archive;
//
//			sw->WriteLine("Approximation of the Pareto set (initial population)");
//			//sw_1->WriteLine("Approximation of the Pareto set({0} iterations)", iter);
//			for (int i = 0; i < vns.get_m(); i++)
//			{
//				for (int j = 0; j < vns.phi_P_approx.size(); j++)
//					sw->Write("{0};", vns.phi_P_approx[j][i]);
//		
//				sw->WriteLine();
//			}
//
//			sw->WriteLine();
//			
//
//			//вычисление метрики для НАЧАЛЬНОЙ популяции - аппроксимации множества Парето (если выше присвоено имя файла)
//			if (file_name_source_Pareto_set_str)
//			{
//				//заголовок
//				if (index_run == 0)
//				{
//					sw_1->WriteLine("N_PS; {0}", phi_Pareto_set.size());
//					sw_1->WriteLine();
//
//					sw_1->Write("Begin; ; ; ;");
//					sw_1->WriteLine("End;");
//
//					sw_1->Write("Metric 'Convergence approximation to Pareto set';");
//					sw_1->Write("Metric 'Convergence Pareto set to approximation';");
//					sw_1->Write("Points in approx to Pareto set;");
//					sw_1->Write("Points from Pareto set;");
//
//					sw_1->Write("Metric 'Convergence approximation to Pareto set';");
//					sw_1->Write("Metric 'Convergence Pareto set to approximation';");
//					sw_1->Write("Points in approx to Pareto set;");
//					sw_1->Write("Points from Pareto set;");
//					sw_1->WriteLine("Run");
//				}
//
//				//вычисляем метрики
//				vns.count_P_eq_approx = 0;
//				vns.evaluate_metric_of_approx(sw_1, phi_Pareto_set, true);
//				vns.evaluate_metric_of_approx(sw, phi_Pareto_set, false);
//				sw->WriteLine();
//				sw_1->Write("{0};", vns.count_P_eq_approx);
//
//				//для средней метрики по всем запускам
//				total_dist_begin_1 += vns.dist_conver_approx_to_P_set_val;
//				total_dist_begin_2 += vns.dist_conver_P_set_to_approx_val;
//
//				total_num_approx_P_begin += vns.phi_P_approx.size();
//				total_num_approx_in_P_begin += vns.count_P_eq_approx;
//
//				if (0 == index_run)
//				{
//					max_dist_begin_1 = min_dist_begin_1 = vns.dist_conver_approx_to_P_set_val;
//					max_dist_begin_2 = min_dist_begin_2 = vns.dist_conver_P_set_to_approx_val;
//					max_num_approx_P_begin = min_num_approx_P_begin = vns.phi_P_approx.size();
//					max_num_approx_in_P_begin = min_num_approx_in_P_begin = vns.count_P_eq_approx;
//				}
//
//				//определение максимума метрики по всем запускам
//				if (max_dist_begin_1 < vns.dist_conver_approx_to_P_set_val)
//					max_dist_begin_1 = vns.dist_conver_approx_to_P_set_val;
//				if (max_dist_begin_2 < vns.dist_conver_P_set_to_approx_val)
//					max_dist_begin_2 = vns.dist_conver_P_set_to_approx_val;
//
//				if (max_num_approx_P_begin < vns.phi_P_approx.size())
//					max_num_approx_P_begin = vns.phi_P_approx.size();
//				if (max_num_approx_in_P_begin < vns.count_P_eq_approx)
//					max_num_approx_in_P_begin = vns.count_P_eq_approx;
//
//				//определение минимума метрики по всем запускам
//				if (min_dist_begin_1 > vns.dist_conver_approx_to_P_set_val)
//					min_dist_begin_1 = vns.dist_conver_approx_to_P_set_val;
//				if (min_dist_begin_2 > vns.dist_conver_P_set_to_approx_val)
//					min_dist_begin_2 = vns.dist_conver_P_set_to_approx_val;
//
//				if (min_num_approx_P_begin > vns.phi_P_approx.size())
//					min_num_approx_P_begin = vns.phi_P_approx.size();
//				if (min_num_approx_in_P_begin > vns.count_P_eq_approx)
//					min_num_approx_in_P_begin = vns.count_P_eq_approx;
//			}
//
//
//			int i_p1, i_p2;
//			p1_temp = p1;
//			p2_temp = p2;
//
//
////СФОРМИРОВАН АРХИВ: СЛУЧ. ПОП. + ЛП			
//
//
////ОСНОВНОЙ ЦИКЛ ПО ИТЕРАЦИЯМ
//// итерация: VNS к особи. особь выбир. случ.
//// ограничение на число итерация num_iter
//
//			int iter = 0;
//			while (++iter <= num_iter)
//			{
//
//				/////////////////////////////////////////////////////////
//				//////////////////// ЛОКАЛЬНЫЙ ПОИСК (конеч. поп.)
//				////////////////////////////////////////////////////////
//
//							//время локального поиска (в конце)
//							//time_local_search_f -= ::GetTickCount();
//
//
//							//ПРИМЕНЕНИЕ ЛОКАЛЬНОГО ПОИСКА К ОСОБЯМ ИЗ АРХИВА
//				vector<int> new_to_arch;
//				printf("\n Local search at the end\n");
//				sw->WriteLine();
//				sw->WriteLine("Local search at the end");
//
//				//для рандомизатора
//				temp_time = ::GetTickCount();
//				srand(temp_time);
//				unsigned i_tmp = rand() % arch.ar_index_not_cons_lst.size();
//
//				// переходим к элементу архива, соотв. случ. сгенерированному индексу
//				list<unsigned>::const_iterator it_lst = arch.ar_index_not_cons_lst.cbegin();
//				advance(it_lst, i_tmp);
//				// дальше итератор it_lst использовать нельзя,
//				// т.к. на его позиции удаляется элелемент ar_index_not_cons_lst
//				i_tmp = *it_lst;
//				//флаг: особь продоминирована
//				bool pop_is_improved = true;
//				//индекс окрестности
//				int k_opt = 1;
//
//				// локальный поиск - ПЕРЕДЕЛАТЬ: по окр.1-6          /* 6 */
//				while (pop_is_improved || k_opt == MAX_NUM_K_OPT + 2)
//				{
//					vector<int> new_to_arch = shaking_Kopt(arch.archive[i_tmp], k_opt);
//					if (!new_to_arch.empty()) // не пустой, если i_lst был продоминирован
//					{
//						// добавляем в список неживых
//						arch.ar_index_no_lst.push_back(*it_lst);
//						// удаляем из непросмотренных
//						arch.ar_index_not_cons_lst.erase(it_lst);
//
//						// сравниваем с архивом
//						if (arch.check_new(new_to_arch))
//						{
//							arch.arch_modify(new_to_arch, vns.multi_phitness(new_to_arch));
//							pop_is_improved = false;
//						}
//						else
//							k_opt+=2;
//					}
//					else // i_lst не был продоминирован
//					{
//						// добавляем к просмотренным
//						arch.ar_index_cons_lst.push_back(*it_lst);
//						// удаляем из непросмотренных
//						arch.ar_index_not_cons_lst.erase(it_lst);
//						k_opt += 2;
//					}
//
//				}
//
//
//				sw->WriteLine("Iteration {0}; problem {1}", iter, iter_prbl+1);
//				printf("Iteration %d, problem %d", iter, iter_prbl + 1);
//			
//				sw->WriteLine("Population");
//				printf("Population \n");
//				for (int j = 0; j < vns.get_n(); j++)
//				{
//					for (int i = 0; i < arch.archive.size(); i++)
//					{
//						sw->Write("{0};", arch.archive[i][j]);
//						printf("%d\t", arch.archive[i][j]);
//					}
//					sw->WriteLine();
//
//				}
//
//				sw->WriteLine("Values of vector criterion");
//				printf("Values of vector criterion \n");
//				for (int j = 0; j < vns.get_m(); j++)
//				{
//					for (int i = 0; i < arch.archive.size(); i++)
//					{
//						sw->Write("{0};", arch.val_crit_archive[i][j]);
//						printf("%d\t", arch.val_crit_archive[i][j]);
//					}
//					sw->WriteLine();
//
//				}
//
//
//				//вычисление метрики после локального поиска в финальной популяции
//				vns.phi_P_approx = arch.val_crit_archive;
//				sw->WriteLine("Metrics after VNS");
//				if (file_name_source_Pareto_set_str)
//				{
//					vns.evaluate_metric_of_approx(sw, phi_Pareto_set, false);
//					sw->WriteLine();
//				}
//
//				//засекаем время локального поиска (в конце)
//				//time_local_search_f += ::GetTickCount();
//
//
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////				
//				
//
//			
//
//				
//
//
//				
//				
///*
//				// ЧТО ВЫВОДИТЬ? КОГДА ВЫВОДИТЬ?
//				//выводим популяцию, ранги, расстояния и аппрокимацию мн-ва Парето (векторный критерий) без повторов
//				if (  ( ((iter % FREQ_SHOW_COMP) == 0) && file_name_source_Pareto_set_str ) || 
//					( ((iter % FREQ_SHOW_RED ) == 0) && reduction )  )
//				{
//					//str_temp = problem_name_str->ToString + "\n";
//					printf("Problem %d\n", iter_prbl+1);
//					sw->WriteLine("Run {0}, Iteration {1}", index_run+1, iter);
//					//sw_1->WriteLine("Iteration {0}", iter);
//					printf("----Run %d, Iteration %d------\n", index_run+1, iter);
//					for (int j = 0; j < ga.get_n(); j++)
//					{
//						for (int i = 0; i < ga.get_N(); i++)
//						{
//							sw->Write("{0};", ga.pop[i][j]);
//							printf("%d\t", ga.pop[i][j]);
//						}
//						sw->WriteLine();
//					}
//
//					sw->WriteLine();
//					printf("\n");
//
//					sw->WriteLine("Ranks of poulation (problem {0}, run {1}, iteration {2})", iter_prbl+1, index_run+1, iter);
//					printf("Ranks of poulation (problem %d, run %d, iteration %d):\n", iter_prbl+1, index_run+1, iter);
//					printf("Start index of last front: %d\n", i_start_last_front_exceeded);
//					printf("Final index of last front: %d\n", i_stop_last_front_exceeded);
//					for (int i = 0; i < ga.get_N(); i++)
//					{
//						sw->Write("{0};", ga.i_rank[i]);
//						printf("%d\t", ga.i_rank[i]);
//					}
//
//					sw->WriteLine();
//					printf("\n");
//
//					//расстояния популяции
//					sw->WriteLine("Crowding distances of population (problem {0}, run {1}, iteration {2})", iter_prbl+1, index_run+1, iter);
//					printf("Crowding distances of population (problem %d, run %d, iteration %d):\n", iter_prbl+1, index_run+1, iter);
//					for (int i = 0; i < ga.get_N(); i++)
//					{
//						sw->Write("{0:F2};", ga.i_dist[i]);
//						printf("%.2f\n", ga.i_dist[i]);
//					}
//
//					sw->WriteLine();
//					printf("\n");
//
//					//значания критериев
//					sw->WriteLine("Values of vector criterion (problem {0}, run {1}, iteration {2})", iter_prbl+1, index_run+1, iter);
//					printf("Values of vector criterion (problem %d, run %d, iteration %d):\n", iter_prbl+1, index_run+1, iter);
//					//заполнение значений критерия
//// TO DO: перенести заполнение (не вывод) до "лок. поиска в конце итераций"
//					for (int i = 0; i < ga.get_N(); i++)
//						ga.phi[i] = ga.multi_phitness(ga.pop[i]);
//
//					//вывод значений критерия
//					for (int j = 0; j < ga.get_m(); j++)
//					{
//						for (int i = 0; i < ga.get_N(); i++)
//						{
//							sw->Write("{0};", ga.phi[i][j]);
//							printf("%d\t", ga.phi[i][j]);
//						}
//						sw->WriteLine();
//						printf("\n");
//					}
//					printf("\n");
//					sw->WriteLine();
//
//					
//					sw->WriteLine("Approximation of the Pareto set (problem {0}, run {1}, iteration {2})", iter_prbl+1, index_run+1, iter);
//					//sw_1->WriteLine("Approximation of the Pareto set({0} iterations)", iter);
//					for (int i = 0; i < ga.get_m(); i++)
//					{
//						for (int j = 0; j < ga.phi_P_approx.size(); j++)
//						{
//							sw->Write("{0};", ga.phi_P_approx[j][i]);
//							//sw_1->Write("{0};", ga.phi_P_approx[j][i]);
//						}
//						sw->WriteLine();
//						//sw_1->WriteLine();
//					}
//
//					sw->WriteLine();
//					//sw_1->WriteLine();
//
//					//вычисление метрики - аппроксимации множества Парето (если выше присвоено имя файла)
//					if (file_name_source_Pareto_set_str)
//					{
//						ga.evaluate_metric_of_approx(sw, phi_Pareto_set, false);
//						sw->WriteLine();
//					}
//
//				}
//*/
//				
//
//
//				
//			}//основной цикл итераций
//
//
//
//
//		
//
//			//ПОСЛЕ ОСНОВНОГО ЦИКЛА ИТЕРАЦИЙ
//			//вычисление метрики - аппроксимации множества Парето (если выше присвоено имя файла)
//			if (file_name_source_Pareto_set_str)
//			{
//				//в файл sw выводим в цикле итераций
//				vns.count_P_eq_approx = 0;
//				vns.evaluate_metric_of_approx(sw_1, phi_Pareto_set, true);
//				sw->WriteLine();
//				sw_1->Write("{0};", vns.count_P_eq_approx);
//				sw_1->WriteLine("{0};", index_run + 1);
//
//				//для средней метрики по всем запускам
//				total_dist_end_1 += vns.dist_conver_approx_to_P_set_val;
//				total_dist_end_2 += vns.dist_conver_P_set_to_approx_val;
//				
//				//для среднего числа точек в аппрокс мн-ва Парето по всем запускам
//				total_num_approx_P_end += vns.phi_P_approx.size();
//				total_num_approx_in_P_end += vns.count_P_eq_approx;
//
//				if (0 == index_run)
//				{
//					max_dist_end_1 = min_dist_end_1 = vns.dist_conver_approx_to_P_set_val;
//					max_dist_end_2 = min_dist_end_2 = vns.dist_conver_P_set_to_approx_val;
//					max_num_approx_P_end = min_num_approx_P_end = vns.phi_P_approx.size();
//					max_num_approx_in_P_end = min_num_approx_in_P_end = vns.count_P_eq_approx;
//				}
//
//				//определение максимума метрики по всем запускам
//				if (max_dist_end_1 < vns.dist_conver_approx_to_P_set_val)
//					max_dist_end_1 = vns.dist_conver_approx_to_P_set_val;
//				if (max_dist_end_2 < vns.dist_conver_P_set_to_approx_val)
//					max_dist_end_2 = vns.dist_conver_P_set_to_approx_val;
//
//				if (max_num_approx_P_end < vns.phi_P_approx.size())
//					max_num_approx_P_end = vns.phi_P_approx.size();
//
//				if (max_num_approx_in_P_end < vns.count_P_eq_approx)
//					max_num_approx_in_P_end = vns.count_P_eq_approx;
//
//				//определение минимума метрики по всем запускам
//				if (min_dist_end_1 > vns.dist_conver_approx_to_P_set_val)
//					min_dist_end_1 = vns.dist_conver_approx_to_P_set_val;
//				if (min_dist_end_2 > vns.dist_conver_P_set_to_approx_val)
//					min_dist_end_2 = vns.dist_conver_P_set_to_approx_val;
//
//				if (min_num_approx_P_end > vns.phi_P_approx.size())
//					min_num_approx_P_end = vns.phi_P_approx.size();
//
//				if (min_num_approx_in_P_end > vns.count_P_eq_approx)
//					min_num_approx_in_P_end = vns.count_P_eq_approx;
//				
//			
//				//множество Парето (векторный критерий)
//				sw->WriteLine("The Pareto set");
//				for (int i = 0; i < vns.get_m(); i++)
//				{
//					for (int j = 0; j < phi_Pareto_set.size(); j++)
//					{
//						sw->Write("{0};", phi_Pareto_set[j][i]);
//					}
//					sw->WriteLine();
//				}
//				sw->WriteLine();
//			}
//
//			//очищаем популяцию и ранги
//			vns.pop.clear();
//			for (int l = 0; l < vns.get_N(); l++)
//				vns.i_rank[l] = 0;
////			arch.~Archive();
//
//		}//цикл запусков
//
//
//		//время окончания решения одной задачи (все запуски)
//		unsigned long long stop_time = ::GetTickCount();
//
//
//
//		//------------------------------------------------------------------------------------
//		//----------ЭКСПЕРИМЕНТ ПО СУЖЕНИЮ МН-ВА ПАРЕТО (если установлен флаг)
//
//		if (reduction)
//		{
//			sw_3->WriteLine("Approximation of the Pareto set");
//			for (int i = 0; i < vns.get_m(); i++)
//			{
//				for (int j = 0; j < vns.phi_P_approx.size(); j++)
//				{
//					sw_3->Write("{0};", vns.phi_P_approx[j][i]);
//				}
//				sw_3->WriteLine();
//			}
//
//			sw_3->WriteLine("Points in approximation");
//			sw_3->WriteLine("{0}", vns.phi_P_approx.size());
//			sw_3->WriteLine();
//
//
//			//определение минимума, максимума и среднего числа точек в мн-ве Парето по всем запускам
//			total_num_P_approx.push_back(vns.phi_P_approx.size());
//			
//			/*
//			if (0 == iter_prbl)
//				min_num_P_approx = max_num_P_approx = ga.phi_P_approx.size();
//
//			if (min_num_P_approx > ga.phi_P_approx.size())
//				min_num_P_approx = ga.phi_P_approx.size();
//			if (max_num_P_approx < ga.phi_P_approx.size())
//				max_num_P_approx = ga.phi_P_approx.size();
//			*/
//			
//			if (quantum_inf == _1ST_2ND_)
//			{ //задан один "квант информации"
//
//				//1-ый критерий важнее 2-го
//				sw_3->WriteLine("Reduction of the Pareto set");
//				sw_3->WriteLine("1st is more important than 2nd");
//				index_reduced_1_2.push_back(vns.experiment_reduction(sw_3, problem_name_str, 0.1, NUM_STEPS_TETA, 0, _1ST_2ND_));
//				sw_3->WriteLine();
//			}
//			if (quantum_inf == _2ND_1ST_ )
//			{ //задан один "квант информации"
//
//				//2-ой критерий важнее 1-го
//				sw_3->WriteLine("Reduction of the Pareto set");
//				sw_3->WriteLine("2nd is more important than 1st");
//				index_reduced_2_1.push_back(vns.experiment_reduction(sw_3, problem_name_str, 0.1, NUM_STEPS_TETA, 0, _2ND_1ST_));
//				sw_3->WriteLine();
//			}
//			if (quantum_inf == _1ST_2ND_+_2ND_1ST_ )
//			{ //задано два "кванта информации"
//
//				//1-ый критерий важнее 2-го + 2-ой критерий важнее 1-го
//				sw_3->WriteLine("Reduction of the Pareto set");
//				sw_3->WriteLine("1st -> 2nd and 2nd -> 1st");
//				index_reduced_tmp = vns.experiment_reduction(sw_3, problem_name_str, 0.1, NUM_STEPS_TETA+1, -0.1, _1ST_2ND_ + _2ND_1ST_);
//				//накапливаем сумму по всем задачам, чтобы потом вычислить среднее
////!!! инициализация index_reduced_both и index_reduced_tmp
//				for (int i = 0; i < index_reduced_tmp.size(); i++)
//					index_reduced_both[i] += index_reduced_tmp[i];
//				sw_3->WriteLine();
//				//вывод таблицы процента сужения
//				print_exp_red_two(sw_4, problem_name_str, index_reduced_tmp, NUM_STEPS_TETA+1, total_num_P_approx[iter_prbl]);
//
//
//				//таблица (в случае двух "квантов информации")
//
//			}
//
//
//	
//		}//эксперимент по сужению мн-ва Парето
//
//
//		//------------------------------------------------------------------------------------
//		//------------------------------------------------------------------------------------
//
//
//		
//
//		 //статистика по метрике после всех запусков
//		if (file_name_source_Pareto_set_str)
//		{
//			total_dist_begin_1 = total_dist_begin_1 / num_runs;
//			total_dist_end_1 = total_dist_end_1 / num_runs;
//			total_dist_begin_2 = total_dist_begin_2 / num_runs;
//			total_dist_end_2 = total_dist_end_2 / num_runs;
//
//			total_num_approx_P_begin = total_num_approx_P_begin / num_runs;
//			total_num_approx_P_end = total_num_approx_P_end / num_runs;
//
//			total_num_approx_in_P_begin = total_num_approx_in_P_begin / num_runs;
//			total_num_approx_in_P_end = total_num_approx_in_P_end / num_runs;
//						
//			sw->Write("{0:F4};", total_dist_begin_1);
//			sw->Write("{0:F4};", total_dist_begin_2);
//			sw->Write("{0:F2};", total_num_approx_P_begin);
//			sw->Write("{0:F2};", total_num_approx_in_P_begin);
//			sw->Write("{0:F4};", total_dist_end_1);
//			sw->Write("{0:F4};", total_dist_end_2);
//			sw->Write("{0:F2};", total_num_approx_P_end);
//			sw->Write("{0:F2};", total_num_approx_in_P_end);
//			sw->WriteLine("Average");
//
//			sw->Write("{0:F4};", max_dist_begin_1);
//			sw->Write("{0:F4};", max_dist_begin_2);
//			sw->Write("{0};", max_num_approx_P_begin);
//			sw->Write("{0};", max_num_approx_in_P_begin);
//			sw->Write("{0:F4};", max_dist_end_1);
//			sw->Write("{0:F4};", max_dist_end_2);
//			sw->Write("{0};", max_num_approx_P_end);
//			sw->Write("{0};", max_num_approx_in_P_end);
//			sw->WriteLine("Max");
//
//			sw->Write("{0:F4};", min_dist_begin_1);
//			sw->Write("{0:F4};", min_dist_begin_2);
//			sw->Write("{0};", min_num_approx_P_begin);
//			sw->Write("{0};", min_num_approx_in_P_begin);
//			sw->Write("{0:F4};", min_dist_end_1);
//			sw->Write("{0:F4};", min_dist_end_2);
//			sw->Write("{0};", min_num_approx_P_end);
//			sw->Write("{0};", min_num_approx_in_P_end);
//			sw->WriteLine("Min");
//			sw->WriteLine();
//
//			sw_1->Write("{0:F4};", total_dist_begin_1);
//			sw_1->Write("{0:F4};", total_dist_begin_2);
//			sw_1->Write("{0};", total_num_approx_P_begin);
//			sw_1->Write("{0};", total_num_approx_in_P_begin);
//			sw_1->Write("{0:F4};", total_dist_end_1);
//			sw_1->Write("{0:F4};", total_dist_end_2);
//			sw_1->Write("{0};", total_num_approx_P_end);
//			sw_1->Write("{0};", total_num_approx_in_P_end);
//			sw_1->WriteLine("Average");
//			
//			sw_1->Write("{0:F4};", max_dist_begin_1);
//			sw_1->Write("{0:F4};", max_dist_begin_2);
//			sw_1->Write("{0};", max_num_approx_P_begin);
//			sw_1->Write("{0};", max_num_approx_in_P_begin);
//			sw_1->Write("{0:F4};", max_dist_end_1);
//			sw_1->Write("{0:F4};", max_dist_end_2);
//			sw_1->Write("{0};", max_num_approx_P_end);
//			sw_1->Write("{0};", max_num_approx_in_P_end);
//			sw_1->WriteLine("Max");
//
//			sw_1->Write("{0:F4};", min_dist_begin_1);
//			sw_1->Write("{0:F4};", min_dist_begin_2);
//			sw_1->Write("{0:F2};", min_num_approx_P_begin);
//			sw_1->Write("{0:F2};", min_num_approx_in_P_begin);
//			sw_1->Write("{0:F4};", min_dist_end_1);
//			sw_1->Write("{0:F4};", min_dist_end_2);
//			sw_1->Write("{0:F2};", min_num_approx_P_end);
//			sw_1->Write("{0:F2};", min_num_approx_in_P_end);
//			sw_1->WriteLine("Min");
//			sw_1->WriteLine();
//		}
//
//
//		//вычисляем время решения одной задачи (все запуски)
//		unsigned long long result_alg_time = stop_time - start_time - time_local_search_b - time_local_search_b_pre;
//		unsigned long long result_time = stop_time - start_time;
//		total_time += result_time; //общее время решения всех задач
//		//printf("Time: %d\n", result_time);
//		time_format(result_time, "Total time (one problem)", sw);
//		time_format(result_time, "Total time (one problem)", sw_1);
//		time_format(result_time, "Total time (one problem)", sw_3);
//		sw_3->WriteLine();
//		sw_3->WriteLine();
//
//		sw_4->WriteLine(problem_name_str);
//		time_format(result_time, "Total time (one problem)", sw_4);
//		sw_4->WriteLine();
//
//
//		if (num_runs > 1)
//		{
//			aver_time = result_time / num_runs;
//			time_format(aver_time, "Average total time of all runs (one problem)", sw);
//			time_format(aver_time, "Average total time of all runs (one problem)", sw_1);
//		}
//
//
//		// среднее время работы алгоритма без локальных поисков
//		time_format(((double)result_alg_time) / num_runs, "Average time of algorithm", sw);
//		time_format(((double)result_alg_time) / num_runs, "Average time of algorithm", sw_1);
//
//		// среднее время локального поиска (начальная популяция)
//		time_format( ((double)time_local_search_b) / num_runs + time_local_search_b_pre, "Average time of LS (begin)", sw);
//		time_format( ((double)time_local_search_b) / num_runs + time_local_search_b_pre, "Average time of LS (begin)", sw_1);
//
//		// среднее время локального поиска (конечная популяция)
//		//time_format(((double)time_local_search_f) / num_runs, "Average time of LS (end)", sw);
//		//time_format(((double)time_local_search_f) / num_runs, "Average time of LS (end)", sw_1);
//
//
//		sw->WriteLine();
//		sw->Close();
//
//		sw_1->WriteLine();
//		sw_1->WriteLine();
//
//		vns.~GA_path();
//
//	}//цикл по задачам
//
//
//	 //вывод 2-х таблиц: 1) 1-ый важнее 2-го; 2) 2-ой важнее 1-го
//	 //вывод статистики по сужению по всем задачам
//	if ( reduction )
//	{
//		if (quantum_inf < _1ST_2ND_ + _2ND_1ST_)
//		{
//			//ТАБЛ. 1) 1-ый важнее 2-го
//			print_exp_red(sw_4, "1st criterion is more important then the 2nd", index_reduced_1_2, total_num_P_approx);
//
//			//ТАБЛ. 2) 2-ой важнее 1-го
//			print_exp_red(sw_4, "2nd criterion is more important then the 1st", index_reduced_2_1, total_num_P_approx);
//		}
//		else
//		{
//			for (int i = 0; i < index_reduced_tmp.size(); i++)
//				index_reduced_both[i] = index_reduced_both[i]/ num_prbl_val;
//			double aver_num = 0;
//			for (int i = 0; i < total_num_P_approx.size(); i++)
//				aver_num += total_num_P_approx[i];
//			print_exp_red_two(sw_4, "average statistics", index_reduced_both, NUM_STEPS_TETA+1, aver_num / total_num_P_approx.size());
//		}
//
//	}
//
//	//общее время по всем задачам
//	time_format(total_time, "Total time of all problems", sw_1);
//	time_format(total_time, "Total time of all problems", sw_3);
//	time_format(total_time, "Total time of all problems", sw_4);
//
//
//	sw_1->Close();
//	sw_3->Close();
//	sw_4->Close();
//	sr->Close();
//
//	printf("\n");
//	//system("pause");
//
//	return 0;
//}
