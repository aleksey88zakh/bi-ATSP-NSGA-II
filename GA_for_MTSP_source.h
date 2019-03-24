#pragma once

#include <vector>
#include <windows.h>
#include <queue>
#include <set>
#include <map>
#include <list>
#include <string>
#include <array>
#include <direct.h>
#include "common.h"
//#include "archive.h"
//#include "common.cpp"

#define ITER_MAX 200
#define MODIF true
#define flag_VNSnew          0
#define flag_LS_VNS_from_GA  0
#define flag_VNS_multi_start 1

//ГА
//локальный поиск
#define LS_GA_BEGIN_ALPHA   0.5
#define LS_GA_BEGIN_BETA    0.5
#define LS_GA_END_ALPHA     0.5
#define LS_GA_END_BETA      0.5
//турнирная селекция
#define TOURN_SIZE 10
//мутация
#define P_MUTATION 0.1

//VNS
//локальный поиск
#define LS_VNS_BEGIN_ALPHA    1
#define LS_VNS_END_ALPHA      1
#define LS_VNS_BEGIN_BETA	  1
#define LS_VNS_END_BETA		  1
//shaking
#define MAX_NUM_K_OPT 11
//VNS_multi_start
#define MAX_ELITE_NOT_CHANGED 5

//эксперимент по сужению
//число шагов для teta
#define NUM_STEPS_TETA 9
//размер контейнера для хранения процента сужения (два "кванта")
#define SIZE_EXCL_TETA (NUM_STEPS_TETA-1)*NUM_STEPS_TETA/2
//"кванты информации"
#define _1ST_2ND_ 1
#define _2ND_1ST_ 2

//частота вывода при оценке аппроксимации (число итераций)
#define FREQ_SHOW_COMP 20
//частота вывода при сужении (число итераций)
#define FREQ_SHOW_RED 100


using namespace std;
using namespace System;
using namespace System::IO;

//struct Fronts
//{
//	vector<int> ranks;
//	vector<int> fronts_index;
//};

//операторы кроссинговера
public enum class recomb_oper {DEC_new, DPX, CH_MAX};

//extern template bool Pareto_pref(const vector<int> a, const vector<int> b);
//extern template bool Pareto_pref(const vector<double> a, const vector<double> b);



class GA_path
{
public:
	GA_path();
	GA_path(int n, int N, int m, int s_max_N);
	~GA_path();
	vector<vector<int>> pop;//популяция
	vector<vector<vector<int>>> s_m;//матрица значений по каждому критерию
	vector<vector<int>> phi;//пригодность особей (векторный критерий)
	vector<int> i_rank;//ранг фронта по Парето
	vector<double> i_dist;//расстояние (для ранжирования внутри форнта)
	vector<int> s_aver;//средняя величина элемента матрицы (по каждому критерию)
	vector<int> c_max_all; //макс длина дуги (по каждому критерию)
	vector<vector<int>> c_all; //массив всех дуг


	//родители + потомки (R_t = P_t + Q_t)
	vector<vector<int>> pop_R_t;
	//вектор рангов фронтов для R_t
	vector<int> i_rank_R_t;
	//вектор (для ранжирования внутри форнта) для R_t
	vector<double> i_dist_R_t;

	//НАЧАЛЬНАЯ ПОПУЛЯЦИЯ
	///!!! уточнить про порядок start_time
	void init_pop(vector<vector<vector<int>>> s, int S_max, unsigned long long start_time,
		vector<int> p1, vector<int> p2, vector<int> p3, vector<int> p4);
		//vector<vector<int>> vertex, vector<int> vertex_initial); - пока не используем
	//построение особи с помощью рандомизатора
	vector<int> random_individual();
	//функции по заполнению матриц расстояний
	void set_matrix_criteria(vector<vector<int>> s);
	void set_matrices(StreamReader^ sr);
	//общая функция
	void generate_init_pop_VNS(StreamWriter^ sw, StreamWriter^ sw_1, int iter_prbl, String^ file_name_source_Pareto_set_str,
		vector<vector<int>> phi_Pareto_set, long long* time_local_search_b, int index_run);


	//значение векторного критерия для особи
	vector<int> multi_phitness(vector<int> p);
	//вычисление пригодности особи
	int phitness(vector<vector<int>> s, vector<int> p);
	//функция строит ранжировнные фронты популяции pop_cur парето-оптимальных решений (без crowding distance)
	vector<int> range_front(vector<vector<int>>& pop, bool flag_sort_pop);
	//функция строит ранжировнные фронты популяции pop_cur парето-оптимальных решений (без crowding distance)
	//алгоритм Jensen (NlogN)
	vector<int> range_front_J(vector<vector<int>> phi_cur, vector<vector<int>>& pop_cur, vector<int>& indeces_fronts, unsigned int type);
	//вычисляет расстояния для особей из популяции pop_cur с рангом rank (ранг от 1 до макс элмента i_rank)
	void crowd_dist(vector<double>& i_dist_cur, int rank, vector<int> i_rank_cur, vector<vector<int>> pop_cur, bool flag_sort);
	//НОВАЯ вычисляет расстояния для особей из популяции pop_cur с рангом rank (ранг от 1 до макс элмента i_rank)
	void crowd_dist_new(vector<vector<int>> pop_cur, bool flag_sort);
	//crowded-comparison operator
	bool crowd_comp_oper(int i_p1, int i_p2);
	//построение особи Венгерским методом + склейка циклов
	vector<int> patching_algorithm(vector<vector<int>> s, int c_max, boolean orig_or_mod);
	//вариант для хранения проранжированной популяции (и по фронтам, и внутри фронта)
	//? сложность сортировки в multimap
	//multimap<int, multimap<float, vector<int> > > a


	//СОРТИРОВКА
	//алгоритм быстрой сортировки
	void quick_sort(vector<int>& arr, vector<int>& arr_index, int left, int right);
	//void GA_path::heap_sort_new(vector<vector<int>>& pop, int i_start, int i_stop);
	//пирамидальная сортровка
	//flag_phi_sort = true - сортировка по компоненте критерия
	//flag_phi_sort = false - сортировка по i_dist
	void heap_sort(vector<vector<int>> pop_cur, vector<int>& numbers_index, int num_citeria, int index_begin, int index_end);
	//void heap_sort_ls(vector<vector<int>> pop_cur, vector<int>& numbers_index, int num_citeria, int index_begin, int index_end);
	//сортировка по i_dist
	void heap_sort(vector<int>& numbers_index, int index_begin, int index_end);
	// функция "просеивания" через кучу - формирование кучи
	//flag_phi_sort = true - сортировка по компоненте критерия
	//flag_phi_sort = false - сортировка по i_dist
	void sift_down(vector<vector<int>> pop_cur, vector<int>& numbers_index, int num_criteria, int root, int bottom, int delta);
	//
	void sift_down(vector<vector<int>> pop_cur, vector<int>& numbers_index, int root, int bottom, int delta);
	//void sift_down_ls(vector<vector<int>> pop_cur, vector<int>& numbers_index, int root, int bottom, int delta);
	//сортировка по i_dist
	void sift_down(vector<int>& numbers_index, int root, int bottom, int delta);


	//ЛОКАЛЬНЫЙ ПОИСК
	
	//препроцессинг
	//массив максимальных дуг по каждому критерию
	vector<vector<int>> c_max;
	//функция строит ранжировнные фронты популяции pop_cur парето-оптимальных решений (без crowding distance)
	//алгоритм Jensen (NlogN)
	//Fronts range_front_J_ls(vector<vector<int>>& pop, bool flag_sort_pop);
	//бинарный поиск
	//для поиска фронта F_b при вычислении рангов
	//int binary_search_Fb_ls(vector<vector<int>> pop_cur, int cur_index, vector<vector<int>> F_j);
	vector<int> index_pi;
	vector<int> i_rank_pi;
	vector<vector<int>> index_p;

	//функции ЛП
	vector<int> local_search(vector<int> p, float alpha, float beta, void* p_arch = NULL);
	//VNS
	bool local_search_VNS_new(vector<int> p, float alpha, float beta, void* p_arch);


	//ОПЕРАТОРЫ
	//турнирная селекция
	int tourn_selection();
	//мутация
	void mutation(int i_p1, int i_p2, vector<int>& p1_temp, vector<int>& p2_temp);
	//мутация обена
	vector<int> exchange_mutation(vector<int> p);
	//рандомизированный кроссинговер, основанный на наследовании дуг
	vector<int> DEC_old(vector<vector<int>> s, vector<int> p1, vector<int> p2);
	// << julia:  новый опретор кроссинговера
	vector<int> DEC_new(vector< vector <vector<int> > > s, vector<int> p1, vector<int> p2);
	// << julia
	vector<int> DPX(vector< vector <vector<int> > > s, vector<int> p1, vector<int> p2);
	// обучение с подкрепелением в операторах ГА
	int GA_path::update_rate_crossover(vector<int> p1, vector<int> p2, vector<int> ch);

	//МЕТРИКА
	//построение аппроксимации мн-ва Парето (значения векторного критерия, без повторов)
	void build_phi_P_approx();
	//аппроксимация мн-ва Парето (значения векторного критерия, без повторов)
	vector<vector<int>> phi_P_approx;
	//вычисление расстояния от особи (из аппроксимации/мн-ва Парето) до ближайшего вектора в мн-ве Парето/аппроксимации
	double dist_p_to_nearest_set(vector<int> phi_p, vector<vector<int>> phi_set);
	//вычисление метрики - сходимость первого мн-ва ко второму мн-ву
	double dist_convergence(vector<vector<int>> first_set, vector<vector<int>> second_set, bool flag_count);
	//среднеквадратическое отклонение метрики "сходимость аппроксимации к мн-ву Парето"
	double standard_devation_conver_approx_to_P_set(vector<vector<int>> P_set, double dist_conver);
	//вывод метрики
	void evaluate_metric_of_approx(StreamWriter^ sw, vector<vector<int>> P_set, bool flag_count);
	double dist_conver_approx_to_P_set_val;
	double dist_conver_P_set_to_approx_val;
	int count_P_eq_approx;
	//гиперобъем (сравнение алгоритмов)
	unsigned hyper_volume(vector<int> r, vector<vector<int>> f);

	//СУЖЕНИЕ МН-ВА ПАРЕТО
	//сам эксперимент
	vector<double> experiment_reduction(StreamWriter^ sw, String^ problem_name_str, 
		double h, int num_steps, double teta_start, unsigned quantum_inf);
	//аппрокимации мн-ва Парето (без повторов) относительного "нового" критерия
	vector<vector<double>> build_new_phi_P_approx(vector<vector<int>> init_phi_P_approx, double teta_1, double teta_2, unsigned quantum_inf);
	//построение мн-ва Парето перебором
	vector<vector<int>> build_phi_P_enum(vector<vector<double>> init_set,  vector<vector<int>> phi_P_approx);
	//вывод результатов эксперимента
	friend void print_exp_red(StreamWriter^ sw_4, String^ quantum_inf_name_str, vector<vector<double>> index_reduced, vector<int> total_num_P_approx);
	friend void print_exp_red_two(StreamWriter^ sw_4, String^ problem_name_str, vector<double> index_reduced, int num_steps, double total_num_P_approx);

	

	int get_n() { return n; };
	int get_N() { return N; };
	int get_ext_N() { return extended_N; };
	void set_ext_N(int ext_N) { extended_N = ext_N; }
	int get_m() { return m; };
	int get_tourn_size() { return tourn_size; };
	double get_p_mut() { return p_mut; };
	vector<vector<vector<int>>> get_s_m() { return s_m; };

	void set_tourn_size(int s) { tourn_size = s; };
	void set_p_mut(double p) { p_mut = p; };


private:
	int n;//число позиций в перестановке
	int m;//число критериев
	int N;//численность популяции
	int extended_N; //размер расширенной популяции
	//vector<vector<int>> pop;//популяция
	//vector<vector<int>> phi;//пригодность особей (векторный критерий)
	//vector<vector<vector<int>>> s_m;//матрица значений по каждому критерию

	int tourn_size;//размер турнира
	double p_mut;//вероятность мутации

	int count_best_child;//число итераций где потомок лучше родителей
	int iter_best_child;//последняя итерация где потомок лучше родителей
	int* rank;//ранги особей
	double avg_phi;//средняя пригодность особи в популяции
	int phi_best;// пригодность лучшей особи
	int ord_best_ind;//лучшая особь
	int iter_best;//итерация, на которой получена лучшая особь
	long time_best;//время получения лучшей особи
				   //для рекомбинации
	int number_solution_in_rec;//число потомков в рекомбинации
	int phi_best_in_rec;// пригодность лучшей особи в рекомбинации
	bool is_AP_solution_opt;
	bool is_improve_LS;//решение в локальном поиске локально улучшено
	int q_current;//значение q для текущей задачи ОР (по алгоритму Сердюкова)

	//РЕКОМБИНАЦИЯ
	// << Yulechka: Используется в функциях DEC_new и DCX
	vector<bool> flag_Pareto_sol(int k, vector<vector<int>> s);
	// << Yulechka: Используется в функциях DEC_new и DCX
	//           поиск следующего активного элемента
	int next(int j, int k, vector<bool> flag_S);


	//Венгерский метод
	vector<int> Hungarian_method(int n, int m, vector<vector<int>> cost);
	

	//бинарный поиск
	int binary_search_Fb(vector<vector<int>> pop_cur, int cur_index, vector<vector<int>> F_j);

	
	//3-opt замена в мутации
	vector<int> random_change2(vector<int> assignment, int Nchange, vector<vector<int>> c,
		int c_aver, int alpha);
	
	//метод не нужен
	//vector<int> GA_path::local_search_fast(vector<int> assignment, vector<vector<int>> c,
	//	vector<vector<int>> vertex, vector<int> vertex_initial, int alpha);
};

class Archive
{
public:
	Archive();
	Archive(GA_path* ga); // на основе данных генетического алгоритма
	~Archive();

	vector<vector<int>> archive;
	vector<vector<int>> val_crit_archive;
	list<unsigned> ar_index_cons_lst;
	list<unsigned> ar_index_not_cons_lst;
	list<unsigned> ar_index_no_lst;

	int check_new(vector<int> val_crit_new);
	void arch_modify(vector<int> pop_new, vector<int> val_crit_new);
	//обновление архива на основе элементов другого архива (объединение архивов)
	// this = (this U new_arch)
	// !!! справедлива, если текущий архив является элитой (просмотр особи = пусто, непросмотр особи = живые особи)
	bool elite_modify(Archive new_arch);
};

//template <typename T>
//class Statistics
//{
//public:
//	Statistics();
//	Statistics(T cur_val);
//
//	T total_val;
//	T max_val;
//	T min_val;
//	int cnt_val;
//	
//	void set_to_zero();
//	void refresh(T new_val);
//	void init_max_min(T val);
//	float get_aver();
//	T get_min();
//	T get_max();
//};

float С_metric(vector<vector<int>> P_set_1, vector<vector<int>> P_set_2);