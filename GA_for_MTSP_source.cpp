#include "stdafx.h"
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

	phi.resize(m);
	for (int i = 0; i < m; i++)
		phi[i].resize(N);

	i_rank.resize(N);
	i_dist.resize(N);

	i_rank_R_t.resize(2*N);
	i_dist_R_t.resize(2*N);

	this->tourn_size = 10;//размер турнира

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
//Локальный поиск
////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::local_search(vector<int> assignment, vector<vector<int>> s_m_crit_index)
{
	int G = 0;
	int F = 0;
	int F_max = -1;
	vector<int> result(this->get_n());
	vector<vector<int>> path(2, vector<int>(this->get_n()));

	//            Random rnd = new Random();//датчик случайных чисел
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
	{//пока есть улучшения - движемся вперед!
					   //          i_head=rnd.nextInt(n);//случайным образом выбирается начальная вершина (i_head - это конец пути)
		for (int i_head = 0; i_head < this->get_n(); i_head++)
		{
			is_improve = false;
			j_head = path[1][i_head]; //(j_head - это начало пути) 
			path[1][i_head] = -1;//удаляем дугу
			path[0][j_head] = -1;

			//добавляем дугу для образования цикла 
			for (int i_t = 0; i_t < this->get_n(); i_t++)
			{
				if (i_head != i_t && j_head != i_t)
				{
					i_next = i_t;
					G = 0;//начальный выигрыш
						  //добавляем дугу и тем самым создаем цикл
						  //добавляем (i_head, i_next)
					path[1][i_head] = i_next;
					i_temp = path[0][i_next];//дуга для удаления формируется однозначно
					path[0][i_next] = i_head;
					G = G + s_m_crit_index[i_head][j_head] - s_m_crit_index[i_head][i_next];//обновляем выигрыш
																  //удаляем (i_temp, i_next)
					path[1][i_temp] = -1;
					//разрываем цикл
					//выбираем вершину из цикла j_tail
					count = 0;
					F_max = -1;
					j_tail = -1;
					i_tail = path[1][i_next];//просматриваем вершины цикла
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
					{//в данном направлении нет улучшения (возвращаем метки на место)
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
				//                     System.out.println("Улучшение: " +costAssignment(c, result));
				break;
			}
			else {
				path[1][i_head] = j_head;//возвращаем дугу
				path[0][j_head] = i_head;
			}
		}
	}

	//формируем перестановку (циклическая)
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
	//                     System.out.println("Итог: " +costAssignment(c, result));     
	return result;
}

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
	for (int i=0; i<s_m.size(); i++)
		temp.push_back(phitness(s_m[i], p));

	return temp;
}


////////////////////////////////////////////////////////////////////////////////
//задание матрицы весов для ребер (по всем критериям)
////////////////////////////////////////////////////////////////////////////////
void GA_path::set_matrix_criteria(vector<vector<int>> s)
{
	this->s_m.push_back(s);
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
//Отношение Парето
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

	if (pop_cur.size() == 2 * this->get_N())
	{
		//crowded dist считаем только для первых N особей, дальше не надо
		for (int i = 0; i < pop_cur.size(); i++)
		{
			//считаем, сколько особей из последнего фронта не войдут в новую популяцию
			//для них тоже считаем i_dist, но их не включаем в pop и i_rank
			if (i >= this->get_N())
				num_extra++;


			if ((i_rank_R_t[i] > i_rank_cur) || (i == 2 * this->get_N() - 1)) //признак окончания фронта
			{

				//если признак окончания фронта - конец вектора, то последний индекс добавляем
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
	
	if ((i_rank[i_p1] < i_rank[i_p2]) || ((i_rank[i_p1] = i_rank[i_p2]) && (i_dist[i_p1] > i_dist[i_p2])))
		return true;
	else
		return false; 
}


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


void GA_path::heap_sort(vector<vector<int>> pop_cur, vector<int>& numbers_index, int num_criteria, int index_begin, int index_end)
{
	// Формируем нижний ряд пирамиды
	for (int i = ((index_end - index_begin + 1) / 2) - 1; i >= 0; i--)
		sift_down(pop_cur, numbers_index, num_criteria, i, index_end - index_begin + 1, 0);
	// Просеиваем через пирамиду остальные элементы
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

////////////////////////////////////////////////////////////////////////////////
//Турнирная селекция
////////////////////////////////////////////////////////////////////////////////
int GA_path::tourn_selection()
{
	int j;
	int i_best = (rand() % this->get_N()); //лучшая особь в турнире
	//!!!вопрос: сколько раз надо пройти цикл? (сейчас 10)
	for (int s = this->get_tourn_size(); s>0; s--)
	{
		j = (rand() % this->get_N());
		if ( crowd_comp_oper(j, i_best) ) //исп. croweded-comp oper
			i_best = j;
	}
	return i_best;
}

////////////////////////////////////////////////////////////////////////////////
//Мутация
////////////////////////////////////////////////////////////////////////////////
void GA_path::mutation(int i_p1, int i_p2, vector<int>& p1_temp, vector<int>& p2_temp)
{
	// рандомизация генератора случайных чисел
	//unsigned rand_value = 11;
	//srand(rand_value);

	for (int i = 0; i< n - 1; ++i)
	{
		p1_temp[this->pop[i_p1][i] - 1] = this->pop[i_p1][i + 1] - 1;
		p2_temp[this->pop[i_p2][i] - 1] = this->pop[i_p2][i + 1] - 1;
	}
	p1_temp[this->pop[i_p1][n - 1] - 1] = this->pop[i_p1][0] - 1;
	p2_temp[this->pop[i_p2][n - 1] - 1] = this->pop[i_p2][0] - 1;
	
	//мутация
	double rand_mut = (double)rand() / RAND_MAX;
	if ( rand_mut < this->get_p_mut() )
	{
		rand_mut = (double)rand() / RAND_MAX;
		if ( rand_mut < 0.5 )
		{
			//3-замена по 1-му критерию
			p1_temp = random_change2(p1_temp, 1, this->s_m[0], this->s_aver[0], 50);
		}
		else
		{ 
			//3-замена по 2-му критерию
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
//3-opt замена в мутации
////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::random_change2(vector<int> assignment, int N_change, vector<vector<int>> c,
	int c_aver, int alpha)
{
	int G = 0, x = -1, y_1 = -1, F = 0, F_max = -1, C = 0;
	//используется при выборе дуги для образования цикла
	vector<vector<int>> F_cicle(2, vector<int>(n));
	vector<int> result(n);
	vector<vector<int>> path(2, vector<int>(n));//хранит предков и потомков по циклу
	vector<vector<boolean>> view(n, vector<boolean>(n));//метки просмотренных дуг 
	
	// рандомизация генератора случайных чисел
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
	i_head = rand() % n;//случайным образом выбирается начальная вершина (i_head - это конец пути)
	x = j_head = path[1][i_head]; //(j_head - это начало пути) 
	path[1][i_head] = -1;//удаляем дугу
	path[0][j_head] = -1;
	view[i_head][j_head] = true;//помечаем дугу просмотренной
	
	for (int i = 0; i < n; ++i)//с началом замыкаемся в конце
	{
		view[i][j_head] = true;//помечаем дугу просмотренной   
	}
	//          printf("i_head= %d",i_head);//имеем гамильтонов путь

	//          printf("Deleted (%d, %d), i_head, j_head);
	for (int iter = 0; iter<N_change; ++iter)
	{
		//              printf("iter = %d", iter);
		//добавляем дугу для образования цикла 
		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
		//вычисляем приспособленность F_cicle для каждой вершины
		count = 0;
		for (int i = 0; i < n; ++i)
		{
			F_cicle[0][i] = i;
			F_cicle[1][i] = -n * c_aver;

			//дуги-кондидаты на создание цикла
			if (view[i_head][i] == false && view[path[0][i]][i] == false)
			{
				F_cicle[1][i] = c[path[0][i]][i];//удаление дуги
				C = 1;
				y_1 = i;
				while (y_1 != i_head)
				{
					C++;
					y_1 = path[1][y_1];
				}
				F_cicle[1][i] = F_cicle[1][i] + c_aver * C;//+длина цикла
				count++;
			}
		}
		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
		//сортируем   F_cicle по убыванию 
		F = -1; F_max = -1;
		i_next = -1;
		while (F_max == -1)
		{
			F_max = 1;
			for (int i = 0; i < n - 1; ++i)
			{
				if (F_cicle[1][i]<F_cicle[1][i + 1])//меняем местами
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
		// МОЖЕТ ЛИ ЗДЕСЬ БЫТЬ НОЛЬ????
		if (count == 0)
		{
			//              printf("Экстренная остановка при создании цикла");
			break;
		}//из i_head не получится сделать цикл
		 //выбираем одну вершину из возможных, случайным образом
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
		//добавляем дугу и тем самым создаем цикл
		//добавляем (i_head, i_next)
		path[1][i_head] = i_next;
		i_temp = path[0][i_next];//дуга для удаления формируется однозначно
		path[0][i_next] = i_head;
		view[i_head][i_next] = true;//помечаем дугу просмотренной
		G = G + c[i_head][x] - c[i_head][i_next];//обновляем выигрыш
												 //          printf("i_temp = %d", i_temp);
												 //удаляем (i_temp, i_next)
		path[1][i_temp] = -1;
		view[i_temp][i_next] = true;//помечаем дугу просмотренной
									//          interMatrix(n,2,path);
									//          printf("Added (%d, %d)", i_head, i_next);
									//          printf("Added (%d, %d)", i_temp, i_next);
									//разрываем цикл
									//выбираем  вершину из цикла j_tail жадным способом
		count = 0; F_max = -n * c_aver; j_tail = -1;
		i_tail = i_head;//просматриваем вершины цикла
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
		//          printf("count = &d", count);// МОЖЕТ ЛИ ЗДЕСЬ БЫТЬ НОЛЬ????
		if (count == 0) //не удалось сделать шаг
		{
			//возвращаем все на место и останавливаемся
			path[1][i_head] = -1;
			path[0][j_head] = -1;
			path[0][i_next] = i_temp;
			path[1][i_temp] = i_next;
			//          printf("Экстренная остановка при поиске вершины в цикле");
			break;
		}
		//выбираем случайную вершину из цикла j_tail
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
		//добавляем (i_temp, j_tail)
		path[1][i_temp] = j_tail;
		i_tail = path[0][j_tail];
		path[0][j_tail] = i_temp;
		view[i_temp][j_tail] = true;//помечаем дугу просмотренной
									//удаляем (i_tail, j_tail)
		path[1][i_tail] = -1;
		view[i_tail][j_tail] = true;//помечаем дугу просмотренной
									//          printf("Added (%d, %d)", i_temp, j_tail);
									//          printf("Deleted (%d, %d)", i_tail, j_tail);
		i_head = i_tail; x = j_tail;
		G = G + c[i_temp][i_next] - c[i_temp][j_tail];//обновляем пошаговый
													  //          printf("i_tail = %d", i_tail);
													  //          interMatrix(n,2,path);
	}
	//добавляем (i_head, j_head)
	path[1][i_head] = j_head;
	path[0][j_head] = i_head;//замыкаем цикл
							 //          printf("Added (%d, %d)", i_head, j_head);
							 //          interMatrix(n,2,path);
							 //формируем перестановку (циклическая)
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
//мутация обмена
////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::exchange_mutation(vector<int> p)
{
	//p -- особь для мутации
	// с вероятностью 1 меняем содержимое двух позиций (4 дуги меняется в итоге)

	int i_m1;//первая позиция для обмена
	int i_m2;//вторая позиция для обмена
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
 //<< julia:  новый оператор кроссинговера
 ////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::DEC_new(vector< vector <vector<int> > > s, vector<int> p1, vector<int> p2)
{
	//        for (int i = 0; i < p1.length; ++i) {
	//            System.out.print(p1[i] + " ");
	//        }
	//        System.out.println();
	//        //второй родитель
	//        for (int i = 0; i < p2.length; ++i) {
	//            System.out.print(p2[i] + " ");
	//        }
	//        System.out.println();

	vector<vector<int>> list_in(n, vector<int>(3));//входящие вершины
	vector<vector<int>> list_out(n, vector<int>(4));//исходящие вершины
	vector<int> child(n);//исходящие вершины
						 // заменить 2 на m?
	vector<vector<int>> arc_weights(n, vector<int>(2));//веса дуг
	vector<int> vertex_number(n);//исходящие вершины
								 // заполнение flag?
	vector<bool> flag;


	int vertex, vertex_prev, vertexN, i_parts, i1, i_temp;
	vertex = 1; vertexN = 0;

	// рандомизация генератора случайных чисел
	//unsigned rand_value = 11;
	//srand(rand_value);

	////////////////////////////////////////////////////////////////////////////////   
	/////!заполняем списки!/////
	//++++++++++++++++++++++++++
	for (int i = 0; i < p1.size() - 1; ++i)
	{
		//++++++++++++++++++++++++++
		//++++++++++++++++++++++++++
		//списки
		//исходящие вершины для p1[i]
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
		//входящие вершины для p1[i+1]
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
		//исходящие вершины для p2[i]
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
		//входящие вершины для p2[i+1]
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

	//для TSP нужно добавить
	//         //списки
	//        //исходящие вершиныля p1[n-1]
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
	//входящие вершины для p1[0]
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
	//исходящие вершины для p2[n-1]
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
	//входящие вершины для p2[0]
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
	/////!заполняем списки!///// 
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
		list_out[vertex - 1][3] = 1;//помечаем вершину как удаленную
									//удаляем текущую вершину из list_in
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
		//удаляем текущую вершину из list_out  
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
		//выбираем следующую вершину
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
					//конец if (list_out[vertex - 1][0] == 2)
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

//СТРАРЫЙ ОПЕРАТОР
//////////////////////////////////////////////////////////////////
//рандомизированный кроссинговер, основанный на наследовании дуг
//////////////////////////////////////////////////////////////////
vector<int> GA_path::DEC_old(vector<vector<int>> s, vector<int> p1, vector<int> p2)
{
	//        for (int i = 0; i < p1.length; ++i) {
	//            System.out.print(p1[i] + " ");
	//        }
	//        System.out.println();
	//        //второй родитель
	//        for (int i = 0; i < p2.length; ++i) {
	//            System.out.print(p2[i] + " ");
	//        }
	//        System.out.println();

	vector<vector<int>> list_in(n, vector<int>(4));//входящие вершины
	vector<vector<int>> list_out(n, vector<int>(4));//исходящие вершины
	vector<int> child(n);//исходящие вершины

	int vertex, vertexN;
	vertex = 1; vertexN = 0;
	
	// рандомизация генератора случайных чисел
	//unsigned rand_value = 11;
	//srand(rand_value);

	////////////////////////////////////////////////////////////////////////////////   
	/////!заполняем списки!/////
	//++++++++++++++++++++++++++
	for (int i = 0; i < p1.size() - 1; ++i)
	{
		//++++++++++++++++++++++++++
		//++++++++++++++++++++++++++
		//списки
		//исходящие вершины для p1[i]
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
		//входящие вершины для p1[i+1]
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
		//исходящие вершины для p2[i]
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
		//входящие вершины для p2[i+1]
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

	//для TSP нужно добавить
	//         //списки
	//        //исходящие вершиныля p1[n-1]
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
	//входящие вершины для p1[0]
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
	//исходящие вершины для p2[n-1]
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
	//входящие вершины для p2[0]
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
	/////!заполняем списки!///// 
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
		list_out[vertex - 1][3] = 1;//помечаем вершину как удаленную
									//удаляем текущую вершину из list_in
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
		//удаляем текущую вершину из list_out  
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
		//выбираем следующую вершину
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
					//конец if (list_out[vertex - 1][0] == 2)
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
// << julia: оператор кроссинговера 
////////////////////////////////////////////////////////////////////////////////
vector<int> GA_path::DPX(vector< vector <vector<int> > > s, vector<int> p1, vector<int> p2)
{
	vector<vector<int>> list_out(n, vector<int>(3));//исходящие вершины
	vector<vector<int>>  parts(n, vector<int>(3));//блоки с общими дугами
// заменить 2 на m?	
	vector<vector<int>> arc_weights(n, vector<int>(2));//веса дуг
	vector<int> block_number(n);//потомок
	vector<int> child(n);//потомок
// заполнение flag?
	vector<bool> flag;

	int N_parts, i_parts;//число блоков
	int i1, i_temp, i_count, block;

	// рандомизация генератора случайных чисел
	//unsigned rand_value = 11;
	//srand(rand_value);


	////////////////////////////////////////////////////////////////////////////////   
	/////!заполняем списки!/////
	//++++++++++++++++++++++++++
	for (int i = 0; i < p1.size() - 1; ++i)
	{
		//++++++++++++++++++++++++++
		//++++++++++++++++++++++++++
		//списки
		//исходящие вершины для p1[i]
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


		//исходящие вершины для p2[i]
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

	//исходящие вершины для p1[n-1]
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

	//исходящие вершины для p2[n-1]
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
	/////!заполняем списки!///// 
	////////////////////////////////////////////////////////////////////////////////

	//формируем блоки дуг, общие для родителей
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
			parts[N_parts][0] = 0;
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

	//начинаем строить потомка
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

	//склейка блоков
	for (int t = 1; t < N_parts; ++t)
	{
		//for (int i = 0; i < N_parts; ++i)
		//{
		//	System.out.println(parts[i][0] + " " + parts[i][1] + " " + parts[i][2] + "; " + i);
		//}

		i_parts = 0; //число доступных блоков для первого блока
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
		{ //если доступных блоков нет
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
		} //if(i_parts==0) в результате i_parts == (1 or 2)

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
	}//склейка блоков

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
// << julia: Используется в функциях DEC_new и DCX
////////////////////////////////////////////////////////////////////////////////
vector<bool> GA_path::flag_Pareto_sol(int k, vector<vector<int>> s)
{
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
	// заполнение flag?
	vector<bool> flag_S(k, false);

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
		{ //строим фронт Парето
			if ((s[i][0] <= s[j][0]) && (s[i][1] <= s[j][1]))
			{
				flag = true;
			}
			else {
				flag = false;
			}

			if (flag == true)
			{
				//доминирование есть 
				//для решений с одинаковыми значениями по всем критериям оставляем один экземпляр

				flag_S[j] = true; //удаляем j
				jprev = j;
				j = next(j, k, flag_S); //переходим к следующему j

				if (jprev == j)
				{
					iprev = i;
					i = next(i, k, flag_S); //переходим к следующему i
					if (iprev == i)
					{
						i = -1;
					}
					else {
						jprev = j = i;
						j = next(i, k, flag_S); //переходим к следующему j
						if (jprev == j)
						{
							j = -1;
						}
					}
				}
			}
			else {
				//доминирования нет

				//есть ли доминирование j > i?
				flag = true;
				if ((s[j][0] <= s[i][0]) && (s[j][1] <= s[i][1]))
				{
					flag = true;
				}
				else {
					flag = false;
				}

				if (flag == true) {
					//доминирование есть
					flag_S[i] = true; //удаляем i
					iprev = i;
					i = next(i, k, flag_S); //переходим к следующему i
					if (iprev == i)
					{
						i = -1;
					}
					else {
						jprev = j = i;
						j = next(i, k, flag_S); //переходим к следующему j
						if (jprev == j)
						{
							j = -1;
						}
					}
				}
				else {
					jprev = j;
					j = next(j, k, flag_S); //переходим к следующему j
					if (jprev == j)
					{
						iprev = i;
						i = next(i, k, flag_S); //переходим к следующему i
						if (iprev == i)
						{
							i = -1;
						}
						else {
							jprev = j = i;
							j = next(i, k, flag_S); //переходим к следующему j
							if (jprev == j)
							{
								j = -1;
							}
						}
					}
				} //else
			} //else
		} //while
	} // большой else

	return flag_S;

}

//поиск следующего активного элемента
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

	//вывод сужения множества Парето
	//если num_rows = num_steps (два "кванта информации"), то каждый вывод для пары (teta_1, teta_2)
	if ( quantum_inf == (_1ST_2ND_ + _2ND_1ST_) )
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
			if ( ( ((double)((int)(teta_1*10+0.5))/10) + ((double)((int)(teta_2*10+0.5))/10) >= 1) && (quantum_inf == _1ST_2ND_ + _2ND_1ST_) )
				break;
			
			//пересчитываем векторы в новом вектороном критерии (значения вещественные, т.к. teta вещественные)
			new_phi_P_approx = this->build_new_phi_P_approx(this->phi_P_approx, teta_1, teta_2, quantum_inf);
			//находим мн-во Парето в новом критеритериальном пр-ве
			phi_P_approx_reduced = this->build_phi_P_enum(new_phi_P_approx);

			if ( quantum_inf == (_1ST_2ND_ + _2ND_1ST_) )
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
			new_phi_temp[i][1] = teta_1*init_phi_P_approx[i][0] + (1 - teta_1)*init_phi_P_approx[i][1];
			break;
		//2-ой критерий важнее 1-го с коэффициентом teta_1
		case _2ND_1ST_:
			new_phi_temp[i][0] = (1 - teta_1)*init_phi_P_approx[i][0] + teta_1*init_phi_P_approx[i][1];
			new_phi_temp[i][1] = init_phi_P_approx[i][1];
			break;
		//1-ый критерий важнее 2-го с коэффициентом teta_1 + 2-ой критерий важнее 1-го с коэффициентом teta_2
		case _1ST_2ND_+_2ND_1ST_:
			new_phi_temp[i][0] = (1 - teta_2)*init_phi_P_approx[i][0] + teta_2*init_phi_P_approx[i][1];
			new_phi_temp[i][1] = teta_1*init_phi_P_approx[i][0] + (1 - teta_1)*init_phi_P_approx[i][1];
		}
	
	}
	return new_phi_temp;
}


////////////////////////////////////////////////////////////////////////////////
//построение мн-ва Парето перебором
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

////////////////////////////////////////////////////////////////////////////////
//Чтение множества Парето из файла
////////////////////////////////////////////////////////////////////////////////
vector<vector<int>> GA_path::read_Pareto_set_from_file(String^ file_name_source_str, String^ problem_name_str)
{
	//вектор назначения
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

	//заполнение множества Парето (вектор vector_dest)
	vector<vector<int>> vector_dest_temp;
	int i = 0; //индекс строки
	vector<int> vec_temp(2);
	string str_temp = "";

	while (true) 
	{
		cur_line_str = sr->ReadLine();
		if (cur_line_str[0] == 'N')
			break;

		int j = 0; //индекс столбца
		//разбираем текущую строку
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
