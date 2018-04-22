#include "stdafx.h"
#include "GA_for_MTSP_source.h"


//��������, ��� ������, ��� ��� ������� ��-�������
const int INF = 1000;

GA_path::GA_path()
{
}

GA_path::GA_path(int n, int N, int m, int s_max_N)
{
	this->n = n;//����� �������
	this->N = N;//������ ���������
	this->m = m;//����� ���������

	phi.resize(m);
	for (int i = 0; i < m; i++)
		phi[i].resize(N);

	i_rank.resize(N);
	i_dist.resize(N);

	i_rank_R_t.resize(2*N);
	i_dist_R_t.resize(2*N);

	this->tourn_size = 10;//������ �������

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
//��������� ��������� ���������
///////////////////////////////////////////////////////////////////////////////
void GA_path::init_pop(vector<vector<vector<int>>> s, int S_max, unsigned long long temp_time,
	vector<int> p1, vector<int> p2, vector<int> p3, vector<int> p4)
	//vector<vector<int>> vertex, vector<int> vertex_initial) - ���� �� ����������
{
	this->phi_best = S_max*this->n;
	this->phi_best_in_rec = S_max*this->n;
	this->number_solution_in_rec = 0;
	this->count_best_child = 0;
	this->iter_best_child = 0;
	this->avg_phi = 0;
	//!!! pop.reserve(n); ������-�� �� ��������
	////////////////////////////////////////////////////////////////////////////////
	//        ������ ����� �������� ����������� ������� patching algorithm (���������� �����+������� ������) �����������
	pop.push_back(p1);//���������������� �����
	//phi[0][0] = this->phitness(s1, pop[0]);
	//phi[0][1] = this->phitness(s2, pop[0]);
	//phi_best = phi[0];
	//ord_best_ind = 1;
	//������ ����� �������� ����������� ������� patching algorithm (���������� �����+������� ������) original
	pop.push_back(p2);//������������ �����
	//phi[1][0] = this->phitness(s1, pop[1]);
	//phi[1][1] = this->phitness(s2, pop[1]);
	//if (phi_best>phi[1]) {
	//	phi_best = phi[1];
	//	ord_best_ind = 2;
	//}
	//avg_phi = (phi[0] + phi[1]) / 2;//������� �����������
	//time_best = GetTickCount64() - start_time;

	pop.push_back(p3);
	pop.push_back(p4);

	//��������� ��������� �����
	//pop[0] = this->local_search(pop[0], this->s_m[0]); //��� ������ ������ ����������?
	//pop[1] = this->local_search(pop[1], this->s_m[0]); //��� ������ ������ ����������?
	//pop[2] = this->local_search(pop[2], this->s_m[0]); //��� ������ ������ ����������?
	//pop[3] = this->local_search(pop[3], this->s_m[0]); //��� ������ ������ ����������?

	////////////////////////////////////////////////////////////////////////////////
	srand(temp_time);
	for (int i = 4; i < N; ++i)
	{
		//������ ����� � ������� �������������
		pop.push_back(this->random_individual());

		//��������� ��������� �����
		//pop[i] = this->local_search(pop[i], this->s_m[0]); //��� ������ ������ ����������?

		//            for(int j=0;j<n;++j){
		//            System.out.print(pop[i][j]+" ");
		//            }
		//            System.out.println();
		//��������� ����������� �����
		//phi[i] = this->phitness(s, pop[i]);
		// ������� ������ ����� � ��������� � �� �����������
	//	if (phi[i] < phi_best)
	//	{
	//		phi_best = phi[i];
	//		ord_best_ind = i + 1;
	//		time_best = GetTickCount64() - start_time;
	//	}
	//	avg_phi = (double)((avg_phi*(i)+phi[i]) / (i + 1));//������� �����������
	}
	//iter_best = 0;//�������� ������ �����
	//			  //         time_best=System.currentTimeMillis()-StartTime;//����� ��������� ������ �����

}

////////////////////////////////////////////////////////////////////////////////
//��������� �����
////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::local_search(vector<int> assignment, vector<vector<int>> s_m_crit_index)
{
	int G = 0;
	int F = 0;
	int F_max = -1;
	vector<int> result(this->get_n());
	vector<vector<int>> path(2, vector<int>(this->get_n()));

	//            Random rnd = new Random();//������ ��������� �����
	int j_head = -1;
	int i_next = -1;
	int i_temp = -1;
	int i_tail = -1;
	int j_tail = -1;
	int count = 0;
	bool is_improve = true;

	for (int i = 0; i< this->get_n() - 1; i++)
	{
		path[1][assignment[i] - 1] = assignment[i + 1] - 1;
		path[0][assignment[i + 1] - 1] = assignment[i] - 1;
	}
	path[1][assignment[this->get_n() - 1] - 1] = assignment[0] - 1;
	path[0][assignment[0] - 1] = assignment[this->get_n() - 1] - 1;

	while (is_improve)
	{//���� ���� ��������� - �������� ������!
					   //          i_head=rnd.nextInt(n);//��������� ������� ���������� ��������� ������� (i_head - ��� ����� ����)
		for (int i_head = 0; i_head < this->get_n(); i_head++)
		{
			is_improve = false;
			j_head = path[1][i_head]; //(j_head - ��� ������ ����) 
			path[1][i_head] = -1;//������� ����
			path[0][j_head] = -1;

			//��������� ���� ��� ����������� ����� 
			for (int i_t = 0; i_t < this->get_n(); i_t++)
			{
				if (i_head != i_t && j_head != i_t)
				{
					i_next = i_t;
					G = 0;//��������� �������
						  //��������� ���� � ��� ����� ������� ����
						  //��������� (i_head, i_next)
					path[1][i_head] = i_next;
					i_temp = path[0][i_next];//���� ��� �������� ����������� ����������
					path[0][i_next] = i_head;
					G = G + s_m_crit_index[i_head][j_head] - s_m_crit_index[i_head][i_next];//��������� �������
																  //������� (i_temp, i_next)
					path[1][i_temp] = -1;
					//��������� ����
					//�������� ������� �� ����� j_tail
					count = 0;
					F_max = -1;
					j_tail = -1;
					i_tail = path[1][i_next];//������������� ������� �����
					while (count < this->get_n())
					{
						if (i_next != i_tail)
						{
							F = G + s_m_crit_index[i_temp][i_next] - s_m_crit_index[i_temp][i_tail] +
								s_m_crit_index[path[0][i_tail]][i_tail] - s_m_crit_index[path[0][i_tail]][j_head];
							if (F > 0)
							{
								count++;
								if (F > F_max)
								{
									j_tail = i_tail;
									F_max = F;
								}
							}
						}

						i_tail = path[1][i_tail];
						if (i_tail == i_next)
							break;
					}

					if (count == 0)
					{//� ������ ����������� ��� ��������� (���������� ����� �� �����)
						path[1][i_head] = -1;
						path[0][j_head] = -1;
						path[0][i_next] = i_temp;
						path[1][i_temp] = i_next;
					}
					else {//����� ���������� �������
						  //��������� (i_temp, j_tail)
						path[1][i_temp] = j_tail;
						i_tail = path[0][j_tail];
						path[0][j_tail] = i_temp;
						//������� (i_tail, j_tail)
						path[1][i_tail] = -1;
						//��������� (i_tail, j_head)
						path[1][i_tail] = j_head;
						path[0][j_head] = i_tail;
						is_improve = true;
						break;
					}
				}

			}

			if (is_improve)
			{
				//                    for (int i = 0; i < n; ++i) {
				//                    result[i] = path[1][i];
				//                    System.out.print(result[i]+" ");
				//                    }
				//                    System.out.println();
				//                     System.out.println("���������: " +costAssignment(c, result));
				break;
			}
			else {
				path[1][i_head] = j_head;//���������� ����
				path[0][j_head] = i_head;
			}
		}
	}

	//��������� ������������ (�����������)
	result[0] = 1;
	i_temp = 0;
	for (int i = 1; i<n; ++i)
	{
		result[i] = path[1][i_temp] + 1;
		i_temp = path[1][i_temp];
	}

	//                   for (int i = 0; i < n; ++i) {
	//                    result[i] = path[1][i];
	////                    System.out.print(result[i]+" ");
	//                    }
	//                    System.out.println();
	//                     System.out.println("����: " +costAssignment(c, result));     
	return result;
}

////////////////////////////////////////////////////////////////////////////////    
//���������� �������� ����� �� ���������� ��������
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
//�������� ���������� �������� ��� �����
///////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::multi_phitness(vector<int> p)
{
	vector<int> temp;
	for (int i=0; i<s_m.size(); i++)
		temp.push_back(phitness(s_m[i], p));

	return temp;
}


////////////////////////////////////////////////////////////////////////////////
//������� ������� ����� ��� ����� (�� ���� ���������)
////////////////////////////////////////////////////////////////////////////////
void GA_path::set_matrix_criteria(vector<vector<int>> s)
{
	this->s_m.push_back(s);
}


////////////////////////////////////////////////////////////////////////////////
//���������� ����� � ������� �������������
////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::random_individual()
{
	// ������������ ���������� ��������� �����
	//unsigned rand_value = 11;
	//srand(rand_value);

	vector<int> individual(n);//����������� �����
	int jNext, iNext = 0;//������� ���������� ��������� ������� � ����������� ��������������
	int count = 0;//������� ������������� ������
	vector<bool> indicator(n);//��������� ��� ����������� ������
	for (int j = 0; j < n; ++j)
	{
		indicator[j] = false;
	}
	
	//������� ����� rand(), ��. ����
	//Random^ rnd = gcnew Random();//������ ��������� �����
							  ////////////////////////////////////////////////////////////////
							  //������ ������� ���������� ��������� �������
	iNext = 0;
	individual[0] = iNext + 1;
	indicator[iNext] = true;//�������� ������� ��� �����������
						   ////////////////////////////////////////////////////////////////
						   //��������� ������� �������� ��������� ������� � ����������� ��������������
	for (int j = 1; j < n - 1; ++j)
	{
		//iNext = rnd->Next(n - j) + 1;
		iNext = (rand() % (n - j)) + 1;
		jNext = -1;
		count = 0;
		for (int i = 0; i < n; ++i)
		{
			//���� ������� ��� �� �����������, �� ����������� �������
			if (indicator[i] == false)
				count++;
		
			// ����� ��������� ������� ����� ��� �������������
			if (count == iNext)
			{
				jNext = i;// ���������� ����� ��������� �������
				break;
			}
		}
		individual[j] = jNext + 1;
		indicator[jNext] = true;//�������� ������� ��� �����������
	} // ��������� ������ n-1 �������
	
	for (int i = 0; i < n; ++i)
	{
		//���������� ������� ��������� � ��������� �������
		if (indicator[i] == false)
		{
			individual[n - 1] = i + 1;
			break;
		}
	}
	return individual;
}


////////////////////////////////////////////////////////////////////////////////
//��������� ������
////////////////////////////////////////////////////////////////////////////////
template <typename T>
bool GA_path::Pareto_pref(const vector<T> a, const vector<T> b)
{
	if (a.size() != b.size())
		return false;

	int counter_greater = 0;
	for (int i = 0; i < a.size(); i++)
		if (a[i] <= b[i]) counter_greater++;

	if ((counter_greater == a.size()) && (a != b))
		return true;
	else
		return false;
}


////////////////////////////////////////////////////////////////////////////////
//���������� ������� (��� ����������� ������ ������)
////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::range_front(vector<vector<int>>& pop_cur, bool flag_pop_sort)
{
	//������ ������� (������������ ������ ����� � ��������� pop)
	vector<vector<int>> index_front;
	vector<int> index_front_temp;
	//������ ������ �������
	vector<int> i_rank_temp;
	if (!flag_pop_sort)
		i_rank_temp.reserve(pop_cur.size());
	
	//��������������� ���������
	vector<vector<int>> pop_sort;


	//������: ������ ������� - ������ �������� ������, ������� ���������� ����� � ������ ��������
	vector<vector<int>> S_p(pop_cur.size(), vector<int>());
	//������: ������ ������� - ���������� ������, ������� ���������� ����� � ������ ��������
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
		//���� ����� ����������� ������� ������
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

	//����� ������
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
//���������� crowding distance (����������� ������ ������ � ������ = rank)
//flag = true - ������������ �������� ������������� ����������
//flag = false - ������������� �������� ������� ����������
////////////////////////////////////////////////////////////////////////////////
void GA_path::crowd_dist(vector<double>& i_dist_cur, int rank, vector<int> i_rank_cur,
	vector<vector<int>> pop_cur, bool flag_sort)
{
	if ((rank < 1) || (rank > *max_element(i_rank_cur.cbegin(), i_rank_cur.cend())))
		return;
	
	//���������� ����� ��������� ��� �����������

	//��� �������� �������� ������ �� ������ � ������=rank
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
				//��������� � ���������� ������ multi_phitness(pop_cur[i])[j]
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
		//���� ��� � ���� ��������
		for (int j = 1; j < index_front_temp.size(); j++)
		{
			//����� ��� �������� �� �������� i
			if (phitness(s_m[i], pop_cur[index_front_temp[j]]) < min_phi[i])
			{
				min_phi[i] = phitness(s_m[i], pop_cur[index_front_temp[j]]);
				j_min[i] = j;
			}
			//����� ���� �������� �� �������� i
			if (phitness(s_m[i], pop_cur[index_front_temp[j]]) > max_phi[i])
			{
				max_phi[i] = phitness(s_m[i], pop_cur[index_front_temp[j]]);
				j_max[i] = j;
			}
		}
		
		//�������������� ������� ���-�� �������� ������ ��������� �� ���������� � ���������
		index_front_cur_crit_temp = index_front_temp;
		//��������� ���-�� �������� ������ � ������������ � �� ���������� �� i-�� ��������
		if (flag_sort)
		{
			//��� ������ ���������� ���������
			//heap_sort(phi_front_temp[i], index_front_cur_crit_temp, phi_front_temp[i].size());
		}
		else
		{
			//��� ������ ������� ����������
			quick_sort(phi_front_temp[i], index_front_cur_crit_temp, 0, phi_front_temp[i].size() - 1);
		}
		
		i_dist_cur[index_front_cur_crit_temp[0]] = i_dist_cur[index_front_cur_crit_temp[index_front_cur_crit_temp.size()-1]] = INF;
		
		//���������� ���������� ��� ����� �� pop � ������� index_front_temp[j]
		for (int j = 1; j < index_front_cur_crit_temp.size()-1; j++)
			i_dist_cur[index_front_cur_crit_temp[j]] += (double) (this->multi_phitness( pop_cur[ index_front_cur_crit_temp[j + 1] ] )[i]
				- this->multi_phitness( pop_cur[ index_front_cur_crit_temp[j - 1] ] )[i] ) / (max_phi[i] - min_phi[i]);
	}
}

//����� �������
////////////////////////////////////////////////////////////////////////////////
//���������� crowding distance (����������� ������ ������ � ������ = rank)
//flag = true - ������������ �������� ������������� ����������
//flag = false - ������������� �������� ������� ����������
////////////////////////////////////////////////////////////////////////////////
void GA_path::crowd_dist_new(vector<vector<int>> pop_cur, bool flag_sort)
{
	//���������������, ��� ��������� pop_cur �������������

	//���� �������� ������
	int i_rank_cur = 1;
	//���������� ������, �� ��������� ��������� � ����� ���������, ��� ������������ ��������� N 
	int num_extra = 0;

	int min_phi, max_phi;


	 
	//��� �������� �������� ������ �� ������ � ������=rank
	vector<int> index_front_temp;
	vector<int> index_front_temp_not_sorted;
	
	for (int l = 0; l < this->get_N(); l++)
		i_dist[l] = 0;

	if (pop_cur.size() == this->get_N())
	{
		//crowded dist ������� ������ ��� ������ N ������, ������ �� ����
		for (int i = 0; i < pop_cur.size(); i++)
		{
			if ((i_rank[i] > i_rank_cur) || (i == this->get_N() - 1)) //������� ��������� ������
			{
				//���� ������� ��������� ������ - ����� �������, �� ��������� ������ ���������
				if (i == this->get_N() - 1)
					index_front_temp_not_sorted.push_back(i);

				for (int j = 0; j < this->get_m(); j++)
				{
					index_front_temp = index_front_temp_not_sorted;

					min_phi = phitness(s_m[j], pop_cur[index_front_temp[0]]);
					max_phi = phitness(s_m[j], pop_cur[index_front_temp[0]]);
					//���� ��� � ���� ��������
					for (int k = index_front_temp[0] + 1; k <= index_front_temp[index_front_temp.size() - 1]; k++)
					{
						//����� ��� �������� �� �������� ��������
						if (phitness(s_m[j], pop_cur[k]) < min_phi)
							min_phi = phitness(s_m[j], pop_cur[k]);

						//����� ���� ��������\ �� �������� ��������
						if (phitness(s_m[j], pop_cur[k]) > max_phi)
							max_phi = phitness(s_m[j], pop_cur[k]);
					}

					//�������������� ������� ���-�� �������� ������ ��������� �� ���������� � ���������
					//index_front_cur_crit_temp = index_front_temp;
					//��������� ���-�� �������� ������ � ������������ � �� ���������� �� i-�� ��������
					if (flag_sort)
					{
						//��� ������ ���������� ���������
						//true - ���������� �� ���������� ��������
						heap_sort(pop_cur, index_front_temp, j, index_front_temp[0], index_front_temp[index_front_temp.size() - 1]);
					}
					else
					{
						//��� ������ ������� ����������
						//quick_sort(index_front_temp, 0, index_front_temp.size() - 1);
					}

					i_dist[index_front_temp[0]] = i_dist[index_front_temp[index_front_temp.size() - 1]] = INF;

					//���������� ���������� ��� ����� �� pop � ������� index_front_temp[j]
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
				//��������� ������ ��������� �������� ������
				index_front_temp_not_sorted.push_back(i);
			}
		}
	}

	if (pop_cur.size() == 2 * this->get_N())
	{
		//crowded dist ������� ������ ��� ������ N ������, ������ �� ����
		for (int i = 0; i < pop_cur.size(); i++)
		{
			//�������, ������� ������ �� ���������� ������ �� ������ � ����� ���������
			//��� ��� ���� ������� i_dist, �� �� �� �������� � pop � i_rank
			if (i >= this->get_N())
				num_extra++;


			if ((i_rank_R_t[i] > i_rank_cur) || (i == 2 * this->get_N() - 1)) //������� ��������� ������
			{

				//���� ������� ��������� ������ - ����� �������, �� ��������� ������ ���������
				if (i == 2 * this->get_N() - 1)
				{
					index_front_temp_not_sorted.push_back(i);
					i_dist.resize(2 * this->get_N());
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
					//���� ��� � ���� ��������
					for (int k = index_front_temp[0] + 1; k <= index_front_temp[index_front_temp.size() - 1]; k++)
					{
						//����� ��� �������� �� �������� ��������
						if (phitness(s_m[j], pop_cur[k]) < min_phi)
							min_phi = phitness(s_m[j], pop_cur[k]);

						//����� ���� ��������\ �� �������� ��������
						if (phitness(s_m[j], pop_cur[k]) > max_phi)
							max_phi = phitness(s_m[j], pop_cur[k]);
					}
					
					//�������������� ������� ���-�� �������� ������ ��������� �� ���������� � ���������
					//index_front_cur_crit_temp = index_front_temp;
					//��������� ���-�� �������� ������ � ������������ � �� ���������� �� i-�� ��������
					if (flag_sort)
					{
						//��� ������ ���������� ���������
						//true - ���������� �� ���������� ��������
						heap_sort(pop_cur, index_front_temp, j, index_front_temp[0], index_front_temp[index_front_temp.size() - 1]);
					}
					else
					{
						//��� ������ ������� ����������
						//quick_sort(index_front_temp, 0, index_front_temp.size() - 1);
					}
					
					i_dist[index_front_temp[0]] = i_dist[index_front_temp[index_front_temp.size() - 1]] = INF;

					//���������� ���������� ��� ����� �� pop � ������� index_front_temp[j]
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
				//��������� ������ ��������� �������� ������
				index_front_temp_not_sorted.push_back(i);
			}
		}
		
	}
}

////////////////////////////////////////////////////////////////////////////////
//�������� Crowded-Comparison
//� �������� ���������� ���������� ������� �������
////////////////////////////////////////////////////////////////////////////////
bool GA_path::crowd_comp_oper(int i_p1, int i_p2) //(vector<int> p1, vector<int> p2)
{
	//������� ��� ������ p1, p2
	//int i_p1, i_p2;

	//�������� ����� p1
	//vector<vector<int>>::const_iterator it_p1;
	//it_p1 = find(pop.cbegin(), pop.cend(), p1);
	//�������� ����� p2
	//vector<vector<int>>::const_iterator it_p2;
	//it_p2 = find(pop.cbegin(), pop.cend(), p2);

	//����� �������� ������ p1, p2 - �� ������������
	/*
	for (int i = 0; i < pop.size(); i++)
	{
		if (p1 == pop[i])
			i_p1 = i;
		if (p2 == pop[i])
			i_p2 = i;
	}
	*/
	
	if ((i_rank[i_p1] < i_rank[i_p2]) || ((i_rank[i_p1] = i_rank[i_p2]) && (i_dist[i_p1] > i_dist[i_p2])))
		return true;
	else
		return false; 
}


////////////////////////////////////////////////////////////////////////////////
//������� ����������
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
//������������� ����������
////////////////////////////////////////////////////////////////////////////////
/*
void GA_path::heap_sort_new(vector<vector<int>>& pop_cur, int i_start, int i_stop)
{
	int delta = i_start;
	// ��������� ������ ��� ��������
	for (int i = ((i_stop - i_start + 1) / 2) - 1; i >= 0; i--)
		sift_down(pop_cur, this->i_dist, i, delta);
	// ���������� ����� �������� ��������� ��������
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


void GA_path::heap_sort(vector<vector<int>> pop_cur, vector<int>& numbers_index, int num_criteria, int index_begin, int index_end)
{
	// ��������� ������ ��� ��������
	for (int i = ((index_end - index_begin + 1) / 2) - 1; i >= 0; i--)
		sift_down(pop_cur, numbers_index, num_criteria, i, index_end - index_begin + 1, 0);
	// ���������� ����� �������� ��������� ��������
	for (int i = index_end - index_begin; i >= 1; i--)
	{
		//int temp = numbers[0];
		int temp_index = numbers_index[0];
		//numbers[0] = numbers[i];
		numbers_index[0] = numbers_index[i];
		//numbers[i] = temp;
		numbers_index[i] = temp_index;
		sift_down(pop_cur, numbers_index, num_criteria, 0, i, 0);
	}
}

void GA_path::heap_sort(vector<int>& numbers_index, int index_begin, int index_end)
{
	// ��������� ������ ��� ��������
	for (int i = ((index_end - index_begin + 1) / 2) - 1; i >= 0; i--)
		sift_down(numbers_index, i, index_end - index_begin + 1, index_begin);
	// ���������� ����� �������� ��������� ��������
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

//������� "�����������" ����� ���� (��. � ������� ������������� ����������)
void GA_path::sift_down(vector<vector<int>> pop_cur, vector<int>& numbers_index, int num_criteria, int root, int bottom, int delta)
{
	int max_child; // ������ ������������� �������
	int done = 0; // ���� ����, ��� ���� ������������
				  // ���� �� ����� �� ���������� ����
	while ( (root * 2 + 1 < bottom) && (!done) )
	{
		if ( root * 2 + 2 == bottom)    // ���� �� � ��������� ����, 
			max_child = root * 2 + 1;    // ���������� ����� �������
									// ����� ���������� ������� ������� �� ����
		else
		{
				if (phitness(s_m[num_criteria], pop_cur[numbers_index[delta + root * 2 + 1]]) > 
						phitness(s_m[num_criteria], pop_cur[numbers_index[delta + root * 2 + 2]]))
					max_child = root * 2 + 1;
				else
					max_child = root * 2 + 2;
		}
		// ���� ������� ������� ������ ������������� �������
			if (phitness(s_m[num_criteria], pop_cur[numbers_index[delta + root]]) <
					phitness(s_m[num_criteria], pop_cur[numbers_index[delta + max_child]]))
			{
				//int temp = numbers[delta + root]; // ������ �� �������
				int temp_index = numbers_index[delta + root]; // ������ ������� �������		
				//numbers[delta + root] = numbers[delta + max_child];
				numbers_index[delta + root] = numbers_index[delta + max_child];
				//numbers[delta + max_child] = temp;
				numbers_index[delta + max_child] = temp_index;
				root = max_child;
			}
			else // �����
				done = 1; // �������� ������������
		
	}
}


void GA_path::sift_down(vector<int>& numbers_index, int root, int bottom, int delta)
{
	int max_child; // ������ ������������� �������
	int done = 0; // ���� ����, ��� ���� ������������
				  // ���� �� ����� �� ���������� ����
	while ((root * 2 + 1 < bottom) && (!done))
	{
		if (root * 2 + 2 == bottom)    // ���� �� � ��������� ����, 
			max_child = root * 2 + 1;    // ���������� ����� �������
										 // ����� ���������� ������� ������� �� ����
		else
		{		
				if (i_dist[delta + root * 2 + 1] < i_dist[delta + root * 2 + 2])
					max_child = root * 2 + 1;
				else
					max_child = root * 2 + 2;
		}
		// ���� ������� ������� ������ ������������� �������
			if (i_dist[delta + root] > i_dist[delta + max_child])
			{
				int temp_index = numbers_index[root]; // ������ ������� �������
				double temp = i_dist[delta + root]; // ������ �� �������

				numbers_index[root] = numbers_index[max_child];
				i_dist[delta + root] = i_dist[delta + max_child];

				numbers_index[max_child] = temp_index;
				i_dist[delta + max_child] = temp;

				root = max_child;
			}
			else // �����
				done = 1; // �������� ������������
	}
}

////////////////////////////////////////////////////////////////////////////////
//��������� ��������
////////////////////////////////////////////////////////////////////////////////
int GA_path::tourn_selection()
{
	int j;
	int i_best = (rand() % this->get_N()); //������ ����� � �������
	//!!!������: ������� ��� ���� ������ ����? (������ 10)
	for (int s = this->get_tourn_size(); s>0; s--)
	{
		j = (rand() % this->get_N());
		if ( crowd_comp_oper(j, i_best) ) //���. croweded-comp oper
			i_best = j;
	}
	return i_best;
}

////////////////////////////////////////////////////////////////////////////////
//�������
////////////////////////////////////////////////////////////////////////////////
void GA_path::mutation(int i_p1, int i_p2, vector<int>& p1_temp, vector<int>& p2_temp)
{
	// ������������ ���������� ��������� �����
	//unsigned rand_value = 11;
	//srand(rand_value);

	for (int i = 0; i< n - 1; ++i)
	{
		p1_temp[this->pop[i_p1][i] - 1] = this->pop[i_p1][i + 1] - 1;
		p2_temp[this->pop[i_p2][i] - 1] = this->pop[i_p2][i + 1] - 1;
	}
	p1_temp[this->pop[i_p1][n - 1] - 1] = this->pop[i_p1][0] - 1;
	p2_temp[this->pop[i_p2][n - 1] - 1] = this->pop[i_p2][0] - 1;
	
	//�������
	double rand_mut = (double)rand() / RAND_MAX;
	if ( rand_mut < this->get_p_mut() )
	{
		rand_mut = (double)rand() / RAND_MAX;
		if ( rand_mut < 0.5 )
		{
			//3-������ �� 1-�� ��������
			p1_temp = random_change2(p1_temp, 1, this->s_m[0], this->s_aver[0], 50);
		}
		else
		{ 
			//3-������ �� 2-�� ��������
			p1_temp = random_change2(p1_temp, 1, this->s_m[1], this->s_aver[1], 50);
		}
	}

	rand_mut = (double)rand() / RAND_MAX;
	if ( rand_mut < this->get_p_mut() )
	{
		rand_mut = (double)rand() / RAND_MAX;
		if (rand_mut < 0.5)
		{
			p2_temp = random_change2(p2_temp, 1, this->s_m[0], this->s_aver[0], 50);
		}
		else 
		{ 
			p2_temp = random_change2(p2_temp, 1, this->s_m[1], this->s_aver[1], 50);
		}
	}
	
}

////////////////////////////////////////////////////////////////////////////////
//3-opt ������ � �������
////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::random_change2(vector<int> assignment, int N_change, vector<vector<int>> c,
	int c_aver, int alpha)
{
	int G = 0, x = -1, y_1 = -1, F = 0, F_max = -1, C = 0;
	//������������ ��� ������ ���� ��� ����������� �����
	vector<vector<int>> F_cicle(2, vector<int>(n));
	vector<int> result(n);
	vector<vector<int>> path(2, vector<int>(n));//������ ������� � �������� �� �����
	vector<vector<boolean>> view(n, vector<boolean>(n));//����� ������������� ��� 
	
	// ������������ ���������� ��������� �����
	//unsigned rand_value = 11;
	//srand(rand_value);
	
	int i_head, j_head, count, i_next = -1, i_temp, i_tail, j_tail;

	for (int i = 0; i < n; ++i)
	{
		path[1][i] = assignment[i];
		path[0][assignment[i]] = i;
		view[i][i] = true;
	}
	//          interMatrix(n,2,path);
	i_head = rand() % n;//��������� ������� ���������� ��������� ������� (i_head - ��� ����� ����)
	x = j_head = path[1][i_head]; //(j_head - ��� ������ ����) 
	path[1][i_head] = -1;//������� ����
	path[0][j_head] = -1;
	view[i_head][j_head] = true;//�������� ���� �������������
	
	for (int i = 0; i < n; ++i)//� ������� ���������� � �����
	{
		view[i][j_head] = true;//�������� ���� �������������   
	}
	//          printf("i_head= %d",i_head);//����� ����������� ����

	//          printf("Deleted (%d, %d), i_head, j_head);
	for (int iter = 0; iter<N_change; ++iter)
	{
		//              printf("iter = %d", iter);
		//��������� ���� ��� ����������� ����� 
		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
		//��������� ����������������� F_cicle ��� ������ �������
		count = 0;
		for (int i = 0; i < n; ++i)
		{
			F_cicle[0][i] = i;
			F_cicle[1][i] = -n * c_aver;

			//����-��������� �� �������� �����
			if (view[i_head][i] == false && view[path[0][i]][i] == false)
			{
				F_cicle[1][i] = c[path[0][i]][i];//�������� ����
				C = 1;
				y_1 = i;
				while (y_1 != i_head)
				{
					C++;
					y_1 = path[1][y_1];
				}
				F_cicle[1][i] = F_cicle[1][i] + c_aver * C;//+����� �����
				count++;
			}
		}
		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
		//���������   F_cicle �� �������� 
		F = -1; F_max = -1;
		i_next = -1;
		while (F_max == -1)
		{
			F_max = 1;
			for (int i = 0; i < n - 1; ++i)
			{
				if (F_cicle[1][i]<F_cicle[1][i + 1])//������ �������
				{
					F = F_cicle[1][i];
					i_next = F_cicle[0][i];
					F_cicle[1][i] = F_cicle[1][i + 1];
					F_cicle[0][i] = F_cicle[0][i + 1];
					F_cicle[1][i + 1] = F;
					F_cicle[0][i + 1] = i_next;
					F_max = -1;
				}
			}
		}
		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
		// ����� �� ����� ���� ����????
		if (count == 0)
		{
			//              printf("���������� ��������� ��� �������� �����");
			break;
		}//�� i_head �� ��������� ������� ����
		 //�������� ���� ������� �� ���������, ��������� �������
		F_max = (int)((alpha*n) / 100);
		if (F_max>count) { F_max = count; }
		F = rand() % F_max;
		i_next = F_cicle[0][F];
		//          printf("i_next = d% F = %d", i_next, F_cicle[1][F]);

		//          i_temp=rnd.nextInt(count)+1;
		////          System.out.println("count= "+count);
		////          System.out.println("i_temp= "+i_temp);
		//          count=0;
		//          for (int i = 0; i < n; ++i) {
		//            if(view[i_head][i]==false && view[path[0][i]][i]==false){count++;
		//            if(count==i_temp){i_next=i;break;}}  
		//          }
		//          System.out.println("i_next= "+i_next);
		//          interMatrix(n,2,path);
		//��������� ���� � ��� ����� ������� ����
		//��������� (i_head, i_next)
		path[1][i_head] = i_next;
		i_temp = path[0][i_next];//���� ��� �������� ����������� ����������
		path[0][i_next] = i_head;
		view[i_head][i_next] = true;//�������� ���� �������������
		G = G + c[i_head][x] - c[i_head][i_next];//��������� �������
												 //          printf("i_temp = %d", i_temp);
												 //������� (i_temp, i_next)
		path[1][i_temp] = -1;
		view[i_temp][i_next] = true;//�������� ���� �������������
									//          interMatrix(n,2,path);
									//          printf("Added (%d, %d)", i_head, i_next);
									//          printf("Added (%d, %d)", i_temp, i_next);
									//��������� ����
									//��������  ������� �� ����� j_tail ������ ��������
		count = 0; F_max = -n * c_aver; j_tail = -1;
		i_tail = i_head;//������������� ������� �����
		while (count<n)
		{
			if (view[i_temp][i_tail] == false && view[path[0][i_tail]][i_tail] == false)
			{
				F = G + c[i_temp][i_next] - c[i_temp][i_tail] + c[path[0][i_tail]][i_tail] - c[path[0][i_tail]][j_head];
				count++;
				if (F>F_max)
				{
					j_tail = i_tail;
					F_max = F;
				}

			}
			i_tail = path[1][i_tail];
			if (i_tail == i_head)
				break;
		}
		//          printf("count = &d", count);// ����� �� ����� ���� ����????
		if (count == 0) //�� ������� ������� ���
		{
			//���������� ��� �� ����� � ���������������
			path[1][i_head] = -1;
			path[0][j_head] = -1;
			path[0][i_next] = i_temp;
			path[1][i_temp] = i_next;
			//          printf("���������� ��������� ��� ������ ������� � �����");
			break;
		}
		//�������� ��������� ������� �� ����� j_tail
		//          i_tail=rnd.nextInt(count)+1; j_tail=-1;
		//          System.out.println("j_tail= "+j_tail+" F_max= "+F_max );
		//          count=0;
		//          i_next=i_head;
		//          while (count<n) {
		//             if(view[i_temp][i_next]==false && view[path[0][i_next]][i_next]==false){count++;
		//             if(count==i_tail){j_tail=i_next;break;}}  
		//             i_next=path[1][i_next];
		//          }
		//          System.out.println("j_tail= "+j_tail);
		//          interMatrix(n,2,path);
		//��������� (i_temp, j_tail)
		path[1][i_temp] = j_tail;
		i_tail = path[0][j_tail];
		path[0][j_tail] = i_temp;
		view[i_temp][j_tail] = true;//�������� ���� �������������
									//������� (i_tail, j_tail)
		path[1][i_tail] = -1;
		view[i_tail][j_tail] = true;//�������� ���� �������������
									//          printf("Added (%d, %d)", i_temp, j_tail);
									//          printf("Deleted (%d, %d)", i_tail, j_tail);
		i_head = i_tail; x = j_tail;
		G = G + c[i_temp][i_next] - c[i_temp][j_tail];//��������� ���������
													  //          printf("i_tail = %d", i_tail);
													  //          interMatrix(n,2,path);
	}
	//��������� (i_head, j_head)
	path[1][i_head] = j_head;
	path[0][j_head] = i_head;//�������� ����
							 //          printf("Added (%d, %d)", i_head, j_head);
							 //          interMatrix(n,2,path);
							 //��������� ������������ (�����������)
							 //          result[0]=1; i_temp=0;
							 //          for(int i=1;i<n;++i){
							 //          result[i]= path[1][i_temp];
							 //          i_temp= path[1][i_temp];
							 //          }
	for (int i = 0; i < n; ++i)
		result[i] = path[1][i];

	return result;
}

////////////////////////////////////////////////////////////////////////////////
//������� ������
////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::exchange_mutation(vector<int> p)
{
	//p -- ����� ��� �������
	// � ������������ 1 ������ ���������� ���� ������� (4 ���� �������� � �����)

	int i_m1;//������ ������� ��� ������
	int i_m2;//������ ������� ��� ������
	int p_temp;
	
	i_m1 = rand() % (n - 1) + 1;
	i_m2 = rand() % (n - 1) + 1;
	/////////////////////////

	p_temp = p[i_m1];
	p[i_m1] = p[i_m2];
	p[i_m2] = p_temp;
	
	return p;
}////////////////////////////////////////////////////////////////////////////////  

 ////////////////////////////////////////////////////////////////////////////////
 //<< julia:  ����� �������� �������������
 ////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::DEC_new(vector< vector <vector<int> > > s, vector<int> p1, vector<int> p2)
{
	//        for (int i = 0; i < p1.length; ++i) {
	//            System.out.print(p1[i] + " ");
	//        }
	//        System.out.println();
	//        //������ ��������
	//        for (int i = 0; i < p2.length; ++i) {
	//            System.out.print(p2[i] + " ");
	//        }
	//        System.out.println();

	vector<vector<int>> list_in(n, vector<int>(3));//�������� �������
	vector<vector<int>> list_out(n, vector<int>(4));//��������� �������
	vector<int> child(n);//��������� �������
						 // �������� 2 �� m?
	vector<vector<int>> arc_weights(n, vector<int>(2));//���� ���
	vector<int> vertex_number(n);//��������� �������
								 // ���������� flag?
	vector<bool> flag;


	int vertex, vertex_prev, vertexN, i_parts, i1, i_temp;
	vertex = 1; vertexN = 0;

	// ������������ ���������� ��������� �����
	//unsigned rand_value = 11;
	//srand(rand_value);

	////////////////////////////////////////////////////////////////////////////////   
	/////!��������� ������!/////
	//++++++++++++++++++++++++++
	for (int i = 0; i < p1.size() - 1; ++i)
	{
		//++++++++++++++++++++++++++
		//++++++++++++++++++++++++++
		//������
		//��������� ������� ��� p1[i]
		if (list_out[p1[i] - 1][0] == 0)
		{
			list_out[p1[i] - 1][0] = 1;
			list_out[p1[i] - 1][1] = p1[i + 1];
		}
		if (list_out[p1[i] - 1][0] == 1 && list_out[p1[i] - 1][1] != p1[i + 1])
		{
			list_out[p1[i] - 1][0] = 2;
			list_out[p1[i] - 1][2] = p1[i + 1];
		}
		//�������� ������� ��� p1[i+1]
		if (list_in[p1[i + 1] - 1][0] == 0)
		{
			list_in[p1[i + 1] - 1][0] = 1;
			list_in[p1[i + 1] - 1][1] = p1[i];
		}
		if (list_in[p1[i + 1] - 1][0] == 1 && list_in[p1[i + 1] - 1][1] != p1[i])
		{
			list_in[p1[i + 1] - 1][0] = 2;
			list_in[p1[i + 1] - 1][2] = p1[i];
		}
		//��������� ������� ��� p2[i]
		if (list_out[p2[i] - 1][0] == 0)
		{
			list_out[p2[i] - 1][0] = 1;
			list_out[p2[i] - 1][1] = p2[i + 1];
		}
		if (list_out[p2[i] - 1][0] == 1 && list_out[p2[i] - 1][1] != p2[i + 1])
		{
			list_out[p2[i] - 1][0] = 2;
			list_out[p2[i] - 1][2] = p2[i + 1];
		}
		//�������� ������� ��� p2[i+1]
		if (list_in[p2[i + 1] - 1][0] == 0)
		{
			list_in[p2[i + 1] - 1][0] = 1;
			list_in[p2[i + 1] - 1][1] = p2[i];
		}
		if (list_in[p2[i + 1] - 1][0] == 1 && list_in[p2[i + 1] - 1][1] != p2[i])
		{
			list_in[p2[i + 1] - 1][0] = 2;
			list_in[p2[i + 1] - 1][2] = p2[i];
		}
	}

	//��� TSP ����� ��������
	//         //������
	//        //��������� ��������� p1[n-1]
	if (list_out[p1[p1.size() - 1] - 1][0] == 0)
	{
		list_out[p1[p1.size() - 1] - 1][0] = 1;
		list_out[p1[p1.size() - 1] - 1][1] = p1[0];
	}
	if (list_out[p1[p1.size() - 1] - 1][0] == 1 && list_out[p1[p1.size() - 1] - 1][1] != p1[0])
	{
		list_out[p1[p1.size() - 1] - 1][0] = 2;
		list_out[p1[p1.size() - 1] - 1][2] = p1[0];
	}
	//�������� ������� ��� p1[0]
	if (list_in[p1[0] - 1][0] == 0)
	{
		list_in[p1[0] - 1][0] = 1;
		list_in[p1[0] - 1][1] = p1[p1.size() - 1];
	}
	if (list_in[p1[0] - 1][0] == 1 && list_in[p1[0] - 1][1] != p1[p1.size() - 1])
	{
		list_in[p1[0] - 1][0] = 2;
		list_in[p1[0] - 1][2] = p1[p1.size() - 1];
	}
	//��������� ������� ��� p2[n-1]
	if (list_out[p2[p2.size() - 1] - 1][0] == 0)
	{
		list_out[p2[p2.size() - 1] - 1][0] = 1;
		list_out[p2[p2.size() - 1] - 1][1] = p2[0];
	}
	if (list_out[p2[p2.size() - 1] - 1][0] == 1 && list_out[p2[p2.size() - 1] - 1][1] != p2[0])
	{
		list_out[p2[p2.size() - 1] - 1][0] = 2;
		list_out[p2[p2.size() - 1] - 1][2] = p2[0];
	}
	//�������� ������� ��� p2[0]
	if (list_in[p2[0] - 1][0] == 0)
	{
		list_in[p2[0] - 1][0] = 1;
		list_in[p2[0] - 1][1] = p2[p2.size() - 1];
	}
	if (list_in[p2[0] - 1][0] == 1 && list_in[p2[0] - 1][1] != p2[p2.size() - 1])
	{
		list_in[p2[0] - 1][0] = 2;
		list_in[p2[0] - 1][2] = p2[p2.size() - 1];
	}
	//++++++++++++++++++++++++++
	//++++++++++++++++++++++++++
	/////!��������� ������!///// 
	////////////////////////////////////////////////////////////////////////////////
	//       
	//           
	//        System.out.println();
	//           for (int i = 0; i < n; ++i) {
	//           System.out.print((i + 1) + " ");
	//           for (int j = 0; j < 3; ++j) {
	//               System.out.print(list_in[i][j] + " ");
	//           }
	//           System.out.println();
	//          }
	//          System.out.println();
	//        System.out.println();
	//           for (int i = 0; i < n; ++i) {
	//           System.out.print((i + 1) + " ");
	//           for (int j = 0; j < 4; ++j) {
	//               System.out.print(list_out[i][j] + " ");
	//           }
	//           System.out.println();
	//          }
	//          System.out.println();
	vertex = 1;
	if (((double)(rand() / RAND_MAX)) <= 0.5)
		vertex = p1[0];
	else
		vertex = p2[0];

	child[0] = vertex;
	for (vertexN = 1; vertexN < n; vertexN++)
	{
		list_out[vertex - 1][3] = 1;//�������� ������� ��� ���������
									//������� ������� ������� �� list_in
		if (list_out[vertex - 1][0] == 1)
		{
			list_in[list_out[vertex - 1][1] - 1][0]--;
			if (list_in[list_out[vertex - 1][1] - 1][1] == vertex)
			{
				list_in[list_out[vertex - 1][1] - 1][1] = list_in[list_out[vertex - 1][1] - 1][2];
				list_in[list_out[vertex - 1][1] - 1][2] = 0;
			}
		}
		else {
			if (list_out[vertex - 1][0] == 2)
			{
				list_in[list_out[vertex - 1][1] - 1][0]--;
				if (list_in[list_out[vertex - 1][1] - 1][1] == vertex)
				{
					list_in[list_out[vertex - 1][1] - 1][1] = list_in[list_out[vertex - 1][1] - 1][2];
					list_in[list_out[vertex - 1][1] - 1][2] = 0;
				}
				list_in[list_out[vertex - 1][2] - 1][0]--;
				if (list_in[list_out[vertex - 1][2] - 1][1] == vertex)
				{
					list_in[list_out[vertex - 1][2] - 1][1] = list_in[list_out[vertex - 1][2] - 1][2];
					list_in[list_out[vertex - 1][2] - 1][2] = 0;
				}
			}
		}

		//             System.out.println();
		//           for (int i = 0; i < n; ++i) {
		//           System.out.print((i + 1) + " ");
		//           for (int j = 0; j < 3; ++j) {
		//               System.out.print(list_in[i][j] + " ");
		//           }
		//           System.out.println();
		//          }
		//         System.out.println();
		//������� ������� ������� �� list_out  
		if (list_in[vertex - 1][0] == 1)
		{
			list_out[list_in[vertex - 1][1] - 1][0] --;
			if (list_out[list_in[vertex - 1][1] - 1][1] == vertex)
			{
				list_out[list_in[vertex - 1][1] - 1][1] = list_out[list_in[vertex - 1][1] - 1][2];
				list_out[list_in[vertex - 1][1] - 1][2] = 0;
			}
			else {
				list_out[list_in[vertex - 1][1] - 1][2] = 0;
			}
		}
		else {
			if (list_in[vertex - 1][0] == 2)
			{
				list_out[list_in[vertex - 1][1] - 1][0]--;
				if (list_out[list_in[vertex - 1][1] - 1][1] == vertex)
				{
					list_out[list_in[vertex - 1][1] - 1][1] = list_out[list_in[vertex - 1][1] - 1][2];
					list_out[list_in[vertex - 1][1] - 1][2] = 0;
				}
				else {
					list_out[list_in[vertex - 1][1] - 1][2] = 0;
				}

				list_out[list_in[vertex - 1][2] - 1][0]--;
				if (list_out[list_in[vertex - 1][2] - 1][1] == vertex)
				{
					list_out[list_in[vertex - 1][2] - 1][1] = list_out[list_in[vertex - 1][2] - 1][2];
					list_out[list_in[vertex - 1][2] - 1][2] = 0;
				}
				else {
					list_out[list_in[vertex - 1][2] - 1][2] = 0;
				}
			}
		}

		//          System.out.println();
		//           for (int i = 0; i < n; ++i) {
		//           System.out.print((i + 1) + " ");
		//           for (int j = 0; j < 4; ++j) {
		//               System.out.print(list_out[i][j] + " ");
		//           }
		//           System.out.println();
		//          }
		//         System.out.println(vertex);
		//�������� ��������� �������
		//vertex=-1;

		vertex_prev = vertex;
		if (list_out[vertex - 1][0] == 0)
		{
			i_parts = 0;
			for (int i = 0; i < n; ++i)
			{
				if (list_out[i][3] == 0)
				{
					arc_weights[i_parts][0] = s[0][vertex_prev - 1][i];
					arc_weights[i_parts][1] = s[1][vertex_prev - 1][i];
					vertex_number[i_parts] = i + 1;
					i_parts++;
				}
			}

			flag = flag_Pareto_sol(i_parts, arc_weights);
			i1 = 0;
			for (int i = 0; i < i_parts; ++i)
			{
				if (flag[i] == false)
					i1++;

				//                 System.out.println(arcWeights[i][0] + " " + arcWeights[i][1] + "; " + vertexNumber[i] + "; " + flag[i]);
			}
			//             System.out.println("i1: " + i1);
			i_temp = rand() % i1 + 1;
			//             System.out.println("i_temp: " + i_temp);
			i1 = 0;
			vertex = -1;
			for (int i = 0; i < i_parts; ++i)
			{
				if (flag[i] == false)
					i1++;

				if (i1 == i_temp)
				{
					vertex = vertex_number[i];
					break;
				}
			}
			//               vertex = list_out[vertex - 1][1];
		}
		else {
			if (list_out[vertex - 1][0] == 1)
			{
				vertex = list_out[vertex - 1][1];
			}
			else {
				if (list_out[vertex - 1][0] == 2)
				{

					if (list_in[list_out[vertex - 1][1] - 1][0] == 0 || list_in[list_out[vertex - 1][2] - 1][0] == 0)
					{
						if (list_in[list_out[vertex - 1][1] - 1][0] == 0)
						{
							vertex = list_out[vertex - 1][1];
						}
						else {
							vertex = list_out[vertex - 1][2];
						}
					}
					else {

						if (list_out[list_out[vertex - 1][1] - 1][0] == list_out[list_out[vertex - 1][2] - 1][0])
						{
							if (s[0][vertex_prev - 1][list_out[vertex - 1][1] - 1] <= s[0][vertex_prev - 1][list_out[vertex - 1][2] - 1] &&
								s[1][vertex_prev - 1][list_out[vertex - 1][1] - 1] <= s[1][vertex_prev - 1][list_out[vertex - 1][2] - 1])
							{
								vertex = list_out[vertex - 1][1];
							}
							else {
								if (s[0][vertex_prev - 1][list_out[vertex - 1][2] - 1] <= s[0][vertex_prev - 1][list_out[vertex - 1][1] - 1] &&
									s[1][vertex_prev - 1][list_out[vertex - 1][2] - 1] <= s[1][vertex_prev - 1][list_out[vertex - 1][1] - 1])
								{
									vertex = list_out[vertex - 1][2];
								}
								else {
									if (((double)(rand() / RAND_MAX)) <= 0.5)
									{
										vertex = list_out[vertex - 1][1];
									}
									else {
										vertex = list_out[vertex - 1][2];
									}
								}
							}

						}
						else {
							if (list_out[list_out[vertex - 1][1] - 1][0] == 0 || list_out[list_out[vertex - 1][2] - 1][0] == 0)
							{
								if (list_out[list_out[vertex - 1][2] - 1][0] == 0)
								{
									vertex = list_out[vertex - 1][1];
								}
								else {
									vertex = list_out[vertex - 1][2];
								}
							}
							else {
								if (list_out[list_out[vertex - 1][1] - 1][0] < list_out[list_out[vertex - 1][2] - 1][0])
								{
									vertex = list_out[vertex - 1][1];
								}
								else {
									vertex = list_out[vertex - 1][2];
								}
							}
						}
					}
					//����� if (list_out[vertex - 1][0] == 2)
				}
			}
		}
		child[vertexN] = vertex;
		//           System.out.println(vertex);

	}
	//        System.out.println();
	//           for (int i = 0; i < n; ++i) {
	//           System.out.print((i + 1) + " ");
	//           for (int j = 0; j < 3; ++j) {
	//               System.out.print(list_out[i][j] + " ");
	//           }
	//           System.out.println();
	//       }
	//       System.out.println();
	//       for (int i = 0; i < n; ++i) {
	//           System.out.print((i + 1) + " ");
	//           for (int j = 0; j < 3; ++j) {
	//               System.out.print(list_in[i][j] + " ");
	//           }
	//           System.out.println();
	//       }
	//       System.out.println();
	//       System.out.println();
	//       for (int i = 0; i < n; ++i) {
	//           
	//               System.out.print(child[i] + " ");
	//          
	//       }
	//       System.out.println();
	//       System.out.println(this.phitnessTSP(s, p1));
	//       System.out.println(this.phitnessTSP(s, p2));
	//       System.out.println(this.phitnessTSP(s, child));
	//	  	 this->phi_best_in_rec = this->phitness(s, child);
	//		 this->number_solution_in_rec = 1;
	return child;
}

//������� ��������
//////////////////////////////////////////////////////////////////
//����������������� ������������, ���������� �� ������������ ���
//////////////////////////////////////////////////////////////////
vector<int> GA_path::DEC_old(vector<vector<int>> s, vector<int> p1, vector<int> p2)
{
	//        for (int i = 0; i < p1.length; ++i) {
	//            System.out.print(p1[i] + " ");
	//        }
	//        System.out.println();
	//        //������ ��������
	//        for (int i = 0; i < p2.length; ++i) {
	//            System.out.print(p2[i] + " ");
	//        }
	//        System.out.println();

	vector<vector<int>> list_in(n, vector<int>(4));//�������� �������
	vector<vector<int>> list_out(n, vector<int>(4));//��������� �������
	vector<int> child(n);//��������� �������

	int vertex, vertexN;
	vertex = 1; vertexN = 0;
	
	// ������������ ���������� ��������� �����
	//unsigned rand_value = 11;
	//srand(rand_value);

	////////////////////////////////////////////////////////////////////////////////   
	/////!��������� ������!/////
	//++++++++++++++++++++++++++
	for (int i = 0; i < p1.size() - 1; ++i)
	{
		//++++++++++++++++++++++++++
		//++++++++++++++++++++++++++
		//������
		//��������� ������� ��� p1[i]
		if (list_out[p1[i] - 1][0] == 0)
		{
			list_out[p1[i] - 1][0] = 1;
			list_out[p1[i] - 1][1] = p1[i + 1];
		}
		if (list_out[p1[i] - 1][0] == 1 && list_out[p1[i] - 1][1] != p1[i + 1])
		{
			list_out[p1[i] - 1][0] = 2;
			list_out[p1[i] - 1][2] = p1[i + 1];
		}
		//�������� ������� ��� p1[i+1]
		if (list_in[p1[i + 1] - 1][0] == 0)
		{
			list_in[p1[i + 1] - 1][0] = 1;
			list_in[p1[i + 1] - 1][1] = p1[i];
		}
		if (list_in[p1[i + 1] - 1][0] == 1 && list_in[p1[i + 1] - 1][1] != p1[i])
		{
			list_in[p1[i + 1] - 1][0] = 2;
			list_in[p1[i + 1] - 1][2] = p1[i];
		}
		//��������� ������� ��� p2[i]
		if (list_out[p2[i] - 1][0] == 0)
		{
			list_out[p2[i] - 1][0] = 1;
			list_out[p2[i] - 1][1] = p2[i + 1];
		}
		if (list_out[p2[i] - 1][0] == 1 && list_out[p2[i] - 1][1] != p2[i + 1])
		{
			list_out[p2[i] - 1][0] = 2;
			list_out[p2[i] - 1][2] = p2[i + 1];
		}
		//�������� ������� ��� p2[i+1]
		if (list_in[p2[i + 1] - 1][0] == 0)
		{
			list_in[p2[i + 1] - 1][0] = 1;
			list_in[p2[i + 1] - 1][1] = p2[i];
		}
		if (list_in[p2[i + 1] - 1][0] == 1 && list_in[p2[i + 1] - 1][1] != p2[i])
		{
			list_in[p2[i + 1] - 1][0] = 2;
			list_in[p2[i + 1] - 1][2] = p2[i];
		}
	}

	//��� TSP ����� ��������
	//         //������
	//        //��������� ��������� p1[n-1]
	if (list_out[p1[p1.size() - 1] - 1][0] == 0)
	{
		list_out[p1[p1.size() - 1] - 1][0] = 1;
		list_out[p1[p1.size() - 1] - 1][1] = p1[0];
	}
	if (list_out[p1[p1.size() - 1] - 1][0] == 1 && list_out[p1[p1.size() - 1] - 1][1] != p1[0])
	{
		list_out[p1[p1.size() - 1] - 1][0] = 2;
		list_out[p1[p1.size() - 1] - 1][2] = p1[0];
	}
	//�������� ������� ��� p1[0]
	if (list_in[p1[0] - 1][0] == 0)
	{
		list_in[p1[0] - 1][0] = 1;
		list_in[p1[0] - 1][1] = p1[p1.size() - 1];
	}
	if (list_in[p1[0] - 1][0] == 1 && list_in[p1[0] - 1][1] != p1[p1.size() - 1])
	{
		list_in[p1[0] - 1][0] = 2;
		list_in[p1[0] - 1][2] = p1[p1.size() - 1];
	}
	//��������� ������� ��� p2[n-1]
	if (list_out[p2[p2.size() - 1] - 1][0] == 0)
	{
		list_out[p2[p2.size() - 1] - 1][0] = 1;
		list_out[p2[p2.size() - 1] - 1][1] = p2[0];
	}
	if (list_out[p2[p2.size() - 1] - 1][0] == 1 && list_out[p2[p2.size()  - 1] - 1][1] != p2[0])
	{
		list_out[p2[p2.size() - 1] - 1][0] = 2;
		list_out[p2[p2.size() - 1] - 1][2] = p2[0];
	}
	//�������� ������� ��� p2[0]
	if (list_in[p2[0] - 1][0] == 0)
	{
		list_in[p2[0] - 1][0] = 1;
		list_in[p2[0] - 1][1] = p2[p2.size() - 1];
	}
	if (list_in[p2[0] - 1][0] == 1 && list_in[p2[0] - 1][1] != p2[p2.size() - 1])
	{
		list_in[p2[0] - 1][0] = 2;
		list_in[p2[0] - 1][2] = p2[p2.size() - 1];
	}
	//++++++++++++++++++++++++++
	//++++++++++++++++++++++++++
	/////!��������� ������!///// 
	////////////////////////////////////////////////////////////////////////////////
	//       
	//           
	//        System.out.println();
	//           for (int i = 0; i < n; ++i) {
	//           System.out.print((i + 1) + " ");
	//           for (int j = 0; j < 3; ++j) {
	//               System.out.print(list_in[i][j] + " ");
	//           }
	//           System.out.println();
	//          }
	//          System.out.println();
	//        System.out.println();
	//           for (int i = 0; i < n; ++i) {
	//           System.out.print((i + 1) + " ");
	//           for (int j = 0; j < 4; ++j) {
	//               System.out.print(list_out[i][j] + " ");
	//           }
	//           System.out.println();
	//          }
	//          System.out.println();
	vertex = 1;
	if (( (double)(rand() / RAND_MAX) ) <= 0.5)
		vertex = p1[0];
	else
		vertex = p2[0];

	child[0] = vertex;
	for (vertexN = 1; vertexN < n; vertexN++)
	{
		list_out[vertex - 1][3] = 1;//�������� ������� ��� ���������
									//������� ������� ������� �� list_in
		if (list_out[vertex - 1][0] == 1)
		{
			list_in[list_out[vertex - 1][1] - 1][0]--;
			if (list_in[list_out[vertex - 1][1] - 1][1] == vertex)
			{
				list_in[list_out[vertex - 1][1] - 1][1] = list_in[list_out[vertex - 1][1] - 1][2]; list_in[list_out[vertex - 1][1] - 1][2] = 0;
			}
		}
		else {
			if (list_out[vertex - 1][0] == 2)
			{
				list_in[list_out[vertex - 1][1] - 1][0]--;
				if (list_in[list_out[vertex - 1][1] - 1][1] == vertex)
				{
					list_in[list_out[vertex - 1][1] - 1][1] = list_in[list_out[vertex - 1][1] - 1][2]; list_in[list_out[vertex - 1][1] - 1][2] = 0;
				}
				list_in[list_out[vertex - 1][2] - 1][0]--;
				if (list_in[list_out[vertex - 1][2] - 1][1] == vertex)
				{
					list_in[list_out[vertex - 1][2] - 1][1] = list_in[list_out[vertex - 1][2] - 1][2]; list_in[list_out[vertex - 1][2] - 1][2] = 0;
				}
			}
		}

		//             System.out.println();
		//           for (int i = 0; i < n; ++i) {
		//           System.out.print((i + 1) + " ");
		//           for (int j = 0; j < 3; ++j) {
		//               System.out.print(list_in[i][j] + " ");
		//           }
		//           System.out.println();
		//          }
		//         System.out.println();
		//������� ������� ������� �� list_out  
		if (list_in[vertex - 1][0] == 1)
		{
			list_out[list_in[vertex - 1][1] - 1][0] --;
			if (list_out[list_in[vertex - 1][1] - 1][1] == vertex)
			{
				list_out[list_in[vertex - 1][1] - 1][1] = list_out[list_in[vertex - 1][1] - 1][2];
				list_out[list_in[vertex - 1][1] - 1][2] = 0;
			}
			else {
				list_out[list_in[vertex - 1][1] - 1][2] = 0;
			}
		}
		else {
			if (list_in[vertex - 1][0] == 2)
			{
				list_out[list_in[vertex - 1][1] - 1][0]--;
				if (list_out[list_in[vertex - 1][1] - 1][1] == vertex)
				{
					list_out[list_in[vertex - 1][1] - 1][1] = list_out[list_in[vertex - 1][1] - 1][2];
					list_out[list_in[vertex - 1][1] - 1][2] = 0;
				}
				else {
					list_out[list_in[vertex - 1][1] - 1][2] = 0;
				}

				list_out[list_in[vertex - 1][2] - 1][0]--;
				if (list_out[list_in[vertex - 1][2] - 1][1] == vertex)
				{
					list_out[list_in[vertex - 1][2] - 1][1] = list_out[list_in[vertex - 1][2] - 1][2];
					list_out[list_in[vertex - 1][2] - 1][2] = 0;
				}
				else {
					list_out[list_in[vertex - 1][2] - 1][2] = 0;
				}
			}
		}

		//          System.out.println();
		//           for (int i = 0; i < n; ++i) {
		//           System.out.print((i + 1) + " ");
		//           for (int j = 0; j < 4; ++j) {
		//               System.out.print(list_out[i][j] + " ");
		//           }
		//           System.out.println();
		//          }
		//         System.out.println(vertex);
		//�������� ��������� �������
		//vertex=-1;
		if (list_out[vertex - 1][0] == 0)
		{
			for (int i = 0; i < n; ++i)
			{
				if (list_out[i][3] == 0)
				{
					vertex = i + 1;
					break;
				}
			}

			//               vertex = list_out[vertex - 1][1];
		}
		else {
			if (list_out[vertex - 1][0] == 1)
			{
				vertex = list_out[vertex - 1][1];
			}
			else {
				if (list_out[vertex - 1][0] == 2)
				{

					if (list_in[list_out[vertex - 1][1] - 1][0] == 0 || list_in[list_out[vertex - 1][2] - 1][0] == 0)
					{
						if (list_in[list_out[vertex - 1][1] - 1][0] == 0)
						{
							vertex = list_out[vertex - 1][1];
						}
						else {
							vertex = list_out[vertex - 1][2];
						}
					}
					else {

						if (list_out[list_out[vertex - 1][1] - 1][0] == list_out[list_out[vertex - 1][2] - 1][0])
						{
							if (( (double)(rand() / RAND_MAX) ) <= 0.5)
							{
								vertex = list_out[vertex - 1][1];
							}
							else {
								vertex = list_out[vertex - 1][2];
							}

						}
						else {
							if (list_out[list_out[vertex - 1][1] - 1][0] < list_out[list_out[vertex - 1][2] - 1][0] || list_out[list_out[vertex - 1][2] - 1][0] == 0)
							{
								vertex = list_out[vertex - 1][1];
							}
							else {
								vertex = list_out[vertex - 1][2];
							}
						}
					}
					//����� if (list_out[vertex - 1][0] == 2)
				}
			}
		}
		child[vertexN] = vertex;
		//           System.out.println(vertex);

	}
	//        System.out.println();
	//           for (int i = 0; i < n; ++i) {
	//           System.out.print((i + 1) + " ");
	//           for (int j = 0; j < 3; ++j) {
	//               System.out.print(list_out[i][j] + " ");
	//           }
	//           System.out.println();
	//       }
	//       System.out.println();
	//       for (int i = 0; i < n; ++i) {
	//           System.out.print((i + 1) + " ");
	//           for (int j = 0; j < 3; ++j) {
	//               System.out.print(list_in[i][j] + " ");
	//           }
	//           System.out.println();
	//       }
	//       System.out.println();
	//       System.out.println();
	//       for (int i = 0; i < n; ++i) {
	//           
	//               System.out.print(child[i] + " ");
	//          
	//       }
	//       System.out.println();
	//       System.out.println(this.phitnessTSP(s, p1));
	//       System.out.println(this.phitnessTSP(s, p2));
	//       System.out.println(this.phitnessTSP(s, child));
	this->phi_best_in_rec = this->phitness(s, child);
	this->number_solution_in_rec = 1;
	return child;
}


////////////////////////////////////////////////////////////////////////////////
// << julia: �������� ������������� 
////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::DPX(vector< vector <vector<int> > > s, vector<int> p1, vector<int> p2)
{
	vector<vector<int>> list_out(n, vector<int>(3));//��������� �������
	vector<vector<int>>  parts(n, vector<int>(3));//����� � ������ ������
// �������� 2 �� m?	
	vector<vector<int>> arc_weights(n, vector<int>(2));//���� ���
	vector<int> block_number(n);//�������
	vector<int> child(n);//�������
// ���������� flag?
	vector<bool> flag;

	int N_parts, i_parts;//����� ������
	int i1, i_temp, i_count, block;

	// ������������ ���������� ��������� �����
	//unsigned rand_value = 11;
	//srand(rand_value);

	////////////////////////////////////////////////////////////////////////////////   
	/////!��������� ������!/////
	//++++++++++++++++++++++++++
	for (int i = 0; i < p1.size() - 1; ++i)
	{
		//++++++++++++++++++++++++++
		//++++++++++++++++++++++++++
		//������
		//��������� ������� ��� p1[i]
		if (list_out[p1[i] - 1][0] == 0)
		{
			list_out[p1[i] - 1][0] = 1;
			list_out[p1[i] - 1][1] = p1[i + 1];
		}
		if (list_out[p1[i] - 1][0] == 1 && list_out[p1[i] - 1][1] != p1[i + 1])
		{
			list_out[p1[i] - 1][0] = 2;
			list_out[p1[i] - 1][2] = p1[i + 1];
		}


		//��������� ������� ��� p2[i]
		if (list_out[p2[i] - 1][0] == 0)
		{
			list_out[p2[i] - 1][0] = 1;
			list_out[p2[i] - 1][1] = p2[i + 1];
		}
		if (list_out[p2[i] - 1][0] == 1 && list_out[p2[i] - 1][1] != p2[i + 1])
		{
			list_out[p2[i] - 1][0] = 2;
			list_out[p2[i] - 1][2] = p2[i + 1];
		}

	}

	//��������� ������� ��� p1[n-1]
	if (list_out[p1[p1.size() - 1] - 1][0] == 0)
	{
		list_out[p1[p1.size() - 1] - 1][0] = 1;
		list_out[p1[p1.size() - 1] - 1][1] = p1[0];
	}
	if (list_out[p1[p1.size() - 1] - 1][0] == 1 && list_out[p1[p1.size() - 1] - 1][1] != p1[0])
	{
		list_out[p1[p1.size() - 1] - 1][0] = 2;
		list_out[p1[p1.size() - 1] - 1][2] = p1[0];
	}

	//��������� ������� ��� p2[n-1]
	if (list_out[p2[p2.size() - 1] - 1][0] == 0)
	{
		list_out[p2[p2.size() - 1] - 1][0] = 1;
		list_out[p2[p2.size() - 1] - 1][1] = p2[0];
	}
	if (list_out[p2[p2.size() - 1] - 1][0] == 1 && list_out[p2[p2.size() - 1] - 1][1] != p2[0])
	{
		list_out[p2[p2.size() - 1] - 1][0] = 2;
		list_out[p2[p2.size() - 1] - 1][2] = p2[0];
	}

	//++++++++++++++++++++++++++
	//++++++++++++++++++++++++++
	/////!��������� ������!///// 
	////////////////////////////////////////////////////////////////////////////////

	//��������� ����� ���, ����� ��� ���������
	N_parts = 0;
	parts[0][0] = p1[0];
	parts[0][1] = -1;

	for (int i = 0; i < p1.size() - 1; ++i)
	{
		if (list_out[p1[i] - 1][0] == 2)
		{
			parts[N_parts][1] = p1[i];
			parts[N_parts + 1][0] = p1[i + 1];
			N_parts++;
		}
	}

	if (list_out[p1[p1.size() - 2] - 1][0] == 2)
	{
		if (list_out[p1[p1.size() - 1] - 1][0] == 2)
		{
			parts[N_parts][0] = p1[p1.size() - 1];
			parts[N_parts][1] = p1[p1.size() - 1];
			N_parts++;
		}
		else {
			parts[0][0] = p1[p1.size() - 1];
			parts[N_parts + 1][0] = 0;
		}
	}
	else {
		if (list_out[p1[p1.size() - 1] - 1][0] == 2)
		{
			parts[N_parts][1] = p1[p1.size() - 1];
			N_parts++;
		}
		else {
			if (N_parts == 0)
			{
				N_parts++;
				parts[0][1] = p1[p1.size() - 1];
			}
			else {
				parts[0][0] = parts[N_parts][0];
				parts[N_parts][0] = 0;
			}
		}
	}

	//�������� ������� �������
	i_count = 0;
	i_temp = 1;

	while (i_temp != parts[0][1])
	{
		child[i_count] = i_temp;
		i_temp = list_out[i_temp - 1][1];
		i_count++;
	}
	child[i_count] = i_temp;
	i_count++;

	//������� ������
	for (int t = 1; t < N_parts; ++t)
	{
		//for (int i = 0; i < N_parts; ++i)
		//{
		//	System.out.println(parts[i][0] + " " + parts[i][1] + " " + parts[i][2] + "; " + i);
		//}

		i_parts = 0; //����� ��������� ������ ��� ������� �����
		for (int i = 1; i < N_parts; ++i)
		{
			if (parts[i][2] == 0 && list_out[parts[0][1] - 1][1] != parts[i][0] && list_out[parts[0][1] - 1][2] != parts[i][0])
			{
				arc_weights[i_parts][0] = s[0][parts[0][1] - 1][parts[i][0] - 1];
				arc_weights[i_parts][1] = s[1][parts[0][1] - 1][parts[i][0] - 1];
				block_number[i_parts] = i;
				i_parts++;
			}
		}
		if (i_parts == 0)
		{ //���� ��������� ������ ���
			i_parts = 0;
			for (int i = 1; i < N_parts; ++i)
			{
				if (parts[i][2] == 0)
				{
					arc_weights[i_parts][0] = s[0][parts[0][1] - 1][parts[i][0] - 1];
					arc_weights[i_parts][1] = s[1][parts[0][1] - 1][parts[i][0] - 1];
					block_number[i_parts] = i;
					i_parts++;
				}
			}
		} //if(i_parts==0) � ���������� i_parts == (1 or 2)

		flag = flag_Pareto_sol(i_parts, arc_weights);
		i1 = 0;
		for (int i = 0; i < i_parts; ++i)
		{
			if (flag[i] == false)
				i1++;
			//                 System.out.println(arcWeights[i][0] + " " + arcWeights[i][1] + "; " + blockNumber[i] + "; " + flag[i]);
		}
		//             System.out.println("i1: " + i1);
		i_temp = rand() % i1 + 1;
		//             System.out.println("i_temp: " + i_temp);
		i1 = 0;
		block = -1;
		for (int i = 0; i < i_parts; ++i)
		{
			if (flag[i] == false)
				i1++;

			if (i1 == i_temp)
			{
				block = block_number[i];
				break;
			}
		}
		//             System.out.println("block: " + block);

		parts[block][2] = -1;
		parts[0][1] = parts[block][1];
		i_temp = parts[block][0];

		while (i_temp != parts[block][1])
		{
			child[i_count] = i_temp;
			i_temp = list_out[i_temp - 1][1];
			i_count++;
		}

		child[i_count] = i_temp;
		i_count++;
	}//������� ������

	if (parts[0][0] != 1)
	{
		i_temp = parts[0][0];
		while (i_temp != 1)
		{
			child[i_count] = i_temp;
			i_temp = list_out[i_temp - 1][1];
			i_count++;
		}
	}
	//         for (int i = 0; i < n; ++i) {
	//            System.out.print(child[i]+" ");
	//        }
	//        System.out.println();
	//        i1=-1;
	//         for (int i = 0; i < n; ++i) {
	//            if(child[i]==1){i1=i;break;}
	//        }
	//        icount=0;
	//         for (int i = i1; i < n; ++i) {
	//          p1[icount]=child[i];icount++;  
	//        }
	//        for (int i = 0; i < i1; ++i) {
	//          p1[icount]=child[i];icount++;  
	//        }

	return child;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// << julia: ������������ � �������� DEC_new � DCX
////////////////////////////////////////////////////////////////////////////////
vector<bool> GA_path::flag_Pareto_sol(int k, vector<vector<int>> s)
{
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
	// ���������� flag?
	vector<bool> flag_S(k);

	if (k == 1)
	{
		flag_S[0] = false;
	}
	else {
		bool flag; //int dif;
		int jprev, iprev;
		int i = 0;
		int j = 1;

		while (i != -1 && j != -1)
		{ //������ ����� ������
			if ((s[i][0] <= s[j][0]) && (s[i][1] <= s[j][1]))
			{
				flag = true;
			}
			else {
				flag = false;
			}

			if (flag == true)
			{
				//������������� ���� 
				//��� ������� � ����������� ���������� �� ���� ��������� ��������� ���� ���������

				flag_S[j] = true; //������� j
				jprev = j;
				j = next(j, k, flag_S); //��������� � ���������� j

				if (jprev == j)
				{
					iprev = i;
					i = next(i, k, flag_S); //��������� � ���������� i
					if (iprev == i)
					{
						i = -1;
					}
					else {
						jprev = j = i;
						j = next(i, k, flag_S); //��������� � ���������� j
						if (jprev == j)
						{
							j = -1;
						}
					}
				}
			}
			else {
				//������������� ���

				//���� �� ������������� j > i?
				flag = true;
				if ((s[j][0] <= s[i][0]) && (s[j][1] <= s[i][1]))
				{
					flag = true;
				}
				else {
					flag = false;
				}

				if (flag == true) {
					//������������� ����
					flag_S[i] = true; //������� i
					iprev = i;
					i = next(i, k, flag_S); //��������� � ���������� i
					if (iprev == i)
					{
						i = -1;
					}
					else {
						jprev = j = i;
						j = next(i, k, flag_S); //��������� � ���������� j
						if (jprev == j)
						{
							j = -1;
						}
					}
				}
				else {
					jprev = j;
					j = next(j, k, flag_S); //��������� � ���������� j
					if (jprev == j)
					{
						iprev = i;
						i = next(i, k, flag_S); //��������� � ���������� i
						if (iprev == i)
						{
							i = -1;
						}
						else {
							jprev = j = i;
							j = next(i, k, flag_S); //��������� � ���������� j
							if (jprev == j)
							{
								j = -1;
							}
						}
					}
				} //else
			} //else
		} //while
	} // ������� else

	return flag_S;

}

//����� ���������� ��������� ��������
int GA_path::next(int j, int k, vector<bool> flag_S)
{
	int j_next = j;
	for (int i = j + 1; i < k; ++i)
	{
		if (flag_S[i] == false)
		{
			j_next = i;
			break;
		}
	}
	return j_next;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//���������� ����� ���������� ������� + ������� ������
////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::patching_algorithm(vector<vector<int>> s, int c_max, boolean orig_or_mod)
{
	vector<int> assignment;
	
	//������ ������������� �� ���������
	for (int i = 0; i < n; ++i)
		s[i][i] = (c_max + 1) * n;
	
	//***����� �� ����� ������� ����������
	//        interMatrix(n,n,c);
	
	//������� ������ � �����������
	this->is_AP_solution_opt = false;
	assignment = Hungarian_method(n, n, s);
	//        interPermutation(n, assignment);
	//***��������� ���������� ����������
	//        System.out.println("CostAssignment= "+costAssignment(n, c, assignment));

	////////////////////////////////////////////////////////////////////////
	//���� �����///////////////////////////////////////////////////////////// 
	int count_cycles = 0;//����� ������
	int count_vertex_in_current_cycle = 0;//����� ������ � ������� �����
	vector<int> head_cycles(n);//������ �����
	vector<int> length_cycles(n);//����� ������ � �����
	vector<int> cost_cycles(n);//����� ����� 
	vector<int> cycle_vertex(n);//�������������� ������ ������ (for  modif alg)
	//vector<bool> view(n,false);//��������� ���� ����������� �� ������� ��� ������ ������
	bool* view = new bool[n];
	for (int i = 0; i < n; i++)
		view[i] = false;
	bool temp;
	int i_head = 1, i_tail, i_headM, i_tailM, i_max, i_temp;// ������ � ����� �������� ����� i_head=1, i_tail
															////////////////////////////////////////////////////////////////////////
															//��������� ������ ������ � ������������////////////////////////////////
	while (i_head != -1)
	{
		//���� ������ ���������� �����
		i_head = i_tail = -1;
		for (int i = 0; i < n; ++i)
		{
			if (!view[i])
			{
				i_head = i;
				i_tail = assignment[i];
				cycle_vertex[i] = count_cycles;//����� ����� ��� ������� (for  modif alg)
				view[i] = true;
				head_cycles[count_cycles] = i_head;
				break;
			}
		}
		//����� ����� ����
		if (i_head != -1)
		{
			count_vertex_in_current_cycle = 1;
			cost_cycles[count_cycles] = s[i_head][assignment[i_head]];
			while (i_head != i_tail)
			{
				view[i_tail] = true;
				cycle_vertex[i_tail] = count_cycles;//����� ����� ��� ������� (for  modif alg)
				cost_cycles[count_cycles] += s[i_tail][assignment[i_tail]];
				i_tail = assignment[i_tail];
				count_vertex_in_current_cycle++;
			}
			length_cycles[count_cycles] = count_vertex_in_current_cycle;
			count_cycles++;
		}
	}
	//***����� ���������� � ������   
	printf("Number of cycles: %d\n", count_cycles);
	//        interPermutation(countCycles, headCycles);
	//        interPermutation(countCycles, lengthCycles);
	//        interPermutation(countCycles, costCycles);
	//        interPermutation(n, CycleVertex);//����� ������ ��� ����������������� ������
	//��������� ������ ������ � ������������////////////////////////////////
	////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////
	//��������� �����///////////////////////////////////////////////////////
	if (count_cycles > 1)
	{
		//���� ���� � ������������ �����!
		i_max = -1;//����� ������ �������� �����, ��������� � 0
		i_tail = -1;//����� ������ �������� ����� (����� ���� ���)
		for (int i = 0; i < count_cycles; ++i)
		{
			if (cost_cycles[i] > i_tail)
			{
				i_tail = cost_cycles[i];
				i_max = i;
			}
		}
		if (length_cycles[i_max] < count_cycles - 1) //�� ������� ������ � �����
		{
			//�������� ����� ����� ������� ���� �� ����� ������
			i_max = -1;//����� ������ �������� �����, ��������� � 0
			i_tail = 0;//����� ������ �������� ����� (����� ������)
			for (int i = 0; i < count_cycles; ++i)
			{
				if (length_cycles[i] > i_tail)
				{
					i_tail = length_cycles[i];
					i_max = i;
				}
			}
		}
		//***����� �� �����
		//            System.out.println("����� ������ �������� ����� "+i_max);
		if ( (length_cycles[i_max] < count_cycles - 1) || !orig_or_mod)
		{
			////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////
			//modified patching process
			// lengthCycles[i_max]<countCycles-1
			//������� ��� ������� ������������
			for (int i = 0; i < n; ++i)
				view[i] = false;

			int i_min;
			//���������� ��������� �����
			for (int iter = 0; iter<count_cycles - 1; ++iter)
			{
				i_min = 0;//����� �������� ���� 
				i_tail = cost_cycles[0];//����� ������ ��������� ����� 
				for (int i = 1; i < count_cycles; ++i)
				{
					if (cost_cycles[i] < i_tail)
					{
						i_tail = cost_cycles[i];
						i_min = i;
					}
				}
				
				//***����� �� �����
				//               System.out.println("�������� ���� "+i_min);
				i_tail = (c_max + 1)*n + 1;//����� ������� ��������� ����������
				i_head = i_headM = -1;//������� ��� ������
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
									i_tail = i_tailM;//��������� ������
								}
								//                System.out.println(i+" "+j+" "+i_tail+" "+i_tailM);                

							}
						}
					}
				}

				//��������� ����� � ���������  i_head � i_headM
				//                System.out.println("������� ������ ("+i_head+", "+i_headM+") ="+i_tail);
				view[i_head] = view[i_headM] = true;//�������� ��������� �������
				i_max = cycle_vertex[i_headM];
				cost_cycles[i_min] += cost_cycles[i_max] + i_tail;//���������� ��������� ����� i_min
				cost_cycles[i_max] = (c_max + 1)*n + 1;//��������� ���� i_max �� ������������
				
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
			//***����� �� ����� ������� ������ ������������       
			//        interPermutation(n, assignment);
			//        System.out.println("CostAssignment= "+costAssignment(n, c, assignment));

			
		}

		else
		{
			//original patching process    
			////////////////////////////////////////////////////////////////////
			//��������� ������� ������//////////////////////////////////////////

			//��������� ������� ��� ��������������� ������ � �����������
			// ������ ������������ ��������� ������, � ������� - �������� �������� �����
			// i_head - ����� ������ �������� �����, ��������� � ����   
			vector<vector<int>> c_temp(count_cycles - 1, vector<int>(length_cycles[i_max]));
			vector<int> column(length_cycles[i_max]);//�������� �������� �����
			vector<vector<int>> row(count_cycles - 1, vector<int>(length_cycles[i_max]));//��������� �������� ���������� �����
			i_headM = head_cycles[i_max];//������� ������� �������� �����
			
			for (int j = 0; j < length_cycles[i_max]; ++j)
			{
				column[j] = i_headM;//�������� ��������������� ��������;
				i_temp = 0;//���������� ����� ���������� ���������� �����, ��������� � 0
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
			//***����� �� ����� ��������������� �������
			//              interMatrix(lengthCycles[i_max],countCycles-1,c_temp);
			//              interPermutation(lengthCycles[i_max], column);
			//              interMatrix(lengthCycles[i_max],countCycles-1,row);
			
			//��������������� ������� ������, ������ ������ � �����������
			head_cycles = Hungarian_method(length_cycles[i_max], count_cycles - 1, c_temp);
			
			//����� �� ����� ������� ��������������� ������ � �����������
			//              interPermutation(countCycles-1, headCycles);
			
			//��������� �����
			for (int i = 0; i < count_cycles - 1; ++i)
			{
				i_head = assignment[row[i][head_cycles[i]]];
				assignment[row[i][head_cycles[i]]] = assignment[column[head_cycles[i]]];
				assignment[column[head_cycles[i]]] = i_head;
			}
			//***����� �� ����� ������� ������ ������������
			//              interPermutation(n, assignment);
			//              System.out.println("CostAssignment= "+costAssignment(n, c, assignment));
			
			//��������� ������� ������//////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////// 
		}
	}
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	else {
		//countCycles=1
		this->is_AP_solution_opt = true;
		////
		////            System.out.println("����������� ������� ������ � ����������� ����"
		////                    + " ����������� ������������ - ���� ������������ ������!");
	}

	////////////////////////////////////////////////////////////////////////
	//���������� ���� �� ���������
	for (int i = 0; i < n; ++i)
		s[i][i] = 0;

	//////////////////////////////////////////////////////////////////////// 
	return assignment;
}

////////////////////////////////////////////////////////////////////////////////
//���������� �����
////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::Hungarian_method(int n, int m, vector<vector<int>> cost)
{
	//m - ����� �����, n - ����� �������� (m<=n), cost[i][j] - ��������� ���������� j �� ��������� i
	//�������� ������////////////////////////////////////////////////
	vector<vector<int>> c(n, vector<int>(n));//��������� ����������
	vector<int> result(m);
	
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < n; ++j)
			if (i < m)
				c[i][j] = cost[i][j];
			else
				c[i][j] = 0;

	// interMatrix(n,n,c);
	//���������� ��� ���������
	vector<int> zero(n);//��� �������� ����� ����� � �������
	vector<int> column(n);//��� ������� ����� � ��������
	vector<int> row(n);
	bool* row_temp = new bool[n];
	bool* column_temp = new bool[n];
	bool to_go;
	bool to_go_temp;
	int c_min, c_max, i_min, j_zero;
	
	/////////////////////////////////////////////////////////////
	//��������////////////////////////////////////////////////////
	//��� 1. ��������� ����������� ��������� �� ����� � ��������
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
	//������ ���������//
	while (true)
	{
		//////////////////////////////////////////////////////////////////////////     
		//��� 2. ������������� �������
		//������� ����� ����� � ������ ������ � ������������ ������� �������
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
		//����� ����� � ����������� ������ �����
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
	
			//����� ������ � ������ i_min (����������� � ����������� ����)
			if (c_min != n + 1)
			{
				/////////////////////////////
				zero[i_min] = n + 1;//�������� ������ ��� �������������
									//������ ���� j_min � ������ ����������� - 0
				j_zero = -2;
				for (int j = 0; j < n; ++j)
				{
					if (c[i_min][j] == 0)
					{
						j_zero = j;
						break;
					}
				}
				//��������� ���� � ������ ����������� - c_max+1
				for (int j = j_zero + 1; j < n; ++j)
					if (c[i_min][j] == 0)
						c[i_min][j] = c_max + 1;
				
				//��������� ���� � �������  j_zero  ����������� - c_max+1
				for (int i = 0; i < n; ++i)
					if ( (c[i][j_zero] == 0) && (i != i_min) )
					{
						c[i][j_zero] = c_max + 1;
						zero[i]--;
					}
			
				////////////////////////////
			}//����� ������ � ������ i_min
			else {
				break;
			}
		}
		//��� ������ � ������ zero[i]=n+1 �������� ����������� ���� 
		///////////////////////////////////////////////////////////////////////// 

		//��� 3. ����� ������������ ����� ������, ���������� ����� ��� ���� (����������� � �����������) 
		to_go = false;
		to_go_temp = false;
		for (int i = 0; i < n; ++i)
		{
			column[i] = -1;
			row[i] = -1;
			row_temp[i] = column_temp[i] = false;
		}

		for (int i = 0; i < n; ++i)
			// ������, ��� ��� ���������� ��������� �������� ����� 0
			if (zero[i] != n + 1)
			{
				row[i] = 0;
				to_go = true;
				break;
			}


		if (!to_go)
		{
			//���������� ������� ������
			//            System.out.println("������ ������!"); 
			//cost_opt=0;//�������� ������� �������
			for (int i = 0; i < m; ++i)
				for (int j = 0; j < n; ++j)
					if (c[i][j] == 0)
						result[i] = j;
			break;
		}
		///////////////////////////////////////////////////////////////////////// 
		while (to_go) // ���� to_go=false, �� �������� �������
		{
			to_go = false;
			//�������� ����� �������
			for (int i = 0; i < n; ++i)
				if ( (row[i] >= 0) && (!row_temp[i]) )
				{
					for (int j = 0; j < n; ++j)
						if ( (c[i][j] == c_max + 1) && (!column_temp[j]) )
						{
							column[j] = (i + 1);
							to_go = true;
						}
					row_temp[i] = true;//������ �����������
				}
			
			if (to_go) //���� ��������� ����� �������
			{
				to_go = false;
				//�������� ����� ������
				for (int j = 0; j < n; ++j)
				{
					if ( (column[j]>0) && (!column_temp[j]) )
					{
						i_min = 0;
						for (int i = 0; i < n; ++i)
							if ( (c[i][j] == 0) && (!row_temp[i]))
							{
								i_min++;//� ������� ���� ����������� ����
								row[i] = (j + 1);
								to_go = true;
							}
						column_temp[j] = true;//������� ����������
											 /////////////////////////////////////////////////////////////
											 //������������� ����///////////////////////////////////////
											 /////////////////////////////////////////////////////////////

						if (i_min == 0) //� ������� j ��� ����������� ����� - ������� ������������� ����
						{				//��������  ����������� ���� ������������ � �������� ����� ����
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

							//������� ������ ����� 
							/////////////////// ///////////////////////////////////////////////////////////
							to_go = false;
							for (int i = 0; i < n; ++i)
							{
								column[i] = -1;
								row[i] = -1;
								row_temp[i] = column_temp[i] = false;
							}

							for (int i = 0; i < n; ++i)
								// ������, ��� ��� ���������� ��������� �������� ����� 0
								if (zero[i] != n + 1)
								{
									row[i] = 0;
									to_go = true;
									break;
								}

							if (!to_go) 
							{
								//���������� ������� ������
								//                                System.out.println("������ ������ ������!");
								//cost_opt = 0;//�������� ������� �������
								for (int i = 0; i < m; ++i)
									for (int j1 = 0; j1 < n; ++j1)
										if (c[i][j1] == 0)
											result[i] = j1;
								to_go_temp = true;
							}
							////////////////////////////////////////////////////////////////////////
							break;//������������� ���� �� ��������
						}
						/////////////////////////////////////////////////////////////
						//������������� ����///////////////////////////////////////
						/////////////////////////////////////////////////////////////
					}
				}
			}//if (to_go) {//���� ��������� ����� �������
		}// while (to_go) {
		 
		// ��� ������������ ������ ������ row[i]=-1; ������ ������� column[j]>0
		///////////////////////////////////////////////////////////////////////// 
		if (to_go_temp) //���� ������ ������ ��� ������ ������������� ����
			break; 
		
		//����� ��� �������� �� �������������
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
		//���������� ��� ������� � ����������� ��������
		for (int j = 0; j < n; ++j)
			if (column[j] > 0)
				for (int i = 0; i < n; ++i)
					c[i][j] = c[i][j] + c_min;
		
		//�������� ��� ������� �� ������������� �����
		for (int i = 0; i < n; ++i)
			if (row[i] >= 0)
				for (int j = 0; j < n; ++j)
					c[i][j] = c[i][j] - c_min;
		
		////////////////////////////////////////////  
		//////////////////////////////////////////////////////////////////////////
		//����� ���������//
	}
	return result;
	//////////////////////////////////////////////////////////////////////////   
}


////////////////////////////////////////////////////////////////////////////////
//���������� ������������� ��-�� ������ (�������� ���������� ��������, ��� ��������)
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
//���������� �� ����� (�� �������������) �� ���������� ������� � ��-�� ������
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
//���������� ������� - ���������� ������� ��-�� �� ������� ��-��
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
//C������������������� ���������� ������� "���������� ������������� � ��-�� ������"
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
//���������� ������� - ������������� ��������� ������
////////////////////////////////////////////////////////////////////////////////
void GA_path::evaluate_metric_of_approx(StreamWriter^ sw, vector<vector<int>> phi_Pareto_set, bool flag_count)
{
	//������� - ���������� ������������ � ��-�� ������
	this->dist_conver_approx_to_P_set_val = this->dist_convergence(this->phi_P_approx, phi_Pareto_set, flag_count);

	
	printf("Metric 'Convergence appoximation to Pareto set':\n");
	//����� ������ �������
	sw->Write("{0:F4};", dist_conver_approx_to_P_set_val);
	//sw_short->WriteLine("{0:F4}", dist_conver_approx_to_P_set_val);
	printf("%.4f\n", dist_conver_approx_to_P_set_val);

	//������� - ���������� ��-�� ������ � ��� ������������
	this->dist_conver_P_set_to_approx_val = this->dist_convergence(phi_Pareto_set, this->phi_P_approx, false);

	
	printf("Metric 'Convergence Pareto set to appoximation':\n");
	//����� ������ ������� � ���-�� ����� � ��-�� ������
	sw->Write("{0:F4};{1};", dist_conver_P_set_to_approx_val, this->phi_P_approx.size());
	//sw_short->WriteLine("{0:F4}", dist_conver_P_set_to_approx_val);
	printf("%.4f\n", dist_conver_P_set_to_approx_val);

	//sw->WriteLine();
	//sw_short->WriteLine();
}

////////////////////////////////////////////////////////////////////////////////
//����������� �� ������� ��-�� ������
////////////////////////////////////////////////////////////////////////////////
vector<double> GA_path::experiment_reduction(StreamWriter^ sw, String^ problem_name_str,
								double h, int num_steps, double teta_start, bool is_1st_to_2nd)
{
	//����������� ������������� ��������
	double teta = teta_start;
	
	vector<vector<double>> new_phi_P_approx;
	vector<vector<double>> phi_P_approx_reduced;
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

	//1-�� �������� ������ 2-��
	for (int i = 1; i <= num_steps; i++)
	{
		teta += h;
		//������������� ������� � ����� ���������� �������� (�������� ������������, �.�. teta ������������)
		new_phi_P_approx = this->build_new_phi_P_approx(this->phi_P_approx, teta, is_1st_to_2nd);
		//������� ��-�� ������ � ����� ����������������� ��-��
		phi_P_approx_reduced = this->build_phi_P_enum(new_phi_P_approx);

		sw->WriteLine("teta; {0:F1}", teta);

		
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

		//������� ������� ����������� �����
		index_reduced_temp.push_back( ((double)(this->phi_P_approx.size() - phi_P_approx_reduced.size()) / this->phi_P_approx.size()) * 100 );
		//sw_short->Write("{0:F2};", reduced_index);

		//aver_reduced_index[i - 1] += reduced_index;

		//if (min_reduced_index[i - 1] > reduced_index)
		//	min_reduced_index[i - 1] = reduced_index;
		//if (max_reduced_index[i - 1] < reduced_index)
		//	max_reduced_index[i - 1] = reduced_index;
	}

	return index_reduced_temp;
}


////////////////////////////////////////////////////////////////////////////////
//������������ ��-�� ������ (��� ��������) �������������� "������" ��������
////////////////////////////////////////////////////////////////////////////////
vector<vector<double>> GA_path::build_new_phi_P_approx(vector<vector<int>> init_phi_P_approx, double teta, bool is_1st_to_2nd)
{
	vector<vector<double>> new_phi_temp(init_phi_P_approx.size(), vector<double>(this->get_m(), 0));

	for (int i = 0; i < init_phi_P_approx.size(); i++)
	{
		if (is_1st_to_2nd)
		{
			new_phi_temp[i][0] = init_phi_P_approx[i][0];
			new_phi_temp[i][1] = teta*init_phi_P_approx[i][0] + (1 - teta)*init_phi_P_approx[i][1];
		}
		else
		{
			new_phi_temp[i][0] = (1-teta)*init_phi_P_approx[i][0] + teta*init_phi_P_approx[i][1];
			new_phi_temp[i][1] = init_phi_P_approx[i][1];
		}
	}
	return new_phi_temp;
}


////////////////////////////////////////////////////////////////////////////////
//���������� ��-�� ������ ���������
////////////////////////////////////////////////////////////////////////////////
vector<vector<double>> GA_path::build_phi_P_enum(vector<vector<double>> init_set)
{
	vector<vector<double>> res_set;

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

				if (this->Pareto_pref(init_set[i], init_set[j]))
					flag_s[j] = true;
				if (this->Pareto_pref(init_set[j], init_set[i]))
				{
					flag_s[i] = true;
					break;
				}
			}

	}
	
	for (int i = 0; i < init_set.size(); i++)
	{
		if (!flag_s[i])
			res_set.push_back(init_set[i]);
	}

	return res_set;
}

////////////////////////////////////////////////////////////////////////////////
//������ ��������� ������ �� �����
////////////////////////////////////////////////////////////////////////////////
vector<vector<int>> GA_path::read_Pareto_set_from_file(String^ file_name_source_str, String^ problem_name_str)
{
	//������ ����������
	vector<vector<int>> vector_dest;

	StreamReader^ sr = gcnew StreamReader(file_name_source_str);

	String^ problem_name_csv_str = problem_name_str + ";";
	String^ cur_line_str = sr->ReadLine();
	while (cur_line_str != problem_name_csv_str)
		cur_line_str = sr->ReadLine();

	String^ Pareto_set_name_csv_str = "Pareto Set;";
	cur_line_str = sr->ReadLine();
	while (cur_line_str != Pareto_set_name_csv_str)
		cur_line_str = sr->ReadLine();

	//���������� ��������� ������ (������ vector_dest)
	vector<vector<int>> vector_dest_temp;
	int i = 0; //������ ������
	vector<int> vec_temp(2);
	string str_temp = "";

	while (true) 
	{
		cur_line_str = sr->ReadLine();
		if (cur_line_str[0] == 'N')
			break;

		int j = 0; //������ �������
		//��������� ������� ������
		for (int k = 0; k < cur_line_str->Length; k++)
		{
			if (cur_line_str[k] == ';')
			{
				vec_temp[j] = stoi(str_temp);
				//sw->Write("{0};", s_temp[i][j]);
				//printf("%d \t", s_temp[i][j]);

				j++;
				str_temp = "";

				if (j == this->get_m())
				{
					vector_dest.push_back(vec_temp);
					break;
				}
			}
			else
				str_temp += cur_line_str[k];
		}
		i++;
	}
	sr->Close();

	return vector_dest;
}


void time_format(unsigned long long result_time, String^ title, StreamWriter^ sw)
{
	int res_sec = (result_time / 1000) % 60;
	int res_min = (result_time / (60 * 1000)) % 60;
	int res_hour = result_time / (3600 * 1000);
	sw->WriteLine(title + ": ; {0}; h; {1}; min; {2}; sec", res_hour, res_min, res_sec);
	printf("Time:\n %d:%d:%d\n", res_hour, res_min, res_sec);
}

////////////////////////////////////////////////////////////////////////////////
//����� ���������� ������ - �� �����
////////////////////////////////////////////////////////////////////////////////
/*
vector<int> GA_path::local_search_fast(vector<int> assignment, vector<vector<int>> c,
								vector<vector<int>> vertex, vector<int> vertex_initial, int alpha)
{
	//alpha ����� ��������������� ���������� ��� i_next
	//         System.out.println(Nchange);
	int G = 0, F = 0, F_max = -1;
	vector<int> result(n);
	vector<vector<int>> path(n);
	vector<bool> tabu(n);//����� ��� ��������� ������ 

									//            Random rnd = new Random();//������ ��������� �����
	int i_head = -1, j_head = -1, i_next = -1, i_temp = -1, i_tail = -1, j_tail = -1, count = 0;
	bool isImprove = true;
	//������� ������� ������
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


	//        while (isImprove) {//���� ���� ��������� - �������� ������!
	//          i_head=rnd.nextInt(n);//��������� ������� ���������� ��������� ������� (i_head - ��� ����� ����)
	isImproveLS = false;//��� �������� ���� �� ��������� � ��������� ������
	while (QueueVertex.peek() != null) {//���������� ��� �������� ������� �� ������ �������
		i_head = QueueVertex.poll();//���������� � ��������� ������� �� ������ �������
									//            for (int i_head = 0; i_head < n; i_head++) {
		isImprove = false;
		j_head = path[1][i_head]; //(j_head - ��� ������ ����) 
		path[1][i_head] = -1;//������� ����
		path[0][j_head] = -1;
		//��������� ���� ��� ����������� ����� 
		for (int i_t = 0; i_t < alpha; ++i_t) {
			i_next = vertex[i_head][i_t];
			if (i_head != i_next && j_head != i_next) {
				if (c[i_head][j_head]>c[i_head][i_next]) {
					G = 0;//��������� �������
						  //��������� ���� � ��� ����� ������� ����
						  //��������� (i_head, i_next)
					path[1][i_head] = i_next;
					i_temp = path[0][i_next];//���� ��� �������� ����������� ����������
					path[0][i_next] = i_head;
					G = G + c[i_head][j_head] - c[i_head][i_next];//��������� �������
																  //������� (i_temp, i_next)
					path[1][i_temp] = -1;
					//��������� ����
					//�������� ������� �� ����� j_tail
					count = 0;
					F_max = -1;
					j_tail = -1;
					i_tail = path[1][i_next];//������������� ������� �����, i_tail - ��������
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
					if (count == 0) {//� ������ ����������� ��� ��������� (���������� ����� �� �����)
						path[1][i_head] = -1;
						path[0][j_head] = -1;
						path[0][i_next] = i_temp;
						path[1][i_temp] = i_next;
					}
					else {//����� ���������� �������
						  //��������� (i_temp, j_tail)
						path[1][i_temp] = j_tail;
						i_tail = path[0][j_tail];
						path[0][j_tail] = i_temp;
						//������� (i_tail, j_tail)
						path[1][i_tail] = -1;
						//��������� (i_tail, j_head)
						path[1][i_tail] = j_head;
						path[0][j_head] = i_tail;
						isImprove = true;
						isImproveLS = true;//������� �������� ��������
						break;
					}
				}
				else { break; }//������ �� ������������� ��������
			}

		}
		if (isImprove == true) {
			//���� �����, �� ���������� i_head � ����� �������
			QueueVertex.offer(i_head);
			//����  ������� ������-���� ���������� ������ � ������ ������, �� ���������� �� � �������
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
			//                     System.out.println("���������: " +costAssignment(c, result));
			//                    break;
		}
		else {
			//���� �������, �� �������� i_head � ������ ������
			tabu[i_head] = true;
			path[1][i_head] = j_head;//���������� ����
			path[0][j_head] = i_head;
		}
	}
	//        }
	if (isImproveLS == true) {
		//��������� ������������ (�����������)
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
		//                     System.out.println("����: " +costAssignment(c, result));     
		return result;
	}
	else {//������� �������� ��������� ���������
		return assignment;
	}
}
*/

//����������� �� ������� (������ ������)
/*
//��������� "������ ����������" � ��� ������������
int w_1, w_2, h;
//����������� ������������� ��������
double teta;
vector<vector<int>> new_phi_P_approx;
vector<vector<int>> phi_P_approx_reduced;

//����� min � max �������� �� ������� �������� � ������������� ��-�� ������
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

//����� ������ ����������� �� 90+h �������� �� ��������� (max_1-min_1, max_2-min_2)
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

	//������� ��������� ������ (��������� ��������)
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


//����� ������ ����������� �� ��������� (max_1-min_1, max_2-min_2) �� 180-h ��������
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

	//������� ��������� ������ (��������� ��������)
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
