#pragma once

#include <vector>
#include <windows.h>
#include <queue>
#include <set>
#include <map>
#include <string>
#include <direct.h>

#define ITER_MAX 200
#define MODIF true //ПОМЕНЯТЬ!!!

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

//операторы кроссинговера
public enum class recomb_oper {DEC_new, DPX};


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

	//родители + потомки (R_t = P_t + Q_t)
	vector<vector<int>> pop_R_t;
	//вектор рангов фронтов для R_t
	vector<int> i_rank_R_t;
	//вектор (для ранжирования внутри форнта) для R_t
	vector<double> i_dist_R_t;

	///!!! уточнить про порядок start_time
	void init_pop(vector<vector<vector<int>>> s, int S_max, unsigned long long start_time,
		vector<int> p1, vector<int> p2, vector<int> p3, vector<int> p4);
		//vector<vector<int>> vertex, vector<int> vertex_initial); - пока не используем
	//построение особи с помощью рандомизатора
	vector<int> random_individual();
	//заполнение матриц расстояний
	void set_matrix_criteria(vector<vector<int>> s);
	//значение векторного критерия для особи
	vector<int> multi_phitness(vector<int> p);
	//отношение Парето
	template <typename T>
	bool Pareto_pref(vector<T> a, vector<T> b);
	//функция строит ранжировнные фронты популяции pop_cur парето-оптимальных решений (без crowding distance)
	vector<int> range_front(vector<vector<int>>& pop, bool flag_sort_pop);
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

	//алгоритм быстрой сортировки
	void GA_path::quick_sort(vector<int>& arr, vector<int>& arr_index, int left, int right);
	//void GA_path::heap_sort_new(vector<vector<int>>& pop, int i_start, int i_stop);
	//пирамидальная сортровка
	//flag_phi_sort = true - сортировка по компоненте критерия
	//flag_phi_sort = false - сортировка по i_dist
	void GA_path::heap_sort(vector<vector<int>> pop_cur, vector<int>& numbers_index, int num_citeria, int index_begin, int index_end);
	//
	void GA_path::heap_sort(vector<int>& numbers_index, int index_begin, int index_end);
	

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


	//СУЖЕНИЕ МН-ВА ПАРЕТО
	//сам эксперимент
	vector<double> experiment_reduction(StreamWriter^ sw, String^ problem_name_str, 
		double h, int num_steps, double teta_start, unsigned quantum_inf);
	//аппрокимации мн-ва Парето (без повторов) относительного "нового" критерия
	vector<vector<double>> build_new_phi_P_approx(vector<vector<int>> init_phi_P_approx, double teta_1, double teta_2, unsigned quantum_inf);
	//построение мн-ва Парето перебором
	vector<vector<double>> build_phi_P_enum(vector<vector<double>> init_set);
	//вывод результатов эксперимента
	friend void print_exp_red(StreamWriter^ sw_4, String^ quantum_inf_name_str, vector<vector<double>> index_reduced, vector<int> total_num_P_approx);
	friend void print_exp_red_two(StreamWriter^ sw_4, String^ problem_name_str, vector<double> index_reduced, int num_steps, double total_num_P_approx);

	

	int get_n() { return n; };
	int get_N() { return N; };
	int get_m() { return m; };
	int get_tourn_size() { return tourn_size; };
	double get_p_mut() { return p_mut; };
	vector<vector<vector<int>>> get_s_m() { return s_m; };

	void set_tourn_size(int s) { tourn_size = s; };
	void set_p_mut(double p) { p_mut = p; };

	//чтение множества Парето из файла
	vector<vector<int>> read_Pareto_set_from_file(String^ file_name_source_str, String^ problem_name_str);

	friend void time_format(unsigned long long result_time, String^ title, StreamWriter^ sw);

private:
	int n;//число позиций в перестановке
	int m;//число критериев
	int N;//численность популяции
	//vector<vector<int>> pop;//популяция
	//vector<vector<int>> phi;//пригодность особей (векторный критерий)
	//vector<vector<vector<int>>> s_m;//матрица значений по каждому критерию

	int tourn_size = 10;//размер турнира
	double p_mut = 0.1;//вероятность мутации

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
	// << julia: Используется в функциях DEC_new и DCX
	vector<bool> flag_Pareto_sol(int k, vector<vector<int>> s);
	// << julia: Используется в функциях DEC_new и DCX
	//           поиск следующего активного элемента
	int next(int j, int k, vector<bool> flag_S);


	//ЛОКАЛЬНЫЙ ПОИСК
	vector<int> local_search(vector<int> assignment, vector<vector<int>> s_m_crit_index);

	//вычисление пригодности особи
	int phitness(vector<vector<int>> s, vector<int> p);

	//Венгерский метод
	vector<int> Hungarian_method(int n, int m, vector<vector<int>> cost);
	
	// функция "просеивания" через кучу - формирование кучи
	//flag_phi_sort = true - сортировка по компоненте критерия
	//flag_phi_sort = false - сортировка по i_dist
	void sift_down(vector<vector<int>> pop_cur, vector<int>& numbers_index, int num_criteria, int root, int bottom, int delta);
	//
	void sift_down(vector<int>& numbers_index, int root, int bottom, int delta);
	
	//3-opt замена в мутации
	vector<int> random_change2(vector<int> assignment, int Nchange, vector<vector<int>> c,
		int c_aver, int alpha);
	
	//метод не нужен
	//vector<int> GA_path::local_search_fast(vector<int> assignment, vector<vector<int>> c,
	//	vector<vector<int>> vertex, vector<int> vertex_initial, int alpha);
};