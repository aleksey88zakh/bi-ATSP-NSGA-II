// MTSP_GA_1.cpp: определяет точку входа для консольного приложения.
//

//#include "stdafx.h"
#include <iostream>
#include <string>
#include "GA_for_MTSP_source.h"
#include "archive.h"
#include "stat.cpp"
//#include "MTSP_VNS_1.cpp"

using namespace std;
using namespace System;
using namespace System::IO;


int main_GA(int argc, char* argv[])
{

	//число городов
	int num_n = 12;
	//флаг числа городов не по умолчанию
	bool is_num_n_specified = false;

	//число особей
	vector<int> num_N;

	//число итерация
	int num_iter = 200;

	//кол-во запусков на одной задаче
	int num_runs = 1;

	//массив времени работы на каждой задаче
	//если кол-во указанных времен < кол-ва задач,
	//то для ост. задач время выполнения = 0 (=останов по числу итераций)
	vector<unsigned> time_run;

	//имя файла, с которого считываем
	String^ file_name_rd_str = "Example_MTSP_m2_n20_N50_12.txt";//ПОМЕНЯТЬ!!!

	//имя задачи в файле
	String^ problem_name_str;	//ПОМЕНЯТЬ!!!

	//чтение множества Парето из файла
	String^ file_name_source_Pareto_set_str;

	//оператор рекомбинации
	recomb_oper rec_oper = recomb_oper::DEC_new;

	//флаги локального поиска
	bool flag_no_LS_begin = false;
	bool flag_no_LS_end = false;
	float _alpha_LS_[] = { 0.5, 0.5, 0.5 };
	float _beta_LS_[] = {0.5, 0.5, 0.5};

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
	double total_dist_middle_1 = 0;
	double total_dist_middle_2 = 0;
	double total_dist_end_1 = 0;
	double total_dist_end_2 = 0;

	double max_dist_begin_1 = 0;
	double max_dist_begin_2 = 0;
	double max_dist_middle_1 = 0;
	double max_dist_middle_2 = 0;
	double max_dist_end_1 = 0;
	double max_dist_end_2 = 0;

	double min_dist_begin_1 = 0;
	double min_dist_begin_2 = 0;
	double min_dist_middle_1 = 0;
	double min_dist_middle_2 = 0;
	double min_dist_end_1 = 0;
	double min_dist_end_2 = 0;

	//для статистики аппроксимации мн-ва Парето
	double total_num_approx_P_begin = 0;
	double total_num_approx_P_middle = 0;
	double total_num_approx_P_end = 0;

	int max_num_approx_P_begin = 0;
	int max_num_approx_P_middle = 0;
	int max_num_approx_P_end = 0;

	int min_num_approx_P_begin = 0;
	int min_num_approx_P_middle = 0;
	int min_num_approx_P_end = 0;

	double total_num_approx_in_P_begin = 0;
	double total_num_approx_in_P_middle = 0;
	double total_num_approx_in_P_end = 0;

	int max_num_approx_in_P_begin = 0;
	int max_num_approx_in_P_middle = 0;
	int max_num_approx_in_P_end = 0;

	int min_num_approx_in_P_begin = 0;
	int min_num_approx_in_P_middle = 0;
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
//			is_num_n_specified = true;
			continue;
		}

		if (str_temp == "\\N")
		{
			int j = 1;
			try
			{
				while (1)
				{
					//если параметр /N написан последним
					//и считали все значения времени
					if (i + j == argc)
					{
						i = argc;
						break;
					}
					num_N.push_back(stoi(argv[i + j]));
					j++;
				}
			}
			catch (std::invalid_argument)
			{
				i = i + j - 1;
				continue;
			}
		}

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

		if (str_temp == "\\TIME")
		{
			int j = 1;
			try
			{
				while (1)
				{
					//если параметр /TIME написан последним
					//и считали все значения времени
					if (i + j == argc)
					{
						i = argc;
						break;
					}
					// в мсек.
					time_run.push_back(stof(argv[i + j]) * 1000);
					j++;
				}
			}
			catch (std::invalid_argument)
			{
				i = i + j - 1;
				continue;
			}

		}

		if (str_temp == "\\ALPHA_LS")
		{
			_alpha_LS_[0] = stof(argv[i + 1]);
			_alpha_LS_[1] = stof(argv[i + 2]);
			_alpha_LS_[2] = stof(argv[i + 3]);
			continue;
		}

		if (str_temp == "\\BETA_LS")
		{
			_beta_LS_[0] = stof(argv[i + 1]);
			_beta_LS_[1] = stof(argv[i + 2]);
			_beta_LS_[2] = stof(argv[i + 3]);
			continue;
		}

		if (str_temp == "\\NO_LS_begin")
			flag_no_LS_begin = true;

		if (str_temp == "\\NO_LS_end")
			flag_no_LS_end = true;

		if (str_temp == "\\REDUCE")
			reduction = true;
		
		if (str_temp == "\\DPX")
			rec_oper = recomb_oper::DPX;

		if (str_temp == "\\Q_learn")
			rec_oper = recomb_oper::CH_MAX;
	}

	
	//открываем для считывания
	StreamReader^ sr = gcnew StreamReader(file_name_rd_str);
	
	//короткий файл для записи
	String^ file_name_1_st = "results/Example_MTSP_m2_n" + num_n + "_summary.csv";
	StreamWriter^ sw_1 = gcnew StreamWriter(file_name_1_st);

	//файл для записи эксперимента по сужению мн-ва Парето
	String^ file_name_red_P_set_str = "results/Example_MTSP_m2_n" + num_n + "_red_P_set.csv";
	StreamWriter^ sw_3 = gcnew StreamWriter(file_name_red_P_set_str);

	//короткий файл для записи эксперимента по сужению мн-ва Парето
	String^ file_name_short_red_P_set_str = "results/Example_MTSP_m2_n" + num_n + "_red_P_set_short.csv";
	StreamWriter^ sw_4 = gcnew StreamWriter(file_name_short_red_P_set_str);
	sw_4->WriteLine("Reduction of the Pareto set");
	sw_4->WriteLine();

	//файл для вывода аппрокимации мн-ва Парето в отдельный файл для последующего вычисления гиперобъема
																 /*Example_MTSP_m2_n50_1_10.txt*/
	StreamWriter^ sw_5 = gcnew StreamWriter("results/Pareto_set_GA_" + file_name_rd_str->Substring(0, file_name_rd_str->LastIndexOf(".txt")) + ".csv");
		
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

	//если указано только одно время,
	//то его устанавливаем для всех задач
	if (1 == time_run.size())
	{
		for (int i = 0; i < num_prbl_val; i++)
			time_run.push_back(time_run[0]);
	}
	//для задач, у которых не указано время,
	//запускаем на число итераций (время=0)
	else
	{
		int time_run_size = time_run.size();
		for (int i = 0; i < num_prbl_val - time_run_size; i++)
			time_run.push_back(0);
	}

	//если указан только один размер популяции,
	//то его устанавливаем для всех задач
	if (1 == num_N.size())
	{
		for (int i = 1; i < num_prbl_val; i++)
			num_N.push_back(num_N[0]);
	}
	//для задач, у которых не указан размер популяции, уставливаем N = 50
	else
	{
		int num_N_size = num_N.size();
		for (int i = 0; i < num_prbl_val - num_N_size; i++)
			num_N.push_back(50);
	}


//ЗАДАЧИ
//ЦИКЛ ПО ВСЕМ ЗАДАЧАМ
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
        //если число городов не задано с командной строки
		if (!is_num_n_specified)
		{
			while (cur_line_str != "n=")
				cur_line_str = sr->ReadLine();
			cur_line_str = sr->ReadLine();
			str_temp = "";
			int k = 0;
			while (k < cur_line_str->Length)
				str_temp += cur_line_str[k++];
			num_n = stoi(str_temp);
		}

		//значения аргументов конструктора ГА (по порядку)
		//число городов
		//число особей
		//число критериев
		//длина максимального пути (?)
		GA_path ga(num_n, num_N[iter_prbl], 2, 100);


		
		//функция reserve() почему-то не работает
		//s_temp.reserve(10);
		//for (int i = 0; i < 10; i++)
		//	s_temp[i].reserve(10);

		
		int var_rand = 10;
		
		//ФАЙЛЫ ДЛЯ ЧТЕНИЯ И ЗАПИСИ

#ifdef _DEBUG_ 
		String^ file_name_st = "results/Example_MTSP_m2_n" + ga.get_n() + "_N" + ga.get_N() + "_" + problem_name_str + ".csv";
		if (MODIF)
			file_name_st = "results/Example_MTSP_m2_n" + ga.get_n() + "_N" + ga.get_N() + "_" + problem_name_str + "_" + rec_oper.ToString("g") + "_modif_temp.csv";
		else
			file_name_st = "results/Example_MTSP_m2_n" + ga.get_n() + "_N" + ga.get_N() + "_" + problem_name_str + "_" + rec_oper.ToString("g") + "_temp.csv";
		StreamWriter^ sw = gcnew StreamWriter(file_name_st);
#endif

		


		//чтение множества Парето из файла (если присвоена строка с именем)
		//String^ file_name_source_Pareto_set_str = "Example_MTSP_m2_n20_N50_1_10_Temp.csv";
		vector<vector<int>> phi_Pareto_set;
		if (file_name_source_Pareto_set_str)
			phi_Pareto_set = read_Pareto_set_from_file(file_name_source_Pareto_set_str, problem_name_str, ga.get_m());
		
		//счетчик числа совпадений аппроксимации и мн-ва Парето
		ga.count_P_eq_approx = 0;

		//выписываем параметры задачи
		sw_1->WriteLine(problem_name_str);
		sw_1->WriteLine("n; {0}", ga.get_n());
		sw_1->WriteLine("m; {0}", ga.get_m());
		sw_1->WriteLine("N; {0}", ga.get_N());
		sw_1->WriteLine("iter; {0}", num_iter);
		sw_1->WriteLine("run; {0}", num_runs);
		if (flag_no_LS_begin)
			sw_1->WriteLine("NO_LS_begin");
		else
		{
			sw_1->WriteLine("LS begin alpha; {0:F2}", _alpha_LS_[0]);
			sw_1->WriteLine("LS begin beta; {0:F2}", _beta_LS_[0]);
		}
		sw_1->WriteLine("LS middle alpha; {0:F2}", _alpha_LS_[1]);
		sw_1->WriteLine("LS middle beta; {0:F2}", _beta_LS_[1]);
		if (flag_no_LS_end)
			sw_1->WriteLine("NO_LS_end");
		else
		{
			sw_1->WriteLine("LS end alpha; {0:F2}", _alpha_LS_[2]);
			sw_1->WriteLine("LS end beta; {0:F2}", _beta_LS_[2]);
		}
		sw_1->WriteLine("tourn size; {0}", ga.get_tourn_size());
		sw_1->WriteLine("p_mut; {0:F2}", ga.get_p_mut());
		sw_1->WriteLine();
		

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


//		ga.init();
//<<<
		//сбрасываем значения перед циклом по всем запускам
		total_dist_begin_1 = 0;
		total_dist_begin_2 = 0;
		total_dist_middle_1 = 0;
		total_dist_middle_2 = 0;
		total_dist_begin_2 = 0;
		total_dist_end_1 = 0;
		total_dist_end_2 = 0;

		max_dist_begin_1 = 0;
		max_dist_begin_2 = 0;
		max_dist_middle_1 = 0;
		max_dist_middle_2 = 0;
		max_dist_end_1 = 0;
		max_dist_end_2 = 0;

		min_dist_begin_1 = 0;
		min_dist_begin_2 = 0;
		min_dist_middle_1 = 0;
		min_dist_middle_2 = 0;
		min_dist_end_1 = 0;
		min_dist_end_2 = 0;

		total_num_approx_P_begin = 0;
		total_num_approx_P_middle = 0;
		total_num_approx_P_end = 0;

		max_num_approx_P_begin = 0;
		max_num_approx_P_middle = 0;
		max_num_approx_P_end = 0;

		min_num_approx_P_begin = 0;
		min_num_approx_P_middle = 0;
		min_num_approx_P_end = 0;

		total_num_approx_in_P_begin = 0;
		total_num_approx_in_P_middle = 0;
		total_num_approx_in_P_end = 0;

		max_num_approx_in_P_begin = 0;
		max_num_approx_in_P_middle = 0;
		max_num_approx_in_P_end = 0;

		min_num_approx_in_P_begin = 0;
		min_num_approx_in_P_middle = 0;
		min_num_approx_in_P_end = 0;

		//для сужения делаем только один запуск
		if (reduction)
			num_runs = 1;
///>>>

		//засекаем время начала работы алгоритма для данной задачи
		//считаем общее время для одной задачи по всем запускам
		unsigned long long start_time = ::GetTickCount();


//ПРЕПРОЦЕССИНГ для локального поиска
		if (!flag_no_LS_begin)
		{
			//построение перестановки pi (переменная index_pi)
			//сохраняем все дуги в один массив
			//vector<vector<int>> c_all(ga.get_n()*ga.get_n() - ga.get_n(), vector<int>(2));
			//c_all = ga.get_c_all();
			//? определение fronts_struct_temp с размерностью
			vector<vector<int>> tmp; //заглушка
			ga.i_rank_pi = ga.range_front_J(ga.c_max, tmp, ga.index_pi, 0);
			tmp.clear();
			//sw->WriteLine("Permutation pi");
			printf("Permutation pi\n");
			for (int i = 0; i < ga.index_pi.size(); i++)
			{
				//sw->Write("{0};", ga.index_pi[i]);
				printf("%d\t", ga.index_pi[i]);
			}
			//sw->WriteLine();
			printf("\n");
			//sw->WriteLine("Ranks");
			printf("Ranks\n");
			for (int i = 0; i < ga.index_pi.size(); i++)
			{
				//sw->Write("{0};", ga.i_rank_pi[i]);
				printf("%d\t", ga.i_rank_pi[i]);
			}
			//sw->WriteLine();
			printf("\n");


			//построение перестановок p_i (переменная index_p)
			//sw->WriteLine("Permutations p");
			printf("Permutations p\n");
			vector<vector<int>> c_max_temp;
			for (int i = 0; i < ga.get_n(); i++)
			{
				c_max_temp.clear();
				for (int j = 0; j < ga.get_n(); j++)
					//сохраняем весь массив вектров (в том числе при i=j)
					//чтобы использовать в ф-ции постр. рангов
					c_max_temp.push_back({ ga.s_m[0][i][j], ga.s_m[1][i][j] });

				vector<int> index_front_temp;
				vector<vector<int>> tmp; //заглушка
				ga.range_front_J(c_max_temp, tmp, index_front_temp, 0);
				//первым всегда будет вектор (0, 0) (при i=j)
				//поэтому начинаем с k=1
				for (int k = 1; k < index_front_temp.size(); k++)
					ga.index_p[i].push_back(index_front_temp[k]);
				index_front_temp.clear();
				tmp.clear();

				//вывод
				//sw->WriteLine("i = {0}", i + 1);
				printf("i = %d\n", i);
				for (int j = 0; j < ga.index_p[i].size(); j++)
				{
					//sw->Write("{0};", ga.index_p[i][j]);
					printf("%d\t", ga.index_p[i][j]);
				}
				//sw->WriteLine();
				printf("\n");
			}

			//sw->WriteLine();
			printf("\n");
		}
		//засекаем время препроцессинга
		long time_local_search_b_pre = ::GetTickCount() - start_time;
		//время локального поиска (в начале)
		long long time_local_search_b = 0;
		//время локального поиска (в конце)
		long long time_local_search_f = 0;

////////////////////////////////////////////////////////////////////////

		//ЗАПУСКИ
		//цикл по запускаем (на одной задаче)
		for (int index_run = 0; index_run < num_runs; index_run++)
		{
			//sw->WriteLine("Problem {0} Run {1}", iter_prbl + 1, index_run + 1);
			//для рандомизатора
			temp_time = ::GetTickCount();
			srand(temp_time);

			//засекаем время работы на одном run
			unsigned long start_time_run = ::GetTickCount();

			//родители нач популяции
			vector<int> p1, p2, p3, p4;
			p1 = ga.patching_algorithm(ga.s_m[0], ga.c_max_all[0], false);
			p2 = ga.patching_algorithm(ga.s_m[0], ga.c_max_all[0], true);
			p3 = ga.patching_algorithm(ga.s_m[1], ga.c_max_all[1], false);
			p4 = ga.patching_algorithm(ga.s_m[1], ga.c_max_all[1], true);

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


			//0. начальная популяция
			temp_time = ::GetTickCount(); // для рандомизатора
			ga.init_pop(ga.s_m, ga.c_max_all[0], temp_time, p1, p2, p3, p4);

			//sw->WriteLine("Initial population");
			//sw_1->WriteLine("Initial population");
			printf("Initial population (problem %d):\n", iter_prbl+1);
			ga.output_pop();
	

			//заполняем значение векторного критерия по популяции

			//sw->WriteLine("Values of vector criterion (initial population)");
			printf("Values of vector criterion (initial population, problem %d):\n", iter_prbl+1);
			
			//заполнение значений критерия
			ga.calc_phi();

			//вывод значений критерия
			ga.output_phi();
			

//-----------------------------------------------------------------------------------------------
//----------ЛОКАЛЬНЫЙ ПОИСК (нач. поп.)----------------------------------------------------------
			//время локального поиска (в начале)
			time_local_search_b -= ::GetTickCount();
			if (!flag_no_LS_begin)
			{
				// ПРЕПРОЦЕССИНГ см. выше
				// выполняется один раз перед всеми запусками одной задачи


				//ПРИМЕНЕНИЕ ЛОКАЛЬНОГО ПОИСКА К ОСОБЯМ ИЗ МАССИВА pop			
				vector<int> pop_new;
				printf("Local search\n");
				printf("\n");
				//sw->WriteLine("Local search");
				//sw->WriteLine();

				for (int i = 0; i < ga.get_N(); i++)
					ga.pop[i] = ga.local_search(ga.pop[i], _alpha_LS_[0], _beta_LS_[0]);

				//sw->WriteLine("after LS");
				printf("afret LS:\n");
				ga.output_pop();

				//заполняем значение векторного критерия по популяции
				//sw->WriteLine("Values of vector criterion:");
				printf("Values of vector criterion:\n");

				//заполнение значений критерия
				ga.calc_phi();

				//вывод значений критерия
				ga.output_phi();
				
			}

			//засекаем время локального поиска (в начале)
			time_local_search_b += ::GetTickCount();

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------





//------------ПОСТРОЕНИЕ ФРОНТОВ
			//true - популяция в явном виде отсорирована по фронтам + ранги фронтов заданы в векторе i_rank
			//false - популяция в явном виде не отсорирована по фронтам, ранги фронтов заданы в векторе i_rank
			vector<int> tmp2; //заглушка
			ga.i_rank = ga.range_front_J(ga.phi, ga.pop, tmp2, 1);
			tmp2.clear();
			//значение векторного критерия отсортированной популяции
			//sw->WriteLine("Values of vector criterion (sorted):");
			printf("Values of vector criterion (sorted):\n");

			//заполнение значений критерия
			ga.calc_phi();

			//вывод значений критерия
			ga.output_phi();
			

			//sw->WriteLine("Ranks of poulation (initial population)");
			//sw_1->WriteLine("Ranks of poulation");
			printf("Ranks of poulation (initial population, problem %d):\n", iter_prbl+1);
			ga.output_ranks();
	

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

			//sw->WriteLine("Crowding distances of population (initial population)");
			printf("Crowding distances of population (initial population, problem %d):\n", iter_prbl+1);
			ga.output_dist();

			//вывод значений векторного критерия аппроксимации мн-ва Парето для НАЧАЛЬНОЙ популяции
			ga.phi_P_approx.clear();
			ga.build_phi_P_approx();

			//sw->WriteLine("Approximation of the Pareto set (initial population)");
			//sw_1->WriteLine("Approximation of the Pareto set({0} iterations)", iter);
			for (int i = 0; i < ga.get_m(); i++)
			{
				for (int j = 0; j < ga.phi_P_approx.size(); j++)
				{
					//sw->Write("{0};", ga.phi_P_approx[j][i]);
					//sw_1->Write("{0};", ga.phi_P_approx[j][i]);
				}
				//sw->WriteLine();
				//sw_1->WriteLine();
			}

			//sw->WriteLine();
			//sw_1->WriteLine();

			//вычисление метрики для НАЧАЛЬНОЙ популяции - аппроксимации множества Парето
			//если мн-во Парето задано
			if (file_name_source_Pareto_set_str)
			{
				//заголовок
				if (index_run == 0)
				{
					sw_1->WriteLine("N_PS; {0}", phi_Pareto_set.size());
					sw_1->WriteLine();

					sw_1->Write("Begin; ; ; ;");
					sw_1->Write("End before LS; ; ; ;");
					sw_1->WriteLine("End after LS; ; ; ;");

					//в начале после ЛП
					sw_1->Write("Metric 'Convergence approximation to Pareto set';");
					sw_1->Write("Metric 'Convergence Pareto set to approximation';");
					sw_1->Write("Points in approx to Pareto set;");
					sw_1->Write("Points from Pareto set;");

					//в конце до ЛП
					sw_1->Write("Metric 'Convergence approximation to Pareto set';");
					sw_1->Write("Metric 'Convergence Pareto set to approximation';");
					sw_1->Write("Points in approx to Pareto set;");
					sw_1->Write("Points from Pareto set;");

					//в конце после ЛП
					sw_1->Write("Metric 'Convergence approximation to Pareto set';");
					sw_1->Write("Metric 'Convergence Pareto set to approximation';");
					sw_1->Write("Points in approx to Pareto set;");
					sw_1->Write("Points from Pareto set;");

					if (time_run[iter_prbl] > 0)
						sw_1->Write("N of iter;");

					sw_1->WriteLine("Run");
				}

				//вычисляем метрики
				ga.count_P_eq_approx = 0;
				ga.evaluate_metric_of_approx(sw_1, phi_Pareto_set, true);
				//ga.evaluate_metric_of_approx(sw, phi_Pareto_set, false);
				//sw->WriteLine();
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
			//если мн-во Парето не задано
			else 
			{
				//заголовок
				if (index_run == 0)
				{
					sw_1->Write("Begin (after LS); ;");
					sw_1->Write("End (before LS); ;");
					sw_1->WriteLine("End (after LS); ;");

					sw_1->Write("Rank 1;");
					sw_1->Write("Hyper vol;");
					sw_1->Write("Rank 1;");
					sw_1->Write("Hyper vol;");
					sw_1->Write("Rank 1;");

					sw_1->Write("Hyper vol;");
					
					if (time_run[iter_prbl] > 0)	
						sw_1->Write("N of iter;");
					
					sw_1->WriteLine("Run");
					
				}
				sw_1->Write("{0};", ga.phi_P_approx.size());
				sw_1->Write("{0};", ga.hyper_volume({ ga.c_max_all[0]*ga.get_n(), ga.c_max_all[1]*ga.get_n() }, ga.phi_P_approx));
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

//обучение с подкреплением
			//функция выигрыша в кроссинговере 
	//		vector<vector<float>> Q_cross[2];
	//		/*for (int i = 0; i < ga.get_N(); i++)
	//			for (int j = 0; j < ga.get_N(); j++)
	//			{
	//
	//			}*/
	//		Q_cross[0] = vector<vector<float>>(ga.get_N(), vector<float>(ga.get_N(), 0));
	//		Q_cross[1] = vector<vector<float>>(ga.get_N(), vector<float>(ga.get_N(), 0));
	//		float alpha_rec = 1.0;


//ОСНОВНОЙ ЦИКЛ ПО ИТЕРАЦИЯМ
			int iter = 0;
//			unsigned long cur_time_start = ::GetTickCount();
			unsigned long cur_time = ::GetTickCount() - start_time_run;
			                   /*условие на кол-во итераций*/         /*условие на время*/
			while ((++iter <= num_iter && time_run[iter_prbl] == 0) || (cur_time <= time_run[iter_prbl]))
			{
				ga.pop_R_t = ga.pop; //R_t = P_t
#ifdef GA_MAIN //потомок - одна особь
				for (int i = 0; i < ga.get_N(); i++)
				{
#else //потомок - архив
				while (1)
				{
#endif
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
//TO DO: переписать мутацию
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

					vector<int> phi_p1 = ga.multi_phitness(ga.pop[i_p1]);
					vector<int> phi_p2 = ga.multi_phitness(ga.pop[i_p2]);

#ifdef GA_MAIN //потомок - одна особь
					switch (rec_oper)
					{
						case recomb_oper::DEC_new:
							child = ga.DEC_new(ga.s_m, p1, p2);
							if (MODIF)
							{
								// если потомок совпадает с одним из родителей или хуже, применяем DPX
								vector<int> phi_child = ga.multi_phitness(child);
										/*потомок совпадает с одним из родителей*/
								if ( (phi_child == phi_p1) || (phi_child == phi_p2)  ||
										/*потомок хуже одного из родителей*/
									 Pareto_pref(phi_p1, phi_child) || Pareto_pref(phi_p2, phi_child) )
									child = ga.DPX(ga.s_m, p1, p2);
							}
							break;
						case recomb_oper::DPX:
							child = ga.DPX(ga.s_m, p1, p2);
							break;
						//обучение с подрекплением
//						case recomb_oper::CH_MAX:
//							
//							// инициализируем значения текущего состояния
//							int i_first, i_second;
//							if (ga.i_rank[i_p1] > ga.i_rank[i_p2])
//							{
//								i_first = ga.i_rank[i_p2];
//								i_second = ga.i_rank[i_p1];
//							}
//							else
//							{
//								i_first = ga.i_rank[i_p1];
//								i_second = ga.i_rank[i_p2];
//							}
//
//							if (Q_cross[0][i_first][i_second] > Q_cross[1][i_first][i_second])
//							{
//// проверить формат записи p1, p2, child
//								child = ga.DEC_new(ga.s_m, p1, p2);
//								Q_cross[0][i_first][i_second] = Q_cross[0][i_first][i_second] + alpha_rec * ga.update_rate_crossover(ga.pop[i_p1], ga.pop[i_p2], child);
//								printf("Q_DEC[%d, %d] = %.2f\n", i_first, i_second, Q_cross[0][i_first][i_second]);
//								sw->WriteLine("Q_DEC[{0}, {1}]; {2:F2};", i_first, i_second, Q_cross[0][i_first][i_second]);
//							}
//							else
//								if (Q_cross[0][i_first][i_second] < Q_cross[1][i_first][i_second])
//								{
//									child = ga.DPX(ga.s_m, p1, p2);
//									Q_cross[1][i_first][i_second] = Q_cross[1][i_first][i_second] + alpha_rec * ga.update_rate_crossover(ga.pop[i_p1], ga.pop[i_p2], child);
//									printf("Q_DPX[%d, %d] = %.2f\n", i_first, i_second, Q_cross[1][i_first][i_second]);
//									sw->WriteLine("Q_DPX[{0}, {1}]; {2:F2}", i_first, i_second, Q_cross[1][i_first][i_second]);
//								}
//								else // в случае равенства выбираем один из операторов с равной вероятностью
//								{
//									//для рандомизатора
//									temp_time = ::GetTickCount();
//									srand(temp_time);
//
//									double rand_mut = (double)rand() / RAND_MAX;
//									if (rand_mut < 0.5)
//									{
//										child = ga.DEC_new(ga.s_m, p1, p2);
//										Q_cross[0][i_first][i_second] = Q_cross[0][i_first][i_second] + alpha_rec * ga.update_rate_crossover(ga.pop[i_p1], ga.pop[i_p2], child);
//										printf("Q_DEC[%d, %d] = %.2f\n", i_first, i_second, Q_cross[0][i_first][i_second]);
//										sw->WriteLine("Q_DEC[{0}, {1}]; {2:F2};", i_first, i_second, Q_cross[0][i_first][i_second]);
//									}
//									else
//									{
//										child = ga.DPX(ga.s_m, p1, p2);
//										Q_cross[1][i_first][i_second] = Q_cross[1][i_first][i_second] + alpha_rec * ga.update_rate_crossover(ga.pop[i_p1], ga.pop[i_p2], child);
//										printf("Q_DPX[%d, %d] = %.2f\n", i_first, i_second, Q_cross[1][i_first][i_second]);
//										sw->WriteLine("Q_DPX[{0}, {1}]; {2:F2}", i_first, i_second, Q_cross[1][i_first][i_second]);
//									}
//								}
//
//					
//							break;
					}

					//мутируем потомка, у кототорого значения критерия совпадает хотя бы с одним из родителей
					//Юлечка
					if (MODIF)
					{					
						// применяем мутацию обмена, если потомок хуже одного из родителей
						// после DPX потомок не может совпадать с родителями
						if ( Pareto_pref(phi_p1, ga.multi_phitness(child)) || Pareto_pref(phi_p2, ga.multi_phitness(child)) )
						{
							//для рандомизатора
							temp_time = ::GetTickCount();
							srand(temp_time);
						
							//vector<int> child_new(ga.get_n());
							{
								double rand_mut = (double)rand() / RAND_MAX;
								if (rand_mut < 0.5)
									child = ga.exchange_mutation(ga.pop[i_p1]);
								else
									child = ga.exchange_mutation(ga.pop[i_p2]);
								//child = child_new;
							}
						}

						
					}

					//применяем ЛП к потомку после рекомбинации
					child = ga.local_search(child, _alpha_LS_[1], _beta_LS_[1]);

					ga.pop_R_t.push_back(child);//R_t = P_t + child

#else //потомок - архив

					Archive arch_ga;
					arch_ga.archive.push_back(ga.pop[i_p1]);
					arch_ga.archive.push_back(ga.pop[i_p2]);
					arch_ga.val_crit_archive.push_back(phi_p1);
					arch_ga.val_crit_archive.push_back(phi_p2);
					arch_ga.ar_index_not_cons_lst.push_back(0);
					arch_ga.ar_index_not_cons_lst.push_back(1);

					//потомок
					child = ga.DEC_new(ga.s_m, p1, p2);
					vector<int> phi_child = ga.multi_phitness(child);

					//формируем потомка-архив
					//сравниваем потомка с архивом
					if (arch_ga.check_new(phi_child))
						arch_ga.arch_modify(child, phi_child);


					// просматриваем окрестность потомка
					ga.local_search_VNS_new(child, _alpha_LS_[1], _beta_LS_[1], &arch_ga);

					//R_t = P_t + archive
					list<unsigned>::const_iterator it_lst;
					for (it_lst = arch_ga.ar_index_not_cons_lst.cbegin(); it_lst != arch_ga.ar_index_not_cons_lst.cend(); it_lst++)
					{
						if (arch_ga.val_crit_archive[*it_lst] != phi_p1 && arch_ga.val_crit_archive[*it_lst] != phi_p2)
							ga.pop_R_t.push_back(arch_ga.archive[*it_lst]);
					}

					if (ga.pop_R_t.size() >= 2 * ga.get_N())
						break;

#endif
				}

#ifdef GA_MAIN
				ga.set_ext_N(2*ga.get_N());
#else
				ga.set_ext_N(ga.pop_R_t.size());
				ga.phi.resize(ga.get_ext_N());
				ga.i_rank_R_t.resize(ga.get_ext_N());
				ga.i_dist_R_t.resize(ga.get_ext_N());
#endif // GA_MAIN




				//R_t = P_t + Q_t

				//ФОРМИРОВАНИЕ НОВОЙ ПОПУЛЯЦИИ

				//вычисление ранга особей
				//printf("Ranks of poulation:\n");
				//true - популяция в явном виде отсорирована про фронтам, i_rank_R_t - такой же порядок (ранги фронтов)
				ga.calc_ext_phi();
				vector<int> tmp2; //заглушка
				ga.i_rank_R_t = ga.range_front_J(ga.phi, ga.pop_R_t, tmp2, 1);
				tmp2.clear();
				//for (int i = 0; i < ga.get_N(); i++)
				//	printf("%d\t", ga.i_rank[i]);
				//printf("\n");

				//ранг последенго фронта, который целиком войдет в новую популяцию
				int index_rank_last = ga.i_rank_R_t[ga.get_N()] - 1;

				//для проверки, что расстояния сортируются, если все ранги = 1
				int temp = 0;
				if ((index_rank_last == 0) && (ga.i_rank_R_t[ga.get_ext_N() - 1] == 1))
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
				for (int s = 0; s < ga.get_ext_N(); s++)
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
					i_stop_last_front_exceeded = ga.get_ext_N() - 1;

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


				//формируем значения векторного критерия аппроксимации мн-ва Парето (без повторов)
				ga.phi_P_approx.clear();
				ga.build_phi_P_approx();
				
				
				//выводим популяцию, ранги, расстояния и аппрокимацию мн-ва Парето (векторный критерий) без повторов
				if (  ( ((iter % FREQ_SHOW_COMP) == 0) && file_name_source_Pareto_set_str ) || 
					( (iter % FREQ_SHOW_RED ) == 0)  )
				{
					//str_temp = problem_name_str->ToString + "\n";
					printf("Problem %d\n", iter_prbl+1);
					//sw->WriteLine("Run {0}, Iteration {1}", index_run+1, iter);
					//sw_1->WriteLine("Iteration {0}", iter);
					printf("----Run %d, Iteration %d------\n", index_run+1, iter);
					ga.output_pop();

					//sw->WriteLine("Ranks of poulation (problem {0}, run {1}, iteration {2})", iter_prbl+1, index_run+1, iter);
					printf("Ranks of poulation (problem %d, run %d, iteration %d):\n", iter_prbl+1, index_run+1, iter);
					printf("Start index of last front: %d\n", i_start_last_front_exceeded);
					printf("Final index of last front: %d\n", i_stop_last_front_exceeded);
					ga.output_ranks();

					//расстояния популяции
					//sw->WriteLine("Crowding distances of population (problem {0}, run {1}, iteration {2})", iter_prbl+1, index_run+1, iter);
					printf("Crowding distances of population (problem %d, run %d, iteration %d):\n", iter_prbl+1, index_run+1, iter);
					ga.output_dist();

					//значания критериев
					//sw->WriteLine("Values of vector criterion (problem {0}, run {1}, iteration {2})", iter_prbl+1, index_run+1, iter);
					printf("Values of vector criterion (problem %d, run %d, iteration %d):\n", iter_prbl+1, index_run+1, iter);
					//заполнение значений критерия
// TO DO: перенести заполнение (не вывод) до "лок. поиска в конце итераций"
					ga.calc_phi();

					//вывод значений критерия
					ga.output_phi();
						
					
					//sw->WriteLine("Approximation of the Pareto set (problem {0}, run {1}, iteration {2})", iter_prbl+1, index_run+1, iter);
					//sw_1->WriteLine("Approximation of the Pareto set({0} iterations)", iter);
					for (int i = 0; i < ga.get_m(); i++)
					{
						for (int j = 0; j < ga.phi_P_approx.size(); j++)
						{
							//sw->Write("{0};", ga.phi_P_approx[j][i]);
							//sw_1->Write("{0};", ga.phi_P_approx[j][i]);
						}
						//sw->WriteLine();
						//sw_1->WriteLine();
					}

					//sw->WriteLine();
					//sw_1->WriteLine();

					//вычисление метрики - аппроксимации множества Парето (если выше присвоено имя файла)
					/*if (file_name_source_Pareto_set_str)
					{
						ga.evaluate_metric_of_approx(sw, phi_Pareto_set, false);
						sw->WriteLine();
					}*/

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
			
				//обновляем текущее время работы
				cur_time = ::GetTickCount() - start_time_run;

			}//основной цикл итераций


			//до ЛП
			//формируем значения векторного критерия аппроксимации мн-ва Парето (без повторов)
			ga.phi_P_approx.clear();
			ga.build_phi_P_approx();
			//вычисление метрики для КОНЕЧНОЙ популяции до ЛП - аппроксимации множества Парето
			//если мн-во Парето задано
			if (file_name_source_Pareto_set_str)
			{
				//вычисляем метрики
				ga.count_P_eq_approx = 0;
				ga.evaluate_metric_of_approx(sw_1, phi_Pareto_set, true);
				sw_1->Write("{0};", ga.count_P_eq_approx);

				//для средней метрики по всем запускам
				total_dist_middle_1 += ga.dist_conver_approx_to_P_set_val;
				total_dist_middle_2 += ga.dist_conver_P_set_to_approx_val;

				total_num_approx_P_middle += ga.phi_P_approx.size();
				total_num_approx_in_P_middle += ga.count_P_eq_approx;

				if (0 == index_run)
				{
					max_dist_middle_1 = min_dist_middle_1 = ga.dist_conver_approx_to_P_set_val;
					max_dist_middle_2 = min_dist_middle_2 = ga.dist_conver_P_set_to_approx_val;
					max_num_approx_P_middle = min_num_approx_P_middle = ga.phi_P_approx.size();
					max_num_approx_in_P_middle = min_num_approx_in_P_middle = ga.count_P_eq_approx;
				}

				//определение максимума метрики по всем запускам
				if (max_dist_middle_1 < ga.dist_conver_approx_to_P_set_val)
					max_dist_middle_1 = ga.dist_conver_approx_to_P_set_val;
				if (max_dist_middle_2 < ga.dist_conver_P_set_to_approx_val)
					max_dist_middle_2 = ga.dist_conver_P_set_to_approx_val;

				if (max_num_approx_P_middle < ga.phi_P_approx.size())
					max_num_approx_P_middle = ga.phi_P_approx.size();
				if (max_num_approx_in_P_middle < ga.count_P_eq_approx)
					max_num_approx_in_P_middle = ga.count_P_eq_approx;

				//определение минимума метрики по всем запускам
				if (min_dist_middle_1 > ga.dist_conver_approx_to_P_set_val)
					min_dist_middle_1 = ga.dist_conver_approx_to_P_set_val;
				if (min_dist_middle_2 > ga.dist_conver_P_set_to_approx_val)
					min_dist_middle_2 = ga.dist_conver_P_set_to_approx_val;

				if (min_num_approx_P_middle > ga.phi_P_approx.size())
					min_num_approx_P_middle = ga.phi_P_approx.size();
				if (min_num_approx_in_P_middle > ga.count_P_eq_approx)
					min_num_approx_in_P_middle = ga.count_P_eq_approx;
			}
			else //если мн-во Парето не задано
			{
				sw_1->Write("{0};", ga.phi_P_approx.size());
				sw_1->Write("{0};", ga.hyper_volume({ ga.c_max_all[0]*ga.get_n(), ga.c_max_all[1]*ga.get_n() }, ga.phi_P_approx));
			}

			//если запуск был на время, выводим результат последней итерации
			if (time_run[iter_prbl] > 0)
			{
				//sw->WriteLine("Run {0}, Iteration {1}", index_run + 1, iter);
				//for (int j = 0; j < ga.get_n(); j++)
				//{
				//	for (int i = 0; i < ga.get_N(); i++)
				//		sw->Write("{0};", ga.pop[i][j]);
				//	sw->WriteLine();
				//}
				//sw->WriteLine();

				//sw->WriteLine("Ranks of poulation (problem {0}, run {1}, iteration {2})", iter_prbl + 1, index_run + 1, iter);
				//for (int i = 0; i < ga.get_N(); i++)
				//	sw->Write("{0};", ga.i_rank[i]);
				//sw->WriteLine();

				////расстояния популяции
				//sw->WriteLine("Crowding distances of population (problem {0}, run {1}, iteration {2})", iter_prbl + 1, index_run + 1, iter);
				//for (int i = 0; i < ga.get_N(); i++)
				//	sw->Write("{0:F2};", ga.i_dist[i]);
				//sw->WriteLine();

				////значания критериев
				//sw->WriteLine("Values of vector criterion (problem {0}, run {1}, iteration {2})", iter_prbl + 1, index_run + 1, iter);
				
				//заполнение значений критерия
				// TO DO: перенести заполнение (не вывод) до "лок. поиска в конце итераций"
				ga.calc_phi();

				//вывод значений критерия
				/*for (int j = 0; j < ga.get_m(); j++)
				{
					for (int i = 0; i < ga.get_N(); i++)
						sw->Write("{0};", ga.phi[i][j]);
					sw->WriteLine();
				}
				sw->WriteLine();*/
			
				/*sw->WriteLine("Approximation of the Pareto set (problem {0}, run {1}, iteration {2})", iter_prbl + 1, index_run + 1, iter);
				for (int i = 0; i < ga.get_m(); i++)
				{
					for (int j = 0; j < ga.phi_P_approx.size(); j++)
						sw->Write("{0};", ga.phi_P_approx[j][i]);
					sw->WriteLine();
				}
				sw->WriteLine();*/

				//вычисление метрики - аппроксимации множества Парето (если выше присвоено имя файла)
				/*if (file_name_source_Pareto_set_str)
				{
					ga.evaluate_metric_of_approx(sw, phi_Pareto_set, false);
					sw->WriteLine();
				}*/

			}

/////////////////////////////////////////////////////////
//////////////////// ЛОКАЛЬНЫЙ ПОИСК (конеч. поп.)
////////////////////////////////////////////////////////

			//время локального поиска (в конце)
			time_local_search_f -= ::GetTickCount();
			if (!flag_no_LS_end)
			{
				//формируем архив из особей с раном 1
				Archive arch(&ga);
				//ПРИМЕНЕНИЕ ЛОКАЛЬНОГО ПОИСКА К ОСОБЯМ ИЗ АРХИВА
				vector<int> new_to_arch;
				printf("\n Local search at the end\n");
				//sw->WriteLine();
				//sw->WriteLine("Local search at the end");

				// цикл, пока архив непросмотренных не пуст
				while (!arch.ar_index_not_cons_lst.empty())
				{
					//для рандомизатора
					temp_time = ::GetTickCount();
					srand(temp_time);
					unsigned i_tmp = rand() % arch.ar_index_not_cons_lst.size();

					// переходим к элементу архива, соотв. случ. сгенерированному индексу
					list<unsigned>::const_iterator it_lst = arch.ar_index_not_cons_lst.cbegin();
					advance(it_lst, i_tmp);
					// дальше итератор it_lst использовать нельзя,
					// т.к. на его позиции удаляется элелемент ar_index_not_cons_lst
					i_tmp = *it_lst;

					// локальный поиск
					vector<int> new_to_arch = ga.local_search(arch.archive[i_tmp], _alpha_LS_[2], _beta_LS_[2], &arch);
					if (!new_to_arch.empty()) // не пустой, если i_lst был продоминирован
					{
						// добавляем в список неживых
						arch.ar_index_no_lst.push_back(*it_lst);
						// удаляем из непросмотренных
						arch.ar_index_not_cons_lst.erase(it_lst);

						// сравниваем с архивом
						if (arch.check_new(ga.multi_phitness(new_to_arch)))
							arch.arch_modify(new_to_arch, ga.multi_phitness(new_to_arch));
					}
					else // i_lst не был продоминирован
					{
						// добавляем к просмотренным
						arch.ar_index_cons_lst.push_back(*it_lst);
						// удаляем из непросмотренных
						arch.ar_index_not_cons_lst.erase(it_lst);
					}

				}

				//выводим архив
				//заполняем аппроксимацию мн-ва Парето
				ga.phi_P_approx.clear();
				//sw->WriteLine("Archive after LS at the end");
				printf("Archive after LS at the end: \n");
				for (int j = 0; j < ga.get_n(); j++)
				{
					//все живые особи = все просмотренные особи
					list<unsigned>::const_iterator it_lst;
					for (it_lst = arch.ar_index_cons_lst.cbegin(); it_lst != arch.ar_index_cons_lst.cend(); )
					{
						//sw->Write("{0};", arch.archive[*it_lst][j]);
						printf("%d\t", arch.archive[*it_lst][j]);
						if (j == 0) // только на одной итерации внешнего цикла
							ga.phi_P_approx.push_back(arch.val_crit_archive[*it_lst]);
						it_lst++;
					}
					//sw->WriteLine();

				}

				//выводим аппроксимацию мн-ва Парето
				//sw->WriteLine("Approx of the Pareto set after LS (end)");
				printf("Approx of the Pareto set after LS (end): \n");
				ga.output_P_approx();


				//вычисление метрики после локального поиска в финальной популяции
				//sw->WriteLine("After LS at the end (archive)");
				/*if (file_name_source_Pareto_set_str)
				{
					ga.evaluate_metric_of_approx(sw, phi_Pareto_set, false);
					sw->WriteLine();
				}*/
			}
			//засекаем время локального поиска (в конце)
			time_local_search_f += ::GetTickCount();

			//выводим аппрокимацию мн-ва Парето в отдельный файл для последующего вычисления гиперобъема
			sw_5->WriteLine(problem_name_str + ";");
			sw_5->WriteLine("Pareto Set;");
			for (int i = 0; i < ga.phi_P_approx.size(); i++)
			{
				for (int j = 0; j < ga.get_m(); j++)
					sw_5->Write("{0};", ga.phi_P_approx[i][j]);
				sw_5->WriteLine();
			}
			sw_5->WriteLine("N; {0};", ga.phi_P_approx.size());

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

		

			//ПОСЛЕ ОСНОВНОГО ЦИКЛА ИТЕРАЦИЙ и ЛП
			//вычисление метрики - аппроксимации множества Парето (если выше присвоено имя файла)
			if (file_name_source_Pareto_set_str)
			{
				//в файл sw выводим в цикле итераций
				ga.count_P_eq_approx = 0;
				ga.evaluate_metric_of_approx(sw_1, phi_Pareto_set, true);
				//sw->WriteLine();
				sw_1->Write("{0};", ga.count_P_eq_approx);
				if (time_run[iter_prbl] > 0)
					sw_1->Write("{0};", iter);
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
				/*sw->WriteLine("The Pareto set");
				for (int i = 0; i < ga.get_m(); i++)
				{
					for (int j = 0; j < phi_Pareto_set.size(); j++)
					{
						sw->Write("{0};", phi_Pareto_set[j][i]);
					}
					sw->WriteLine();
				}
				sw->WriteLine();*/
			}
			else
			{
				sw_1->Write("{0};", ga.phi_P_approx.size());
				sw_1->Write("{0};", ga.hyper_volume({ ga.c_max_all[0]*ga.get_n(), ga.c_max_all[1]*ga.get_n() }, ga.phi_P_approx));
				
				if (time_run[iter_prbl] > 0)
					sw_1->Write("{0};", iter);
			
				sw_1->WriteLine("{0};", index_run + 1);
			}

			//очищаем популяцию и ранги
			ga.pop.clear();
			for (int l = 0; l < ga.get_N(); l++)
				ga.i_rank[l] = 0;
//			arch.~Archive();

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
			total_dist_middle_1 = total_dist_middle_1 / num_runs;
			total_dist_end_1 = total_dist_end_1 / num_runs;
			total_dist_begin_2 = total_dist_begin_2 / num_runs;
			total_dist_middle_2 = total_dist_middle_2 / num_runs;
			total_dist_end_2 = total_dist_end_2 / num_runs;

			total_num_approx_P_begin = total_num_approx_P_begin / num_runs;
			total_num_approx_P_middle = total_num_approx_P_middle / num_runs;
			total_num_approx_P_end = total_num_approx_P_end / num_runs;

			total_num_approx_in_P_begin = total_num_approx_in_P_begin / num_runs;
			total_num_approx_in_P_middle = total_num_approx_in_P_middle / num_runs;
			total_num_approx_in_P_end = total_num_approx_in_P_end / num_runs;
						
			/*sw->Write("{0:F4};", total_dist_begin_1);
			sw->Write("{0:F4};", total_dist_begin_2);
			sw->Write("{0:F2};", total_num_approx_P_begin);
			sw->Write("{0:F2};", total_num_approx_in_P_begin);
			sw->Write("{0:F4};", total_dist_middle_1);
			sw->Write("{0:F4};", total_dist_middle_2);
			sw->Write("{0:F2};", total_num_approx_P_middle);
			sw->Write("{0:F2};", total_num_approx_in_P_middle);
			sw->Write("{0:F4};", total_dist_end_1);
			sw->Write("{0:F4};", total_dist_end_2);
			sw->Write("{0:F2};", total_num_approx_P_end);
			sw->Write("{0:F2};", total_num_approx_in_P_end);
			sw->WriteLine("Average");

			sw->Write("{0:F4};", max_dist_begin_1);
			sw->Write("{0:F4};", max_dist_begin_2);
			sw->Write("{0};", max_num_approx_P_begin);
			sw->Write("{0};", max_num_approx_in_P_begin);
			sw->Write("{0:F4};", max_dist_middle_1);
			sw->Write("{0:F4};", max_dist_middle_2);
			sw->Write("{0};", max_num_approx_P_middle);
			sw->Write("{0};", max_num_approx_in_P_middle);
			sw->Write("{0:F4};", max_dist_end_1);
			sw->Write("{0:F4};", max_dist_end_2);
			sw->Write("{0};", max_num_approx_P_end);
			sw->Write("{0};", max_num_approx_in_P_end);
			sw->WriteLine("Max");

			sw->Write("{0:F4};", min_dist_begin_1);
			sw->Write("{0:F4};", min_dist_begin_2);
			sw->Write("{0};", min_num_approx_P_begin);
			sw->Write("{0};", min_num_approx_in_P_begin);
			sw->Write("{0:F4};", min_dist_middle_1);
			sw->Write("{0:F4};", min_dist_middle_2);
			sw->Write("{0};", min_num_approx_P_middle);
			sw->Write("{0};", min_num_approx_in_P_middle);
			sw->Write("{0:F4};", min_dist_end_1);
			sw->Write("{0:F4};", min_dist_end_2);
			sw->Write("{0};", min_num_approx_P_end);
			sw->Write("{0};", min_num_approx_in_P_end);
			sw->WriteLine("Min");
			sw->WriteLine();*/

			sw_1->Write("{0:F4};", total_dist_begin_1);
			sw_1->Write("{0:F4};", total_dist_begin_2);
			sw_1->Write("{0};", total_num_approx_P_begin);
			sw_1->Write("{0};", total_num_approx_in_P_begin);
			sw_1->Write("{0:F4};", total_dist_middle_1);
			sw_1->Write("{0:F4};", total_dist_middle_2);
			sw_1->Write("{0};", total_num_approx_P_middle);
			sw_1->Write("{0};", total_num_approx_in_P_middle);
			sw_1->Write("{0:F4};", total_dist_end_1);
			sw_1->Write("{0:F4};", total_dist_end_2);
			sw_1->Write("{0};", total_num_approx_P_end);
			sw_1->Write("{0};", total_num_approx_in_P_end);
			sw_1->WriteLine("Average");
			
			sw_1->Write("{0:F4};", max_dist_begin_1);
			sw_1->Write("{0:F4};", max_dist_begin_2);
			sw_1->Write("{0};", max_num_approx_P_begin);
			sw_1->Write("{0};", max_num_approx_in_P_begin);
			sw_1->Write("{0:F4};", max_dist_middle_1);
			sw_1->Write("{0:F4};", max_dist_middle_2);
			sw_1->Write("{0};", max_num_approx_P_middle);
			sw_1->Write("{0};", max_num_approx_in_P_middle);
			sw_1->Write("{0:F4};", max_dist_end_1);
			sw_1->Write("{0:F4};", max_dist_end_2);
			sw_1->Write("{0};", max_num_approx_P_end);
			sw_1->Write("{0};", max_num_approx_in_P_end);
			sw_1->WriteLine("Max");

			sw_1->Write("{0:F4};", min_dist_begin_1);
			sw_1->Write("{0:F4};", min_dist_begin_2);
			sw_1->Write("{0:F2};", min_num_approx_P_begin);
			sw_1->Write("{0:F2};", min_num_approx_in_P_begin);
			sw_1->Write("{0:F4};", min_dist_middle_1);
			sw_1->Write("{0:F4};", min_dist_middle_2);
			sw_1->Write("{0:F2};", min_num_approx_P_middle);
			sw_1->Write("{0:F2};", min_num_approx_in_P_middle);
			sw_1->Write("{0:F4};", min_dist_end_1);
			sw_1->Write("{0:F4};", min_dist_end_2);
			sw_1->Write("{0:F2};", min_num_approx_P_end);
			sw_1->Write("{0:F2};", min_num_approx_in_P_end);
			sw_1->WriteLine("Min");
			sw_1->WriteLine();
		}
		else
		{
			sw_1->WriteLine();
		}


		//вычисляем время решения одной задачи (все запуски)
		unsigned long long result_alg_time = stop_time - start_time - time_local_search_f - time_local_search_b - time_local_search_b_pre;
		unsigned long long result_time = stop_time - start_time;
		total_time += result_time; //общее время решения всех задач
		//printf("Time: %d\n", result_time);
		//time_format(result_time, "Total time (one problem)", sw);
		time_format(result_time, "Total time (one problem)", sw_1);
		time_format(result_time, "Total time (one problem)", sw_3);
		sw_3->WriteLine();
		sw_3->WriteLine();

		sw_4->WriteLine(problem_name_str);
		time_format(result_time, "Total time (one problem)", sw_4);
		sw_4->WriteLine();


		if (num_runs > 1)
		{
			aver_time = result_time / num_runs;
			//time_format(aver_time, "Average time through all runs (one problem)", sw);
			time_format(aver_time, "Average time through all runs (one problem)", sw_1);
		}


		// среднее время работы алгоритма без локальных поисков
		//time_format(((double)result_alg_time) / num_runs, "Average time of algorithm", sw);
		time_format(((double)result_alg_time) / num_runs, "Average time of algorithm", sw_1);

		// среднее время локального поиска (начальная популяция)
		//time_format( ((double)time_local_search_b) / num_runs + time_local_search_b_pre, "Average time of LS (begin)", sw);
		time_format( ((double)time_local_search_b) / num_runs + time_local_search_b_pre, "Average time of LS (begin)", sw_1);

		// среднее время локального поиска (конечная популяция)
		//time_format(((double)time_local_search_f) / num_runs, "Average time of LS (end)", sw);
		time_format(((double)time_local_search_f) / num_runs, "Average time of LS (end)", sw_1);


		//sw->WriteLine();
		//sw->Close();

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

	}

	//общее время по всем задачам
	time_format(total_time, "Total time of all problems", sw_1);
	time_format(total_time, "Total time of all problems", sw_3);
	time_format(total_time, "Total time of all problems", sw_4);


	sw_1->Close();
	sw_3->Close();
	sw_4->Close();
	sw_5->Close();
	sr->Close();

	printf("\n");
	//system("pause");

	return 0;
}


int main_VNS(int argc, char* argv[])
{
	//число городов
	int num_n = 12;
	//флаг числа городов не по умолчанию
	bool is_num_n_specified = false;

	//число особей
	vector<int> num_N;

	//число итерация
	int num_iter = 200;

	//кол-во запусков на одной задаче
	int num_runs = 1;

	// массив времени работы на каждой задаче
	//если кол-во указанных времен < кол-ва задач,
	//то для ост. задач время выполнения = 0 (=останов по числу итераций)
	vector<unsigned> time_run;

	//имя файла, с которого считываем
	String^ file_name_rd_str = "Example_MTSP_m2_n20_N50_12.txt";//ПОМЕНЯТЬ!!!

																//имя задачи в файле
	String^ problem_name_str;	//ПОМЕНЯТЬ!!!

								//чтение множества Парето из файла
	String^ file_name_source_Pareto_set_str;

	//VNS с мультистартом
	//максимальное число необновления элиты
	int _max_iter_elite_VNS_multi_ = 5;
	//максимальное число необновления архива
	int _max_iter_arch_VNS_multi_ = 5;
	//флаг, задано ли _max_iter_arch_VNS_multi_ пользователем
	bool flag_max_iter_arch_VNS_multi = false;

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
	vector<double> index_reduced_both(SIZE_EXCL_TETA + 19); //1-ый важнее 2-го + 2-ой важнее 1-го (таблица, разложенная по строке)
	vector<double> index_reduced_tmp(SIZE_EXCL_TETA + 19); //хранение текущих значения (два "кванта информации")


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
//			is_num_n_specified = true;
			continue;
		}

		if (str_temp == "\\N")
		{
			int j = 1;
			try
			{
				while (1)
				{
					//если параметр /N написан последним
					//и считали все значения времени
					if (i + j == argc)
					{
						i = argc;
						break;
					}
					num_N.push_back(stoi(argv[i + j]));
					j++;
				}
			}
			catch (std::invalid_argument)
			{
				i = i + j - 1;
				continue;
			}
		}

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

		if (str_temp == "\\TIME")
		{
			int j = 1;
			try
			{
				while (1)
				{
					//если параметр /TIME написан последним
					//и считали все значения времени
					if (i + j == argc)
					{
						i = argc;
						break;
					}
					// в мсек.
					time_run.push_back(stof(argv[i + j]) * 1000);
					j++;
				}
			}
			catch (std::invalid_argument)
			{
				i = i + j - 1;
				continue;
			}

		}

		if (str_temp == "\\MAX_ARCH_VNS_MULTI")
		{
			_max_iter_arch_VNS_multi_ = stoi(argv[i + 1]);
			flag_max_iter_arch_VNS_multi = true;
			continue;
		}

		if (str_temp == "\\MAX_ELITE_VNS_MULTI")
		{
			_max_iter_elite_VNS_multi_ = stoi(argv[i + 1]);
			continue;
		}
	}

	//открываем для считывания
	StreamReader^ sr = gcnew StreamReader(file_name_rd_str);

	//короткий файл для записи
	String^ file_name_1_st = "results/Example_MTSP_m2_n" + num_n + "_summary.csv";
	StreamWriter^ sw_1 = gcnew StreamWriter(file_name_1_st);

	//файл для записи эксперимента по сужению мн-ва Парето
	String^ file_name_red_P_set_str = "results/Example_MTSP_m2_n" + num_n + "_red_P_set.csv";
	StreamWriter^ sw_3 = gcnew StreamWriter(file_name_red_P_set_str);

	//короткий файл для записи эксперимента по сужению мн-ва Парето
	String^ file_name_short_red_P_set_str = "results/Example_MTSP_m2_n" + num_n +  "_red_P_set_short.csv";
	StreamWriter^ sw_4 = gcnew StreamWriter(file_name_short_red_P_set_str);
	sw_4->WriteLine("Reduction of the Pareto set");
	sw_4->WriteLine();

	//файл для вывода аппрокимации мн-ва Парето в отдельный файл для последующего вычисления гиперобъема
	StreamWriter^ sw_5 = gcnew StreamWriter("results/Pareto_set_VNS_" + file_name_rd_str->Substring(0, file_name_rd_str->LastIndexOf(".txt")) + ".csv");
	//короткий файл для VNS_multi_start
	StreamWriter^ sw_6 = gcnew StreamWriter("results/Example_MTSP_m2_n" + num_n + "_short_summary.csv");

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


	//для задач, у которых не указано время, уставливаем его в ноль
	int time_run_size = time_run.size();
	for (int i = 0; i < num_prbl_val - time_run_size; i++)
		time_run.push_back(0);

	//если указан только один размер популяции,
	//то его утанавливаем для всех задач
	if (1 == num_N.size())
	{
		for (int i = 1; i < num_prbl_val; i++)
			num_N.push_back(num_N[0]);
	}
	//для задач, у которых не указан размер популяции, уставливаем N = 50
	else
	{
		int num_N_size = num_N.size();
		for (int i = 0; i < num_prbl_val - num_N_size; i++)
			num_N.push_back(50);
	}


//ЗАДАЧИ
//ЦИКЛ ПО ВСЕМ ЗАДАЧАМ
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
		//если число городов не задано с командной строки
		if (!is_num_n_specified)
		{
			while (cur_line_str != "n=")
				cur_line_str = sr->ReadLine();
			cur_line_str = sr->ReadLine();
			str_temp = "";
			int k = 0;
			while (k < cur_line_str->Length)
				str_temp += cur_line_str[k++];
			num_n = stoi(str_temp);
		}

		//значения аргументов конструктора ГА (по порядку)
		//число городов
		//число особей (в нач. популяции)
		//число критериев
		GA_path vns(num_n, num_N[iter_prbl], 2, 100);



		//функция reserve() почему-то не работает
		//s_temp.reserve(10);
		//for (int i = 0; i < 10; i++)
		//	s_temp[i].reserve(10);


		int var_rand = 10;

		//подрообный файл для отладки
		//String^ file_name_st = "results/Example_MTSP_m2_n" + vns.get_n() + "_N" + vns.get_N() + "_" + problem_name_str + ".csv";
		//StreamWriter^ sw = gcnew StreamWriter(file_name_st);


		//чтение множества Парето из файла (если присвоена строка с именем)
		//String^ file_name_source_Pareto_set_str = "Example_MTSP_m2_n20_N50_1_10_Temp.csv";
		vector<vector<int>> phi_Pareto_set;
		if (file_name_source_Pareto_set_str)
			phi_Pareto_set = read_Pareto_set_from_file(file_name_source_Pareto_set_str, problem_name_str, vns.get_m());

		//счетчик числа совпадений аппроксимации и мн-ва Парето
		vns.count_P_eq_approx = 0;


		/*sw->WriteLine(problem_name_str);
		sw->WriteLine("n; {0}", vns.get_n());
		sw->WriteLine("m; {0}", vns.get_m());
		sw->WriteLine();*/

		sw_1->WriteLine(problem_name_str);
		//printf(problem_name_str);
		sw_1->WriteLine("n; {0}", vns.get_n());
		sw_1->WriteLine("m; {0}", vns.get_m());
		sw_1->WriteLine("N; {0}", vns.get_N());
		sw_1->WriteLine("run; {0}", num_runs);
		sw_1->WriteLine("LS alpha begin; {0:F2}", LS_VNS_BEGIN_ALPHA);
		sw_1->WriteLine("LS beta begin; {0:F2}", LS_VNS_BEGIN_BETA);
		sw_1->WriteLine("LS alpha in algorithm; {0:F2}", LS_VNS_END_ALPHA);
		sw_1->WriteLine("LS beta in algorithm; {0:F2}", LS_VNS_END_BETA);
		sw_1->WriteLine("K shaking; {0}", MAX_NUM_K_OPT);
		sw_1->WriteLine();

		sw_6->WriteLine(problem_name_str);
		sw_6->WriteLine("n; {0}", vns.get_n());
		sw_6->WriteLine("m; {0}", vns.get_m());
		sw_6->WriteLine("N; {0}", vns.get_N());
		sw_6->WriteLine("run; {0}", num_runs);
		sw_6->WriteLine("LS alpha begin; {0:F2}", LS_VNS_BEGIN_ALPHA);
		sw_6->WriteLine("LS beta begin; {0:F2}", LS_VNS_BEGIN_BETA);
		sw_6->WriteLine("LS alpha in algorithm; {0:F2}", LS_VNS_END_ALPHA);
		sw_6->WriteLine("LS beta in algorithm; {0:F2}", LS_VNS_END_BETA);
		sw_6->WriteLine("K shaking; {0}", MAX_NUM_K_OPT);
		sw_6->WriteLine();

		//файл для записи эксперимента по сужению мн-ва Парето
		if (reduction)
		{
			sw_3->WriteLine(problem_name_str);
			sw_3->WriteLine("n; {0}", vns.get_n());
			sw_3->WriteLine("m; {0}", vns.get_m());
			sw_3->WriteLine("N; {0}", vns.get_N());
			sw_3->WriteLine();

		}


//ЗАПОЛНЕНИЕ МАТРИЦ КРИТЕРИЕВ
		vns.set_matrices(sr);

		//sw->WriteLine();


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


		//статистика для VNS_multi_start
		Statistics<int> stat_VNS_MS_i[6];
		Statistics<float> stat_VNS_MS_f[4];
		Statistics<int> stat_VNS_MS_end_i[3];
		Statistics<float> stat_VNS_MS_end_f[12];


		//для сужения делаем только один запуск
		if (reduction)
			num_runs = 1;
		///>>>

		//засекаем время начала работы алгоритма для данной задачи
		//считаем общее время для одной задачи по всем запускам
		unsigned long long start_time = ::GetTickCount();


		//ПРЕПРОЦЕССИНГ для локального поиска
		//построение перестановки pi (переменная index_pi)
		//? определение fronts_struct_temp с размерностью
		vector<vector<int>> tmp; //заглушка
		vns.i_rank_pi = vns.range_front_J(vns.c_max, tmp, vns.index_pi, 0);
		tmp.clear();
		//ga.i_rank_pi = fronts_struct_temp.ranks;
		//ga.index_pi = fronts_struct_temp.fronts_index;
		//sw->WriteLine("Permutation pi");
		printf("Permutation pi\n");
		for (int i = 0; i < vns.index_pi.size(); i++)
		{
			//sw->Write("{0};", vns.index_pi[i]);
			printf("%d\t", vns.index_pi[i]);
		}
		//sw->WriteLine();
		printf("\n");
		//sw->WriteLine("Ranks");
		printf("Ranks\n");
		for (int i = 0; i < vns.index_pi.size(); i++)
		{
			//sw->Write("{0};", vns.i_rank_pi[i]);
			printf("%d\t", vns.i_rank_pi[i]);
		}
		//sw->WriteLine();
		printf("\n");


		//построение перестановок p_i (переменная index_p)
		//sw->WriteLine("Permutations p");
		printf("Permutations p\n");
		vector<vector<int>> c_max_temp;
		for (int i = 0; i < vns.get_n(); i++)
		{
			c_max_temp.clear();
			for (int j = 0; j < vns.get_n(); j++)
				//сохраняем весь массив вектров (в том числе при i=j)
				//чтобы использовать в ф-ции постр. рангов
				c_max_temp.push_back({ vns.s_m[0][i][j], vns.s_m[1][i][j] });

			vector<int> index_front_temp;
			vector<vector<int>> tmp; //заглушка
			vns.range_front_J(c_max_temp, tmp, index_front_temp, 0);
			//первым всегда будет вектор (0, 0) (при i=j)
			//поэтому начинаем с k=1
			for (int k = 1; k < index_front_temp.size(); k++)
				vns.index_p[i].push_back(index_front_temp[k]);
			index_front_temp.clear();
			tmp.clear();

			//вывод
			//sw->WriteLine("i = {0}", i + 1);
			printf("i = %d\n", i);
			for (int j = 0; j < vns.index_p[i].size(); j++)
			{
				//sw->Write("{0};", vns.index_p[i][j]);
				printf("%d\t", vns.index_p[i][j]);
			}
			//sw->WriteLine();
			printf("\n");
		}

		//sw->WriteLine();
		printf("\n");

		//засекаем время препроцессинга ЛП (в начале)
		long time_local_search_b_pre = ::GetTickCount() - start_time;
		//время локального поиска (в начале)
		long long time_local_search_b = 0;
		//время локального поиска (в конце)
		long long time_local_search_f = 0;

		// общее кол-во итераций за все запуски (run) и старты (start) (для VNS_multi_start)
		//int total_iter_run = 0;

		////////////////////////////////////////////////////////////////////////

		//ЗАПУСКИ
		//цикл по запускам (на одной задаче)
		for (int index_run = 0; index_run < num_runs; index_run++)
		{
			//sw->WriteLine("Problem {0}; Run {1}", iter_prbl + 1, index_run + 1);
			
			int iter;
			// счетчик "срабатывания" k-shaking
			int cnt_k_shaking_works = 0;

///////////////////////////////////////////////////////
//////////////////// VNSnew
//////////////////////////////////////////////////////
			if (flag_VNSnew)
			{

				//для рандомизатора
				temp_time = ::GetTickCount();
				srand(temp_time);

				//засекаем время работы на одном run
				unsigned long start_time_run = ::GetTickCount();

				//родители нач популяции
				//4 особи по Зангу
				vector<int> p1, p2, p3, p4;
				p1 = vns.patching_algorithm(vns.s_m[0], vns.c_max_all[0], false);
				p2 = vns.patching_algorithm(vns.s_m[0], vns.c_max_all[0], true);
				p3 = vns.patching_algorithm(vns.s_m[1], vns.c_max_all[1], false);
				p4 = vns.patching_algorithm(vns.s_m[1], vns.c_max_all[1], true);

				//sw->WriteLine("p1; p2; p3; p4");
				//printf("\np1\t p2\t p3\t p4\n");

				vector<int> p1_temp(vns.get_n());
				vector<int> p2_temp(vns.get_n());
				vector<int> p3_temp(vns.get_n());
				vector<int> p4_temp(vns.get_n());

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
				for (int i = 1; i < vns.get_n(); ++i)
				{
					p1_temp[i] = p1[i_temp] + 1;
					i_temp = p1[i_temp];
				}
				i_temp = 0;
				for (int i = 1; i < vns.get_n(); ++i)
				{
					p2_temp[i] = p2[i_temp] + 1;
					i_temp = p2[i_temp];
				}
				i_temp = 0;
				for (int i = 1; i < vns.get_n(); ++i)
				{
					p3_temp[i] = p3[i_temp] + 1;
					i_temp = p3[i_temp];
				}
				i_temp = 0;
				for (int i = 1; i < vns.get_n(); ++i)
				{
					p4_temp[i] = p4[i_temp] + 1;
					i_temp = p4[i_temp];
				}

				p1 = p1_temp;
				p2 = p2_temp;
				p3 = p3_temp;
				p4 = p4_temp;


				//0. начальная популяция
				temp_time = ::GetTickCount(); // для рандомизатора
				vns.init_pop(vns.s_m, vns.c_max_all[0], temp_time, p1, p2, p3, p4);

				//sw->WriteLine("Initial population");
				//sw_1->WriteLine("Initial population");
				printf("Initial population (problem %d):\n", iter_prbl + 1);
				vns.output_pop();
				

				//заполняем значение векторного критерия по популяции
				//phi[i][j], i - индекс критерия, j - индекс особи

				//sw->WriteLine("Values of vector criterion (initial population)");
				printf("Values of vector criterion (initial population, problem %d):\n", iter_prbl + 1);

				//заполнение значений критерия
				vns.calc_phi();

				//вывод значений критерия
				
				vns.output_phi();
				

				//-----------------------------------------------------------------------------------------------
				//----------ЛОКАЛЬНЫЙ ПОИСК (нач. поп.)----------------------------------------------------------

				// ПРЕПРОЦЕССИНГ см. выше
				// выполняется один раз перед всеми запусками одной задачи


				//ПРИМЕНЕНИЕ ЛОКАЛЬНОГО ПОИСКА К ОСОБЯМ ИЗ МАССИВА pop

				//время локального поиска (в начале)
				time_local_search_b -= ::GetTickCount();

				vector<int> pop_new;
				printf("Local search\n");
				printf("\n");
				//sw->WriteLine("Local search");
				//sw->WriteLine();

				for (int i = 0; i < vns.get_N(); i++)
					vns.pop[i] = vns.local_search(vns.pop[i], LS_VNS_BEGIN_ALPHA, LS_VNS_BEGIN_BETA);

				//sw->WriteLine("after LS");
				printf("afret LS:\n");
				vns.output_pop();
				

				//заполняем значение векторного критерия по популяции
				//sw->WriteLine("Values of vector criterion:");
				printf("Values of vector criterion:\n");

				//заполнение значений критерия
				vns.calc_phi();

				//вывод значений критерия
				vns.calc_phi();

				//засекаем время локального поиска (в начале)
				time_local_search_b += ::GetTickCount();

				//-----------------------------------------------------------------------------------------------
				//-----------------------------------------------------------------------------------------------





				//------------ПОСТРОЕНИЕ ФРОНТОВ
				//true - популяция в явном виде отсорирована по фронтам + ранги фронтов заданы в векторе i_rank
				//false - популяция в явном виде не отсорирована по фронтам, ранги фронтов заданы в векторе i_rank
				vector<int> tmp2; //заглушка
				vns.i_rank = vns.range_front_J(vns.phi, vns.pop, tmp2, 1);
				tmp2.clear();
				//значение векторного критерия отсортированной популяции
				//sw->WriteLine("Values of vector criterion (sorted):");
				printf("Values of vector criterion (sorted):\n");

				//заполнение значений критерия
				vns.calc_phi();

				//вывод значений критерия
				vns.output_phi();
				

				//sw->WriteLine("Ranks of population (initial population)");
				//sw_1->WriteLine("Ranks of poulation");
				printf("Ranks of poulation (initial population, problem %d):\n", iter_prbl + 1);
				vns.output_ranks();


				//формируем архив:
				//случ. популяция + ЛП
				Archive arch(&vns);

				//вывод значений векторного критерия аппроксимации мн-ва Парето для НАЧАЛЬНОЙ популяции
				//vns.phi_P_approx.clear();
				// архив = мн-во Парето
				vns.phi_P_approx = arch.val_crit_archive;

				//sw->WriteLine("Approximation of the Pareto set (initial population)");
				//sw_1->WriteLine("Approximation of the Pareto set({0} iterations)", iter);
				/*for (int i = 0; i < vns.get_m(); i++)
				{
					for (int j = 0; j < vns.phi_P_approx.size(); j++)
						sw->Write("{0};", vns.phi_P_approx[j][i]);

					sw->WriteLine();
				}*/

				//sw->WriteLine();


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
						sw_1->Write("N of iter;");
						sw_1->Write("N k_shaking;");
						sw_1->WriteLine("Run;");
					}

					//вычисляем метрики
					vns.count_P_eq_approx = 0;
					vns.evaluate_metric_of_approx(sw_1, phi_Pareto_set, true);
					//vns.evaluate_metric_of_approx(sw, phi_Pareto_set, false);
					//sw->WriteLine();
					sw_1->Write("{0};", vns.count_P_eq_approx);

					//для средней метрики по всем запускам
					if (0 == index_run)
					{
						stat_VNS_MS_f[0].init_max_min(vns.dist_conver_approx_to_P_set_val);
						stat_VNS_MS_f[1].init_max_min(vns.dist_conver_P_set_to_approx_val);

						stat_VNS_MS_i[0].init_max_min(vns.phi_P_approx.size());
						stat_VNS_MS_i[1].init_max_min(vns.count_P_eq_approx);
					}

					stat_VNS_MS_f[0].refresh(vns.dist_conver_approx_to_P_set_val);
					stat_VNS_MS_f[1].refresh(vns.dist_conver_P_set_to_approx_val);

					stat_VNS_MS_i[0].refresh(vns.phi_P_approx.size());
					stat_VNS_MS_i[1].refresh(vns.count_P_eq_approx);

				}
				else
				{
					//заголовок
					if (index_run == 0)
					{
						sw_1->Write("Begin (after LS); ;");
						sw_1->WriteLine("End; ;");

						sw_1->Write("Rank 1;");
						sw_1->Write("Hyper vol;");
						sw_1->Write("Rank 1;");
						sw_1->Write("Hyper vol;");
						sw_1->Write("N of iter;");
						sw_1->Write("N k_shaking;");
						sw_1->WriteLine("Run;");
					}

					unsigned int hyper_vol = vns.hyper_volume({ vns.c_max_all[0] * vns.get_n(), vns.c_max_all[1] * vns.get_n() }, vns.phi_P_approx);
					sw_1->Write("{0};", vns.phi_P_approx.size());
					sw_1->Write("{0};", hyper_vol);

					//для средней метрики по всем запускам
					if (0 == index_run)
					{
						stat_VNS_MS_i[0].init_max_min(vns.phi_P_approx.size());
						stat_VNS_MS_i[1].init_max_min(hyper_vol);
					}

					stat_VNS_MS_i[0].refresh(vns.phi_P_approx.size());
					stat_VNS_MS_i[1].refresh(hyper_vol);					

				}


				//СФОРМИРОВАН АРХИВ: СЛУЧ. ПОП. + ЛП			


				//ОСНОВНОЙ ЦИКЛ ПО ИТЕРАЦИЯМ
				// итерация: VNS к особи., особь выбир. случ.
				// ограничение на число итерация num_iter


				iter = 0;
				int max_num_k_opt = vns.get_n() / 2;
//				unsigned long cur_time_start = ::GetTickCount();
				unsigned long cur_time = ::GetTickCount() - start_time_run;
				         /*условие на кол-во итераций*/                        /*условие на время*/
				while ( (++iter <= num_iter && time_run[iter_prbl] == 0) || (cur_time <= time_run[iter_prbl]) )
				{    /*останов, если архив просмотрен, см. ниже*/

					/////////////////////////////////////////////////////////
					//////////////////// ОСНОВНАЯ ИТЕРАЦИЯ VNS
					////////////////////////////////////////////////////////

					unsigned i_tmp;
					//sw->WriteLine("Iteration {0}; problem {1}", iter, iter_prbl + 1);
					printf("Iteration %d, problem %d \n", iter, iter_prbl + 1);

					//для рандомизатора
					temp_time = ::GetTickCount();
					srand(temp_time);

					/*останов, если архив просмотрен*/
					if (arch.ar_index_not_cons_lst.empty())
						break;
					else
						i_tmp = rand() % arch.ar_index_not_cons_lst.size();

					// переходим к элементу архива, соотв. случ. сгенерированному индексу
					list<unsigned>::const_iterator it_lst = arch.ar_index_not_cons_lst.cbegin();
					advance(it_lst, i_tmp);
					// дальше итератор it_lst использовать нельзя,
					// т.к. на его позиции удаляется элелемент ar_index_not_cons_lst
					i_tmp = *it_lst;
					// добавляем к просмотренным
					arch.ar_index_cons_lst.push_back(i_tmp);
					// удаляем из непросмотренных
					arch.ar_index_not_cons_lst.erase(it_lst);
					//индекс окрестности (кол-во заменяемых дуг)
					int k_opt = 3;
					// новая особь
					vector<int> new_to_arch;
					//флаг "соабатывания" k-shking
					bool is_k_shaking_works = false;
					//кол-во неживых особей в архиве
					int ar_index_no_lst_old;

					// просматриваем окрестность выбранной точки
					vns.local_search_VNS_new(arch.archive[i_tmp], LS_VNS_END_ALPHA, LS_VNS_END_BETA, &arch);

					// shaking + новый ЛП по 3-opt окрестности
					while (k_opt < max_num_k_opt + 2)
					{
						new_to_arch = shaking_Kopt(arch.archive[i_tmp], k_opt);
						//фиксируем кол-во неживых особей
						ar_index_no_lst_old = arch.ar_index_no_lst.size();
						// сравниваем "центр" 3-opt окрестности c архивом
						if (arch.check_new(vns.multi_phitness(new_to_arch))) // true, если new_to_arch не был продоминирован
						{
							//"центр" 3-opt окрестности продоминировал особь в архве
							// => кол-во неживых особей в архиве увеличилось
							if (arch.ar_index_no_lst.size() > ar_index_no_lst_old)
								is_k_shaking_works = true;
							arch.arch_modify(new_to_arch, vns.multi_phitness(new_to_arch));
						}

						// новый ЛП
						// true, если найдено улучшение в окрестности                                          /*центр окр. продоминировал точку в архиве*/ 
						if ( vns.local_search_VNS_new(new_to_arch, LS_VNS_END_ALPHA, LS_VNS_END_BETA, &arch) || is_k_shaking_works)
						{
							is_k_shaking_works = true;
							break;
						}
						else
							k_opt += 2;
					}

					if (is_k_shaking_works)
						cnt_k_shaking_works++;

					//sw->WriteLine("k_neigh = {0}", k_opt);

					//sw->WriteLine("Population");
					printf("Population \n");
					vns.phi_P_approx.clear();
					for (int j = 0; j < vns.get_n(); j++)
					{
						for (it_lst = arch.ar_index_not_cons_lst.cbegin(); it_lst != arch.ar_index_not_cons_lst.cend(); it_lst++)
						{
							//sw->Write("{0};", arch.archive[*it_lst][j]);
							printf("%d\t", arch.archive[*it_lst][j]);
							if (j == 0) // только на одной итерации внешнего цикла
								vns.phi_P_approx.push_back(arch.val_crit_archive[*it_lst]);
						}
						for (it_lst = arch.ar_index_cons_lst.cbegin(); it_lst != arch.ar_index_cons_lst.cend(); it_lst++)
						{
							//sw->Write("{0};", arch.archive[*it_lst][j]);
							printf("%d\t", arch.archive[*it_lst][j]);
							if (j == 0) // только на одной итерации внешнего цикла
								vns.phi_P_approx.push_back(arch.val_crit_archive[*it_lst]);

						}
						//sw->WriteLine();
						printf("\n");
					}

					//sw->WriteLine("Values of vector criterion");
					printf("Values of vector criterion \n");
					vns.output_P_approx();
					
					//вычисление метрики после локального поиска в финальной популяции
					/*sw->WriteLine("Metrics after VNS");
					if (file_name_source_Pareto_set_str)
					{
						vns.evaluate_metric_of_approx(sw, phi_Pareto_set, false);
						sw->WriteLine();
					}*/

					////////////////////////////////////////////////////////////////////////////////////
					////////////////////////////////////////////////////////////////////////////////////				

					//обновляем текущее время работы
					cur_time = ::GetTickCount() - start_time_run;

				}//основной цикл итераций

				 //ПОСЛЕ ОСНОВНОГО ЦИКЛА ИТЕРАЦИЙ
				 //вычисление метрики - аппроксимации множества Парето (если выше присвоено имя файла)
				if (file_name_source_Pareto_set_str)
				{
					//в файл sw выводим в цикле итераций
					vns.count_P_eq_approx = 0;
					vns.evaluate_metric_of_approx(sw_1, phi_Pareto_set, true);
					//sw->WriteLine();
					sw_1->Write("{0};", vns.count_P_eq_approx);
					//см. после else
					//sw_1->Write("{0};", index_run + 1);
					//sw_1->WriteLine("{0};", iter); //кол-во итераций


					//для средней метрики по всем запускам
					if (0 == index_run)
					{
						stat_VNS_MS_f[2].init_max_min(vns.dist_conver_approx_to_P_set_val);
						stat_VNS_MS_f[3].init_max_min(vns.dist_conver_P_set_to_approx_val);

						stat_VNS_MS_i[2].init_max_min(vns.phi_P_approx.size());
						stat_VNS_MS_i[3].init_max_min(vns.count_P_eq_approx);
					}

					stat_VNS_MS_f[2].refresh(vns.dist_conver_approx_to_P_set_val);
					stat_VNS_MS_f[3].refresh(vns.dist_conver_P_set_to_approx_val);

					stat_VNS_MS_i[2].refresh(vns.phi_P_approx.size());
					stat_VNS_MS_i[3].refresh(vns.count_P_eq_approx);


					//множество Парето (векторный критерий)
					/*sw->WriteLine("The Pareto set");
					for (int i = 0; i < vns.get_m(); i++)
					{
						for (int j = 0; j < phi_Pareto_set.size(); j++)
						{
							sw->Write("{0};", phi_Pareto_set[j][i]);
						}
						sw->WriteLine();
					}
					sw->WriteLine();*/
				}
				else
				{
					unsigned int hyper_vol = vns.hyper_volume({ vns.c_max_all[0] * vns.get_n(), vns.c_max_all[1] * vns.get_n() }, vns.phi_P_approx);

					sw_1->Write("{0};", vns.phi_P_approx.size()); // кол-во точек в аппроксимации мн-ва Парето
					sw_1->Write("{0};", hyper_vol);
					//см. после else
					//sw_1->Write("{0};", iter); //кол-во итераций
					//sw_1->Write("{0};", cnt_k_shaking_works); //счетчик "срабатывания k-shaking"
					//sw_1->WriteLine("{0};", index_run + 1); // номер запуска

					//для средней метрики по всем запускам
					if (0 == index_run)
					{
						stat_VNS_MS_i[2].init_max_min(vns.phi_P_approx.size());
						stat_VNS_MS_i[3].init_max_min(hyper_vol);
					}			

					stat_VNS_MS_i[2].refresh(vns.phi_P_approx.size());
					stat_VNS_MS_i[3].refresh(hyper_vol);

				}
				sw_1->Write("{0};", iter); //кол-во итераций
				sw_1->Write("{0};", cnt_k_shaking_works); //счетчик "срабатывания k-shaking"
				sw_1->WriteLine("{0};", index_run + 1); // номер запуска
				
				if (0 == index_run)
				{
					stat_VNS_MS_i[4].init_max_min(iter);
					stat_VNS_MS_i[5].init_max_min(cnt_k_shaking_works);
				}
				stat_VNS_MS_i[4].refresh(iter);
				stat_VNS_MS_i[5].refresh(cnt_k_shaking_works);



			} //VNSnew

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////


///////////////////////////////////////////////////////
//////////////////// VNS_multi_start
//////////////////////////////////////////////////////
			if (flag_VNS_multi_start)
			{
				//инициализируем элиту
					// класс Archive      | что в элите 
					// особи              = особи
					// знач критерия      = знач критерия
					// просмотр индексы   = пусто
					// непросмотр индексы = живые особи
					// нежив особи        = нежив особи
				Archive elite;
				// счетчик стационарности элиты
				int cnt_elite_not_changed = 0;
				// счетчик стартов VNS
				int cnt_start_VNS = 0;
				//обнуляем показатели статистики по start
				for (int i = 0; i < 6; i++)
					stat_VNS_MS_i[i].set_to_zero();
				for (int i = 0; i < 4; i++)
					stat_VNS_MS_f[i].set_to_zero();

				//String^ file_name_6_st = "results/Example_MTSP_m2_n" + num_n + "_N" + num_N + "_summary.csv";
				//StreamWriter^ sw_6 = gcnew StreamWriter("results/Example_MTSP_m2_n" + num_n + "_N" + num_N + "_short_summary.csv");

				//вывод кол-ва точек во мн-ве Парето
				//вывод заголовка
				if (0 == index_run)
				{
					if (file_name_source_Pareto_set_str)
					{
						sw_1->WriteLine("N_PS; {0}", phi_Pareto_set.size());
						sw_1->WriteLine();

						sw_1->Write("Begin; ; ; ;");
						sw_1->Write("Current archive; ; ; ;");
						sw_1->WriteLine("Elite;");

						sw_1->Write("Metric 'Convergence approximation to Pareto set';");
						sw_1->Write("Metric 'Convergence Pareto set to approximation';");
						sw_1->Write("Points in approx to Pareto set;");
						sw_1->Write("Points from Pareto set;");

						sw_1->Write("Metric 'Convergence approximation to Pareto set';");
						sw_1->Write("Metric 'Convergence Pareto set to approximation';");
						sw_1->Write("Points in approx to Pareto set;");
						sw_1->Write("Points from Pareto set;");

						sw_1->Write("Metric 'Convergence approximation to Pareto set';");
						sw_1->Write("Metric 'Convergence Pareto set to approximation';");
						sw_1->Write("Points in approx to Pareto set;");
						sw_1->Write("Points from Pareto set;");

						sw_1->Write("N of iterations;");
						sw_1->Write("N elite not changed;");
						sw_1->Write("N of k-shaking;");
						sw_1->Write("Start;");
						sw_1->WriteLine("Run;");


						//короткий файл для VNS_multi_start
						sw_6->WriteLine("N_PS; {0}", phi_Pareto_set.size());
						sw_6->WriteLine();

						sw_6->Write("Begin; ; ; ;");
						sw_6->Write("Current archive; ; ; ;");
						sw_6->WriteLine("Elite;");

						sw_6->Write("Metric 'Convergence approximation to Pareto set';");
						sw_6->Write("Metric 'Convergence Pareto set to approximation';");
						sw_6->Write("Points in approx to Pareto set;");
						sw_6->Write("Points from Pareto set;");

						sw_6->Write("Metric 'Convergence approximation to Pareto set';");
						sw_6->Write("Metric 'Convergence Pareto set to approximation';");
						sw_6->Write("Points in approx to Pareto set;");
						sw_6->Write("Points from Pareto set;");

						sw_6->Write("Metric 'Convergence approximation to Pareto set';");
						sw_6->Write("Metric 'Convergence Pareto set to approximation';");
						sw_6->Write("Points in approx to Pareto set;");
						sw_6->Write("Points from Pareto set;");

						sw_6->Write("N of iterations;");
						sw_6->Write("N of k-shaking;");
						sw_6->Write("N of start;");
						sw_6->WriteLine("Run;");
						
						
					}
					else
					{
						sw_1->Write("Begin; ;");
						sw_1->Write("Current archive; ;");
						sw_1->WriteLine("Elite;");

						sw_1->Write("Rank 1;");
						sw_1->Write("Hyper vol;");
						sw_1->Write("Rank 1;");
						sw_1->Write("Hyper vol;");
						sw_1->Write("Rank 1;");
						sw_1->Write("Hyper vol;");
						
						sw_1->Write("N of iterations;");
						sw_1->Write("N elite not changed;");
						sw_1->Write("N of k-shaking;");
						sw_1->Write("N of start;");
						sw_1->WriteLine("Run;");

						//короткий файл для VNS_multi_start
						sw_6->Write("Begin; ;");
						sw_6->Write("Current archive; ;");
						sw_6->WriteLine("Elite;");

						sw_6->Write("Rank 1;");
						sw_6->Write("Hyper vol;");
						sw_6->Write("Rank 1;");
						sw_6->Write("Hyper vol;");
						sw_6->Write("Rank 1;");
						sw_6->Write("Hyper vol;");

						sw_6->Write("N of iterations;");
						sw_6->Write("N of k-shaking;");
						sw_6->Write("N of start;");
						sw_6->WriteLine("Run;");

					}
				}
				
				//ЦИКЛ ПО СТАРТАМ (VNS_multi_start)
				unsigned long cur_time_start = ::GetTickCount();
				unsigned long cur_time = 0;
										/*условие на необновляемость элиты*/                                    /*условие на время*/
				while ( (cnt_elite_not_changed < _max_iter_elite_VNS_multi_ && time_run[iter_prbl] == 0) || (cur_time <= time_run[iter_prbl]))
				{
					//sw->WriteLine("Start; {0};", ++cnt_start_VNS);
					
					// обнуляем кол-во срабатываний k-shaking
					cnt_k_shaking_works = 0;

					//для рандомизатора
					temp_time = ::GetTickCount();
					srand(temp_time);

					//родители нач популяции
					//4 особи по Зангу
					vector<int> p1, p2, p3, p4;
					p1 = vns.patching_algorithm(vns.s_m[0], vns.c_max_all[0], false);
					p2 = vns.patching_algorithm(vns.s_m[0], vns.c_max_all[0], true);
					p3 = vns.patching_algorithm(vns.s_m[1], vns.c_max_all[1], false);
					p4 = vns.patching_algorithm(vns.s_m[1], vns.c_max_all[1], true);

					//sw->WriteLine("p1; p2; p3; p4");
					//printf("\np1\t p2\t p3\t p4\n");

					vector<int> p1_temp(vns.get_n());
					vector<int> p2_temp(vns.get_n());
					vector<int> p3_temp(vns.get_n());
					vector<int> p4_temp(vns.get_n());

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
					for (int i = 1; i < vns.get_n(); ++i)
					{
						p1_temp[i] = p1[i_temp] + 1;
						i_temp = p1[i_temp];
					}
					i_temp = 0;
					for (int i = 1; i < vns.get_n(); ++i)
					{
						p2_temp[i] = p2[i_temp] + 1;
						i_temp = p2[i_temp];
					}
					i_temp = 0;
					for (int i = 1; i < vns.get_n(); ++i)
					{
						p3_temp[i] = p3[i_temp] + 1;
						i_temp = p3[i_temp];
					}
					i_temp = 0;
					for (int i = 1; i < vns.get_n(); ++i)
					{
						p4_temp[i] = p4[i_temp] + 1;
						i_temp = p4[i_temp];
					}

					p1 = p1_temp;
					p2 = p2_temp;
					p3 = p3_temp;
					p4 = p4_temp;


					//0. начальная популяция
					temp_time = ::GetTickCount(); // для рандомизатора
					vns.init_pop(vns.s_m, vns.c_max_all[0], temp_time, p1, p2, p3, p4);

					//sw->WriteLine("Initial population");
					//sw_1->WriteLine("Initial population");
					printf("Initial population (problem %d):\n", iter_prbl + 1);
					vns.output_pop();
					

					//заполняем значение векторного критерия по популяции
					//phi[i][j], i - индекс критерия, j - индекс особи

					//sw->WriteLine("Values of vector criterion (initial population)");
					printf("Values of vector criterion (initial population, problem %d):\n", iter_prbl + 1);

					//заполнение значений критерия
					vns.calc_phi();

					//вывод значений критерия
					vns.output_phi();
					

					//-----------------------------------------------------------------------------------------------
					//----------ЛОКАЛЬНЫЙ ПОИСК (нач. поп.)----------------------------------------------------------

					// ПРЕПРОЦЕССИНГ см. выше
					// выполняется один раз перед всеми запусками одной задачи


					//ПРИМЕНЕНИЕ ЛОКАЛЬНОГО ПОИСКА К ОСОБЯМ ИЗ МАССИВА pop

					//время локального поиска (в начале)
					time_local_search_b -= ::GetTickCount();

					vector<int> pop_new;
					printf("Local search\n");
					printf("\n");
					//sw->WriteLine("Local search");
					//sw->WriteLine();

					for (int i = 0; i < vns.get_N(); i++)
						vns.pop[i] = vns.local_search(vns.pop[i], LS_VNS_BEGIN_ALPHA, LS_VNS_BEGIN_BETA);

					//sw->WriteLine("after LS");
					printf("afret LS:\n");
					vns.output_pop();
					
					//заполняем значение векторного критерия по популяции
					//sw->WriteLine("Values of vector criterion:");
					printf("Values of vector criterion:\n");

					//заполнение значений критерия
					vns.calc_phi();
					
					//вывод значений критерия
					vns.output_phi();
					

					//засекаем время локального поиска (в начале)
					time_local_search_b += ::GetTickCount();

					//-----------------------------------------------------------------------------------------------
					//-----------------------------------------------------------------------------------------------





					//------------ПОСТРОЕНИЕ ФРОНТОВ
					//true - популяция в явном виде отсорирована по фронтам + ранги фронтов заданы в векторе i_rank
					//false - популяция в явном виде не отсорирована по фронтам, ранги фронтов заданы в векторе i_rank
					vector<int> tmp2; //заглушка
					vns.i_rank = vns.range_front_J(vns.phi, vns.pop, tmp2, 1);
					tmp2.clear();
					//значение векторного критерия отсортированной популяции
					//sw->WriteLine("Values of vector criterion (sorted):");
					printf("Values of vector criterion (sorted):\n");

					//заполнение значений критерия
					vns.calc_phi();

					//вывод значений критерия
					vns.output_phi();
					

					//sw->WriteLine("Ranks of population (initial population)");
					//sw_1->WriteLine("Ranks of poulation");
					printf("Ranks of poulation (initial population, problem %d):\n", iter_prbl + 1);
					vns.output_ranks();
					

					//формируем архив:
					//случ. популяция + ЛП
					Archive arch(&vns);

					//вывод значений векторного критерия аппроксимации мн-ва Парето для НАЧАЛЬНОЙ популяции
					// архив = мн-во Парето
					vns.phi_P_approx = arch.val_crit_archive;

					//sw->WriteLine("Approximation of the Pareto set (initial population)");
					//sw_1->WriteLine("Approximation of the Pareto set({0} iterations)", iter);
					/*for (int i = 0; i < vns.get_m(); i++)
					{
						for (int j = 0; j < vns.phi_P_approx.size(); j++)
							sw->Write("{0};", vns.phi_P_approx[j][i]);

						sw->WriteLine();
					}

					sw->WriteLine();*/


					//вычисление метрики для НАЧАЛЬНОЙ популяции - аппроксимации множества Парето (если выше присвоено имя файла)
					if (file_name_source_Pareto_set_str)
					{					
						//вычисляем метрики
						vns.count_P_eq_approx = 0;
						vns.evaluate_metric_of_approx(sw_1, phi_Pareto_set, true);
						//vns.evaluate_metric_of_approx(sw, phi_Pareto_set, false);
						//sw->WriteLine();
						sw_1->Write("{0};", vns.count_P_eq_approx);

						//короткий файл для статистики по всем start в данном run
						//первая итерация по элите (первый старт)
						if (1 == cnt_start_VNS)
						{
							stat_VNS_MS_f[0].init_max_min(vns.dist_conver_approx_to_P_set_val);
							stat_VNS_MS_f[1].init_max_min(vns.dist_conver_P_set_to_approx_val);
							stat_VNS_MS_i[1].init_max_min(vns.count_P_eq_approx);
						}

						//для средней метрики по всем start в данном run
						stat_VNS_MS_f[0].refresh(vns.dist_conver_approx_to_P_set_val);
						stat_VNS_MS_f[1].refresh(vns.dist_conver_P_set_to_approx_val);
						stat_VNS_MS_i[1].refresh(vns.count_P_eq_approx);
						
					}
					else
					{
						unsigned int hyper_vol = vns.hyper_volume({ vns.c_max_all[0] * vns.get_n(), vns.c_max_all[1] * vns.get_n() }, vns.phi_P_approx);
						sw_1->Write("{0};", vns.phi_P_approx.size());
						sw_1->Write("{0};", hyper_vol);

						//первая итерация по элите (первый старт)
						if (1 == cnt_start_VNS)
							stat_VNS_MS_i[1].init_max_min(hyper_vol);
						
						stat_VNS_MS_i[1].refresh(hyper_vol);
					}
					if (1 == cnt_start_VNS)
						stat_VNS_MS_i[0].init_max_min(vns.phi_P_approx.size());
					stat_VNS_MS_i[0].refresh(vns.phi_P_approx.size());


					//ЦИКЛ ПО ТЕКУЩЕМУ АРХИВУ
					// итерация: VNS прим. к особи., особь выбир. случ.
					// ограничение на число итерация num_iter


					iter = 0;
					int cnt_arch_not_changed = 0;
					int max_num_k_opt = vns.get_n() / 2;
					//если пользователем не задано макс кол-во необновления текущего архива
					if (!flag_max_iter_arch_VNS_multi)
						_max_iter_arch_VNS_multi_ = arch.val_crit_archive.size();
					            /*не происходит обновление архива*/               /*условие на кол-во итераций*/
					while ( cnt_arch_not_changed < _max_iter_arch_VNS_multi_ ) // && ++iter <= num_iter )
					{       /*останов, если архив просмотрен, см. ниже*/
						
						iter++;

						//ПРИМЕНЕНИЕ ЛОКАЛЬНОГО ПОИСКА К ОСОБЯМ ИЗ АРХИВА
						//vector<int> new_to_arch;
						unsigned i_tmp;
						printf("\n Local search at the end\n");
						//sw->WriteLine();
						//sw->WriteLine("Local search at the end");

						//для рандомизатора
						temp_time = ::GetTickCount();
						srand(temp_time);

						/*останов, если архив просмотрен*/
						if (arch.ar_index_not_cons_lst.empty())
							break;
						else
							i_tmp = rand() % arch.ar_index_not_cons_lst.size();

						// переходим к элементу архива, соотв. случ. сгенерированному индексу
						list<unsigned>::const_iterator it_lst = arch.ar_index_not_cons_lst.cbegin();
						advance(it_lst, i_tmp);
						// дальше итератор it_lst использовать нельзя,
						// т.к. на его позиции удаляется элелемент ar_index_not_cons_lst
						i_tmp = *it_lst;
						// добавляем к просмотренным
						arch.ar_index_cons_lst.push_back(i_tmp);
						// удаляем из непросмотренных
						arch.ar_index_not_cons_lst.erase(it_lst);
						//индекс окрестности
						int k_opt = 3;
						vector<int> new_to_arch;

						// локальный поиск - по окр. 3-5-7-9-11 (5 типов окрестностей)
						while (k_opt < max_num_k_opt + 2)
						{
							new_to_arch = shaking_Kopt(arch.archive[i_tmp], k_opt);
							// ЛП как на начальной популяции
							new_to_arch = vns.local_search(new_to_arch, LS_VNS_END_ALPHA, LS_VNS_END_BETA);
							// сравниваем с архивом

							if (arch.check_new(vns.multi_phitness(new_to_arch))) // true, если new_to_arch не был продоминирован
							{
								arch.arch_modify(new_to_arch, vns.multi_phitness(new_to_arch));
								cnt_k_shaking_works++;
								break;
							}
							k_opt += 2;
						}

						//просмотрели все типы окр. k-shaking и не обновили архив
						if (max_num_k_opt + 2 == k_opt)
							cnt_arch_not_changed++;
						else
							cnt_arch_not_changed = 0;

						//sw->WriteLine("k_neigh = {0}", k_opt);

						//sw->WriteLine("Iteration {0}; problem {1}", iter, iter_prbl + 1);
						printf("Iteration %d, problem %d \n", iter, iter_prbl + 1);

						//sw->WriteLine("Population");
						printf("Population \n");
						for (int j = 0; j < vns.get_n(); j++)
						{
							for (it_lst = arch.ar_index_not_cons_lst.cbegin(); it_lst != arch.ar_index_not_cons_lst.cend(); )
							{
								//sw->Write("{0};", arch.archive[*it_lst][j]);
								printf("%d\t", arch.archive[*it_lst][j]);
								it_lst++;
							}
							for (it_lst = arch.ar_index_cons_lst.cbegin(); it_lst != arch.ar_index_cons_lst.cend(); )
							{
								//sw->Write("{0};", arch.archive[*it_lst][j]);
								printf("%d\t", arch.archive[*it_lst][j]);
								it_lst++;
							}
							//sw->WriteLine();
							printf("\n");
						}

						//sw->WriteLine("Values of vector criterion");
						printf("Values of vector criterion \n");
						for (int j = 0; j < vns.get_m(); j++)
						{
							for (it_lst = arch.ar_index_not_cons_lst.cbegin(); it_lst != arch.ar_index_not_cons_lst.cend(); )
							{
								//sw->Write("{0};", arch.val_crit_archive[*it_lst][j]);
								printf("%d\t", arch.val_crit_archive[*it_lst][j]);
								it_lst++;
							}
							for (it_lst = arch.ar_index_cons_lst.cbegin(); it_lst != arch.ar_index_cons_lst.cend(); )
							{
								//sw->Write("{0};", arch.val_crit_archive[*it_lst][j]);
								printf("%d\t", arch.val_crit_archive[*it_lst][j]);
								it_lst++;
							}
							//sw->WriteLine();
							printf("\n");
						}


						//вычисление метрики после локального поиска в финальной популяции
						//vns.phi_P_approx = arch.val_crit_archive;
						vns.phi_P_approx.clear();
						for (it_lst = arch.ar_index_not_cons_lst.cbegin(); it_lst != arch.ar_index_not_cons_lst.cend(); )
						{
							vns.phi_P_approx.push_back(arch.val_crit_archive[*it_lst]);
							it_lst++;
						}
						for (it_lst = arch.ar_index_cons_lst.cbegin(); it_lst != arch.ar_index_cons_lst.cend(); )
						{
							vns.phi_P_approx.push_back(arch.val_crit_archive[*it_lst]);
							it_lst++;
						}

						/*sw->WriteLine("Metrics after VNS");
						if (file_name_source_Pareto_set_str)
						{
							vns.evaluate_metric_of_approx(sw, phi_Pareto_set, false);
							sw->WriteLine();
						}*/


						////////////////////////////////////////////////////////////////////////////////////
						////////////////////////////////////////////////////////////////////////////////////				

					}//цикл по текущему архиву

					//total_iter_run += iter;

					//обновляем элиту текущим архивом
					// на 1-ой интерации: элита = текущий архив
					if (elite.archive.empty())
					{
						elite.archive = arch.archive;
						elite.val_crit_archive = arch.val_crit_archive;
						elite.ar_index_not_cons_lst = arch.ar_index_cons_lst;
						elite.ar_index_no_lst = arch.ar_index_no_lst;
					}
					else
					{
						//возращает true, если элита изменилась
						if (!elite.elite_modify(arch))
							cnt_elite_not_changed++;
						else
							cnt_elite_not_changed = 0;
					}

					// метрика по текущему архиву
					if (file_name_source_Pareto_set_str)
					{
						//в файл sw выводим в цикле итераций
						vns.count_P_eq_approx = 0;
						vns.evaluate_metric_of_approx(sw_1, phi_Pareto_set, true);
						//sw->WriteLine();
						sw_1->Write("{0};", vns.count_P_eq_approx);

						//короткий для статистики по всем start в данном run
						if (1 == cnt_start_VNS)
						{
							stat_VNS_MS_f[2].init_max_min(vns.dist_conver_approx_to_P_set_val);
							stat_VNS_MS_f[3].init_max_min(vns.dist_conver_P_set_to_approx_val);
							stat_VNS_MS_i[3].init_max_min(vns.count_P_eq_approx);
						}

						stat_VNS_MS_f[2].refresh(vns.dist_conver_approx_to_P_set_val);
						stat_VNS_MS_f[3].refresh(vns.dist_conver_P_set_to_approx_val);
						stat_VNS_MS_i[3].refresh(vns.count_P_eq_approx);
					}
					else
					{
						unsigned int hyper_vol = vns.hyper_volume({ vns.c_max_all[0] * vns.get_n(), vns.c_max_all[1] * vns.get_n() }, vns.phi_P_approx);

						sw_1->Write("{0};", vns.phi_P_approx.size()); // кол-во точек в аппроксимации мн-ва Парето
						sw_1->Write("{0};", hyper_vol);
						
						if (1 == cnt_start_VNS)
							stat_VNS_MS_i[3].init_max_min(hyper_vol);

						stat_VNS_MS_i[3].refresh(hyper_vol);
					}
					if (1 == cnt_start_VNS)
					{
						stat_VNS_MS_i[2].init_max_min(vns.phi_P_approx.size());
						stat_VNS_MS_i[4].init_max_min(iter);
						stat_VNS_MS_i[5].init_max_min(cnt_k_shaking_works);
					}
					stat_VNS_MS_i[2].refresh(vns.phi_P_approx.size());
					stat_VNS_MS_i[4].refresh(iter);
					stat_VNS_MS_i[5].refresh(cnt_k_shaking_works);

					//sw->WriteLine("N elite not changed; {0}", cnt_elite_not_changed);
					//sw_1->WriteLine("N elite not changed; {0}", cnt_elite_not_changed);

					//формируем аппроксимацию мн-ва Парето по текущей элите
					vns.phi_P_approx.clear();
					for (list<unsigned>::const_iterator it_lst = elite.ar_index_not_cons_lst.cbegin(); it_lst != elite.ar_index_not_cons_lst.cend(); it_lst++)
						vns.phi_P_approx.push_back(elite.val_crit_archive[*it_lst]);

///////////////////////////////////////////////////////////
/////////ОТЛАДКА
					/*sw->WriteLine("Cur elite");
					for (int j = 0; j < vns.get_m(); j++)
					{
						for (list<unsigned>::const_iterator it_lst = elite.ar_index_not_cons_lst.cbegin(); it_lst != elite.ar_index_not_cons_lst.cend();  it_lst++)
						{
							sw->Write("{0};", elite.val_crit_archive[*it_lst][j]);
						}
						sw->WriteLine();
					}
					sw->WriteLine();
					for (list<unsigned>::const_iterator it_lst = elite.ar_index_not_cons_lst.cbegin(); it_lst != elite.ar_index_not_cons_lst.cend(); it_lst++)
					{
						sw->Write("{0};", *it_lst);
					}
					sw->WriteLine();*/
///////////////////////////////////////////////////////////

					// метрика по элите
					if (file_name_source_Pareto_set_str)
					{
						//в файл sw выводим в цикле итераций
						vns.count_P_eq_approx = 0;
						vns.evaluate_metric_of_approx(sw_1, phi_Pareto_set, true);
						//sw->WriteLine();
						sw_1->Write("{0};", vns.count_P_eq_approx);

					}
					else
					{
						unsigned int hyper_vol = vns.hyper_volume({ vns.c_max_all[0] * vns.get_n(), vns.c_max_all[1] * vns.get_n() }, vns.phi_P_approx);
						
						sw_1->Write("{0};", vns.phi_P_approx.size()); // кол-во точек в аппроксимации мн-ва Парето
						sw_1->Write("{0};", hyper_vol);
					}
					sw_1->Write("{0};", iter); //кол-во итераций
					sw_1->Write("{0};", cnt_elite_not_changed); //счетчик необновления элиты
					sw_1->Write("{0};", cnt_k_shaking_works);   //счетчик срабатывания k-shaking
					sw_1->Write("{0};", cnt_start_VNS); //номер старта VNS
					sw_1->WriteLine("{0};", index_run + 1); //номер run

					//очищаем популяцию и ранги
					vns.pop.clear();
					for (int l = 0; l < vns.get_N(); l++)
						vns.i_rank[l] = 0;

					//обновляем текущее время работы алгоритма
					cur_time = ::GetTickCount() - cur_time_start;

				} // while (cnt_elite_not_changed < _max_iter_elite_VNS_multi_) //цикл по элите

//				iter = total_iter / cnt_start_VNS;


				  //ПОСЛЕ ЦИКЛА ПО ЭЛИТЕ
				  //вычисление метрики - аппроксимации множества Парето (если выше присвоено имя файла)
				if (file_name_source_Pareto_set_str)
				{
					//выводим статистику по данному run
					
					//архив в начале
					sw_6->Write("{0:F2};", stat_VNS_MS_f[0].get_aver());
					sw_6->Write("{0:F2};", stat_VNS_MS_f[1].get_aver());
					sw_6->Write("{0:F2};", stat_VNS_MS_i[0].get_aver());
					sw_6->Write("{0:F2};", stat_VNS_MS_i[1].get_aver());

					//архив в конце
					sw_6->Write("{0:F2};", stat_VNS_MS_f[2].get_aver());
					sw_6->Write("{0:F2};", stat_VNS_MS_f[3].get_aver());
					sw_6->Write("{0:F2};", stat_VNS_MS_i[2].get_aver());
					sw_6->Write("{0:F2};", stat_VNS_MS_i[3].get_aver());

					vns.count_P_eq_approx = 0;
					vns.evaluate_metric_of_approx(sw_6, phi_Pareto_set, true);
					sw_6->Write("{0};", vns.count_P_eq_approx);


					//для статистики по всем run
					//stat_VNS_MS_end_f - float
					//stat_VNS_MS_end_i - int
					//на первом run инициализируем max и min
					if (0 == index_run)
					{
						for (int i = 0; i < 4; i++)
							stat_VNS_MS_end_f[i].init_max_min(stat_VNS_MS_f[i].get_aver());
						for (int i = 0; i < 6; i++)
							stat_VNS_MS_end_f[i + 4].init_max_min(stat_VNS_MS_i[i].get_aver());

						stat_VNS_MS_end_f[10].init_max_min(vns.dist_conver_approx_to_P_set_val);
						stat_VNS_MS_end_f[11].init_max_min(vns.dist_conver_P_set_to_approx_val);

						stat_VNS_MS_end_i[0].init_max_min(vns.phi_P_approx.size());
						stat_VNS_MS_end_i[1].init_max_min(vns.count_P_eq_approx);
						stat_VNS_MS_end_i[2].init_max_min(cnt_start_VNS);
					}
					//обновляем
					for (int i = 0; i < 4; i++)
						stat_VNS_MS_end_f[i].refresh(stat_VNS_MS_f[i].get_aver());
					for (int i = 0; i < 6; i++)
						stat_VNS_MS_end_f[i + 4].refresh(stat_VNS_MS_i[i].get_aver());

					stat_VNS_MS_end_f[10].refresh(vns.dist_conver_approx_to_P_set_val);
					stat_VNS_MS_end_f[11].refresh(vns.dist_conver_P_set_to_approx_val);

					stat_VNS_MS_end_i[0].refresh(vns.phi_P_approx.size());
					stat_VNS_MS_end_i[1].refresh(vns.count_P_eq_approx);
					stat_VNS_MS_end_i[2].refresh(cnt_start_VNS);


					//множество Парето (векторный критерий)
					/*sw->WriteLine("The Pareto set");
					for (int i = 0; i < vns.get_m(); i++)
					{
						for (int j = 0; j < phi_Pareto_set.size(); j++)
						{
							sw->Write("{0};", phi_Pareto_set[j][i]);
						}
						sw->WriteLine();
					}
					sw->WriteLine();*/
				}
				else
				{
					//архив в начале
					sw_6->Write("{0:F2};", stat_VNS_MS_i[0].get_aver());
					sw_6->Write("{0:F2};", stat_VNS_MS_i[1].get_aver());

					//архив в конце
					sw_6->Write("{0:F2};", stat_VNS_MS_i[2].get_aver());
					sw_6->Write("{0:F2};", stat_VNS_MS_i[3].get_aver());

					//результирующая элита
					int hyper_vol = vns.hyper_volume({ vns.c_max_all[0] * vns.get_n(), vns.c_max_all[1] * vns.get_n() }, vns.phi_P_approx);
					sw_6->Write("{0};", vns.phi_P_approx.size());
					sw_6->Write("{0};", hyper_vol);


					//для статистики по всем run
					//stat_VNS_MS_end_f - float
					//stat_VNS_MS_end_i - int
					//на первом run инициализируем max и min
					if (0 == index_run)
					{
						for (int i = 0; i < 6; i++)
							stat_VNS_MS_end_f[i].init_max_min(stat_VNS_MS_i[i].get_aver());

						stat_VNS_MS_end_i[0].init_max_min(vns.phi_P_approx.size());
						stat_VNS_MS_end_i[1].init_max_min(hyper_vol);
						stat_VNS_MS_end_i[2].init_max_min(cnt_start_VNS);
					}
					//обновляем
					for (int i = 0; i < 6; i++)
						stat_VNS_MS_end_f[i].refresh(stat_VNS_MS_i[i].get_aver());

					stat_VNS_MS_end_i[0].refresh(vns.phi_P_approx.size());
					stat_VNS_MS_end_i[1].refresh(hyper_vol);
					stat_VNS_MS_end_i[2].refresh(cnt_start_VNS);

				}
				sw_6->Write("{0:F2};", stat_VNS_MS_i[4].get_aver());
				sw_6->Write("{0:F2};", stat_VNS_MS_i[5].get_aver());
				sw_6->Write("{0};", cnt_start_VNS);
				sw_6->WriteLine("{0};", index_run + 1);

				sw_1->WriteLine();

				
			} // if (VNS_multi_start)

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////


			//выводим аппрокимацию мн-ва Парето в отдельный файл для последующего вычисления гиперобъема
			sw_5->WriteLine(problem_name_str+";");
			sw_5->WriteLine("Pareto Set;");
			for (int i = 0; i < vns.phi_P_approx.size(); i++)
			{
				for (int j = 0; j < vns.get_m(); j++)
					sw_5->Write("{0};", vns.phi_P_approx[i][j]);
				sw_5->WriteLine();
			}
			sw_5->WriteLine("N; {0}", vns.phi_P_approx.size());


			 

			//очищаем популяцию и ранги
			vns.pop.clear();
			for (int l = 0; l < vns.get_N(); l++)
				vns.i_rank[l] = 0;
//			arch.~Archive();

//			sw_1->WriteLine();

		}//цикл запусков
//		sw_1->WriteLine();	

		 //время окончания решения одной задачи (все запуски)
		unsigned long long stop_time = ::GetTickCount();



		//------------------------------------------------------------------------------------
		//----------ЭКСПЕРИМЕНТ ПО СУЖЕНИЮ МН-ВА ПАРЕТО (если установлен флаг)

		if (reduction)
		{
			sw_3->WriteLine("Approximation of the Pareto set");
			for (int i = 0; i < vns.get_m(); i++)
			{
				for (int j = 0; j < vns.phi_P_approx.size(); j++)
				{
					sw_3->Write("{0};", vns.phi_P_approx[j][i]);
				}
				sw_3->WriteLine();
			}

			sw_3->WriteLine("Points in approximation");
			sw_3->WriteLine("{0}", vns.phi_P_approx.size());
			sw_3->WriteLine();


			//определение минимума, максимума и среднего числа точек в мн-ве Парето по всем запускам
			total_num_P_approx.push_back(vns.phi_P_approx.size());

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
				index_reduced_1_2.push_back(vns.experiment_reduction(sw_3, problem_name_str, 0.1, NUM_STEPS_TETA, 0, _1ST_2ND_));
				sw_3->WriteLine();
			}
			if (quantum_inf == _2ND_1ST_)
			{ //задан один "квант информации"

			  //2-ой критерий важнее 1-го
				sw_3->WriteLine("Reduction of the Pareto set");
				sw_3->WriteLine("2nd is more important than 1st");
				index_reduced_2_1.push_back(vns.experiment_reduction(sw_3, problem_name_str, 0.1, NUM_STEPS_TETA, 0, _2ND_1ST_));
				sw_3->WriteLine();
			}
			if (quantum_inf == _1ST_2ND_ + _2ND_1ST_)
			{ //задано два "кванта информации"

			  //1-ый критерий важнее 2-го + 2-ой критерий важнее 1-го
				sw_3->WriteLine("Reduction of the Pareto set");
				sw_3->WriteLine("1st -> 2nd and 2nd -> 1st");
				index_reduced_tmp = vns.experiment_reduction(sw_3, problem_name_str, 0.1, NUM_STEPS_TETA + 1, -0.1, _1ST_2ND_ + _2ND_1ST_);
				//накапливаем сумму по всем задачам, чтобы потом вычислить среднее
				//!!! инициализация index_reduced_both и index_reduced_tmp
				for (int i = 0; i < index_reduced_tmp.size(); i++)
					index_reduced_both[i] += index_reduced_tmp[i];
				sw_3->WriteLine();
				//вывод таблицы процента сужения
				print_exp_red_two(sw_4, problem_name_str, index_reduced_tmp, NUM_STEPS_TETA + 1, total_num_P_approx[iter_prbl]);


				//таблица (в случае двух "квантов информации")

			}



		}//эксперимент по сужению мн-ва Парето


		 //------------------------------------------------------------------------------------
		 //------------------------------------------------------------------------------------



//СТАТИСТИКА VNS по метрике после всех запусков
		if (flag_VNS_multi_start)
		{
			if (file_name_source_Pareto_set_str)
			{
				//выводим статистику по всем run
			
			//СРЕДНЕЕ
				//архив в начале
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[0].get_aver()); //метрика 1
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[1].get_aver()); //метрика 2
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[4].get_aver()); //точек в аппроксимации
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[5].get_aver()); //сколько из низ из мн-ва Парето

				//архив в конце
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[2].get_aver()); //метрика 1
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[3].get_aver()); //метрика 2
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[6].get_aver()); //точек в аппроксимации
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[7].get_aver()); //сколько из низ из мн-ва Парето

				//элита
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[10].get_aver()); //метрика 1
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[11].get_aver()); //метрика 2
				sw_6->Write("{0:F2};", stat_VNS_MS_end_i[0].get_aver()); //точек в аппроксимации
				sw_6->Write("{0:F2};", stat_VNS_MS_end_i[1].get_aver()); //сколько из низ из мн-ва Парето

				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[8].get_aver()); //N of iterations
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[9].get_aver()); //N of k-shaking
				sw_6->Write("{0:F2};", stat_VNS_MS_end_i[2].get_aver()); //N of start
				sw_6->WriteLine("Average");
			//

			//MAX
				//архив в начале
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[0].get_max()); //метрика 1
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[1].get_max()); //метрика 2
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[4].get_max()); //точек в аппроксимации
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[5].get_max()); //сколько из низ из мн-ва Парето

				//архив в конце
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[2].get_max()); //метрика 1
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[3].get_max()); //метрика 2
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[6].get_max()); //точек в аппроксимации
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[7].get_max()); //сколько из низ из мн-ва Парето

				//элита
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[10].get_max()); //метрика 1
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[11].get_max()); //метрика 2
				sw_6->Write("{0:F2};", stat_VNS_MS_end_i[0].get_max()); //точек в аппроксимации
				sw_6->Write("{0:F2};", stat_VNS_MS_end_i[1].get_max()); //сколько из низ из мн-ва Парето

				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[8].get_max()); //N of iterations
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[9].get_max()); //N of k-shaking
				sw_6->Write("{0:F2};", stat_VNS_MS_end_i[2].get_max()); //N of start
				sw_6->WriteLine("Max");
			//

			//MIN
				//архив в начале
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[0].get_min()); //метрика 1
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[1].get_min()); //метрика 2
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[4].get_min()); //точек в аппроксимации
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[5].get_min()); //сколько из низ из мн-ва Парето

				//архив в конце
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[2].get_min()); //метрика 1
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[3].get_min()); //метрика 2
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[6].get_min()); //точек в аппроксимации
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[7].get_min()); //сколько из низ из мн-ва Парето

				//элита
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[10].get_min()); //метрика 1
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[11].get_min()); //метрика 2
				sw_6->Write("{0:F2};", stat_VNS_MS_end_i[0].get_min()); //точек в аппроксимации
				sw_6->Write("{0:F2};", stat_VNS_MS_end_i[1].get_min()); //сколько из низ из мн-ва Парето

				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[8].get_min()); //N of iterations
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[9].get_min()); //N of k-shaking
				sw_6->Write("{0:F2};", stat_VNS_MS_end_i[2].get_min()); //N of start
				sw_6->WriteLine("Min");
				//
			}
			else
			{
				// выводим статистику по всем run

			//СРЕДНЕЕ
				//архив в начале
				//архив в конце
				for (int i=0;i<4;i++)
					sw_6->Write("{0:F2};", stat_VNS_MS_end_f[i].get_aver());

				sw_6->Write("{0:F2};", stat_VNS_MS_end_i[0].get_aver()); //точек в аппроксимации
				sw_6->Write("{0:F2};", stat_VNS_MS_end_i[1].get_aver()); //гиперобъем

				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[4].get_aver()); //N of iterations
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[5].get_aver()); //N of k-shaking
				sw_6->Write("{0:F2};", stat_VNS_MS_end_i[2].get_aver()); //N of start
				sw_6->WriteLine("Average");
			//

			//MAX
			//архив в начале
			//архив в конце
				for (int i = 0; i<4; i++)
					sw_6->Write("{0:F2};", stat_VNS_MS_end_f[i].get_max());

				sw_6->Write("{0:F2};", stat_VNS_MS_end_i[0].get_max()); //точек в аппроксимации
				sw_6->Write("{0:F2};", stat_VNS_MS_end_i[1].get_max()); //гиперобъем

				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[4].get_max()); //N of iterations
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[5].get_max()); //N of k-shaking
				sw_6->Write("{0:F2};", stat_VNS_MS_end_i[2].get_max()); //N of start
				sw_6->WriteLine("Max");
			//

			//MIN
				//архив в начале
				//архив в конце
				for (int i = 0; i<4; i++)
					sw_6->Write("{0:F2};", stat_VNS_MS_end_f[i].get_min());

				sw_6->Write("{0:F2};", stat_VNS_MS_end_i[0].get_min()); //точек в аппроксимации
				sw_6->Write("{0:F2};", stat_VNS_MS_end_i[1].get_min()); //гиперобъем

				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[4].get_min()); //N of iterations
				sw_6->Write("{0:F2};", stat_VNS_MS_end_f[5].get_min()); //N of k-shaking
				sw_6->Write("{0:F2};", stat_VNS_MS_end_i[2].get_min()); //N of start
				sw_6->WriteLine("Min");
			//
			}
			sw_6->WriteLine();
		}

		if (flag_VNSnew)
		{
			if (file_name_source_Pareto_set_str)
			{
				//выводим статистику по всем run

			//СРЕДНЕЕ
				//архив в начале
				sw_1->Write("{0:F2};", stat_VNS_MS_f[0].get_aver()); //метрика 1
				sw_1->Write("{0:F2};", stat_VNS_MS_f[1].get_aver()); //метрика 2
				sw_1->Write("{0:F2};", stat_VNS_MS_i[0].get_aver()); //точек в аппроксимации
				sw_1->Write("{0:F2};", stat_VNS_MS_i[1].get_aver()); //сколько из низ из мн-ва Парето

				//архив в конце
				sw_1->Write("{0:F2};", stat_VNS_MS_f[2].get_aver()); //метрика 1
				sw_1->Write("{0:F2};", stat_VNS_MS_f[3].get_aver()); //метрика 2
				sw_1->Write("{0:F2};", stat_VNS_MS_i[2].get_aver()); //точек в аппроксимации
				sw_1->Write("{0:F2};", stat_VNS_MS_i[3].get_aver()); //сколько из низ из мн-ва Парето

				sw_1->Write("{0:F2};", stat_VNS_MS_i[4].get_aver()); //N of iterations
				sw_1->Write("{0:F2};", stat_VNS_MS_i[5].get_aver()); //N of k-shaking
				sw_1->WriteLine("Average");
				//

			//MAX
				//архив в начале
				sw_1->Write("{0:F2};", stat_VNS_MS_f[0].get_max()); //метрика 1
				sw_1->Write("{0:F2};", stat_VNS_MS_f[1].get_max()); //метрика 2
				sw_1->Write("{0:F2};", stat_VNS_MS_i[0].get_max()); //точек в аппроксимации
				sw_1->Write("{0:F2};", stat_VNS_MS_i[1].get_max()); //сколько из низ из мн-ва Парето

				//архив в конце
				sw_1->Write("{0:F2};", stat_VNS_MS_f[2].get_max()); //метрика 1
				sw_1->Write("{0:F2};", stat_VNS_MS_f[3].get_max()); //метрика 2
				sw_1->Write("{0:F2};", stat_VNS_MS_i[2].get_max()); //точек в аппроксимации
				sw_1->Write("{0:F2};", stat_VNS_MS_i[3].get_max()); //сколько из низ из мн-ва Парето

				sw_1->Write("{0:F2};", stat_VNS_MS_i[4].get_max()); //N of iterations
				sw_1->Write("{0:F2};", stat_VNS_MS_i[5].get_max()); //N of k-shaking
				sw_1->WriteLine("Max");
				//

				//MIN
				//архив в начале
				sw_1->Write("{0:F2};", stat_VNS_MS_f[0].get_min()); //метрика 1
				sw_1->Write("{0:F2};", stat_VNS_MS_f[1].get_min()); //метрика 2
				sw_1->Write("{0:F2};", stat_VNS_MS_i[0].get_min()); //точек в аппроксимации
				sw_1->Write("{0:F2};", stat_VNS_MS_i[1].get_min()); //сколько из низ из мн-ва Парето

				//архив в конце
				sw_1->Write("{0:F2};", stat_VNS_MS_f[2].get_min()); //метрика 1
				sw_1->Write("{0:F2};", stat_VNS_MS_f[3].get_min()); //метрика 2
				sw_1->Write("{0:F2};", stat_VNS_MS_i[2].get_min()); //точек в аппроксимации
				sw_1->Write("{0:F2};", stat_VNS_MS_i[3].get_min()); //сколько из низ из мн-ва Парето

				sw_1->Write("{0:F2};", stat_VNS_MS_i[4].get_min()); //N of iterations
				sw_1->Write("{0:F2};", stat_VNS_MS_i[5].get_min()); //N of k-shaking
				sw_1->WriteLine("Min");
				//
			}
			else
			{
				// выводим статистику по всем run

				//СРЕДНЕЕ
				for (int i = 0; i < 6; i++)
					sw_1->Write("{0:F2};", stat_VNS_MS_i[i].get_aver());
				sw_1->WriteLine("Average");
				//

				//MAX
				for (int i = 0; i < 6; i++)
					sw_1->Write("{0:F2};", stat_VNS_MS_i[i].get_max());
				sw_1->WriteLine("Max");
				//

				//MIN
				for (int i = 0; i < 6; i++)
					sw_1->Write("{0:F2};", stat_VNS_MS_i[i].get_min());
				sw_1->WriteLine("Min");
				//
			}

			sw_1->WriteLine();
		}


		//вычисляем время решения одной задачи (все запуски)
		unsigned long long result_alg_time = stop_time - start_time - time_local_search_b - time_local_search_b_pre;
		unsigned long long result_time = stop_time - start_time;
		total_time += result_time; //общее время решения всех задач
								   //printf("Time: %d\n", result_time);
		//time_format(result_time, "Total time (one problem)", sw);
		time_format(result_time, "Total time", sw_1);
		time_format(result_time, "Total time", sw_6);
		time_format(result_time, "Total time (one problem)", sw_3);
		sw_3->WriteLine();
		sw_3->WriteLine();

		sw_4->WriteLine(problem_name_str);
		time_format(result_time, "Total time", sw_4);
		sw_4->WriteLine();

		if (flag_VNSnew)
		{
			if (num_runs > 1)
			{
				aver_time = result_time / num_runs;
				//time_format(aver_time, "Average total time through all runs", sw);
				time_format(aver_time, "Average total time through all runs", sw_1);
			}
			
			// среднее время работы алгоритма без локальных поисков
			//time_format(((double)result_alg_time) / num_runs, "Average time of algorithm through all runs", sw);
			time_format(((double)result_alg_time) / num_runs, "Average time of algorithm through all runs", sw_1);

			// среднее время локального поиска (начальная популяция)
			//time_format(((double)time_local_search_b) / num_runs + time_local_search_b_pre, "Average time of LS (begin)", sw);
			time_format(((double)time_local_search_b) / num_runs + time_local_search_b_pre, "Average time of LS (begin)", sw_1);
		}

		if (flag_VNS_multi_start)
		{
			if (num_runs > 1)
			{
				aver_time = result_time / num_runs;
				// среднее время работы алгоритма по всем запускам
				//time_format(aver_time, "Average time of all runs", sw);
				time_format(aver_time, "Average time of all runs", sw_1);
				time_format(aver_time, "Average time of all runs", sw_6);
			}
			
			// среднее время работы алгоритма по всем стартам и запускам
			//time_format(((double)result_time) / stat_VNS_MS_end_i[2].get_cnt_val(), "Average time of all starts and runs", sw);
			time_format(((double)result_time) / stat_VNS_MS_end_i[2].get_cnt_val(), "Average time of all starts and runs", sw_1);
			time_format(((double)result_time) / stat_VNS_MS_end_i[2].get_cnt_val(), "Average time of all starts and runs", sw_6);
		
			// среднее время локального поиска (начальная популяция)
			//time_format(((double)time_local_search_b) / stat_VNS_MS_end_i[2].get_cnt_val() + time_local_search_b_pre, "Average time of LS (begin)", sw);
			time_format(((double)time_local_search_b) / stat_VNS_MS_end_i[2].get_cnt_val() + time_local_search_b_pre, "Average time of LS (begin)", sw_1);
			time_format(((double)time_local_search_b) / stat_VNS_MS_end_i[2].get_cnt_val() + time_local_search_b_pre, "Average time of LS (begin)", sw_6);
		}


		//sw->WriteLine();
		//sw->Close();

		sw_1->WriteLine();
		sw_1->WriteLine();

		sw_6->WriteLine();
		sw_6->WriteLine();

		vns.~GA_path();

	}//цикл по задачам


	 //вывод 2-х таблиц: 1) 1-ый важнее 2-го; 2) 2-ой важнее 1-го
	 //вывод статистики по сужению по всем задачам
	if (reduction)
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
				index_reduced_both[i] = index_reduced_both[i] / num_prbl_val;
			double aver_num = 0;
			for (int i = 0; i < total_num_P_approx.size(); i++)
				aver_num += total_num_P_approx[i];
			print_exp_red_two(sw_4, "average statistics", index_reduced_both, NUM_STEPS_TETA + 1, aver_num / total_num_P_approx.size());
		}

	}

	//общее время по всем задачам
	time_format(total_time, "Total time of all problems", sw_1);
	time_format(total_time, "Total time of all problems", sw_3);
	time_format(total_time, "Total time of all problems", sw_4);
	time_format(total_time, "Total time of all problems", sw_6);


	sw_1->Close();
	sw_3->Close();
	sw_4->Close();
	sw_5->Close();
	sw_6->Close();
	sr->Close();

	printf("\n");
	//system("pause");

	return 0;
}


int eval_metrics(int argc, char* argv[])
{
	GA_path ga_tmp(12, 50, 2, 100);

	String^ file_name_rd_str_1; // = "Pareto_set_GA_1-10.csv";
	String^ file_name_rd_str_2; // = "Pareto_set_VNS_1-10.csv";

								//инициализация параметров
	string str_temp;
	for (int i = 1; i < argc; i++)
	{
		str_temp = argv[i];
		if (str_temp == "\\GA")
		{
			file_name_rd_str_1 = gcnew String(argv[i + 1]);
			continue;
		}

		if (str_temp == "\\VNS")
		{
			file_name_rd_str_2 = gcnew String(argv[i + 1]);
			continue;
		}
	}

	//считываем название задач
	StreamReader^ sr_1 = gcnew StreamReader(file_name_rd_str_1);
	vector<string> array_problems; // массив имен задач
	string cur_line_str;
	String^ cur_line_str_;
	String^ prev_line_str_;
	while (!sr_1->EndOfStream)
	{
		cur_line_str_ = sr_1->ReadLine();
		if (cur_line_str_->Contains("mtsp"))
		{
			//преобразуем String^ в стандартный string
			cur_line_str = (const char*)(Runtime::InteropServices::Marshal::StringToHGlobalAnsi(cur_line_str_).ToPointer());
			cur_line_str.pop_back(); // убираем ';' в конце имени задачи
			array_problems.push_back(cur_line_str);
		}
	}
	sr_1->Close();

	//считаем гиперобъемы и записываем результаты в файл	
	StreamWriter^ sw_1 = gcnew StreamWriter("results_GA_VNS_hv_" + file_name_rd_str_1->Substring(14, file_name_rd_str_1->LastIndexOf(".csv")-14) + ".csv");
	vector<int> r = { 120, 120 };


	sw_1->WriteLine("HV_GA; HV_VNS; GA->VNS; VNS->GA;");
	prev_line_str_ = gcnew String(array_problems[0].c_str());
	sw_1->WriteLine("{0};", prev_line_str_);

	for (int i = 0; i < array_problems.size(); i++)
	{
		//преобразуем стандартный string в String^  
		cur_line_str_ = gcnew String(array_problems[i].c_str());
		//читаем мн-ва Парето, полученные двумя методами из файла
		vector<vector<int>> Pareto_set_1 = read_Pareto_set_from_file(file_name_rd_str_1, cur_line_str_, 2); //{ {5, 0}, {2, 4}, {9, -1}, {1, 6}, {4, 3} };
		vector<vector<int>> Pareto_set_2 = read_Pareto_set_from_file(file_name_rd_str_2, cur_line_str_, 2);

		//вычисляем гиперобъемы
		unsigned HV_val_1 = ga_tmp.hyper_volume(r, Pareto_set_1);
		unsigned HV_val_2 = ga_tmp.hyper_volume(r, Pareto_set_2);
		float C_m_val_12 = С_metric(Pareto_set_1, Pareto_set_2);
		float C_m_val_21 = С_metric(Pareto_set_2, Pareto_set_1);
		printf("volume_val_1 = %d\n", HV_val_1);
		printf("volume_val_2 = %d\n", HV_val_2);
		printf("GA covers VNS = %.2f %%\n", C_m_val_12*100);
		printf("VNS cover GA = %.2f %%\n", C_m_val_21*100);

		//вычисляем C-measure


		if (cur_line_str_ != prev_line_str_)
			sw_1->WriteLine("{0};", cur_line_str_);
		sw_1->WriteLine("{0}; {1}; {2:F2} %; {3:F2} %;", HV_val_1, HV_val_2, C_m_val_12 * 100, C_m_val_21 * 100);

		prev_line_str_ = cur_line_str_;
	}
	sw_1->Close();

	system("pause");

	return 0;
}

int main(int argc, char* argv[])
{

#ifdef _DEBUG_

	//число городов
	int n = 20;
	//число особей
	int N = 2;
	//число критериев
	int m = 2;
	GA_path ga(n, N, m, 100); //последний аргумент можно не трогать (не влияет)

	//имя файла, с которого считываем
	String^ file_name_rd_str = "Example_MTSP_m2_n50_N50_1_10.txt";//ПОМЕНЯТЬ!!!
	//открываем для считывания
	StreamReader^ sr = gcnew StreamReader(file_name_rd_str);

	//заполнение матриц критериев
	ga.set_matrices(sr);

	//для рандомизатора
	unsigned long temp_time = ::GetTickCount();
	srand(temp_time);
	//генерация 2-х случайных особей в массиве pop
	for (int i = 0; i < ga.get_N(); ++i)
		ga.pop.push_back(ga.random_individual());
	

	printf("Initial population:\n");
	ga.output_pop();
	ga.SAX(ga.s_m, ga.pop[0], ga.pop[1]);
	ga.recombinationOCX(ga.s_m, ga.pop[0], ga.pop[1]);
	


	system("pause");
	
	

#else

#ifdef ALG_GA
	main_GA(argc, argv);
#endif // ALG_GA

#ifdef ALG_VNS
	main_VNS(argc, argv);
#endif // ALG_VNS

#ifdef HYPER_VOL

	eval_metrics(argc, argv);

#endif // HYPER_VOL

#endif

}