//#include "stdafx.h"
#include "GA_for_MTSP_source.h"

#include <string>

using namespace std;
using namespace System;
using namespace System::IO;

////////////////////////////////////////////////////////////////////////////////
//Чтение множества Парето из файла
////////////////////////////////////////////////////////////////////////////////
vector<vector<int>> read_Pareto_set_from_file(String^ file_name_source_str, String^ problem_name_str, int num_criteria)
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

				if (j == num_criteria)
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
	double res_sec = (result_time / 1000) % 60 + ( ((double)result_time) / 1000) - (result_time / 1000);
	int res_min = (result_time / (60 * 1000)) % 60;
	int res_hour = result_time / (3600 * 1000);
	sw->WriteLine(title + ": ; {0}; h; {1}; min; {2:F3}; sec", res_hour, res_min, res_sec);
	printf("Time:\n %d:%d:%.3f\n", res_hour, res_min, res_sec);
}

vector<int> shaking_Kopt(vector<int> s, int k)
{
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//        for(int j=0;j<s.length;++j){//исходная перестановка
	//            System.out.print(s[j]+" ");
	//        }
	//        System.out.println();
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vector<int> s1(s.size());//сосед (заменяем k дуг)
	vector<int> positions(k);// вершины для разрыва
	vector<vector<int>> vertex_path(2, vector<int>(k));//вершины по порядку и их позиции в перестановке
	vector<bool> flag(s.size());//метки - просмотрена вершина или нет
	int number = s.size(), current, temp, i_temp;
	//для рандомизатора
	int temp_time = ::GetTickCount();
	srand(temp_time);

	//**********************************************************************
	for (int j = 0; j < k; ++j)
	{//выбираем точки для разрыва (допустимо и случайным образом) O(n*k)
		current = rand() % number + 1;
		temp = 0;
		i_temp = -1;

		for (int i = 0; i< s.size(); ++i)
		{
			if (flag[s[i] - 1] == false)
			{
				temp++;
			}
			if (temp == current)
			{
				i_temp = i;
				break;
			}
		}

		positions[j] = s[i_temp];
		flag[s[i_temp] - 1] = true;
		//сосед справа от выбранной вершины не может быть кандидатом на удаление?
		number--;
		//        if(i_temp-1!=-1){
		//        if(flag[s[i_temp-1]-1]==false){flag[s[i_temp-1]-1]=true;number--;}
		//        }else
		//        {if(flag[s[s.length-1]-1]==false){flag[s[s.length-1]-1]=true;number--;}
		//        }

		//        if(i_temp+1!=s.length){
		//        if(flag[s[i_temp+1]-1]==false){flag[s[i_temp+1]-1]=true;number--;}
		//        }else{
		//        if(flag[s[0]-1]==false){flag[s[0]-1]=true;number--;}   
		//        }
	}
	//**********************************************************************
	i_temp = 0;
	for (int i = 0; i < s.size(); ++i)
	{//упорядочили кусочки O(n*k)
		for (int j = 0; j < k; ++j)
		{
			if (s[i] == positions[j])
			{
				vertex_path[0][i_temp] = s[i];
				vertex_path[1][i_temp] = i;
				i_temp++;
			}
		}
	}
	//**********************************************************************
	//конструируем соседа
	flag = vector<bool>(k, false); //O(k+n)
	int k_next, k_prev, k_temp;
	if (vertex_path[1][0] == 0)
	{//первый блок оставлем неподвижным
		i_temp = 0;
		for (int j = 0; j < vertex_path[1][1]; ++j)
		{
			s1[j] = s[j];
			i_temp++;
		}
		flag[0] = true;
		number = k - 1;
		k_prev = 0;

		while (number != 0)
		{//каждый следующий блок выбираем случайно
			current = rand() % number + 1;
			temp = 0;
			k_next = -1;
			k_temp = -1;

			for (int i = 1; i < k; ++i)
			{
				if (flag[i] == false)
				{
					temp++;
					k_next = i;
				}
				if (temp == current)
				{
					if (k_next - 1 != k_prev)
						break;
					else
					{
						if (k_temp != -1)
						{
							k_next = k_temp;
							break;
						}
						else
						{
							current++;
						}
					}
				}
				if (flag[i] == false) { k_temp = i; }
			}

			flag[k_next] = true;
			number--;
			k_prev = k_next;
			if (k_next + 1 != k)
			{
				for (int j = vertex_path[1][k_next]; j < vertex_path[1][k_next + 1]; ++j)
				{
					s1[i_temp] = s[j];
					i_temp++;
				}
			}
			else
			{
				for (int j = vertex_path[1][k_next]; j < s.size(); ++j)
				{
					s1[i_temp] = s[j];
					i_temp++;
				}
			}
		}
	}
	else
	{//последний блок оставляем неподвижным
		i_temp = 0;
		for (int j = 0; j < vertex_path[1][0]; ++j)
		{
			s1[j] = s[j];
			i_temp++;
		}
		for (int j = s.size() - 1; j >= vertex_path[1][k - 1]; --j)
		{
			s1[j] = s[j];//i_temp++;
		}
		flag[k - 1] = true;
		number = k - 1;
		k_prev = k - 1;

		while (number != 0)
		{//остальные блоки выбираем случайно
			current = rand() % number + 1;
			temp = 0;
			k_next = -1;
			k_temp = -1;

			for (int i = 0; i < k; ++i)
			{
				if (flag[i] == false)
				{
					temp++;
					k_next = i;
				}
				if (temp == current)
				{
					if (k_next>0)
					{
						if (k_next - 1 != k_prev)
						{
							break;
						}
						else
						{
							if (k_temp != -1)
							{
								k_next = k_temp;
								break;
							}
							else
							{
								current++;
							}
						}
					}
					else
					{//k_next=0
						if (k_prev != k - 1)
						{
							break;
						}
						else
						{
							current++;
						}
					}
				}
				if (flag[i] == false)
				{
					k_temp = i;
				}
			}
			//        current=rnd.nextInt(number)+1; temp=0;k_next=-1;
			//        for(int i=0;i<k;++i){
			//        if(flag[i]==false){temp++;}
			//        if (temp==current){k_next=i;break;}
			//        }
			flag[k_next] = true;
			number--;
			k_prev = k_next;

			for (int j = vertex_path[1][k_next]; j < vertex_path[1][k_next + 1]; ++j)
			{
				s1[i_temp] = s[j];
				i_temp++;
			}
		}

	}
	//        for(int j=0;j<k;++j){
	//            System.out.print(positions[j]+" ");
	//        }
	//        System.out.println();
	//        for(int j=0;j<k;++j){
	//            System.out.print(vertex_path[0][j]+" , "+vertex_path[1][j]+" ; ");
	//        }
	//        System.out.println();
	//        for(int j=0;j<s.length;++j){
	//            System.out.print(s1[j]+" ");
	//        }
	//        System.out.println();
	return s1;
}

unsigned GA_path::hiper_volume(vector<int> r, vector<vector<int>> f)
{
	unsigned vol_res = 0;

	//предсортировка по 1-му критерию
	//? учесть равенство по 1-му критерию
	//вектор фронтов (используется индекс особи в популяции pop)
	vector<int> index_front_temp;
	for (int i = 0; i < f.size(); i++)
		index_front_temp.push_back(i);
	heap_sort(f, index_front_temp, -1, 0, index_front_temp.size() - 1);

	for (int i = 0; i < index_front_temp.size(); i++)
	{
		unsigned vol_tmp = 1;
		for (int j = 0; j < f[0].size(); j++)
			vol_tmp *= (f[index_front_temp[i]][j] - r[j]);
		vol_res += vol_tmp;
		//передвигаем точку r по первой координате
		r[0] = f[index_front_temp[i]][0];

	}

	return vol_res;
}

