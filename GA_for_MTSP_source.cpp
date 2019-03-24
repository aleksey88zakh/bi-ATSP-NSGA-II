//#include "stdafx.h"
#include "GA_for_MTSP_source.h"


//подумать, что ввести, или как сделать по-другому
const int INF = 1000;

GA_path::GA_path()
{
}

GA_path::GA_path(int n, int N, int m, int s_max_N)
{
	this->n = n;//число городов
	this->N = N;//размер популяции
	this->m = m;//число критериев

	phi.resize(2*N);
	
	//for (int i = 0; i < N; i++)  - инициализируется через phi[i] = ...
	//	phi[i].resize(m);

	//----------------------------------
	//ЛОКАЛЬНЫЙ ПОИСК
	//через reserve?
	c_max.resize(n);
	for (int i = 0; i < n; i++)
		c_max[i].resize(m);
	c_all.resize(2);

	i_rank_pi.resize(n);
	index_p.resize(n);
	//----------------------------------

	i_rank.resize(N);
	i_dist.resize(N);

	i_rank_R_t.resize(2*N);
	i_dist_R_t.resize(2*N);

		

	this->tourn_size = TOURN_SIZE;//размер турнира
	this->p_mut = P_MUTATION; //веротяность мутации

	//this->pop = new int*[N];
	//for (int i = 0; i < N; i++)
	//	this->pop[i] = new int[n];
	//this->phi = new int[N];
	this->rank = new int[N];
	this->phi_best = s_max_N;
	this->avg_phi = 0;
	this->phi_best_in_rec = s_max_N;
	this->ord_best_ind = -1;
	this->iter_best = -1;
	this->number_solution_in_rec = 0;
	this->time_best = 0;
	this->count_best_child = 0;
	this->iter_best_child = 0;
	this->is_improve_LS = false;

}

GA_path::~GA_path()
{
	phi.clear();
	i_rank.clear();
	//i_dist.clear();
	i_rank_R_t.clear();
	i_dist_R_t.clear();
	pop.clear();
	s_m.clear();
}


///////////////////////////////////////////////////////////////////////////////
//Генерация начальной популяции
///////////////////////////////////////////////////////////////////////////////
void GA_path::init_pop(vector<vector<vector<int>>> s, int S_max, unsigned long long temp_time,
	vector<int> p1, vector<int> p2, vector<int> p3, vector<int> p4)
	//vector<vector<int>> vertex, vector<int> vertex_initial) - пока не используем
{
	this->phi_best = S_max*this->n;
	this->phi_best_in_rec = S_max*this->n;
	this->number_solution_in_rec = 0;
	this->count_best_child = 0;
	this->iter_best_child = 0;
	this->avg_phi = 0;
	//!!! pop.reserve(n); почему-то не работает
	////////////////////////////////////////////////////////////////////////////////
	//        первая особь строится специальным методом patching algorithm (венгерский метод+склейка циклов) модификация
	pop.push_back(p1);//модифицированный метод
	//phi[0][0] = this->phitness(s1, pop[0]);
	//phi[0][1] = this->phitness(s2, pop[0]);
	//phi_best = phi[0];
	//ord_best_ind = 1;
	//вторая особь строится специальным методом patching algorithm (венгерский метод+склейка циклов) original
	pop.push_back(p2);//оригинальный метод
	//phi[1][0] = this->phitness(s1, pop[1]);
	//phi[1][1] = this->phitness(s2, pop[1]);
	//if (phi_best>phi[1]) {
	//	phi_best = phi[1];
	//	ord_best_ind = 2;
	//}
	//avg_phi = (phi[0] + phi[1]) / 2;//средняя пригодность
	//time_best = GetTickCount64() - start_time;

	pop.push_back(p3);
	pop.push_back(p4);

	//применяем локальный поиск
	//pop[0] = this->local_search(pop[0], this->s_m[0]); //что писать вторым аргументом?
	//pop[1] = this->local_search(pop[1], this->s_m[0]); //что писать вторым аргументом?
	//pop[2] = this->local_search(pop[2], this->s_m[0]); //что писать вторым аргументом?
	//pop[3] = this->local_search(pop[3], this->s_m[0]); //что писать вторым аргументом?

	////////////////////////////////////////////////////////////////////////////////
	srand(temp_time);
	for (int i = 4; i < N; ++i)
	{
		//строим особь с помощью рандомизатора
		pop.push_back(this->random_individual());

		//применяем локальный поиск
		//pop[i] = this->local_search(pop[i], this->s_m[0]); //что писать вторым аргументом?

		//            for(int j=0;j<n;++j){
		//            System.out.print(pop[i][j]+" ");
		//            }
		//            System.out.println();
		//вычисляем пригодность особи
		//phi[i] = this->phitness(s, pop[i]);
		// находим лучшую особь в популяции и ее пригодность
	//	if (phi[i] < phi_best)
	//	{
	//		phi_best = phi[i];
	//		ord_best_ind = i + 1;
	//		time_best = GetTickCount64() - start_time;
	//	}
	//	avg_phi = (double)((avg_phi*(i)+phi[i]) / (i + 1));//средняя пригодность
	}
	//iter_best = 0;//итерация лучшей особи
	//			  //         time_best=System.currentTimeMillis()-StartTime;//время получения лучшей особи

}

////////////////////////////////////////////////////////////////////////////////
//Общая функция по генерации начальной популяции
////////////////////////////////////////////////////////////////////////////////
void GA_path::generate_init_pop_VNS(StreamWriter^ sw, StreamWriter^ sw_1, int iter_prbl, String^ file_name_source_Pareto_set_str,
									vector<vector<int>> phi_Pareto_set, long long* time_local_search_b, int index_run)
{
	unsigned long temp_time; //для рандомизатора

	//для рандомизатора
	temp_time = ::GetTickCount();
	srand(temp_time);

	//родители нач популяции
	//4 особи по Зангу
	vector<int> p1, p2, p3, p4;
	p1 = this->patching_algorithm(this->s_m[0], this->c_max_all[0], false);
	p2 = this->patching_algorithm(this->s_m[0], this->c_max_all[0], true);
	p3 = this->patching_algorithm(this->s_m[1], this->c_max_all[1], false);
	p4 = this->patching_algorithm(this->s_m[1], this->c_max_all[1], true);

	//sw->WriteLine("p1; p2; p3; p4");
	//printf("\np1\t p2\t p3\t p4\n");

	vector<int> p1_temp(this->get_n());
	vector<int> p2_temp(this->get_n());
	vector<int> p3_temp(this->get_n());
	vector<int> p4_temp(this->get_n());

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
	for (int i = 1; i < this->get_n(); ++i)
	{
		p1_temp[i] = p1[i_temp] + 1;
		i_temp = p1[i_temp];
	}
	i_temp = 0;
	for (int i = 1; i < this->get_n(); ++i)
	{
		p2_temp[i] = p2[i_temp] + 1;
		i_temp = p2[i_temp];
	}
	i_temp = 0;
	for (int i = 1; i < this->get_n(); ++i)
	{
		p3_temp[i] = p3[i_temp] + 1;
		i_temp = p3[i_temp];
	}
	i_temp = 0;
	for (int i = 1; i < this->get_n(); ++i)
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
	this->init_pop(this->s_m, this->c_max_all[0], temp_time, p1, p2, p3, p4);

	sw->WriteLine("Initial population");
	//sw_1->WriteLine("Initial population");
	printf("Initial population (problem %d):\n", iter_prbl + 1);
	for (int j = 0; j < this->get_n(); j++)
	{
		for (int i = 0; i < this->get_N(); i++)
		{
			sw->Write("{0};", this->pop[i][j]);
			printf("%d\t", this->pop[i][j]);
		}
		sw->WriteLine();
	}

	sw->WriteLine();
	printf("\n");



	//заполняем значение векторного критерия по популяции
	//phi[i][j], i - индекс критерия, j - индекс особи

	sw->WriteLine("Values of vector criterion (initial population)");
	printf("Values of vector criterion (initial population, problem %d):\n", iter_prbl + 1);

	//заполнение значений критерия
	for (int i = 0; i < this->get_N(); i++)
		this->phi[i] = this->multi_phitness(this->pop[i]);

	//вывод значений критерия
	for (int j = 0; j < this->get_m(); j++)
	{
		for (int i = 0; i < this->get_N(); i++)
		{
			sw->Write("{0};", this->phi[i][j]);
			printf("%d\t", this->phi[i][j]);
		}
		sw->WriteLine();
		printf("\n");
	}
	printf("\n");
	sw->WriteLine();



	//-----------------------------------------------------------------------------------------------
	//----------ЛОКАЛЬНЫЙ ПОИСК (нач. поп.)----------------------------------------------------------

	// ПРЕПРОЦЕССИНГ см. выше
	// выполняется один раз перед всеми запусками одной задачи


	//ПРИМЕНЕНИЕ ЛОКАЛЬНОГО ПОИСКА К ОСОБЯМ ИЗ МАССИВА pop

	//время локального поиска (в начале)
	(*time_local_search_b) -= ::GetTickCount();

	vector<int> pop_new;
	printf("Local search\n");
	printf("\n");
	sw->WriteLine("Local search");
	sw->WriteLine();

	for (int i = 0; i < this->get_N(); i++)
		this->pop[i] = this->local_search(this->pop[i], LS_VNS_BEGIN_ALPHA, LS_VNS_BEGIN_BETA);

	sw->WriteLine("after LS");
	printf("afret LS:\n");
	for (int j = 0; j <  this->get_n(); j++)
	{
		for (int i = 0; i <  this->get_N(); i++)
		{
			sw->Write("{0};", this->pop[i][j]);
			printf("%d\t", this->pop[i][j]);
		}
		sw->WriteLine();

	}

	sw->WriteLine();
	printf("\n");


	//заполняем значение векторного критерия по популяции
	sw->WriteLine("Values of vector criterion:");
	printf("Values of vector criterion:\n");

	//заполнение значений критерия
	for (int i = 0; i <  this->get_N(); i++)
		this->phi[i] = this->multi_phitness(this->pop[i]);

	//вывод значений критерия
	for (int j = 0; j <  this->get_m(); j++)
	{
		for (int i = 0; i <  this->get_N(); i++)
		{
			sw->Write("{0};", this->phi[i][j]);
			printf("%d\t", this->phi[i][j]);
		}
		sw->WriteLine();
		printf("\n");
	}
	printf("\n");
	sw->WriteLine();

	//засекаем время локального поиска (в начале)
	(*time_local_search_b) += ::GetTickCount();

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------





	//------------ПОСТРОЕНИЕ ФРОНТОВ
	//true - популяция в явном виде отсорирована по фронтам + ранги фронтов заданы в векторе i_rank
	//false - популяция в явном виде не отсорирована по фронтам, ранги фронтов заданы в векторе i_rank
	vector<int> tmp2; //заглушка
	this->i_rank = this->range_front_J(this->phi, this->pop, tmp2, 1);
	tmp2.clear();
	//значение векторного критерия отсортированной популяции
	sw->WriteLine("Values of vector criterion (sorted):");
	printf("Values of vector criterion (sorted):\n");

	//заполнение значений критерия
	for (int i = 0; i <  this->get_N(); i++)
		this->phi[i] = this->multi_phitness(this->pop[i]);

	//вывод значений критерия
	for (int j = 0; j <  this->get_m(); j++)
	{
		for (int i = 0; i <  this->get_N(); i++)
		{
			sw->Write("{0};", this->phi[i][j]);
			printf("%d\t", this->phi[i][j]);
		}
		sw->WriteLine();
		printf("\n");
	}
	printf("\n");
	sw->WriteLine();



	sw->WriteLine("Ranks of population (initial population)");
	//sw_1->WriteLine("Ranks of poulation");
	printf("Ranks of poulation (initial population, problem %d):\n", iter_prbl + 1);
	for (int i = 0; i <  this->get_N(); i++)
	{
		sw->Write("{0};", this->i_rank[i]);
		//sw_1->Write("{0};", ga.i_rank[i]);
		printf("%d\t", this->i_rank[i]);
	}
	sw->WriteLine();
	//sw_1->WriteLine();
	printf("\n");


	//формируем архив:
	//случ. популяция + ЛП
	Archive arch(this); //как передать указатель на объект, членом которого будет данная функция?

	//вывод значений векторного критерия аппроксимации мн-ва Парето для НАЧАЛЬНОЙ популяции
	//vns.phi_P_approx.clear();
	// архив = мн-во Парето
	this->phi_P_approx = arch.val_crit_archive;

	sw->WriteLine("Approximation of the Pareto set (initial population)");
	//sw_1->WriteLine("Approximation of the Pareto set({0} iterations)", iter);
	for (int i = 0; i < this->get_m(); i++)
	{
		for (int j = 0; j < this->phi_P_approx.size(); j++)
			sw->Write("{0};", this->phi_P_approx[j][i]);

		sw->WriteLine();
	}

	sw->WriteLine();


	//вычисление метрики для НАЧАЛЬНОЙ популяции - аппроксимации множества Парето (если выше присвоено имя файла)
	//if (file_name_source_Pareto_set_str)
	//{
	//	//заголовок
	//	if (index_run == 0)
	//	{
	//		sw_1->WriteLine("N_PS; {0}", phi_Pareto_set.size());
	//		sw_1->WriteLine();

	//		sw_1->Write("Begin; ; ; ;");
	//		sw_1->WriteLine("End;");

	//		sw_1->Write("Metric 'Convergence approximation to Pareto set';");
	//		sw_1->Write("Metric 'Convergence Pareto set to approximation';");
	//		sw_1->Write("Points in approx to Pareto set;");
	//		sw_1->Write("Points from Pareto set;");

	//		sw_1->Write("Metric 'Convergence approximation to Pareto set';");
	//		sw_1->Write("Metric 'Convergence Pareto set to approximation';");
	//		sw_1->Write("Points in approx to Pareto set;");
	//		sw_1->Write("Points from Pareto set;");
	//		sw_1->Write("Run;");
	//		sw_1->WriteLine("N of iterations");
	//	}

	//	//вычисляем метрики
	//	this->count_P_eq_approx = 0;
	//	this->evaluate_metric_of_approx(sw_1, phi_Pareto_set, true);
	//	this->evaluate_metric_of_approx(sw, phi_Pareto_set, false);
	//	sw->WriteLine();
	//	sw_1->Write("{0};", this->count_P_eq_approx);

	//	//для средней метрики по всем запускам
	//	total_dist_begin_1 += this->dist_conver_approx_to_P_set_val;
	//	total_dist_begin_2 += this->dist_conver_P_set_to_approx_val;

	//	total_num_approx_P_begin += this->phi_P_approx.size();
	//	total_num_approx_in_P_begin += this->count_P_eq_approx;

	//	if (0 == index_run)
	//	{
	//		max_dist_begin_1 = min_dist_begin_1 = this->dist_conver_approx_to_P_set_val;
	//		max_dist_begin_2 = min_dist_begin_2 = this->dist_conver_P_set_to_approx_val;
	//		max_num_approx_P_begin = min_num_approx_P_begin = this->phi_P_approx.size();
	//		max_num_approx_in_P_begin = min_num_approx_in_P_begin = this->count_P_eq_approx;
	//	}

	//	//определение максимума метрики по всем запускам
	//	if (max_dist_begin_1 < this->dist_conver_approx_to_P_set_val)
	//		max_dist_begin_1 = this->dist_conver_approx_to_P_set_val;
	//	if (max_dist_begin_2 < this->dist_conver_P_set_to_approx_val)
	//		max_dist_begin_2 = this->dist_conver_P_set_to_approx_val;

	//	if (max_num_approx_P_begin < this->phi_P_approx.size())
	//		max_num_approx_P_begin = this->phi_P_approx.size();
	//	if (max_num_approx_in_P_begin < this->count_P_eq_approx)
	//		max_num_approx_in_P_begin = this->count_P_eq_approx;

	//	//определение минимума метрики по всем запускам
	//	if (min_dist_begin_1 > this->dist_conver_approx_to_P_set_val)
	//		min_dist_begin_1 = this->dist_conver_approx_to_P_set_val;
	//	if (min_dist_begin_2 > this->dist_conver_P_set_to_approx_val)
	//		min_dist_begin_2 = this->dist_conver_P_set_to_approx_val;

	//	if (min_num_approx_P_begin > this->phi_P_approx.size())
	//		min_num_approx_P_begin = this->phi_P_approx.size();
	//	if (min_num_approx_in_P_begin > this->count_P_eq_approx)
	//		min_num_approx_in_P_begin = this->count_P_eq_approx;
	//}
	//else
	//	//заголовок
	//	if (0 == index_run)
	//	{
	//		sw_1->Write("N of iterations;");
	//		sw_1->Write("N of approx Pareto set;");
	//		sw_1->WriteLine("Run;");
	//	}

}


////////////////////////////////////////////////////////////////////////////////
//задание матрицы весов для ребер (по всем критериям)
////////////////////////////////////////////////////////////////////////////////
void GA_path::set_matrix_criteria(vector<vector<int>> s)
{
	this->s_m.push_back(s);
}
////////////////////////////////////////////////////////////////////////////////
//разбор матриц из файла и запись в контейнер s_m (через set_matrix_criteria())
////////////////////////////////////////////////////////////////////////////////
void GA_path::set_matrices(StreamReader^ sr)
{
	String^ cur_line_str = sr->ReadLine();
	
	//временный контейнер для матриц расстояний
	vector<vector<int>> s_temp(this->get_n(), vector<int>(this->get_n()));
	int c_max_temp = 0;
	double s_aver_temp = 0;
	int cnt = 0;
	
	while (cur_line_str != "s1=")
		cur_line_str = sr->ReadLine();

	//заполнение матрицы 1-го критерия из файла
	//printf("Criterion 1");
	//sw->WriteLine("Criterion 1");
	for (int i = 0; i < this->get_n(); i++) //индекс строки
	{
		cur_line_str = sr->ReadLine();

		int j = 0; //индекс столбца
		this->c_max[i][0] = 0; //максимальная дуга по каждой строке
		int num_temp;
		string str_temp;
		//разбираем текущую строку
		for (int k = 0; k < cur_line_str->Length; k++)
		{
			if (cur_line_str[k] == ' ')
			{
				num_temp = stoi(str_temp);
				s_temp[i][j] = num_temp;
				//sw->Write("{0};", s_temp[i][j]);
				//printf("%d \t", s_temp[i][j]);

				//подсчет среднего элемента
				if (i != j) //не учитываем диагональ
				{
					cnt++;
					if (1==cnt)
						s_aver_temp = s_temp[i][j];
					else
						s_aver_temp = s_aver_temp*(cnt-1)/cnt + ((double)s_temp[i][j])/cnt;

					c_all[0].push_back(s_temp[i][j]); //массив всех дуг
				}
				//подсчет максимального элемента
				if (c_max_temp < s_temp[i][j])
					c_max_temp = s_temp[i][j];
				if (this->c_max[i][0] < s_temp[i][j])
					this->c_max[i][0] = s_temp[i][j];

				j++;
				str_temp = "";

				if (j == this->get_n())
					break;
			}
			else
				str_temp += cur_line_str[k];
		}
		//sw->WriteLine();

	}

	//добавляем матрицу 1-го критерия в массив s_m
	this->set_matrix_criteria(s_temp);

	//s_aver_temp = (int)s_aver_temp / (this->get_n()*this->get_n());
	//добавляем средний и макс элементы в свои массивы
	this->s_aver.push_back((int) s_aver_temp);
	this->c_max_all.push_back(c_max_temp);

	//sw->WriteLine();
	//printf("\n");

	s_aver_temp = 0;
	cnt = 0;
	c_max_temp = 0;

	/*
	//Матрица расстояний 1-го критерия
	int k = 0;
	printf("Criterion %d\n", k + 1);
	sw->WriteLine("Criterion; {0}", k + 1);

	srand(23);//ПОМЕНЯТЬ!!!

	vector <vector<int>> s_temp1(20, vector<int>(20));
	for (int i = 0; i < 20; i++)
	{
	for (int j = 0; j < 20; j++)
	{
	if (i == j)
	s_temp1[i][i] = 0;
	else
	s_temp1[i][j] = (rand() % 20) + 1; //ПОМЕНЯТЬ!!!
	}
	}



	c_max_temp = 0;
	for (int i = 0; i < ga.get_n(); i++)
	{
	for (int j = 0; j < ga.get_n(); j++)
	{
	s_temp[i][j] = s_temp1[i][j];

	sw->Write("{0};", s_temp[i][j]);
	printf("%d \t", s_temp[i][j]);

	s_aver_temp += s_temp[i][j];

	if (c_max_temp < s_temp[i][j])
	c_max_temp = s_temp[i][j];
	}
	sw->WriteLine();
	}

	ga.set_matrix_criteria(s_temp);

	s_aver_temp = (int)s_aver_temp / ga.get_n()*ga.get_n();
	ga.s_aver.push_back(s_aver_temp);
	c_max.push_back(c_max_temp);


	sw->WriteLine();
	printf("\n");
	*/


	while (cur_line_str != "s2=")
		cur_line_str = sr->ReadLine();

	//заполнение матрицы 2-го критерия из файла
	//printf("Criterion 2");
	//sw->WriteLine("Criterion 2");
	for (int i = 0; i < this->get_n(); i++) //индекс строки
	{
		cur_line_str = sr->ReadLine();

		int j = 0; //индекс столбца
		this->c_max[i][1] = 0; //максимальная дуга по каждой строке
		int num_temp;
		string str_temp;
		//разбираем текущую строку
		for (int k = 0; k < cur_line_str->Length; k++)
		{
			if (cur_line_str[k] == ' ')
			{
				num_temp = stoi(str_temp);
				s_temp[i][j] = num_temp;
				//sw->Write("{0};", s_temp[i][j]);
				//printf("%d \t", s_temp[i][j]);

				//подсчет среднего элемента
				if (i != j) //не учитываем диагональ
				{
					cnt++;
					if (1==cnt)
						s_aver_temp = s_temp[i][j];
					else
						s_aver_temp = s_aver_temp*(cnt-1)/cnt + ((double)s_temp[i][j])/cnt;

					c_all[1].push_back(s_temp[i][j]); //массив всех дуг
				}
				if (c_max_temp < s_temp[i][j])
					c_max_temp = s_temp[i][j];
				if (this->c_max[i][1] < s_temp[i][j])
					this->c_max[i][1] = s_temp[i][j];

				j++;
				str_temp = "";

				if (j == this->get_n())
					break;
			}
			else
				str_temp += cur_line_str[k];
		}
		//sw->WriteLine();

	}

	//добавляем матрицу 2-го критерия в массив s_m
	this->set_matrix_criteria(s_temp);

	//s_aver_temp = (int)s_aver_temp / (this->get_n()*this->get_n());
	//добавляем средний и макс элементы в свои массивы
	this->s_aver.push_back((int) s_aver_temp);
	this->c_max_all.push_back(c_max_temp);

	//sw->WriteLine();
	//printf("\n");


	/*
	//Матрица расстояний 2-го критерия
	k++;
	printf("Criterion %d\n", k + 1);
	sw->WriteLine("Criterion; {0}", k + 1);


	srand(32);//ПОМЕНЯТЬ!!!

	for (int i = 0; i < 20; i++)
	{
	for (int j = 0; j < 20; j++)
	{
	if (i == j)
	s_temp1[i][i] = 0;
	else
	s_temp1[i][j] = (rand() % 20) + 1;//ПОМЕНЯТЬ!!!
	}
	}



	c_max_temp = 0;
	s_aver_temp = 0;
	for (int i = 0; i < ga.get_n(); i++)
	{
	for (int j = 0; j < ga.get_n(); j++)
	{

	s_temp[i][j] = s_temp1[i][j];

	sw->Write("{0};", s_temp[i][j]);
	printf("%d \t", s_temp[i][j]);

	s_aver_temp += s_temp[i][j];

	if (c_max_temp < s_temp[i][j])
	c_max_temp = s_temp[i][j];
	}
	sw->WriteLine();
	}

	ga.set_matrix_criteria(s_temp);

	s_aver_temp = (int)s_aver_temp / ga.get_n()*ga.get_n();
	ga.s_aver.push_back(s_aver_temp);
	c_max.push_back(c_max_temp);
	*/

}


////////////////////////////////////////////////////////////////////////////////
//Построение особи с помощью рандомизатора
////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::random_individual()
{
	// рандомизация генератора случайных чисел
	//unsigned rand_value = 11;
	//srand(rand_value);

	vector<int> individual(n);//строящаясяя особь
	int jNext, iNext = 0;//вершины выбираются случайным образом с равномерным распределением
	int count = 0;//счетчик просмотренных вершин
	vector<bool> indicator(n);//индикатор уже размещенных вершин
	for (int j = 0; j < n; ++j)
	{
		indicator[j] = false;
	}
	
	//сделано через rand(), см. ниже
	//Random^ rnd = gcnew Random();//датчик случайных чисел
							  ////////////////////////////////////////////////////////////////
							  //первая вершина выбирается случайным образом
	iNext = 0;
	individual[0] = iNext + 1;
	indicator[iNext] = true;//помечаем вершину уже размещенной
						   ////////////////////////////////////////////////////////////////
						   //остальные вершины выбираем случайным образом с равномерным распределением
	for (int j = 1; j < n - 1; ++j)
	{
		//iNext = rnd->Next(n - j) + 1;
		iNext = (rand() % (n - j)) + 1;
		jNext = -1;
		count = 0;
		for (int i = 0; i < n; ++i)
		{
			//если вершина еще не просмотрена, то увеличиваем счетчик
			if (indicator[i] == false)
				count++;
		
			// номер выбранной вершины среди еще неразмещенных
			if (count == iNext)
			{
				jNext = i;// абсолютный номер выбранной вершины
				break;
			}
		}
		individual[j] = jNext + 1;
		indicator[jNext] = true;//помечаем вершину уже размещенной
	} // заполнили первые n-1 позиции
	
	for (int i = 0; i < n; ++i)
	{
		//оставшуюся вершину размещаем в последней позиции
		if (indicator[i] == false)
		{
			individual[n - 1] = i + 1;
			break;
		}
	}
	return individual;
}

////////////////////////////////////////////////////////////////////////////////
//Локальный поиск
////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::local_search(vector<int> p, float alpha, float beta, void* p_arch)//это локальный поиск по принципу first-improvement
																		//котороый делает полный перебор, его нужно изменить!!!!!
{
	//////////////////////////////////////////////////////////////////////////////////
	vector<vector<int>> path(2, vector<int>(n));//хранит предков и потомков по туру
	int i_head, j_head, i_next, i_temp, i_tail, j_tail;//точки разрыва цикла
	int k_alpha = (int)(alpha*n);//число вершин для просмотра в перестановке pi
	int k_beta = (int)(beta*(n - 1));//число вершин для просмотра в перестановке p_i
	boolean is_improve;//для сигнала, что текущее решение улучшено
	int s1_p, s2_p, s1, s2;//значения критериев на p
	s1_p = phitness(s_m[0], p);//значение первого критерия для текущей лучшей точки
	s2_p = phitness(s_m[1], p);//значение второго критерия для текущей лучшей точки

						  //очередь вершин
						  //объявление очереди
	deque<int> pi_deque;
	//для вершин в черном списке
	vector<boolean> tabu(this->get_n());
	//запись перестановки pi (переменная index_pi) в очередь
	for (int i = 0; i < k_alpha; i++) {
		pi_deque.push_back(this->index_pi[i] - 1);
		tabu[i] = false;//изначально все вершины вне черного списка (активны!)
	}

	for (int i = 0; i < n - 1; ++i) {
		path[1][p[i] - 1] = p[i + 1] - 1;
		path[0][p[i + 1] - 1] = p[i] - 1;
	}
	path[1][p[n - 1] - 1] = p[0] - 1;
	path[0][p[0] - 1] = p[n - 1] - 1;
	//////////////////////////////////////////////////////////////////////////////////
	//+++++++++++++++++++++++++++++++++++++++++++
	//+++++++++++++++++++++++++++++++++++++++++++
	is_improve = false;
	//основной цикл просмотра окрестности
	while (pi_deque.size() != 0)
	{//просматриваем вершины очереди по порядку
								  //////////////////////////////////////////////////////////////////
		i_head = pi_deque.front(); //возвращает первый элемент очереди (нет проверки сущестования)
		pi_deque.pop_front(); //удаление первого элемента очереди (не возвращает его)
		j_head = path[1][i_head]; //идентифицируем следующую вершину цикла 
		is_improve = false;
		//path[1][i_head] = -1;//удаляем дугу (i_head,j_head)
		//path[0][j_head] = -1;
		/////////////////////////////////////////////////////////////////
		//добавляем дугу для образования цикла (i_head,i_next) //перебираем только alpha вариантов
		for (int i_t = 0; i_t < k_beta; i_t++)
		{	//2
			i_next = (this->index_p[i_head][i_t]) - 1;
			//printf("%d %d %d %d\t", i_head, j_head, i_next,i_t);
			//printf("\n");
			if (i_head != i_next && j_head != i_next)
			{	//3
				//добавляем (i_head, i_next)
				//path[1][i_head] = i_next;
				i_temp = path[0][i_next];//дуга для удаления формируется однозначно
										 //path[0][i_next] = i_head;
										 //удаляем (i_temp, i_next)
										 //path[1][i_temp] = -1;
				j_tail = i_next;//просматриваем вершины цикла, j_tail - кандидат

				while (true)
				{
					j_tail = path[1][j_tail];
					if (j_head == j_tail) {//!
						break;
					}//!
					if (j_head != j_tail)
					{	//if1
						//обновляем значения критериев по новому циклу
						s1 = s1_p - s_m[0][i_head][j_head] - s_m[0][i_temp][i_next] - s_m[0][path[0][j_tail]][j_tail] +
							s_m[0][i_head][i_next] + s_m[0][i_temp][j_tail] + s_m[0][path[0][j_tail]][j_head];
						s2 = s2_p - s_m[1][i_head][j_head] - s_m[1][i_temp][i_next] - s_m[1][path[0][j_tail]][j_tail] +
							s_m[1][i_head][i_next] + s_m[1][i_temp][j_tail] + s_m[1][path[0][j_tail]][j_head];

						// локальный поиск начальной популяции
						if (!p_arch)
						{
							// новая особь доминирует текущую					
							if ((s1 < s1_p && s2 <= s2_p) || (s1 <= s1_p && s2 < s2_p))
							{	//if2
								//нашли улучшающее решение
								//printf("%d %d\t", s1, s2);
								//printf("\n");

								is_improve = true;
								break;
							}//if2
						}
						//локальный поиск в финальной популяции
						else
						{
							// "центр" окрестности продоминировал новую особь
							if ((s1_p < s1 && s2_p <= s2) || (s1_p <= s1 && s2_p < s2))
								continue;

							// формируем перестановку, соотв. новой особи
							vector<vector<int>> path_tmp = path;
							//удаляем (i_head,j_head)
							path_tmp[1][i_head] = -1;
							path_tmp[0][j_head] = -1;
							//добавляем (i_head, i_next)
							path_tmp[1][i_head] = i_next;
							path_tmp[0][i_next] = i_head;
							//удаляем (i_temp, i_next)
							path_tmp[1][i_temp] = -1;
							//добавляем (i_temp, j_tail)
							path_tmp[1][i_temp] = j_tail;
							i_tail = path_tmp[0][j_tail];
							path_tmp[0][j_tail] = i_temp;
							//удаляем (i_tail, j_tail)
							path_tmp[1][i_tail] = -1;
							//добавляем (i_tail, j_head)
							path_tmp[1][i_tail] = j_head;
							path_tmp[0][j_head] = i_tail;

							vector<int> result_tmp(this->get_n());
							result_tmp[0] = 1;
							int i_temp2 = 0;
							for (int i = 1; i < n; ++i)
							{
								result_tmp[i] = path_tmp[1][i_temp2] + 1;
								i_temp2 = path_tmp[1][i_temp2];
							}

							// новая особь продоминировала "центр" окрестности
							if ((s1 < s1_p && s2 <= s2_p) || (s1 <= s1_p && s2 < s2_p))
							{
								//? сравниваем {s1, s2} c архивом\{s1_p, s2_p}
								return result_tmp; // возвращаем новую особь
							}

							// дошли до сюда, значит новая и текущая особи не сравнимы
							// сравним новую особь с архивом
							if (((Archive*)p_arch)->check_new({ s1, s2 }))
							{
								((Archive*)p_arch)->arch_modify(result_tmp, { s1, s2 });
							}
						}

					}//if 1

				}//while(true)

				if (is_improve == false) {
					//в данном направлении нет улучшения (возвращаем метки на место)
					//идет речь о направлении, где удалены (i_head,j_head) и (i_temp,i_next),
					//но добавлена (i_head,i_next) 
					//path[1][i_head] = -1;//остается только одна удаленная дуга (i_head,j_head)
					//path[0][j_head] = -1;
					//path[0][i_next] = i_temp;
					//path[1][i_temp] = i_next;
				}
				else
				{//нашли улучшающее решение в окрестности текущего решения
					  //удаляем (i_head,j_head)
					path[1][i_head] = -1;
					path[0][j_head] = -1;
					//добавляем (i_head, i_next)
					path[1][i_head] = i_next;
					path[0][i_next] = i_head;
					//удаляем (i_temp, i_next)
					path[1][i_temp] = -1;
					//добавляем (i_temp, j_tail)
					path[1][i_temp] = j_tail;
					i_tail = path[0][j_tail];
					path[0][j_tail] = i_temp;
					//удаляем (i_tail, j_tail)
					path[1][i_tail] = -1;
					//добавляем (i_tail, j_head)
					path[1][i_tail] = j_head;
					path[0][j_head] = i_tail;
					s1_p = s1; s2_p = s2;//обновляем значения критериев
					break;
				}
			}//3
		}//2

		if (p_arch) continue;

		if (is_improve)
		{
			//добавляем i_head в конец очереди
			pi_deque.push_back(i_head); //добавление переменной i_head в конец очереди
										//если  вершина какого-либо удаленного ребера в черном списке, то возвращаем ее в очередь
			if (tabu[j_head] == true) { tabu[j_head] = false; pi_deque.push_back(j_head); }
			if (tabu[i_next] == true) { tabu[i_next] = false; pi_deque.push_back(i_next); }
			if (tabu[i_temp] == true) { tabu[i_temp] = false; pi_deque.push_back(i_temp); }
			if (tabu[j_tail] == true) { tabu[j_tail] = false; pi_deque.push_back(j_tail); }
			if (tabu[i_tail] == true) { tabu[i_tail] = false; pi_deque.push_back(i_tail); }

		}
		else {
			tabu[i_head] = true;
			//еслии неудача, то возвращаем дугу
			// path[1][i_head] = j_head;
			// path[0][j_head] = i_head;
		}

	}//while(pi_deque.size()!=0)
	 //+++++++++++++++++++++++++++++++++++++++++++
	 //+++++++++++++++++++++++++++++++++++++++++++

	if (p_arch)
		return {}; // возвращаем пустой вектор

	vector<int> result(this->get_n());
	result[0] = 1; i_temp = 0;
	for (int i = 1; i < n; ++i) {
		result[i] = path[1][i_temp] + 1;
		i_temp = path[1][i_temp];
	}
	return result;
}

////////////////////////////////////////////////////////////////////////////////
//Новый локальный поиск для VNS
////////////////////////////////////////////////////////////////////////////////
bool GA_path::local_search_VNS_new(vector<int> p, float alpha, float beta, void* p_arch)//это локальный поиск по принципу first-improvement
																					   //котороый делает полный перебор, его нужно изменить!!!!!
{
	//////////////////////////////////////////////////////////////////////////////////
	vector<vector<int>> path(2, vector<int>(n));//хранит предков и потомков по туру
	int i_head, j_head, i_next, i_temp, i_tail, j_tail;//точки разрыва цикла
	int k_alpha = (int)(alpha*n);//число вершин для просмотра в перестановке pi
	int k_beta = (int)(beta*(n - 1));//число вершин для просмотра в перестановке p_i
	boolean is_improve;//для сигнала, что текущее решение улучшено
	int s1_p, s2_p, s1, s2;//значения критериев на p
	s1_p = phitness(s_m[0], p);//значение первого критерия для текущей лучшей точки
	s2_p = phitness(s_m[1], p);//значение второго критерия для текущей лучшей точки

							   //очередь вершин
							   //объявление очереди
	deque<int> pi_deque;
	//запись перестановки pi (переменная index_pi) в очередь
	for (int i = 0; i < k_alpha; i++)
		pi_deque.push_back(this->index_pi[i] - 1);


	for (int i = 0; i < n - 1; ++i)
	{
		path[1][p[i] - 1] = p[i + 1] - 1;
		path[0][p[i + 1] - 1] = p[i] - 1;
	}
	path[1][p[n - 1] - 1] = p[0] - 1;
	path[0][p[0] - 1] = p[n - 1] - 1;
	//////////////////////////////////////////////////////////////////////////////////
	//+++++++++++++++++++++++++++++++++++++++++++
	//+++++++++++++++++++++++++++++++++++++++++++
	is_improve = false;
	//основной цикл просмотра окрестности
	while (pi_deque.size() != 0) //перебираем только alpha вариантов
	{//просматриваем вершины очереди по порядку
	 //////////////////////////////////////////////////////////////////
		i_head = pi_deque.front(); //возвращает первый элемент очереди (нет проверки сущестования)
		pi_deque.pop_front(); //удаление первого элемента очереди (не возвращает его)
		j_head = path[1][i_head]; //идентифицируем следующую вершину цикла 

		//path[1][i_head] = -1;//удаляем дугу (i_head,j_head)
		//path[0][j_head] = -1;
		/////////////////////////////////////////////////////////////////
		//добавляем дугу для образования цикла (i_head,i_next) //перебираем только beta вариантов
		for (int i_t = 0; i_t < k_beta; i_t++)
		{	//2
			i_next = (this->index_p[i_head][i_t]) - 1;
			//printf("%d %d %d %d\t", i_head, j_head, i_next,i_t);
			//printf("\n");
			if (i_head != i_next && j_head != i_next)
			{	//3
				//добавляем (i_head, i_next)
				//path[1][i_head] = i_next;
				i_temp = path[0][i_next];//дуга для удаления формируется однозначно
										 //path[0][i_next] = i_head;
										 //удаляем (i_temp, i_next)
										 //path[1][i_temp] = -1;
				j_tail = i_next;//просматриваем вершины цикла, j_tail - кандидат

				while (true)
				{
					j_tail = path[1][j_tail];
					if (j_head == j_tail) {//!
						break;
					}//!
					if (j_head != j_tail)
					{	//if1
						//обновляем значения критериев по новому циклу
						s1 = s1_p - s_m[0][i_head][j_head] - s_m[0][i_temp][i_next] - s_m[0][path[0][j_tail]][j_tail] +
							s_m[0][i_head][i_next] + s_m[0][i_temp][j_tail] + s_m[0][path[0][j_tail]][j_head];
						s2 = s2_p - s_m[1][i_head][j_head] - s_m[1][i_temp][i_next] - s_m[1][path[0][j_tail]][j_tail] +
							s_m[1][i_head][i_next] + s_m[1][i_temp][j_tail] + s_m[1][path[0][j_tail]][j_head];

											
						// "центр" окрестности продоминировал новую особь
						// значит ее точно не добавляем в архив
						if ((s1_p < s1 && s2_p <= s2) || (s1_p <= s1 && s2_p < s2))
							continue;

						// формируем перестановку, соотв. новой особи
						vector<vector<int>> path_tmp = path;
						//удаляем (i_head,j_head)
						path_tmp[1][i_head] = -1;
						path_tmp[0][j_head] = -1;
						//добавляем (i_head, i_next)
						path_tmp[1][i_head] = i_next;
						path_tmp[0][i_next] = i_head;
						//удаляем (i_temp, i_next)
						path_tmp[1][i_temp] = -1;
						//добавляем (i_temp, j_tail)
						path_tmp[1][i_temp] = j_tail;
						i_tail = path_tmp[0][j_tail];
						path_tmp[0][j_tail] = i_temp;
						//удаляем (i_tail, j_tail)
						path_tmp[1][i_tail] = -1;
						//добавляем (i_tail, j_head)
						path_tmp[1][i_tail] = j_head;
						path_tmp[0][j_head] = i_tail;

						vector<int> result_tmp(this->get_n());
						result_tmp[0] = 1;
						int i_temp2 = 0;
						for (int i = 1; i < n; ++i)
						{
							result_tmp[i] = path_tmp[1][i_temp2] + 1;
							i_temp2 = path_tmp[1][i_temp2];
						}

						//фиксируем кол-во неживых особей
						int ar_index_no_lst_old = ((Archive*)p_arch)->ar_index_no_lst.size();

						// сравнение новой особи с архивом
						// удаление продоминированных элементов архива
						if (((Archive*)p_arch)->check_new({ s1, s2 }))
						{
							//новая особь продоминировала особь в архве
							// => кол-во неживых особей в архиве увеличилось
							if (((Archive*)p_arch)->ar_index_no_lst.size() > ar_index_no_lst_old)
								// существует улучшение в 3-opt окрестности
								is_improve = true;

							// добавление новой особи в архив
							((Archive*)p_arch)->arch_modify(result_tmp, { s1, s2 });
							
						}
					}
				}//while(true)
			}//3
		}//2

	}//while(pi_deque.size()!=0)
	 //+++++++++++++++++++++++++++++++++++++++++++
	 //+++++++++++++++++++++++++++++++++++++++++++

	return is_improve;
}


////////////////////////////////////////////////////////////////////////////////
//Построение фронтов
//Алгортим Jensen (NlogN)
////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::range_front_J(vector<vector<int>> phi_cur, vector<vector<int>>& pop_cur, vector<int>& indeces_fronts, unsigned int type)
{
	//вектор фронтов (используется индекс особи в популяции pop)
	vector<int> index_front_temp;

	//вектор рангов фронтов
	vector<int> i_rank_temp;

	//отсортированный массив особей (pop_cur)
	vector<vector<int>> pop_sort;

	//предсортировка по 1-му критерию
	//? учесть равенство по 1-му критерию
	for (int i = 0; i < ( (pop_cur.size()>0) ? pop_cur.size(): phi_cur.size() ); i++)
		index_front_temp.push_back(i);
	heap_sort(phi_cur, index_front_temp, -1, 0, index_front_temp.size() - 1);


	//множество фронтов
	//vector<int> - фронт индексов особей
	//vector< vector<int> > - массив фронтов
	vector<vector<int>> F_j;
	vector<int> F_j_temp;
	F_j_temp.push_back(index_front_temp[0]);
	F_j.push_back(F_j_temp);

	//число фронтов
	int E = 1;

	int b;

	for (int i = 1; i < index_front_temp.size(); i++)
	{
		if (phi_cur[F_j[F_j.size() - 1][F_j[F_j.size() - 1].size() - 1]] == phi_cur[index_front_temp[i]])
		{
			F_j[F_j.size() - 1].push_back(index_front_temp[i]);
			continue;
		}

		if (phi_cur[F_j[F_j.size() - 1][F_j[F_j.size() - 1].size() - 1]][1] <= phi_cur[index_front_temp[i]][1])
		{
			//последний фронт доминирует рассматриваемую особь
			//создаем новый фронт и добавляем туда особь
			E++;
			vector<int> F_j_new;
			F_j_new.push_back(index_front_temp[i]);
			F_j.push_back(F_j_new);
		}
		else
		{
			//последний фронт не доминирует рассматриваемую особь
			//ищем фронт с мин индексом, который не доминирует особь
			//используем бинарный поиск
			b = binary_search_Fb(phi_cur, index_front_temp[i], F_j);
			F_j[b].push_back(index_front_temp[i]);
		}
	}


	//index_front_temp.clear();
	for (int i = 0; i < E; i++)
		for (int j = 0; j < F_j[i].size(); j++)
		{
			switch (type)
			{
			case 0:
				indeces_fronts.push_back(F_j[i][j] + 1); //заполняем отсортированными индексами
				i_rank_temp.push_back(i + 1);
				break;
			case 1:
				i_rank_temp.push_back(i + 1);
				pop_sort.push_back(pop_cur[F_j[i][j]]); //заполняем отсортированными особями (если pop_cur не пусто)
				break;
			}
		}
	if (!pop_sort.empty())
	 pop_cur = pop_sort;

	
	return i_rank_temp;
	

	//создаем структры для возраващения значения функции
	//структура содержит:
	//отсортированные индексы векторов
	//ранги
	//Fronts fronts_struct;
	//fronts_struct.ranks = i_rank_temp;
	//fronts_struct.fronts_index = F_j_temp;
}

	////////////////////////////////////////////////////////////////////////////////
	//Бинарный поиск
	//для поиска фронта F_b при вычислении рангов
	////////////////////////////////////////////////////////////////////////////////
	int GA_path::binary_search_Fb(vector<vector<int>> phi_cur, int cur_index, vector<vector<int>> F_j)
	{
		//как правильно возращать значение?
		int index_left = 0;
		int index_right = F_j.size() - 1;
		//int index_end = index_right;

		while (index_right - index_left)
		{
			int index_mid = index_left + (index_right - index_left) / 2;
			if (phi_cur[cur_index] == phi_cur[F_j[index_mid][F_j[index_mid].size() - 1]])
				return index_mid;

			if (phi_cur[cur_index][1] < phi_cur[F_j[index_mid][F_j[index_mid].size() - 1]][1])
				index_right = index_mid;
			else
				index_left = index_mid + 1;
		}

		//if (multi_phitness(pop_cur[cur_index])[1] == multi_phitness(pop_cur[F_j[index_right][F_j[index_right].size() - 1]])[1])
		return index_right;

		//return index_end;
	}


////////////////////////////////////////////////////////////////////////////////
//Построение фронтов (без ранжированя внутри фронта)
////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::range_front(vector<vector<int>>& pop_cur, bool flag_pop_sort)
{
	//вектор фронтов (используется индекс особи в популяции pop)
	vector<vector<int>> index_front;
	vector<int> index_front_temp;
	//вектор рангов фронтов
	vector<int> i_rank_temp;
	if (!flag_pop_sort)
		i_rank_temp.reserve(pop_cur.size());
	
	//отсортированная популяция
	vector<vector<int>> pop_sort;


	//вектор: каждый элемент - вектор индексов особей, которые доминирует особь с данным индексом
	vector<vector<int>> S_p(pop_cur.size(), vector<int>());
	//вектор: каждый элемент - количество особей, которые доминируют особь с данным индексом
	vector<int> n_p(pop_cur.size(), 0);

	for (int i = 0; i < pop_cur.size(); i++)
	{
		for (int j = 0; j < pop_cur.size(); j++)
		{
			if (Pareto_pref(multi_phitness(pop_cur[i]), multi_phitness(pop_cur[j])))
				S_p[i].push_back(j);
			else
				if (Pareto_pref(multi_phitness(pop_cur[j]), multi_phitness(pop_cur[i])))
					n_p[i]++;
		}
		//если особь принадлежит первому фронту
		if (n_p[i] == 0)
		{
			index_front_temp.push_back(i);
			if (flag_pop_sort)
			{
				pop_sort.push_back(pop_cur[i]);
				i_rank_temp.push_back(1);
			}
			else
				i_rank_temp[i] = 1;
			//index_front[0].push_back(i);
		}
	}
	index_front.push_back(index_front_temp);

	//номер фронта
	int i_front = 0;
	while (!index_front[i_front].empty())
	{
		index_front_temp.clear();
		for (int k = 0; k < index_front[i_front].size(); k++)
			for ( int s = 0; s < S_p[index_front[i_front][k]].size(); s++)
			{
				n_p[ S_p[ index_front[i_front][k] ][s] ]--;
				if (n_p[ S_p[index_front[i_front][k]][s] ] == 0)
				{
					
					index_front_temp.push_back(S_p[index_front[i_front][k]][s]);
					if (flag_pop_sort)
					{
						pop_sort.push_back(pop_cur[index_front[i_front][k]]);
						i_rank_temp.push_back(i_front + 2);
					}
					else
						i_rank_temp[S_p[index_front[i_front][k]][s]] = i_front + 2;
				}
			}
		i_front++;
		index_front.push_back(index_front_temp);
	}

	if (flag_pop_sort)
		pop_cur = pop_sort;

	return i_rank_temp;
}

////////////////////////////////////////////////////////////////////////////////
//Построение фронтов (без ранжированя внутри фронта)
//Алгортим Jensen (NlogN)
////////////////////////////////////////////////////////////////////////////////
/*
vector<int> GA_path::range_front_J_(vector<vector<int>>& pop_cur, bool flag_pop_sort)
{
	//вектор фронтов (используется индекс особи в популяции pop)
	vector<vector<int>> index_front;
	vector<int> index_front_temp;
	
	//вектор рангов фронтов
	vector<int> i_rank_temp;
	if (!flag_pop_sort)
		i_rank_temp.resize(pop_cur.size());

	//отсортированная популяция
	vector<vector<int>> pop_sort;

	//предсортировка по 1-му критерию
	//? учесть равенство по 1-му критерию
	for (int i = 0; i < pop_cur.size(); i++)
		index_front_temp.push_back(i);
	heap_sort(pop_cur, index_front_temp, -1, 0, pop_cur.size()-1);

	//множество фронтов
	//vector<int> - особь
	//vector< vector<int> > - фронт
	//vector< vector< vector<int> > > - множество фронтов
	vector<vector<int>> F_j;
	vector<int> F_j_temp;
	F_j_temp.push_back(index_front_temp[0]);
	F_j.push_back(F_j_temp);
	
	//число фронтов
	int E = 1;

	int b;

	for (int i = 1; i < pop_cur.size(); i++)
	{
		if (multi_phitness(pop_cur[F_j[F_j.size() - 1][F_j[F_j.size() - 1].size() - 1]]) == multi_phitness(pop_cur[index_front_temp[i]]))
		{
			F_j[F_j.size() - 1].push_back(index_front_temp[i]);
			continue;
		}
				
		if ( multi_phitness(pop_cur[F_j[ F_j.size() - 1 ][ F_j[F_j.size() - 1].size() - 1 ]])[1] <= multi_phitness(pop_cur[index_front_temp[i]])[1] )
		{
			//последний фронт доминирует рассматриваемую особь
			//создаем новый фронт и добавляем туда особь
			E++;
			vector<int> F_j_new;
			F_j_new.push_back(index_front_temp[i]);
			F_j.push_back(F_j_new);
		}
		else
		{
			//последний фронт не доминирует рассматриваемую особь
			//ищем фронт с мин индексом, который не доминирует особь
			//используем бинарный поиск
			b = binary_search_Fb_(pop_cur, index_front_temp[i], F_j);
			F_j[b].push_back(index_front_temp[i]);
		}
	}

	for (int i = 0; i < E; i++)
	{
		for (int j = 0; j < F_j[i].size();j++)
		{
			if (flag_pop_sort)
			{
				i_rank_temp.push_back(i+1);
				pop_sort.push_back(pop_cur[F_j[i][j]]);
			}
			else
				i_rank_temp[F_j[i][j]] =i+1;
		}
	}

	if (flag_pop_sort)
		pop_cur = pop_sort;

	return i_rank_temp;
}
*/



////////////////////////////////////////////////////////////////////////////////
//Бинарный поиск
//для поиска фронта F_b при вычислении рангов
////////////////////////////////////////////////////////////////////////////////
/*
int GA_path::binary_search_Fb_(vector<vector<int>> pop_cur, int cur_index, vector<vector<int>> F_j)
{	
	//как правильно возращать значение?
	int index_left = 0;
	int index_right = F_j.size() - 1;
	//int index_end = index_right;

	while (index_right - index_left)
	{
		int index_mid = index_left + (index_right - index_left) / 2;
		if ( multi_phitness(pop_cur[cur_index]) == multi_phitness(pop_cur[F_j[index_mid][F_j[index_mid].size() - 1]]) )
			return index_mid;

		if (multi_phitness(pop_cur[cur_index])[1] < multi_phitness(pop_cur[F_j[index_mid][F_j[index_mid].size() - 1]])[1])
			index_right = index_mid;
		else
			index_left = index_mid + 1;
	}

	//if (multi_phitness(pop_cur[cur_index])[1] == multi_phitness(pop_cur[F_j[index_right][F_j[index_right].size() - 1]])[1])
		return index_right;

	//return index_end;
}
*/


////////////////////////////////////////////////////////////////////////////////
//Построение crowding distance (ранжированя внутри фронта с рангом = rank)
//flag = true - используется алгоритм пирамидальной сортировки
//flag = false - исопользуется алгоритм быстрой сортировки
////////////////////////////////////////////////////////////////////////////////
void GA_path::crowd_dist(vector<double>& i_dist_cur, int rank, vector<int> i_rank_cur,
	vector<vector<int>> pop_cur, bool flag_sort)
{
	if ((rank < 1) || (rank > *max_element(i_rank_cur.cbegin(), i_rank_cur.cend())))
		return;
	
	//переписать через алгоритмы для контейнеров

	//для хранения индексов особей из фронта с рангом=rank
	vector<int> index_front_temp;
	vector<vector<int>> phi_front_temp(m, vector<int>());
	//vector<double> i_dist(pop_cur.size());
	//int** phi_front_temp;
	for (int i = 0; i < i_rank_cur.size(); i++)
	{
		if (i_rank_cur[i] == rank)
		{
			index_front_temp.push_back(i);
			for (int j = 0; j < m; j++)
			{
				//заполнить в отедельный массив multi_phitness(pop_cur[i])[j]
				phi_front_temp[j].push_back(this->multi_phitness(pop_cur[i])[j]);
			}
			i_dist_cur[i] = 0;
		}
	}

	vector<int> min_phi, max_phi;
	vector<int> j_min(this->get_m(), 0);
	vector<int> j_max(this->get_m(), 0);
	vector<int> index_front_cur_crit_temp;
	for (int i = 0; i < m; i++)
	{
		min_phi.push_back(phitness(s_m[i], pop_cur[index_front_temp[0]]));
		max_phi.push_back(phitness(s_m[i], pop_cur[index_front_temp[0]]));
		//ищем мин и макс значения
		for (int j = 1; j < index_front_temp.size(); j++)
		{
			//поиск мин значения по критерию i
			if (phitness(s_m[i], pop_cur[index_front_temp[j]]) < min_phi[i])
			{
				min_phi[i] = phitness(s_m[i], pop_cur[index_front_temp[j]]);
				j_min[i] = j;
			}
			//поиск макс значения по критерию i
			if (phitness(s_m[i], pop_cur[index_front_temp[j]]) > max_phi[i])
			{
				max_phi[i] = phitness(s_m[i], pop_cur[index_front_temp[j]]);
				j_max[i] = j;
			}
		}
		
		//инициализируем текущую пос-ть индексов особей согласано их следованию в популяции
		index_front_cur_crit_temp = index_front_temp;
		//сортируем пос-ть индексов особей в соответствии с их значениями по i-му критерию
		if (flag_sort)
		{
			//при помощи сортировки пирамидой
			//heap_sort(phi_front_temp[i], index_front_cur_crit_temp, phi_front_temp[i].size());
		}
		else
		{
			//при помощи быстрой сортировки
			quick_sort(phi_front_temp[i], index_front_cur_crit_temp, 0, phi_front_temp[i].size() - 1);
		}
		
		i_dist_cur[index_front_cur_crit_temp[0]] = i_dist_cur[index_front_cur_crit_temp[index_front_cur_crit_temp.size()-1]] = INF;
		
		//вычисление расстояния для особи из pop с индеком index_front_temp[j]
		for (int j = 1; j < index_front_cur_crit_temp.size()-1; j++)
			i_dist_cur[index_front_cur_crit_temp[j]] += (double) (this->multi_phitness( pop_cur[ index_front_cur_crit_temp[j + 1] ] )[i]
				- this->multi_phitness( pop_cur[ index_front_cur_crit_temp[j - 1] ] )[i] ) / (max_phi[i] - min_phi[i]);
	}
}

//НОВАЯ ФУНКЦИЯ
////////////////////////////////////////////////////////////////////////////////
//Построение crowding distance (ранжированя внутри фронта с рангом = rank)
//flag = true - используется алгоритм пирамидальной сортировки
//flag = false - исопользуется алгоритм быстрой сортировки
////////////////////////////////////////////////////////////////////////////////
void GA_path::crowd_dist_new(vector<vector<int>> pop_cur, bool flag_sort)
{
	//предполоагается, что популяция pop_cur отсортирована

	//ранг текущего фронта
	int i_rank_cur = 1;
	//превышение фронта, не полностью попавшего в новую популяцию, над размерностью популяции N 
	int num_extra = 0;

	int min_phi, max_phi;


	 
	//для хранения индексов особей из фронта с рангом=rank
	vector<int> index_front_temp;
	vector<int> index_front_temp_not_sorted;
	
	for (int l = 0; l < this->get_N(); l++)
		i_dist[l] = 0;

	if (pop_cur.size() == this->get_N())
	{
		//crowded dist считаем только для первых N особей, дальше не надо
		for (int i = 0; i < pop_cur.size(); i++)
		{
			if ((i_rank[i] > i_rank_cur) || (i == this->get_N() - 1)) //признак окончания фронта
			{
				//если признак окончания фронта - конец вектора, то последний индекс добавляем
				if (i == this->get_N() - 1)
					index_front_temp_not_sorted.push_back(i);

				for (int j = 0; j < this->get_m(); j++)
				{
					index_front_temp = index_front_temp_not_sorted;

					min_phi = phitness(s_m[j], pop_cur[index_front_temp[0]]);
					max_phi = phitness(s_m[j], pop_cur[index_front_temp[0]]);
					//ищем мин и макс значения
					for (int k = index_front_temp[0] + 1; k <= index_front_temp[index_front_temp.size() - 1]; k++)
					{
						//поиск мин значения по текущему критерию
						if (phitness(s_m[j], pop_cur[k]) < min_phi)
							min_phi = phitness(s_m[j], pop_cur[k]);

						//поиск макс значения\ по текущему критерию
						if (phitness(s_m[j], pop_cur[k]) > max_phi)
							max_phi = phitness(s_m[j], pop_cur[k]);
					}

					//инициализируем текущую пос-ть индексов особей согласано их следованию в популяции
					//index_front_cur_crit_temp = index_front_temp;
					//сортируем пос-ть индексов особей в соответствии с их значениями по i-му критерию
					if (flag_sort)
					{
						//при помощи сортировки пирамидой
						//true - сортировка по компоненте критерия
						heap_sort(pop_cur, index_front_temp, j, index_front_temp[0], index_front_temp[index_front_temp.size() - 1]);
					}
					else
					{
						//при помощи быстрой сортировки
						//quick_sort(index_front_temp, 0, index_front_temp.size() - 1);
					}

					i_dist[index_front_temp[0]] = i_dist[index_front_temp[index_front_temp.size() - 1]] = INF;

					//вычисление расстояния для особи из pop с индеком index_front_temp[j]
					int prev, next;
					for (int l = 1; l < index_front_temp.size() - 1; l++)
					{
						next = this->multi_phitness(pop_cur[index_front_temp[l + 1]])[j];
						prev = this->multi_phitness(pop_cur[index_front_temp[l - 1]])[j];
						i_dist[index_front_temp[l]] += (double)(next - prev) / (max_phi - min_phi);
					}


				}
				i_rank_cur++;
				index_front_temp_not_sorted.clear();
				index_front_temp_not_sorted.push_back(i);
			}
			else
			{
				//заполняем вектор индексами текущего фронта
				index_front_temp_not_sorted.push_back(i);
			}
		}
	}

	if (pop_cur.size() == this->get_ext_N())
	{
		//crowded dist считаем только для первых N особей, дальше не надо
		for (int i = 0; i < pop_cur.size(); i++)
		{
			//считаем, сколько особей из последнего фронта не войдут в новую популяцию
			//для них тоже считаем i_dist, но их не включаем в pop и i_rank
			if (i >= this->get_N())
				num_extra++;


			if ((i_rank_R_t[i] > i_rank_cur) || (i == this->get_ext_N() - 1)) //признак окончания фронта
			{

				//если признак окончания фронта - конец вектора, то последний индекс добавляем
				if (i == this->get_ext_N() - 1)
				{
					index_front_temp_not_sorted.push_back(i);
					i_dist.resize(this->get_ext_N());
				}
				else
				{
					if (num_extra > 1)
						i_dist.resize(this->get_N() + num_extra - 1);
				}
				
				for (int j = 0; j < this->get_m(); j++)
				{
					index_front_temp = index_front_temp_not_sorted;

					min_phi = phitness(s_m[j], pop_cur[index_front_temp[0]]);
					max_phi = phitness(s_m[j], pop_cur[index_front_temp[0]]);
					//ищем мин и макс значения
					for (int k = index_front_temp[0] + 1; k <= index_front_temp[index_front_temp.size() - 1]; k++)
					{
						//поиск мин значения по текущему критерию
						if (phitness(s_m[j], pop_cur[k]) < min_phi)
							min_phi = phitness(s_m[j], pop_cur[k]);

						//поиск макс значения по текущему критерию
						if (phitness(s_m[j], pop_cur[k]) > max_phi)
							max_phi = phitness(s_m[j], pop_cur[k]);
					}
					
					//инициализируем текущую пос-ть индексов особей согласано их следованию в популяции
					//index_front_cur_crit_temp = index_front_temp;
					//сортируем пос-ть индексов особей в соответствии с их значениями по i-му критерию
					if (flag_sort)
					{
						//при помощи сортировки пирамидой
						//true - сортировка по компоненте критерия
						heap_sort(pop_cur, index_front_temp, j, index_front_temp[0], index_front_temp[index_front_temp.size() - 1]);
					}
					else
					{
						//при помощи быстрой сортировки
						//quick_sort(index_front_temp, 0, index_front_temp.size() - 1);
					}
					
					i_dist[index_front_temp[0]] = i_dist[index_front_temp[index_front_temp.size() - 1]] = INF;

					//вычисление расстояния для особи из pop с индеком index_front_temp[j]
					int prev, next;
					for (int l = 1; l < index_front_temp.size() - 1; l++)
					{
						next = this->multi_phitness(pop_cur[index_front_temp[l + 1]])[j];
						prev = this->multi_phitness(pop_cur[index_front_temp[l - 1]])[j];
						i_dist[index_front_temp[l]] += (double)(next - prev) / (max_phi - min_phi);
					}


				}
				i_rank_cur++;
				index_front_temp_not_sorted.clear();
				
				if (num_extra)
					break;
				index_front_temp_not_sorted.push_back(i);
			}
			else
			{
				//заполняем вектор индексами текущего фронта
				index_front_temp_not_sorted.push_back(i);
			}
		}
		
	}
}

////////////////////////////////////////////////////////////////////////////////
//Оператор Crowded-Comparison
//в качестве аргументов используем индексы ососбей
////////////////////////////////////////////////////////////////////////////////
bool GA_path::crowd_comp_oper(int i_p1, int i_p2) //(vector<int> p1, vector<int> p2)
{
	//индексы для особей p1, p2
	//int i_p1, i_p2;

	//итератор особи p1
	//vector<vector<int>>::const_iterator it_p1;
	//it_p1 = find(pop.cbegin(), pop.cend(), p1);
	//итератор особи p2
	//vector<vector<int>>::const_iterator it_p2;
	//it_p2 = find(pop.cbegin(), pop.cend(), p2);

	//поиск индексов особей p1, p2 - не используется
	/*
	for (int i = 0; i < pop.size(); i++)
	{
		if (p1 == pop[i])
			i_p1 = i;
		if (p2 == pop[i])
			i_p2 = i;
	}
	*/
	
	if ((i_rank[i_p1] < i_rank[i_p2]) || ((i_rank[i_p1] == i_rank[i_p2]) && (i_dist[i_p1] > i_dist[i_p2])))
		return true;
	else
		return false; 
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////СОРТИРОВКА///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//Быстрая сортировка
////////////////////////////////////////////////////////////////////////////////
void GA_path::quick_sort(vector<int>& arr_val, vector<int>& arr_index, int left, int right)
{
	int i = left, j = right;
	int tmp, tmp_index;
	int pivot = arr_val[(left + right) / 2];

	/* partition */
	while (i <= j)
	{
		while (arr_val[i] < pivot)
			i++;
		while (arr_val[j] > pivot)
			j--;
		if (i <= j)
		{
			tmp = arr_val[i];
			arr_val[i] = arr_val[j];
			arr_val[j] = tmp;
			tmp_index = arr_index[i];
			arr_index[i] = arr_index[j];
			arr_index[j] = tmp_index;
			i++;
			j--;
		}
	};

	/* recursion */
	if (left < j)
		quick_sort(arr_val, arr_index, left, j);
	if (i < right)
		quick_sort(arr_val, arr_index, i, right);
}


////////////////////////////////////////////////////////////////////////////////
//Пирамидальная сортировка
////////////////////////////////////////////////////////////////////////////////
/*
void GA_path::heap_sort_new(vector<vector<int>>& pop_cur, int i_start, int i_stop)
{
	int delta = i_start;
	// Формируем нижний ряд пирамиды
	for (int i = ((i_stop - i_start + 1) / 2) - 1; i >= 0; i--)
		sift_down(pop_cur, this->i_dist, i, delta);
	// Просеиваем через пирамиду остальные элементы
	for (int i = i_stop - i_start; i >= 1; i--)
	{
		int temp = pop_cur[delta];
		int temp_index = this->i_dist[delta];
		pop_cur[delta] = pop_cur[delta + i];
		this->i_dist[delta] = this->i_dist[delta + i];
		pop_cur[delta + i] = temp;
		this->i_dist[delta + i] = temp_index;
		sift_down(pop_cur, this->i_dist, 0, i, delta);
	}
}
*/

////////////////////////////////////////////////////////////////////////////////
//Пирамидальная сортировка
////////////////////////////////////////////////////////////////////////////////
void GA_path::heap_sort(vector<vector<int>> pop_cur, vector<int>& numbers_index, int num_criteria, int index_begin, int index_end)
{
	// Формируем нижний ряд пирамиды
	for (int i = ((index_end - index_begin + 1) / 2) - 1; i >= 0; i--)
		if (num_criteria != -1)
			sift_down(pop_cur, numbers_index, num_criteria, i, index_end - index_begin + 1, 0);
		else
			sift_down(pop_cur, numbers_index, i, index_end - index_begin + 1, 0); // Jensen в LS и осн. вычисл. рангов
	// Просеиваем через пирамиду остальные элементы
	for (int i = index_end - index_begin; i >= 1; i--)
	{
		//int temp = numbers[0];
		int temp_index = numbers_index[0];
		//numbers[0] = numbers[i];
		numbers_index[0] = numbers_index[i];
		//numbers[i] = temp;
		numbers_index[i] = temp_index;
		if (num_criteria != -1)
			sift_down(pop_cur, numbers_index, num_criteria, 0, i, 0);
		else
			sift_down(pop_cur, numbers_index, 0, i, 0); // Jensen в LS и осн. вычисл. рангов
	}
}

//НЕ ИСПОЛЬЗУЕТСЯ
/*
void GA_path::heap_sort_ls(vector<vector<int>> pop_cur, vector<int>& numbers_index, int num_criteria, int index_begin, int index_end)
{
	// Формируем нижний ряд пирамиды
	for (int i = ((index_end - index_begin + 1) / 2) - 1; i >= 0; i--)
			sift_down_ls(pop_cur, numbers_index, i, index_end - index_begin + 1, 0);
	// Просеиваем через пирамиду остальные элементы
	for (int i = index_end - index_begin; i >= 1; i--)
	{
		//int temp = numbers[0];
		int temp_index = numbers_index[0];
		//numbers[0] = numbers[i];
		numbers_index[0] = numbers_index[i];
		//numbers[i] = temp;
		numbers_index[i] = temp_index;
			sift_down_ls(pop_cur, numbers_index, 0, i, 0);
	}
}
*/

//при построении crowed dist
void GA_path::heap_sort(vector<int>& numbers_index, int index_begin, int index_end)
{
	// Формируем нижний ряд пирамиды
	for (int i = ((index_end - index_begin + 1) / 2) - 1; i >= 0; i--)
		sift_down(numbers_index, i, index_end - index_begin + 1, index_begin);
	// Просеиваем через пирамиду остальные элементы
	for (int i = index_end - index_begin; i >= 1; i--)
	{
		int temp_index = numbers_index[0];
		double temp = i_dist[index_begin];
		
		numbers_index[0] = numbers_index[i];
		i_dist[index_begin] = i_dist[index_begin + i];
		
		numbers_index[i] = temp_index;
		i_dist[index_begin + i] = temp;
		
		sift_down(numbers_index, 0, i, index_begin);
	}
}

//Функция "просеивания" через кучу (см. в функции пирамидальной сортировки)
//при построении crowed dist
void GA_path::sift_down(vector<vector<int>> pop_cur, vector<int>& numbers_index, int num_criteria, int root, int bottom, int delta)
{
	int max_child; // индекс максимального потомка
	int done = 0; // флаг того, что куча сформирована
				  // Пока не дошли до последнего ряда
	while ( (root * 2 + 1 < bottom) && (!done) )
	{
		if ( root * 2 + 2 == bottom)    // если мы в последнем ряду, 
			max_child = root * 2 + 1;    // запоминаем левый потомок
									// иначе запоминаем больший потомок из двух
		else
		{
				if (phitness(s_m[num_criteria], pop_cur[numbers_index[delta + root * 2 + 1]]) > 
						phitness(s_m[num_criteria], pop_cur[numbers_index[delta + root * 2 + 2]]))
					max_child = root * 2 + 1;
				else
					max_child = root * 2 + 2;
		}
		// если элемент вершины меньше максимального потомка
			if (phitness(s_m[num_criteria], pop_cur[numbers_index[delta + root]]) <
					phitness(s_m[num_criteria], pop_cur[numbers_index[delta + max_child]]))
			{
				//int temp = numbers[delta + root]; // меняем их местами
				int temp_index = numbers_index[delta + root]; // меняем индексы местами		
				//numbers[delta + root] = numbers[delta + max_child];
				numbers_index[delta + root] = numbers_index[delta + max_child];
				//numbers[delta + max_child] = temp;
				numbers_index[delta + max_child] = temp_index;
				root = max_child;
			}
			else // иначе
				done = 1; // пирамида сформирована
		
	}
}


//НЕ ИСПОЛЬЗУЕТСЯ
//Функция "просеивания" через кучу (см. в функции пирамидальной сортировки)
/*
void GA_path::sift_down_(vector<vector<int>> pop_cur, vector<int>& numbers_index, int root, int bottom, int delta)
{
	int max_child; // индекс максимального потомка
	int done = 0; // флаг того, что куча сформирована
				  // Пока не дошли до последнего ряда
	while ((root * 2 + 1 < bottom) && (!done))
	{
		if (root * 2 + 2 == bottom)    // если мы в последнем ряду, 
			max_child = root * 2 + 1;    // запоминаем левый потомок
										 // иначе запоминаем больший потомок из двух
		else
		{
			if ( //сравнение по 1-му критерию
				(multi_phitness(pop_cur[numbers_index[delta + root * 2 + 1]])[0] >
					multi_phitness(pop_cur[numbers_index[delta + root * 2 + 2]])[0])
				|| //если по 1-му равенство, сравнение по 2-му критерию
				((multi_phitness(pop_cur[numbers_index[delta + root * 2 + 1]])[0] ==
					multi_phitness(pop_cur[numbers_index[delta + root * 2 + 2]])[0]) &&
					(multi_phitness(pop_cur[numbers_index[delta + root * 2 + 1]])[1] >
						multi_phitness(pop_cur[numbers_index[delta + root * 2 + 2]])[1]))
				)
				max_child = root * 2 + 1;
			else
				max_child = root * 2 + 2;
		}
		// если элемент вершины меньше максимального потомка
		if ( //сравнение по 1-му критерию
			(multi_phitness(pop_cur[numbers_index[delta + root]])[0] <
				multi_phitness(pop_cur[numbers_index[delta + max_child]])[0])
			|| //если по 1-му равенство, сравнение по 2-му критерию
			((multi_phitness(pop_cur[numbers_index[delta + root]])[0] ==
				multi_phitness(pop_cur[numbers_index[delta + max_child]])[0]) &&
				(multi_phitness(pop_cur[numbers_index[delta + root]])[1] <
					multi_phitness(pop_cur[numbers_index[delta + max_child]])[1]))
			)
		{
			//int temp = numbers[delta + root]; // меняем их местами
			int temp_index = numbers_index[delta + root]; // меняем индексы местами		
														  //numbers[delta + root] = numbers[delta + max_child];
			numbers_index[delta + root] = numbers_index[delta + max_child];
			//numbers[delta + max_child] = temp;
			numbers_index[delta + max_child] = temp_index;
			root = max_child;
		}
		else // иначе
			done = 1; // пирамида сформирована

	}
}
*/

//Функция "просеивания" через кучу (см. в функции пирамидальной сортировки)
//сравнивается по двум критериям
//в препроцессинге локального поиска
//	при построении перестановок pi и p_i
void GA_path::sift_down(vector<vector<int>> pop_cur, vector<int>& numbers_index, int root, int bottom, int delta)
{
	int max_child; // индекс максимального потомка
	int done = 0; // флаг того, что куча сформирована
				  // Пока не дошли до последнего ряда
	while ((root * 2 + 1 < bottom) && (!done))
	{
		if (root * 2 + 2 == bottom)    // если мы в последнем ряду, 
			max_child = root * 2 + 1;    // запоминаем левый потомок
										 // иначе запоминаем больший потомок из двух
		else
		{
			if ( //сравнение по 1-му критерию
				(pop_cur[numbers_index[delta + root * 2 + 1]][0] >
					pop_cur[numbers_index[delta + root * 2 + 2]][0])
				|| //если по 1-му равенство, сравнение по 2-му критерию
				(pop_cur[numbers_index[delta + root * 2 + 1]][0] ==
					pop_cur[numbers_index[delta + root * 2 + 2]][0] &&
					pop_cur[numbers_index[delta + root * 2 + 1]][1] >
						pop_cur[numbers_index[delta + root * 2 + 2]][1])
				)
				max_child = root * 2 + 1;
			else
				max_child = root * 2 + 2;
		}
		// если элемент вершины меньше максимального потомка
		if ( //сравнение по 1-му критерию
			(pop_cur[numbers_index[delta + root]][0] <
				pop_cur[numbers_index[delta + max_child]][0])
			|| //если по 1-му равенство, сравнение по 2-му критерию
			(pop_cur[numbers_index[delta + root]][0] ==
				pop_cur[numbers_index[delta + max_child]][0] &&
				pop_cur[numbers_index[delta + root]][1] <
					pop_cur[numbers_index[delta + max_child]][1])
			)
		{
			//int temp = numbers[delta + root]; // меняем их местами
			int temp_index = numbers_index[delta + root]; // меняем индексы местами		
														  //numbers[delta + root] = numbers[delta + max_child];
			numbers_index[delta + root] = numbers_index[delta + max_child];
			//numbers[delta + max_child] = temp;
			numbers_index[delta + max_child] = temp_index;
			root = max_child;
		}
		else // иначе
			done = 1; // пирамида сформирована

	}
}

//в сортировке последнего фронта (не полностью вошедшего в R_t)
//	сортировка по crowed dist
void GA_path::sift_down(vector<int>& numbers_index, int root, int bottom, int delta)
{
	int max_child; // индекс максимального потомка
	int done = 0; // флаг того, что куча сформирована
				  // Пока не дошли до последнего ряда
	while ((root * 2 + 1 < bottom) && (!done))
	{
		if (root * 2 + 2 == bottom)    // если мы в последнем ряду, 
			max_child = root * 2 + 1;    // запоминаем левый потомок
										 // иначе запоминаем больший потомок из двух
		else
		{		
				if (i_dist[delta + root * 2 + 1] < i_dist[delta + root * 2 + 2])
					max_child = root * 2 + 1;
				else
					max_child = root * 2 + 2;
		}
		// если элемент вершины меньше максимального потомка
			if (i_dist[delta + root] > i_dist[delta + max_child])
			{
				int temp_index = numbers_index[root]; // меняем индексы местами
				double temp = i_dist[delta + root]; // меняем их местами

				numbers_index[root] = numbers_index[max_child];
				i_dist[delta + root] = i_dist[delta + max_child];

				numbers_index[max_child] = temp_index;
				i_dist[delta + max_child] = temp;

				root = max_child;
			}
			else // иначе
				done = 1; // пирамида сформирована
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//Построение особи Венгерским методом + склейка циклов
////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::patching_algorithm(vector<vector<int>> s, int c_max, boolean orig_or_mod)
{
	vector<int> assignment;
	
	//ставим бесконечность по диагонали
	for (int i = 0; i < n; ++i)
		s[i][i] = (c_max + 1) * n;
	
	//***вывод на экран матрицы стоимостей
	//        interMatrix(n,n,c);
	
	//решение задачи о назначениях
	this->is_AP_solution_opt = false;
	assignment = Hungarian_method(n, n, s);
	//        interPermutation(n, assignment);
	//***стоимость найденного назначения
	//        System.out.println("CostAssignment= "+costAssignment(n, c, assignment));

	////////////////////////////////////////////////////////////////////////
	//ищем циклы///////////////////////////////////////////////////////////// 
	int count_cycles = 0;//число циклов
	int count_vertex_in_current_cycle = 0;//число вершин в текущем цикле
	vector<int> head_cycles(n);//начало цикла
	vector<int> length_cycles(n);//число вершин в цикле
	vector<int> cost_cycles(n);//длина цикла 
	vector<int> cycle_vertex(n);//принадлежность вершин циклам (for  modif alg)
	//vector<bool> view(n,false);//индикатор того просмотрена ли вершина при поиске циклов
	bool* view = new bool[n];
	for (int i = 0; i < n; i++)
		view[i] = false;
	bool temp;
	int i_head = 1, i_tail, i_headM, i_tailM, i_max, i_temp;// начало и конец текущего цикла i_head=1, i_tail
															////////////////////////////////////////////////////////////////////////
															//процедура поиска циклов в перестановке////////////////////////////////
	while (i_head != -1)
	{
		//ищем начало очередного цикла
		i_head = i_tail = -1;
		for (int i = 0; i < n; ++i)
		{
			if (!view[i])
			{
				i_head = i;
				i_tail = assignment[i];
				cycle_vertex[i] = count_cycles;//номер цикла для вершины (for  modif alg)
				view[i] = true;
				head_cycles[count_cycles] = i_head;
				break;
			}
		}
		//нашли новый цикл
		if (i_head != -1)
		{
			count_vertex_in_current_cycle = 1;
			cost_cycles[count_cycles] = s[i_head][assignment[i_head]];
			while (i_head != i_tail)
			{
				view[i_tail] = true;
				cycle_vertex[i_tail] = count_cycles;//номер цикла для вершины (for  modif alg)
				cost_cycles[count_cycles] += s[i_tail][assignment[i_tail]];
				i_tail = assignment[i_tail];
				count_vertex_in_current_cycle++;
			}
			length_cycles[count_cycles] = count_vertex_in_current_cycle;
			count_cycles++;
		}
	}
	//***вывод информации о циклах   
	printf("Number of cycles: %d\n", count_cycles);
	//        interPermutation(countCycles, headCycles);
	//        interPermutation(countCycles, lengthCycles);
	//        interPermutation(countCycles, costCycles);
	//        interPermutation(n, CycleVertex);//нужно только для модифицированного метода
	//процедура поиска циклов в перестановке////////////////////////////////
	////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////
	//склеиваем циклы///////////////////////////////////////////////////////
	if (count_cycles > 1)
	{
		//ищем цикл с максимальным весом!
		i_max = -1;//номер самого длинного цикла, нумерация с 0
		i_tail = -1;//длина самого большого цикла (сумма длин дуг)
		for (int i = 0; i < count_cycles; ++i)
		{
			if (cost_cycles[i] > i_tail)
			{
				i_tail = cost_cycles[i];
				i_max = i;
			}
		}
		if (length_cycles[i_max] < count_cycles - 1) //не хватает вершин в цикле
		{
			//пытаемся найти самый длинный цикл по числу вершин
			i_max = -1;//номер самого длинного цикла, нумерация с 0
			i_tail = 0;//длина самого большого цикла (число вершин)
			for (int i = 0; i < count_cycles; ++i)
			{
				if (length_cycles[i] > i_tail)
				{
					i_tail = length_cycles[i];
					i_max = i;
				}
			}
		}
		//***вывод на экран
		//            System.out.println("Номер самого длинного цикла "+i_max);
		if ( (length_cycles[i_max] < count_cycles - 1) || !orig_or_mod)
		{
			////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////
			//modified patching process
			// lengthCycles[i_max]<countCycles-1
			//сначала все вершины непомеченные
			for (int i = 0; i < n; ++i)
				view[i] = false;

			int i_min;
			//постепенно склеиваем циклы
			for (int iter = 0; iter<count_cycles - 1; ++iter)
			{
				i_min = 0;//самый короткий цикл 
				i_tail = cost_cycles[0];//длина самого короткого цикла 
				for (int i = 1; i < count_cycles; ++i)
				{
					if (cost_cycles[i] < i_tail)
					{
						i_tail = cost_cycles[i];
						i_min = i;
					}
				}
				
				//***вывод на экран
				//               System.out.println("Короткий цикл "+i_min);
				i_tail = (c_max + 1)*n + 1;//самая большая стоимость назначения
				i_head = i_headM = -1;//вершины для обмена
				for (int i = 0; i < n; ++i)
				{
					if ( (cycle_vertex[i] == i_min) && (!view[i]) )
					{
						for (int j = 0; j < n; ++j)
						{
							if ( (cycle_vertex[j] != i_min) && (!view[i]) )
							{
								i_tailM = s[i][assignment[j]] + s[j][assignment[i]]
									- s[i][assignment[i]] - s[j][assignment[j]];
								
								if (i_tailM < i_tail)
								{
									i_head = i;
									i_headM = j;
									i_tail = i_tailM;//стоимость обмена
								}
								//                System.out.println(i+" "+j+" "+i_tail+" "+i_tailM);                

							}
						}
					}
				}

				//склеиваем циклы с вершинами  i_head и i_headM
				//                System.out.println("Склейка циклов ("+i_head+", "+i_headM+") ="+i_tail);
				view[i_head] = view[i_headM] = true;//помечаем выбранные вершины
				i_max = cycle_vertex[i_headM];
				cost_cycles[i_min] += cost_cycles[i_max] + i_tail;//увличиваем стоимость цикла i_min
				cost_cycles[i_max] = (c_max + 1)*n + 1;//исключаем цикл i_max из рассмотрения
				
				for (int i = 0; i < n; ++i)
					if (cycle_vertex[i] == i_max)
						cycle_vertex[i] = i_min;

				i_tailM = assignment[i_head];
				assignment[i_head] = assignment[i_headM];
				assignment[i_headM] = i_tailM;

				//        interPermutation(countCycles, costCycles);
				//        interPermutation(n, CycleVertex);
				//        interPermutation(n, assignment);
				//        System.out.println("CostAssignment= "+costAssignment(n, c, assignment));
			}
			//***вывод на экран решения задачи коммивояжера       
			//        interPermutation(n, assignment);
			//        System.out.println("CostAssignment= "+costAssignment(n, c, assignment));

			
		}

		else
		{
			//original patching process    
			////////////////////////////////////////////////////////////////////
			//процедура склейки циклов//////////////////////////////////////////

			//формируем матрицу для вспомогательной задачи о назначениях
			// строки соответсвуют маленьким циклам, а столбцы - вершинам большого цикла
			// i_head - номер самого длинного цикла, нумерация с нуля   
			vector<vector<int>> c_temp(count_cycles - 1, vector<int>(length_cycles[i_max]));
			vector<int> column(length_cycles[i_max]);//элементы большого цикла
			vector<vector<int>> row(count_cycles - 1, vector<int>(length_cycles[i_max]));//наилучшие элементы маленького цикла
			i_headM = head_cycles[i_max];//текущая вершина большого цикла
			
			for (int j = 0; j < length_cycles[i_max]; ++j)
			{
				column[j] = i_headM;//элементы соответствующие столбцам;
				i_temp = 0;//порядковый номер очередного маленького цикла, нумерация с 0
				for (int i = 0; i < count_cycles; ++i)
				{
					if (i != i_max)
					{
						i_head = head_cycles[i];
						i_tail = s[i_head][assignment[i_headM]] + s[i_headM][assignment[i_head]] -
							s[i_headM][assignment[i_headM]] - s[i_head][assignment[i_head]];
						row[i_temp][j] = i_head;
						//System.out.println(i_head+" "+i_headM+" "+i_tail +" "+i_temp+" "+row[i_temp][j]);
						
						for (int l = 1; l < length_cycles[i]; ++l)
						{
							i_head = assignment[i_head];
							i_tailM = s[i_head][assignment[i_headM]] + s[i_headM][assignment[i_head]] -
								s[i_headM][assignment[i_headM]] - s[i_head][assignment[i_head]];
							
							if (i_tailM<i_tail)
							{
								i_tail = i_tailM;
								row[i_temp][j] = i_head;
							}
							//System.out.println(i_head+" "+i_headM+" "+i_tail +" "+i_tailM +" "+i_temp+" "+row[i_temp][j]);
						}
						c_temp[i_temp][j] = i_tail;
						i_temp++;
					}
				}
				i_headM = assignment[i_headM];
			}
			//***вывод на экран вспомогательной матрицы
			//              interMatrix(lengthCycles[i_max],countCycles-1,c_temp);
			//              interPermutation(lengthCycles[i_max], column);
			//              interMatrix(lengthCycles[i_max],countCycles-1,row);
			
			//вспомогательная матрица готова, решаем задачу о назначениях
			head_cycles = Hungarian_method(length_cycles[i_max], count_cycles - 1, c_temp);
			
			//вывод на экран решения вспомогательной задачи о назначениях
			//              interPermutation(countCycles-1, headCycles);
			
			//склеиваем циклы
			for (int i = 0; i < count_cycles - 1; ++i)
			{
				i_head = assignment[row[i][head_cycles[i]]];
				assignment[row[i][head_cycles[i]]] = assignment[column[head_cycles[i]]];
				assignment[column[head_cycles[i]]] = i_head;
			}
			//***вывод на экран решения задачи коммивояжера
			//              interPermutation(n, assignment);
			//              System.out.println("CostAssignment= "+costAssignment(n, c, assignment));
			
			//процедура склейки циклов//////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////// 
		}
	}
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	else {
		//countCycles=1
		this->is_AP_solution_opt = true;
		////
		////            System.out.println("Оптимальное решение задачи о назначениях есть"
		////                    + " циклическая перестановка - цикл коммивояжера найден!");
	}

	////////////////////////////////////////////////////////////////////////
	//возвращаем нули по диагонали
	for (int i = 0; i < n; ++i)
		s[i][i] = 0;

	//////////////////////////////////////////////////////////////////////// 
	return assignment;
}

////////////////////////////////////////////////////////////////////////////////
//Венгерский метод
////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::Hungarian_method(int n, int m, vector<vector<int>> cost)
{
	//m - число строк, n - число столбцов (m<=n), cost[i][j] - стоимость назначения j на должность i
	//ИСХОДНЫЕ ДАННЫЕ////////////////////////////////////////////////
	vector<vector<int>> c(n, vector<int>(n));//стоимость назначения
	vector<int> result(m);
	
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < n; ++j)
			if (i < m)
				c[i][j] = cost[i][j];
			else
				c[i][j] = 0;

	// interMatrix(n,n,c);
	//ПЕРЕМЕННЫЕ ДЛЯ АЛГОРИТМА
	vector<int> zero(n);//для подсчета числа нулей в строках
	vector<int> column(n);//для пометок строк и столбцов
	vector<int> row(n);
	bool* row_temp = new bool[n];
	bool* column_temp = new bool[n];
	bool to_go;
	bool to_go_temp;
	int c_min, c_max, i_min, j_zero;
	
	/////////////////////////////////////////////////////////////
	//АЛГОРИТМ////////////////////////////////////////////////////
	//шаг 1. вычитание минимальных элементов из строк и столбцов
	for (int i = 0; i < n; ++i)
	{
		c_min = c[i][0];
		for (int j = 1; j < n; ++j)
			if (c[i][j] < c_min)
				c_min = c[i][j];
		
		for (int j = 0; j < n; ++j)
			c[i][j] = -c_min + c[i][j];
	}

	for (int i = 0; i < n; ++i)
	{
		c_min = c[0][i];
		for (int j = 1; j < n; ++j)
			if (c[j][i] < c_min)
				c_min = c[j][i];
	
		
		for (int j = 0; j < n; ++j)
			c[j][i] = -c_min + c[j][i];
	}

	//////////////////////////////////////////////////////////////////////////
	//НАЧАЛО АЛГОРИТМА//
	while (true)
	{
		//////////////////////////////////////////////////////////////////////////     
		//шаг 2. преоразование решения
		//считаем число нулей в каждой строке и максимальный элемент матрицы
		c_max = 0;
		for (int i = 0; i < n; ++i)
		{
			zero[i] = 0;
			for (int j = 0; j < n; ++j)
			{
				if (c[i][j] == 0)
					zero[i]++;
				
				if (c[i][j] > c_max)
					c_max = c[i][j];
			}
		}

		///////////////////////////////////////////////////////////////////////// 
		//поиск строк с минимальным числом нулей
		while (true)
		{
			c_min = n + 1;
			i_min = -1;
			for (int i = 0; i < n; ++i)
				if (zero[i] && (zero[i] < c_min) )
				{
					i_min = i;
					c_min = zero[i];
				}
	
			//нашли строку с нулями i_min (закрашиваем и вычеркиваем нули)
			if (c_min != n + 1)
			{
				/////////////////////////////
				zero[i_min] = n + 1;//помечаем строку как просмотренную
									//первый ноль j_min в строке закрашиваем - 0
				j_zero = -2;
				for (int j = 0; j < n; ++j)
				{
					if (c[i_min][j] == 0)
					{
						j_zero = j;
						break;
					}
				}
				//остальные нули в строке вычеркиваем - c_max+1
				for (int j = j_zero + 1; j < n; ++j)
					if (c[i_min][j] == 0)
						c[i_min][j] = c_max + 1;
				
				//остальные нули в столбце  j_zero  вычеркиваем - c_max+1
				for (int i = 0; i < n; ++i)
					if ( (c[i][j_zero] == 0) && (i != i_min) )
					{
						c[i][j_zero] = c_max + 1;
						zero[i]--;
					}
			
				////////////////////////////
			}//нашли строку с нулями i_min
			else {
				break;
			}
		}
		//все строки с меткой zero[i]=n+1 содержат закрашенные нули 
		///////////////////////////////////////////////////////////////////////// 

		//шаг 3. поиск минимального числа прямых, проходящих через все нули (зачеркнутые и закрашенные) 
		to_go = false;
		to_go_temp = false;
		for (int i = 0; i < n; ++i)
		{
			column[i] = -1;
			row[i] = -1;
			row_temp[i] = column_temp[i] = false;
		}

		for (int i = 0; i < n; ++i)
			// строка, где нет отмеченных элементов получает метку 0
			if (zero[i] != n + 1)
			{
				row[i] = 0;
				to_go = true;
				break;
			}


		if (!to_go)
		{
			//выписываем решение задачи
			//            System.out.println("Задача решена!"); 
			//cost_opt=0;//значение целевой функции
			for (int i = 0; i < m; ++i)
				for (int j = 0; j < n; ++j)
					if (c[i][j] == 0)
						result[i] = j;
			break;
		}
		///////////////////////////////////////////////////////////////////////// 
		while (to_go) // если to_go=false, то решениие найдено
		{
			to_go = false;
			//помечаем новые столбцы
			for (int i = 0; i < n; ++i)
				if ( (row[i] >= 0) && (!row_temp[i]) )
				{
					for (int j = 0; j < n; ++j)
						if ( (c[i][j] == c_max + 1) && (!column_temp[j]) )
						{
							column[j] = (i + 1);
							to_go = true;
						}
					row_temp[i] = true;//строка просмотрена
				}
			
			if (to_go) //если появились новые столбцы
			{
				to_go = false;
				//помечаем новые строки
				for (int j = 0; j < n; ++j)
				{
					if ( (column[j]>0) && (!column_temp[j]) )
					{
						i_min = 0;
						for (int i = 0; i < n; ++i)
							if ( (c[i][j] == 0) && (!row_temp[i]))
							{
								i_min++;//в столбце есть закрашенный ноль
								row[i] = (j + 1);
								to_go = true;
							}
						column_temp[j] = true;//столбец просмотрен
											 /////////////////////////////////////////////////////////////
											 //аугментальная цепь///////////////////////////////////////
											 /////////////////////////////////////////////////////////////

						if (i_min == 0) //в столбце j нет закрашенных нулей - найдена аугментальная цепь
						{				//заменяем  закрашенные нули зачеркнутыми и наоборот вдоль цепи
							i_min = j;
							while (i_min >= 0)
							{
								c[column[i_min] - 1][i_min] = 0;
								if (row[column[i_min] - 1])
								{
									c[column[i_min] - 1][row[column[i_min] - 1] - 1] = c_max + 1;
									i_min = row[column[i_min] - 1] - 1;
								}
								else {
									zero[column[i_min] - 1] = n + 1;
									i_min = -1;
								}
							}

							//стираем старые метки 
							/////////////////// ///////////////////////////////////////////////////////////
							to_go = false;
							for (int i = 0; i < n; ++i)
							{
								column[i] = -1;
								row[i] = -1;
								row_temp[i] = column_temp[i] = false;
							}

							for (int i = 0; i < n; ++i)
								// строка, где нет отмеченных элементов получает метку 0
								if (zero[i] != n + 1)
								{
									row[i] = 0;
									to_go = true;
									break;
								}

							if (!to_go) 
							{
								//выписываем решение задачи
								//                                System.out.println("Задача решена внутри!");
								//cost_opt = 0;//значение целевой функции
								for (int i = 0; i < m; ++i)
									for (int j1 = 0; j1 < n; ++j1)
										if (c[i][j1] == 0)
											result[i] = j1;
								to_go_temp = true;
							}
							////////////////////////////////////////////////////////////////////////
							break;//останавливаем цикл по столбцам
						}
						/////////////////////////////////////////////////////////////
						//аугментальная цепь///////////////////////////////////////
						/////////////////////////////////////////////////////////////
					}
				}
			}//if (to_go) {//если появились новые столбцы
		}// while (to_go) {
		 
		// для вычеркивания нужные строки row[i]=-1; нужные столбцы column[j]>0
		///////////////////////////////////////////////////////////////////////// 
		if (to_go_temp) //если задача решена при поиске аугментальной цепи
			break; 
		
		//поиск мин элемента из невычеркнутых
		c_min = c_max + 1;
		for (int j = 0; j < n; ++j)
		{
			for (int i = 0; i < n; ++i)
			{
				if (c[i][j] == c_max + 1)
					c[i][j] = 0;
				
				if ( (row[i] >= 0) && (column[j] == -1) )
					if (c[i][j] < c_min)
						c_min = c[i][j];
			}
		}
		//прибавляем мин элемент к вычеркнутым столбцам
		for (int j = 0; j < n; ++j)
			if (column[j] > 0)
				for (int i = 0; i < n; ++i)
					c[i][j] = c[i][j] + c_min;
		
		//вычитаем мин элемент из невычеркнутых строк
		for (int i = 0; i < n; ++i)
			if (row[i] >= 0)
				for (int j = 0; j < n; ++j)
					c[i][j] = c[i][j] - c_min;
		
		////////////////////////////////////////////  
		//////////////////////////////////////////////////////////////////////////
		//КОНЕЦ АЛГОРИТМА//
	}
	return result;
	//////////////////////////////////////////////////////////////////////////   
}


////////////////////////////////////////////////////////////////////////////////
//метод локального поиска - не нужен
////////////////////////////////////////////////////////////////////////////////
/*
vector<int> GA_path::local_search_fast(vector<int> assignment, vector<vector<int>> c,
								vector<vector<int>> vertex, vector<int> vertex_initial, int alpha)
{
	//alpha число просматриваемых кондидатов для i_next
	//         System.out.println(Nchange);
	int G = 0, F = 0, F_max = -1;
	vector<int> result(n);
	vector<vector<int>> path(n);
	vector<bool> tabu(n);//метки для запретных вершин 

									//            Random rnd = new Random();//датчик случайных чисел
	int i_head = -1, j_head = -1, i_next = -1, i_temp = -1, i_tail = -1, j_tail = -1, count = 0;
	bool isImprove = true;
	//создаем очередь вершин
	queue<int> queue_vertex;
	for (int i = 0; i < n - 1; ++i) {
		QueueVertex.offer(vertexInitial[i]);
		tabu[i] = false;
		path[1][assignment[i] - 1] = assignment[i + 1] - 1;
		path[0][assignment[i + 1] - 1] = assignment[i] - 1;
	}
	QueueVertex.offer(vertexInitial[n - 1]);
	tabu[n - 1] = false;
	path[1][assignment[n - 1] - 1] = assignment[0] - 1;
	path[0][assignment[0] - 1] = assignment[n - 1] - 1;


	//        while (isImprove) {//пока есть улучшения - движемся вперед!
	//          i_head=rnd.nextInt(n);//случайным образом выбирается начальная вершина (i_head - это конец пути)
	isImproveLS = false;//для проверки есть ли улучшение в локальном поиске
	while (QueueVertex.peek() != null) {//возвращает без удаления элемент из начала очереди
		i_head = QueueVertex.poll();//возвращает с удалением элемент из начала очереди
									//            for (int i_head = 0; i_head < n; i_head++) {
		isImprove = false;
		j_head = path[1][i_head]; //(j_head - это начало пути) 
		path[1][i_head] = -1;//удаляем дугу
		path[0][j_head] = -1;
		//добавляем дугу для образования цикла 
		for (int i_t = 0; i_t < alpha; ++i_t) {
			i_next = vertex[i_head][i_t];
			if (i_head != i_next && j_head != i_next) {
				if (c[i_head][j_head]>c[i_head][i_next]) {
					G = 0;//начальный выигрыш
						  //добавляем дугу и тем самым создаем цикл
						  //добавляем (i_head, i_next)
					path[1][i_head] = i_next;
					i_temp = path[0][i_next];//дуга для удаления формируется однозначно
					path[0][i_next] = i_head;
					G = G + c[i_head][j_head] - c[i_head][i_next];//обновляем выигрыш
																  //удаляем (i_temp, i_next)
					path[1][i_temp] = -1;
					//разрываем цикл
					//выбираем вершину из цикла j_tail
					count = 0;
					F_max = -1;
					j_tail = -1;
					i_tail = path[1][i_next];//просматриваем вершины цикла, i_tail - кандидат
					while (count < n) {
						if (i_next != i_tail && c[i_head][j_head] + c[i_temp][i_next]>c[i_head][i_next] + c[i_temp][i_tail]) { // 
							F = G + c[i_temp][i_next] - c[i_temp][i_tail] + c[path[0][i_tail]][i_tail] - c[path[0][i_tail]][j_head];
							if (F > 0) {
								count++;
								if (F > F_max) {
									j_tail = i_tail;
									F_max = F;
								}
							}
						}
						i_tail = path[1][i_tail];
						if (i_tail == i_next) {
							break;
						}
					}
					if (count == 0) {//в данном направлении нет улучшения (возвращаем метки на место)
						path[1][i_head] = -1;
						path[0][j_head] = -1;
						path[0][i_next] = i_temp;
						path[1][i_temp] = i_next;
					}
					else {//нашли улучшающее решение
						  //добавляем (i_temp, j_tail)
						path[1][i_temp] = j_tail;
						i_tail = path[0][j_tail];
						path[0][j_tail] = i_temp;
						//удаляем (i_tail, j_tail)
						path[1][i_tail] = -1;
						//добавляем (i_tail, j_head)
						path[1][i_tail] = j_head;
						path[0][j_head] = i_tail;
						isImprove = true;
						isImproveLS = true;//решение локально улучшено
						break;
					}
				}
				else { break; }//дальше не рассматриваем варианты
			}

		}
		if (isImprove == true) {
			//если успех, то возвращаем i_head в конец очереди
			QueueVertex.offer(i_head);
			//если  вершина какого-либо удаленного ребера в черном списке, то возвращаем ее в очередь
			if (tabu[j_head] == true) { tabu[j_head] = false; QueueVertex.offer(j_head); }
			if (tabu[i_next] == true) { tabu[i_next] = false; QueueVertex.offer(i_next); }
			if (tabu[i_temp] == true) { tabu[i_temp] = false; QueueVertex.offer(i_temp); }
			if (tabu[j_tail] == true) { tabu[j_tail] = false; QueueVertex.offer(j_tail); }
			if (tabu[i_tail] == true) { tabu[i_tail] = false; QueueVertex.offer(i_tail); }
			//                    for (int i = 0; i < n; ++i) {
			//                    result[i] = path[1][i];
			//                    System.out.print(result[i]+" ");
			//                    }
			//                    System.out.println();
			//                     System.out.println("Улучшение: " +costAssignment(c, result));
			//                    break;
		}
		else {
			//если неудача, то помещаем i_head в черный список
			tabu[i_head] = true;
			path[1][i_head] = j_head;//возвращаем дугу
			path[0][j_head] = i_head;
		}
	}
	//        }
	if (isImproveLS == true) {
		//формируем перестановку (циклическая)
		result[0] = 1; i_temp = 0;
		for (int i = 1; i<n; ++i) {
			result[i] = path[1][i_temp] + 1;
			i_temp = path[1][i_temp];
		}
		//                   for (int i = 0; i < n; ++i) {
		//                    result[i] = path[1][i];
		////                    System.out.print(result[i]+" ");
		//                    }
		//                    System.out.println();
		//                     System.out.println("Итог: " +costAssignment(c, result));     
		return result;
	}
	else {//решение является локальным оптимумом
		return assignment;
	}
}
*/

//эксперимент по сужения (старая версия)
/*
//параметры "кванта информации" и шаг эксперимента
int w_1, w_2, h;
//коэффициент относительной важности
double teta;
vector<vector<int>> new_phi_P_approx;
vector<vector<int>> phi_P_approx_reduced;

//поиск min и max значений по каждому критерию в аппроксимации мн-ва Парето
vector<int> max_phi_P_approx(ga.get_m());
vector<int> min_phi_P_approx(ga.get_m());

for (int i = 0; i < ga.get_m(); i++)
{
	max_phi_P_approx[i] = min_phi_P_approx[i] = ga.phi_P_approx[0][i];

	for (int j = 0; j < ga.phi_P_approx.size(); j++)
	{
		if (max_phi_P_approx[i] < ga.phi_P_approx[j][i])
			max_phi_P_approx[i] = ga.phi_P_approx[j][i];

		if (min_phi_P_approx[i] > ga.phi_P_approx[j][i])
			min_phi_P_approx[i] = ga.phi_P_approx[j][i];
	}
}

//грань конуса увеличиваем от 90+h градусов до диагонали (max_1-min_1, max_2-min_2)
w_1 = max_phi_P_approx[0] - min_phi_P_approx[0];
int h_temp = (max_phi_P_approx[1] - min_phi_P_approx[1]) / 10;
h = (h_temp > 1) ? h_temp : 1;

for (int i = h; i <= max_phi_P_approx[1] - min_phi_P_approx[1]; i = i + h)
{
	w_2 = i;
	new_phi_P_approx = ga.build_new_phi_P_approx(ga.phi_P_approx, w_1, w_2);
	phi_P_approx_reduced = ga.build_phi_P_enum(new_phi_P_approx);

	teta = (double)w_2 / (w_1 + w_2);
	sw->WriteLine("w_1; {0}", w_1);
	sw->WriteLine("w_2; {0}", w_2);
	sw->WriteLine("teta; {0:F3}", teta);

	sw_1->WriteLine("w_1; {0}", w_1);
	sw_1->WriteLine("w_2; {0}", w_2);
	sw_1->WriteLine("teta; {0:F3}", teta);

	//сужение множества Парето (векторный критерий)
	sw->WriteLine("Number of vectors in the Pareto set");
	sw->WriteLine("{0}", ga.phi_P_approx.size());

	sw->WriteLine("Number of vectors in the reduced Pareto set");
	sw->WriteLine("{0}", phi_P_approx_reduced.size());

	sw_1->WriteLine("Number of vectors in the Pareto set");
	sw_1->WriteLine("{0}", ga.phi_P_approx.size());

	sw_1->WriteLine("Number of vectors in the reduced Pareto set");
	sw_1->WriteLine("{0}", phi_P_approx_reduced.size());

	for (int i = 0; i < ga.get_m(); i++)
	{
		for (int j = 0; j < phi_P_approx_reduced.size(); j++)
		{
			sw->Write("{0};", phi_P_approx_reduced[j][i]);
			sw_1->Write("{0};", phi_P_approx_reduced[j][i]);
		}
		sw->WriteLine();
		sw_1->WriteLine();
	}
	sw->WriteLine();
	sw_1->WriteLine();
}


//грань конуса увеличиваем от диагонали (max_1-min_1, max_2-min_2) до 180-h градусов
w_2 = max_phi_P_approx[1] - min_phi_P_approx[1];
h_temp = (max_phi_P_approx[0] - min_phi_P_approx[0]) / 10;
h = (h_temp > 1) ? h_temp : 1;

for (int j = max_phi_P_approx[0] - min_phi_P_approx[0]; j > 0; j = j - h)
{
	if ((((max_phi_P_approx[1] - min_phi_P_approx[1]) % 10) == 0) &&
		(j == max_phi_P_approx[0] - min_phi_P_approx[0]))
		j = j - h;
	w_1 = j;
	new_phi_P_approx = ga.build_new_phi_P_approx(ga.phi_P_approx, w_1, w_2);
	phi_P_approx_reduced = ga.build_phi_P_enum(new_phi_P_approx);

	teta = (double)w_2 / (w_1 + w_2);
	sw->WriteLine("w_1; {0}", w_1);
	sw->WriteLine("w_2; {0}", w_2);
	sw->WriteLine("teta; {0:F3}", teta);

	sw_1->WriteLine("w_1; {0}", w_1);
	sw_1->WriteLine("w_2; {0}", w_2);
	sw_1->WriteLine("teta; {0:F3}", teta);

	//сужение множества Парето (векторный критерий)
	sw->WriteLine("Number of vectors in the Pareto set");
	sw->WriteLine("{0}", ga.phi_P_approx.size());

	sw->WriteLine("Number of vectors in the reduced Pareto set");
	sw->WriteLine("{0}", phi_P_approx_reduced.size());

	sw_1->WriteLine("Number of vectors in the Pareto set");
	sw_1->WriteLine("{0}", ga.phi_P_approx.size());

	sw_1->WriteLine("Number of vectors in the reduced Pareto set");
	sw_1->WriteLine("{0}", phi_P_approx_reduced.size());

	for (int i = 0; i < ga.get_m(); i++)
	{
		for (int j = 0; j < phi_P_approx_reduced.size(); j++)
		{
			sw->Write("{0};", phi_P_approx_reduced[j][i]);
			sw_1->Write("{0};", phi_P_approx_reduced[j][i]);
		}
		sw->WriteLine();
		sw_1->WriteLine();
	}
	sw->WriteLine();
	sw_1->WriteLine();
}
*/
