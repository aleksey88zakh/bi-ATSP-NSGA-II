#pragma once

#include <vector>
#include <windows.h>
#include <queue>
#include <set>
#include <map>
#include <string>
#include <direct.h>

#define ITER_MAX 200
#define MODIF true //��������!!!

//����������� �� �������
//����� ����� ��� teta
#define NUM_STEPS_TETA 9
//������ ���������� ��� �������� �������� ������� (��� "������")
#define SIZE_EXCL_TETA (NUM_STEPS_TETA-1)*NUM_STEPS_TETA/2
//"������ ����������"
#define _1ST_2ND_ 1
#define _2ND_1ST_ 2

//������� ������ ��� ������ ������������� (����� ��������)
#define FREQ_SHOW_COMP 20
//������� ������ ��� ������� (����� ��������)
#define FREQ_SHOW_RED 100


using namespace std;
using namespace System;
using namespace System::IO;

//��������� �������������
public enum class recomb_oper {DEC_new, DPX};


class GA_path
{
public:
	GA_path();
	GA_path(int n, int N, int m, int s_max_N);
	~GA_path();
	vector<vector<int>> pop;//���������
	vector<vector<vector<int>>> s_m;//������� �������� �� ������� ��������
	vector<vector<int>> phi;//����������� ������ (��������� ��������)
	vector<int> i_rank;//���� ������ �� ������
	vector<double> i_dist;//���������� (��� ������������ ������ ������)
	vector<int> s_aver;//������� �������� �������� ������� (�� ������� ��������)

	//�������� + ������� (R_t = P_t + Q_t)
	vector<vector<int>> pop_R_t;
	//������ ������ ������� ��� R_t
	vector<int> i_rank_R_t;
	//������ (��� ������������ ������ ������) ��� R_t
	vector<double> i_dist_R_t;

	///!!! �������� ��� ������� start_time
	void init_pop(vector<vector<vector<int>>> s, int S_max, unsigned long long start_time,
		vector<int> p1, vector<int> p2, vector<int> p3, vector<int> p4);
		//vector<vector<int>> vertex, vector<int> vertex_initial); - ���� �� ����������
	//���������� ����� � ������� �������������
	vector<int> random_individual();
	//���������� ������ ����������
	void set_matrix_criteria(vector<vector<int>> s);
	//�������� ���������� �������� ��� �����
	vector<int> multi_phitness(vector<int> p);
	//��������� ������
	template <typename T>
	bool Pareto_pref(vector<T> a, vector<T> b);
	//������� ������ ������������ ������ ��������� pop_cur ������-����������� ������� (��� crowding distance)
	vector<int> range_front(vector<vector<int>>& pop, bool flag_sort_pop);
	//��������� ���������� ��� ������ �� ��������� pop_cur � ������ rank (���� �� 1 �� ���� ������� i_rank)
	void crowd_dist(vector<double>& i_dist_cur, int rank, vector<int> i_rank_cur, vector<vector<int>> pop_cur, bool flag_sort);
	//����� ��������� ���������� ��� ������ �� ��������� pop_cur � ������ rank (���� �� 1 �� ���� ������� i_rank)
	void crowd_dist_new(vector<vector<int>> pop_cur, bool flag_sort);
	//crowded-comparison operator
	bool crowd_comp_oper(int i_p1, int i_p2);
	//���������� ����� ���������� ������� + ������� ������
	vector<int> patching_algorithm(vector<vector<int>> s, int c_max, boolean orig_or_mod);
	//������� ��� �������� ���������������� ��������� (� �� �������, � ������ ������)
	//? ��������� ���������� � multimap
	//multimap<int, multimap<float, vector<int> > > a

	//�������� ������� ����������
	void GA_path::quick_sort(vector<int>& arr, vector<int>& arr_index, int left, int right);
	//void GA_path::heap_sort_new(vector<vector<int>>& pop, int i_start, int i_stop);
	//������������� ���������
	//flag_phi_sort = true - ���������� �� ���������� ��������
	//flag_phi_sort = false - ���������� �� i_dist
	void GA_path::heap_sort(vector<vector<int>> pop_cur, vector<int>& numbers_index, int num_citeria, int index_begin, int index_end);
	//
	void GA_path::heap_sort(vector<int>& numbers_index, int index_begin, int index_end);
	

	//���������
	//��������� ��������
	int tourn_selection();
	//�������
	void mutation(int i_p1, int i_p2, vector<int>& p1_temp, vector<int>& p2_temp);
	//������� �����
	vector<int> exchange_mutation(vector<int> p);
	//����������������� ������������, ���������� �� ������������ ���
	vector<int> DEC_old(vector<vector<int>> s, vector<int> p1, vector<int> p2);
	// << julia:  ����� ������� �������������
	vector<int> DEC_new(vector< vector <vector<int> > > s, vector<int> p1, vector<int> p2);
	// << julia
	vector<int> DPX(vector< vector <vector<int> > > s, vector<int> p1, vector<int> p2);


	//�������
	//���������� ������������� ��-�� ������ (�������� ���������� ��������, ��� ��������)
	void build_phi_P_approx();
	//������������� ��-�� ������ (�������� ���������� ��������, ��� ��������)
	vector<vector<int>> phi_P_approx;
	//���������� ���������� �� ����� (�� �������������/��-�� ������) �� ���������� ������� � ��-�� ������/�������������
	double dist_p_to_nearest_set(vector<int> phi_p, vector<vector<int>> phi_set);
	//���������� ������� - ���������� ������� ��-�� �� ������� ��-��
	double dist_convergence(vector<vector<int>> first_set, vector<vector<int>> second_set, bool flag_count);
	//�������������������� ���������� ������� "���������� ������������� � ��-�� ������"
	double standard_devation_conver_approx_to_P_set(vector<vector<int>> P_set, double dist_conver);
	//����� �������
	void evaluate_metric_of_approx(StreamWriter^ sw, vector<vector<int>> P_set, bool flag_count);
	double dist_conver_approx_to_P_set_val;
	double dist_conver_P_set_to_approx_val;
	int count_P_eq_approx;


	//������� ��-�� ������
	//��� �����������
	vector<double> experiment_reduction(StreamWriter^ sw, String^ problem_name_str, 
		double h, int num_steps, double teta_start, unsigned quantum_inf);
	//������������ ��-�� ������ (��� ��������) �������������� "������" ��������
	vector<vector<double>> build_new_phi_P_approx(vector<vector<int>> init_phi_P_approx, double teta_1, double teta_2, unsigned quantum_inf);
	//���������� ��-�� ������ ���������
	vector<vector<double>> build_phi_P_enum(vector<vector<double>> init_set);
	//����� ����������� ������������
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

	//������ ��������� ������ �� �����
	vector<vector<int>> read_Pareto_set_from_file(String^ file_name_source_str, String^ problem_name_str);

	friend void time_format(unsigned long long result_time, String^ title, StreamWriter^ sw);

private:
	int n;//����� ������� � ������������
	int m;//����� ���������
	int N;//����������� ���������
	//vector<vector<int>> pop;//���������
	//vector<vector<int>> phi;//����������� ������ (��������� ��������)
	//vector<vector<vector<int>>> s_m;//������� �������� �� ������� ��������

	int tourn_size = 10;//������ �������
	double p_mut = 0.1;//����������� �������

	int count_best_child;//����� �������� ��� ������� ����� ���������
	int iter_best_child;//��������� �������� ��� ������� ����� ���������
	int* rank;//����� ������
	double avg_phi;//������� ����������� ����� � ���������
	int phi_best;// ����������� ������ �����
	int ord_best_ind;//������ �����
	int iter_best;//��������, �� ������� �������� ������ �����
	long time_best;//����� ��������� ������ �����
				   //��� ������������
	int number_solution_in_rec;//����� �������� � ������������
	int phi_best_in_rec;// ����������� ������ ����� � ������������
	bool is_AP_solution_opt;
	bool is_improve_LS;//������� � ��������� ������ �������� ��������
	int q_current;//�������� q ��� ������� ������ �� (�� ��������� ���������)

	//������������
	// << julia: ������������ � �������� DEC_new � DCX
	vector<bool> flag_Pareto_sol(int k, vector<vector<int>> s);
	// << julia: ������������ � �������� DEC_new � DCX
	//           ����� ���������� ��������� ��������
	int next(int j, int k, vector<bool> flag_S);


	//��������� �����
	vector<int> local_search(vector<int> assignment, vector<vector<int>> s_m_crit_index);

	//���������� ����������� �����
	int phitness(vector<vector<int>> s, vector<int> p);

	//���������� �����
	vector<int> Hungarian_method(int n, int m, vector<vector<int>> cost);
	
	// ������� "�����������" ����� ���� - ������������ ����
	//flag_phi_sort = true - ���������� �� ���������� ��������
	//flag_phi_sort = false - ���������� �� i_dist
	void sift_down(vector<vector<int>> pop_cur, vector<int>& numbers_index, int num_criteria, int root, int bottom, int delta);
	//
	void sift_down(vector<int>& numbers_index, int root, int bottom, int delta);
	
	//3-opt ������ � �������
	vector<int> random_change2(vector<int> assignment, int Nchange, vector<vector<int>> c,
		int c_aver, int alpha);
	
	//����� �� �����
	//vector<int> GA_path::local_search_fast(vector<int> assignment, vector<vector<int>> c,
	//	vector<vector<int>> vertex, vector<int> vertex_initial, int alpha);
};