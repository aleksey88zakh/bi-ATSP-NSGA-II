//#include "stdafx.h"
#include "GA_for_MTSP_source.h"

//
//Файл содержит определения операторов ГА и их вспомогательные функции
//

enum rate_crossover
{
	VERY_GOOD=2, GOOD=1, MEDIUM=0, BAD=-1, VERY_BAD=-2
};

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
		if (crowd_comp_oper(j, i_best)) //исп. croweded-comp oper
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
	if (rand_mut < this->get_p_mut())
	{
		rand_mut = (double)rand() / RAND_MAX;
		if (rand_mut < 0.5)
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
	if (rand_mut < this->get_p_mut())
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
	//           }	`
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
							if (((double)(rand() / RAND_MAX)) <= 0.5)
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
////////////////////////////////////////////////////////////////////////////////////

///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!///
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!///
vector<int> GA_path::SAX(vector< vector <vector<int> > > s, vector<int> p1, vector<int> p2)
{
	int n = p1.size();
	vector<vector<int>> list_out(n, vector<int>(3));//исходящие вершины
	vector<vector<int>> list_in(n, vector<int>(3));//входящие вершины

	vector<vector<int>> chains(n, vector<int>(3));//начальные и конечные вершины цепей
	int N_chains = 0;//число цепей
	vector<int> child(n);//потомок
	vector<bool> viewed(n);//индикатор просмотренных вершин
	int nNonViewed = n;
	int i_temp, i1, block;
	vector<bool> flag;
	
	
	////////////////////////////////////////////////////////////////////////////////   
	/////!заполняем списки!/////
	//++++++++++++++++++++++++++
	for (int i = 0; i < p1.size() - 1; ++i) {
		viewed[i] = false;
		//++++++++++++++++++++++++++
		//++++++++++++++++++++++++++
		//списки
		//исходящие вершины для p1[i]
		if (list_out[p1[i] - 1][0] == 0) {
			list_out[p1[i] - 1][0] = 1;
			list_out[p1[i] - 1][1] = p1[i + 1];
		}
		if (list_out[p1[i] - 1][0] == 1 && list_out[p1[i] - 1][1] != p1[i + 1]) {
			list_out[p1[i] - 1][0] = 2;
			list_out[p1[i] - 1][2] = p1[i + 1];
		}
		//входящие вершины для p1[i+1]
		if (list_in[p1[i + 1] - 1][0] == 0) {
			list_in[p1[i + 1] - 1][0] = 1;
			list_in[p1[i + 1] - 1][1] = p1[i];
		}
		if (list_in[p1[i + 1] - 1][0] == 1 && list_in[p1[i + 1] - 1][1] != p1[i]) {
			list_in[p1[i + 1] - 1][0] = 2;
			list_in[p1[i + 1] - 1][2] = p1[i];
		}
		//исходящие вершины для p2[i]
		if (list_out[p2[i] - 1][0] == 0) {
			list_out[p2[i] - 1][0] = 1;
			list_out[p2[i] - 1][1] = p2[i + 1];
		}
		if (list_out[p2[i] - 1][0] == 1 && list_out[p2[i] - 1][1] != p2[i + 1]) {
			list_out[p2[i] - 1][0] = 2;
			list_out[p2[i] - 1][2] = p2[i + 1];
		}
		//входящие вершины для p2[i+1]
		if (list_in[p2[i + 1] - 1][0] == 0) {
			list_in[p2[i + 1] - 1][0] = 1;
			list_in[p2[i + 1] - 1][1] = p2[i];
		}
		if (list_in[p2[i + 1] - 1][0] == 1 && list_in[p2[i + 1] - 1][1] != p2[i]) {
			list_in[p2[i + 1] - 1][0] = 2;
			list_in[p2[i + 1] - 1][2] = p2[i];
		}
	}
	viewed[p1.size() - 1] = false;
	//списки
	//исходящие вершины для p1[n-1]
	if (list_out[p1[p1.size() - 1] - 1][0] == 0) {
		list_out[p1[p1.size() - 1] - 1][0] = 1;
		list_out[p1[p1.size() - 1] - 1][1] = p1[0];
	}
	if (list_out[p1[p1.size() - 1] - 1][0] == 1 && list_out[p1[p1.size() - 1] - 1][1] != p1[0]) {
		list_out[p1[p1.size() - 1] - 1][0] = 2;
		list_out[p1[p1.size() - 1] - 1][2] = p1[0];
	}
	//входящие вершины для p1[0]
	if (list_in[p1[0] - 1][0] == 0) {
		list_in[p1[0] - 1][0] = 1;
		list_in[p1[0] - 1][1] = p1[p1.size() - 1];
	}
	if (list_in[p1[0] - 1][0] == 1 && list_in[p1[0] - 1][1] != p1[p1.size() - 1]) {
		list_in[p1[0] - 1][0] = 2;
		list_in[p1[0] - 1][2] = p1[p1.size() - 1];
	}
	//исходящие вершины для p2[n-1]
	if (list_out[p2[p2.size() - 1] - 1][0] == 0) {
		list_out[p2[p2.size() - 1] - 1][0] = 1;
		list_out[p2[p2.size() - 1] - 1][1] = p2[0];
	}
	if (list_out[p2[p2.size() - 1] - 1][0] == 1 && list_out[p2[p2.size() - 1] - 1][1] != p2[0]) {
		list_out[p2[p2.size() - 1] - 1][0] = 2;
		list_out[p2[p2.size() - 1] - 1][2] = p2[0];
	}
	//входящие вершины для p2[0]
	if (list_in[p2[0] - 1][0] == 0) {
		list_in[p2[0] - 1][0] = 1;
		list_in[p2[0] - 1][1] = p2[p2.size() - 1];
	}
	if (list_in[p2[0] - 1][0] == 1 && list_in[p2[0] - 1][1] != p2[p2.size() - 1]) {
		list_in[p2[0] - 1][0] = 2;
		list_in[p2[0] - 1][2] = p2[p2.size() - 1];
	}
	//++++++++++++++++++++++++++
	//++++++++++++++++++++++++++
	///////////////////////////////////////////////////////////////////////////////
	while (nNonViewed != 0) {
		int i_cur = -1;
		i1 = rand() % nNonViewed + 1; //случайное число от 1 до nNonViewed
		i_temp = 0;
		for (int i = 0; i < p1.size(); ++i) {//выбираем случайную вершину из непросмотренных
			if (viewed[i] == false) {
				i_temp++;
				if (i_temp == i1) {
					i_cur = i;
					break;
				}
			}
		}
		printf("cur: %d\n", (i_cur + 1));
		//помечаем вершину как просмотренную и назначаем для нее новую цепь
		viewed[i_cur] = true;
		nNonViewed--;
		N_chains++;
		chains[N_chains - 1][0] = i_cur + 1;
		chains[N_chains - 1][1] = i_cur + 1;
		chains[N_chains - 1][2] = i_cur + 1;
		while (i_cur > -1) {
			//пытаемся расширить цепь, двигаясь вперед
			if (list_out[i_cur][0] == 1) {
				//если есть одна исходящая вершина
				if (viewed[list_out[i_cur][1] - 1] == false) {
					i_cur = list_out[i_cur][1] - 1;//переходим в нее и помечаем как просмотренную
					viewed[i_cur] = true;
					nNonViewed--;
					chains[N_chains - 1][1] = i_cur + 1;
				}
				else {
					i_cur = -1;
				}
			}
			else {
				if (list_out[i_cur][0] == 2) {
					//если две исходящие вершины, то выбираем лучшую
					if (viewed[list_out[i_cur][1] - 1] == false && viewed[list_out[i_cur][2] - 1] == false) {
						//обе вершины непросмотрены
						i_temp = i_cur;//запоминаем текщую вершину
									   //можно усовершенствовать
						if (s[0][i_cur][list_out[i_cur][1] - 1] >= s[0][i_cur][list_out[i_cur][2] - 1]
							&& s[1][i_cur][list_out[i_cur][1] - 1] >= s[1][i_cur][list_out[i_cur][2] - 1]) {
							//почему переходим в вершину, которая хуже по длине обеих дуг?
							//переходим в первую вершину
							i_cur = list_out[i_cur][1] - 1;//переходим в нее и помечаем как просмотренную
							viewed[i_cur] = true;
							nNonViewed--;
							chains[N_chains - 1][1] = i_cur + 1;
							//удаляем дугу
							if (list_in[list_out[i_temp][2] - 1][0] == 1) {
								list_in[list_out[i_temp][2] - 1][0] = 0;
								list_in[list_out[i_temp][2] - 1][1] = 0;
							}
							else {
								if (list_in[list_out[i_temp][2] - 1][2] == i_temp + 1) {
									list_in[list_out[i_temp][2] - 1][0] = 1;
									list_in[list_out[i_temp][2] - 1][2] = 0;
								}
								else {
									list_in[list_out[i_temp][2] - 1][0] = 1;
									list_in[list_out[i_temp][2] - 1][1] = list_in[list_out[i_temp][2] - 1][2];
									list_in[list_out[i_temp][2] - 1][2] = 0;
								}
							}
							list_out[i_temp][0]--;
							list_out[i_temp][2] = 0;

						}
						else {
							//переходим во вторую вершину
							i_cur = list_out[i_cur][2] - 1;//переходим в нее и помечаем как просмотренную
							viewed[i_cur] = true;
							nNonViewed--;
							chains[N_chains - 1][1] = i_cur + 1;
							//удаляем дугу
							if (list_in[list_out[i_temp][1] - 1][0] == 1) {
								list_in[list_out[i_temp][1] - 1][0] = 0;
								list_in[list_out[i_temp][1] - 1][1] = 0;
							}
							else {
								if (list_in[list_out[i_temp][1] - 1][2] == i_temp + 1) {
									list_in[list_out[i_temp][1] - 1][0] = 1;
									list_in[list_out[i_temp][1] - 1][2] = 0;
								}
								else {
									list_in[list_out[i_temp][1] - 1][0] = 1;
									list_in[list_out[i_temp][1] - 1][1] = list_in[list_out[i_temp][1] - 1][2];
									list_in[list_out[i_temp][1] - 1][2] = 0;
								}
							}
							list_out[i_temp][0]--;
							list_out[i_temp][1] = list_out[i_temp][2];
							list_out[i_temp][2] = 0;
						}
					}
					else {
						//если только одна из вершин непросмотрена
						if (viewed[list_out[i_cur][1] - 1] == false) {
							i_temp = i_cur;
							//только первая вершина
							i_cur = list_out[i_cur][1] - 1;//переходим в нее и помечаем как просмотренную
							viewed[i_cur] = true;
							nNonViewed--;
							chains[N_chains - 1][1] = i_cur + 1;
							//удаляем дугу
							if (list_in[list_out[i_temp][2] - 1][0] == 1) {
								list_in[list_out[i_temp][2] - 1][0] = 0;
								list_in[list_out[i_temp][2] - 1][1] = 0;
							}
							else {
								if (list_in[list_out[i_temp][2] - 1][2] == i_temp + 1) {
									list_in[list_out[i_temp][2] - 1][0] = 1;
									list_in[list_out[i_temp][2] - 1][2] = 0;
								}
								else {
									list_in[list_out[i_temp][2] - 1][0] = 1;
									list_in[list_out[i_temp][2] - 1][1] = list_in[list_out[i_temp][2] - 1][2];
									list_in[list_out[i_temp][2] - 1][2] = 0;
								}
							}
							list_out[i_temp][0]--;
							list_out[i_temp][2] = 0;

						}
						else {
							if (viewed[list_out[i_cur][2] - 1] == false) {
								i_temp = i_cur;
								//вторая вершина
								i_cur = list_out[i_cur][2] - 1;//переходим в нее и помечаем как просмотренную
								viewed[i_cur] = true;
								nNonViewed--;
								chains[N_chains - 1][1] = i_cur + 1;
								//удаляем дугу
								if (list_in[list_out[i_temp][1] - 1][0] == 1) {
									list_in[list_out[i_temp][1] - 1][0] = 0;
									list_in[list_out[i_temp][1] - 1][1] = 0;
								}
								else {
									if (list_in[list_out[i_temp][1] - 1][2] == i_temp + 1) {
										list_in[list_out[i_temp][1] - 1][0] = 1;
										list_in[list_out[i_temp][1] - 1][2] = 0;
									}
									else {
										list_in[list_out[i_temp][1] - 1][0] = 1;
										list_in[list_out[i_temp][1] - 1][1] = list_in[list_out[i_temp][1] - 1][2];
										list_in[list_out[i_temp][1] - 1][2] = 0;
									}
								}
								list_out[i_temp][0]--;
								list_out[i_temp][1] = list_out[i_temp][2];
								list_out[i_temp][2] = 0;
							}
							else {
								i_cur = -1;
							}

						}
					}
					///
				}
				else {
					i_cur = -1;
				}

			}
			printf("cur: %d\n", (i_cur + 1));
		}
		printf("start: %d\n", chains[N_chains - 1][0]);
		printf("finish: %d\n", chains[N_chains - 1][1]);
		printf("median: %d\n", chains[N_chains - 1][2]);
		
		i_cur = chains[N_chains - 1][0] - 1;
		while (i_cur > -1) {
			//пытаемся расширить цепь, двигаясь назад
			if (list_in[i_cur][0] == 1) {
				//если есть одна входящая вершина и она непросмотрена
				if (viewed[list_in[i_cur][1] - 1] == false) {
					i_cur = list_in[i_cur][1] - 1;//переходим в нее и помечаем как просмотренную
					viewed[i_cur] = true;
					nNonViewed--;
					chains[N_chains - 1][0] = i_cur + 1;
				}
				else {
					i_cur = -1;
				}
			}
			else {
				if (list_in[i_cur][0] == 2) {
					//если две входящие вершины, то выбираем лучшую
					if (viewed[list_in[i_cur][1] - 1] == false && viewed[list_in[i_cur][2] - 1] == false) {
						i_temp = i_cur;
						//можно усовершенствовать
						if (s[0][list_in[i_cur][1] - 1][i_cur] >= s[0][list_in[i_cur][2] - 1][i_cur]
							&& s[1][list_in[i_cur][1] - 1][i_cur] >= s[1][list_in[i_cur][2] - 1][i_cur]) {
							i_cur = list_in[i_cur][1] - 1;//переходим в нее и помечаем как просмотренную
							//почему переходим в вершину, которая заведомо хуже по двум критериям?
							viewed[i_cur] = true;
							nNonViewed--;
							chains[N_chains - 1][0] = i_cur + 1;
							//удаляем дугу
							if (list_out[list_in[i_temp][2] - 1][0] == 1) {
								list_out[list_in[i_temp][2] - 1][0] = 0;
								list_out[list_in[i_temp][2] - 1][1] = 0;
							}
							else {
								if (list_out[list_in[i_temp][2] - 1][2] == i_temp + 1) {
									list_out[list_in[i_temp][2] - 1][0] = 1;
									list_out[list_in[i_temp][2] - 1][2] = 0;
								}
								else {
									list_out[list_in[i_temp][2] - 1][0] = 1;
									list_out[list_in[i_temp][2] - 1][1] = list_out[list_in[i_temp][2] - 1][2];
									list_out[list_in[i_temp][2] - 1][2] = 0;
								}
							}
							list_in[i_temp][0]--;
							list_in[i_temp][2] = 0;
						}
						else {
							i_cur = list_in[i_cur][2] - 1;//переходим в нее и помечаем как просмотренную
							viewed[i_cur] = true;
							nNonViewed--;
							chains[N_chains - 1][0] = i_cur + 1;
							//удаляем дугу
							if (list_out[list_in[i_temp][1] - 1][0] == 1) {
								list_out[list_in[i_temp][1] - 1][0] = 0;
								list_out[list_in[i_temp][1] - 1][1] = 0;
							}
							else {
								if (list_out[list_in[i_temp][1] - 1][2] == i_temp + 1) {
									list_out[list_in[i_temp][1] - 1][0] = 1;
									list_out[list_in[i_temp][1] - 1][2] = 0;
								}
								else {
									list_out[list_in[i_temp][1] - 1][0] = 1;
									list_out[list_in[i_temp][1] - 1][1] = list_out[list_in[i_temp][1] - 1][2];
									list_out[list_in[i_temp][1] - 1][2] = 0;
								}
							}
							list_in[i_temp][0]--;
							list_in[i_temp][1] = list_in[i_temp][2];
							list_in[i_temp][2] = 0;

						}
					}
					else {
						if (viewed[list_in[i_cur][1] - 1] == false) {
							i_temp = i_cur;
							i_cur = list_in[i_cur][1] - 1;//переходим в нее и помечаем как просмотренную
							viewed[i_cur] = true;
							nNonViewed--;
							chains[N_chains - 1][0] = i_cur + 1;
							//удаляем дугу
							if (list_out[list_in[i_temp][2] - 1][0] == 1) {
								list_out[list_in[i_temp][2] - 1][0] = 0;
								list_out[list_in[i_temp][2] - 1][1] = 0;
							}
							else {
								if (list_out[list_in[i_temp][2] - 1][2] == i_temp + 1) {
									list_out[list_in[i_temp][2] - 1][0] = 1;
									list_out[list_in[i_temp][2] - 1][2] = 0;
								}
								else {
									list_out[list_in[i_temp][2] - 1][0] = 1;
									list_out[list_in[i_temp][2] - 1][1] = list_out[list_in[i_temp][2] - 1][2];
									list_out[list_in[i_temp][2] - 1][2] = 0;
								}
							}
							list_in[i_temp][0]--;
							list_in[i_temp][2] = 0;

						}
						else {
							if (viewed[list_in[i_cur][2] - 1] == false) {
								i_temp = i_cur;
								i_cur = list_in[i_cur][2] - 1;//переходим в нее и помечаем как просмотренную
								viewed[i_cur] = true;
								nNonViewed--;
								chains[N_chains - 1][0] = i_cur + 1;
								//удаляем дугу
								if (list_out[list_in[i_temp][1] - 1][0] == 1) {
									list_out[list_in[i_temp][1] - 1][0] = 0;
									list_out[list_in[i_temp][1] - 1][1] = 0;
								}
								else {
									if (list_out[list_in[i_temp][1] - 1][2] == i_temp + 1) {
										list_out[list_in[i_temp][1] - 1][0] = 1;
										list_out[list_in[i_temp][1] - 1][2] = 0;
									}
									else {
										list_out[list_in[i_temp][1] - 1][0] = 1;
										list_out[list_in[i_temp][1] - 1][1] = list_out[list_in[i_temp][1] - 1][2];
										list_out[list_in[i_temp][1] - 1][2] = 0;
									}
								}
								list_in[i_temp][0]--;
								list_in[i_temp][1] = list_in[i_temp][2];
								list_in[i_temp][2] = 0;

							}
							else {
								i_cur = -1;
							}

						}
					}
					///
				}
				else {
					i_cur = -1;
				}

			}
			printf("cur: %d\n", (i_cur + 1));
		}
		printf("start: %d\n", chains[N_chains - 1][0]);
		printf("finish: %d\n", chains[N_chains - 1][1]);
		printf("median: %d\n", chains[N_chains - 1][2]);
		for (int i = 0; i<n; i++) {
			printf("%s", viewed[i]? "true ": "false ");
		}
		printf("\n");
	}
	printf("Nchains: %d\n", N_chains);
	if (N_chains > 1) {
		//если цепей несколько, то склеиваем их
		vector<vector<int>> arcWeightsChains(N_chains - 1, vector<int>(2));//веса дуг
		vector<bool> ViewedChains(N_chains);//закреплена ли цепь
		vector<int> SequenceChains(N_chains);//закреплена ли цепь
		vector<int> chainNumber(N_chains);//сопоставляем номера цепей

		int nCurrent;
		int currentChain = rand() % N_chains;//начинаем с произвольной цепи
		ViewedChains[currentChain] = true;
		printf("chain: %d\n", currentChain);
	    SequenceChains[0] = currentChain;
		for (int j = 0; j < N_chains - 1; ++j) {//выстраиваем цепи по принципу "иди в ближайший"
			nCurrent = 0;
			//просматриваем неназначенные цепи
			for (int i = 0; i < N_chains; i++) {
				if (ViewedChains[i] == false) {
					arcWeightsChains[nCurrent][0] = s[0][chains[currentChain][1] - 1][chains[i][0] - 1];
					arcWeightsChains[nCurrent][1] = s[1][chains[currentChain][1] - 1][chains[i][0] - 1];
					chainNumber[nCurrent] = i;
					nCurrent++;
				}
			}
			if (nCurrent > 1) {//если цепей 2 и более				
				flag = flag_Pareto_sol(nCurrent, arcWeightsChains);
				i1 = 0;
				for (int i = 0; i < nCurrent; ++i) {
					if (flag[i] == false) {
						i1++;
					}
					printf("%d%c%d%c%d%c%s\n", arcWeightsChains[i][0],':', arcWeightsChains[i][1],':', chainNumber[i],':', flag[i]? "true ":"false ");
					}
				i_temp = rand() % i1 + 1;
				i1 = 0;
				block = -1;
				for (int i = 0; i < nCurrent; ++i) {
					if (flag[i] == false) {
						i1++;
					}
					if (i1 == i_temp) {
						block = i;
						break;
					}
				}
				ViewedChains[chainNumber[block]] = true;
				currentChain = chainNumber[block];
				SequenceChains[j + 1] = chainNumber[block];
	     		printf("chain: %d\n", chainNumber[block]);
			}
			else {
				printf("chain: %d\n", chainNumber[nCurrent - 1]);
				SequenceChains[j + 1] = chainNumber[nCurrent - 1];
			}

		}
		//строим потомка,просматривая цепи в заданном порядке
		i_temp = 0;
		for (int j = 0; j < N_chains; ++j) {
			int i = 0;
			i1 = chains[SequenceChains[j]][2];
			while (i1 != chains[SequenceChains[j]][0]) {
				i1 = list_in[i1 - 1][1];
				printf("%d%c", i1, ' ');
				i++;
			}
			printf("i: %d\n", i);
			if (i>0) {
				i_temp = i_temp + i;
				i1 = chains[SequenceChains[j]][2];
				while (i1 != chains[SequenceChains[j]][0]) {
					i1 = list_in[i1 - 1][1];
					child[i_temp - 1] = i1; i_temp--;
				}
				i_temp = i_temp + i;
			}
			i1 = chains[SequenceChains[j]][2];
			child[i_temp] = i1; i_temp++;
			printf("%d%c", i1, ' ');
			while (i1 != chains[SequenceChains[j]][1]) {
				i1 = list_out[i1 - 1][1];
				printf("%d%c", i1, ' ');
				child[i_temp] = i1; i_temp++;
			}
			printf("\n");
		}
		printf("child: \n");
		for (int j = 0; j < n; ++j) {
			printf("%d%c", child[j], ' ');
		}
		printf("\n");

	}
	else {
		//если цепь одна
		//строим потомка  
		int i = 0; i_temp = 0;
		i1 = chains[0][2];
		while (i1 != chains[0][0]) {
			i1 = list_in[i1 - 1][1];
			printf("%d%c", i1, ' ');
			i++;
		}
		printf("i: %d\n", i);
		if (i>0) {
			i_temp = i_temp + i;
			i1 = chains[0][2];
			while (i1 != chains[0][0]) {
				i1 = list_in[i1 - 1][1];
				child[i_temp - 1] = i1; i_temp--;
			}
			i_temp = i_temp + i;
		}
		i1 = chains[0][2];
		child[i_temp] = i1; i_temp++;
		printf("%d%c", i1, ' ');
		while (i1 != chains[0][1]) {
			i1 = list_out[i1 - 1][1];
			printf("%d%c", i1, ' ');
			child[i_temp] = i1; i_temp++;
		}
		printf("\n");
		printf("child: \n");
		for (int j = 0; j < n; ++j) {
			printf("%d%c", child[j], ' ');
		}
		printf("\n");

	}	
	//записываем потомка, начиная с 1
	vector<int> childRew(n);//потомок
	i1 = -1;
	for (int j = 0; j < n; j++) {
		if (child[j] == 1) {
			i1 = j;
			break;
		}
	}
	i_temp = 0;
	for (int j = i1; j < n; j++) {
		childRew[i_temp] = child[j];
		i_temp++;
	}
	for (int j = 0; j < i1; j++) {
		childRew[i_temp] = child[j];
		i_temp++;
	}
	vector<int> phi_child = multi_phitness(childRew);
	printf("Cmax %d%c%d%c\n", phi_child[0], ' ', phi_child[1], ' ');
	return childRew;
}
//------------------------------------------------------------------------------//
//------------------------------------------------------------------------------//
//оптимальная рекомбинация родительских особей
vector<vector<int>> GA_path::recombinationOCX(vector< vector <vector<int> > > s, vector<int> p1, vector<int> p2) {
	//
	Archive arch_ga;
	arch_ga.archive.push_back(p1);
	vector<int> phi_p1 = multi_phitness(p1);
	arch_ga.val_crit_archive.push_back(phi_p1);	
	arch_ga.ar_index_not_cons_lst.push_back(0);

	vector<int> phi_p2 = multi_phitness(p2);
	//формируем потомка-архив
	//сравниваем второго родителя с архивом
	if (arch_ga.check_new(phi_p2))
		arch_ga.arch_modify(p2, phi_p2);
	///////////////////////////////////////////////////////
	////пример работы с потомком
	//vector<int> child = DEC_new(s, p1, p2);
	//vector<int> phi_child =multi_phitness(child);
	////формируем потомка-архив
	////сравниваем потомка с архивом
	//if (arch_ga.check_new(phi_child))
	//	arch_ga.arch_modify(child, phi_child);
	///////////////////////////////////////////////////////
	//
	//***********************************************************************
	//Алгоритм решения задачи оптимальной рекомбинации
	//***********************************************************************

	//***********************************************************************
	//***********************************************************************
	//сторим граф G=(X_n,X,U): вершины левой доли-позиции, вершины правой доли - работы
	// позиции 0,1,2,...,n-1; работы n,n+1,...,2n-1 для (1,2,...,n)
	vector<vector<int>> G(2, vector<int>(2*n));//для каждой работы храним список смежных с ней работ
	
	for (int i = 0; i < 2 * n; ++i) {
		G[0][i] = 0;
		G[1][i] = 0;
	}

	for (int i = 0; i < n; ++i) {
		if (p1[i] == p2[i]) {
			G[0][i] = p1[i];
			G[0][p1[i] + n - 1] = i + 1;
		}
		else {
			G[0][i] = p1[i];
			G[1][i] = p2[i];
			if (G[0][p1[i] + n - 1] > 0) {
				G[1][p1[i] + n - 1] = i + 1;
			}
			else {
				G[0][p1[i] + n - 1] = i + 1;
			}
			if (G[0][p2[i] + n - 1] > 0) {
				G[1][p2[i] + n - 1] = i + 1;
			}
			else {
				G[0][p2[i] + n - 1] = i + 1;
			}
		}

	}

	//***********************************************************************
	//***********************************************************************
	//строим особые ребра и блоки
	vector<int> b(n);//хранит индекс блока для каждой позиции или 0 (если особое ребро)
	vector<bool> flag(n);//вершины уже включенные в блоки
 	//находим особые ребра
	for (int i = 0; i < n; ++i) {
		b[i] = -1;
		flag[i] = false;
		if (G[1][i] < 1)/*==0*/ {
			b[i] = 0;// вершина принадлежит особому ребру
			flag[i] = true;//считаем эту вершину уже просмотренной для блоков
		}
	}
	//находим блоки
	////////////////////////////////////
	// создаем копию графа G -- G1
	////////////////////////////////////
	vector<vector<int>> G1(2, vector<int>(2 * n));
	for (int j = 0; j < 2; ++j) {
		for (int i = 0; i < 2 * n; ++i) {
			G1[j][i] = G[j][i];
		}
	}
	////////////////////////////////////
	int I = 0;//индекс просмотра вершин
	int J = 1;//индекс блоков
	int q = 0;//общее число блоков
	int w = 0;// число совершенных паросочетаний в графе G
	int X = -1, Y = -1, I1 = -1, Z = -1;
	bool f = false;//индикатор конца построения блока
	while (I < n) {
		if (flag[I] == false)//находим очередную непросмотренную вершину и строим блок ее содержащий
		{// помещаем I в текущий блок J
			b[I] = J;
			// помечаем I как просмотренную
			flag[I] = true;
			// помешаем в текущий блок J вершины смежные с I (X и Y)
			X = G1[0][I]; //b[X+n-1]=J;
			Y = G1[1][I]; //b[Y+n-1]=J;
						  //удаляем вершину I из графа G1
			if (G1[0][X + n - 1] == I + 1) {//удаляем ссылку на I для X
				G1[0][X + n - 1] = 0;
			}
			else {
				G1[1][X + n - 1] = 0;
			}
			if (G1[0][Y + n - 1] == I + 1) {//удаляем ссылку на I для Y
				G1[0][Y + n - 1] = 0;
			}
			else {
				G1[1][Y + n - 1] = 0;
			}
			G1[0][I] = 0;//удаляем ссылку на X для I
			G1[1][I] = 0;//удаляем ссылку на X для I
						 //            System.out.println("I="+I+" X="+X+" Y="+Y);

						 //строим текущий блок J до конца
			f = false;
			while (f == false) {//пока есть вершины смежные с вершинами текущего блока
								//находим единственную вершину смежную с Y -- I1
				I1 = -1;
				if (G1[0][Y + n - 1] > 0) {
					I1 = G1[0][Y + n - 1] - 1;
				}
				else {
					I1 = G1[1][Y + n - 1] - 1;
				}
				//помещаем I1 в текущий блок J
				b[I1] = J;
				// помечаем I1 как просмотренную
				flag[I1] = true;
				//удаляем вершину Y из графа G1
				if (G1[0][I1] == Y) {
					G1[0][I1] = 0;
				}
				else {
					G1[1][I1] = 0;
				}
				if (G1[0][Y + n - 1] > 0) {// у Y только одна смежная вершина осталась
					G1[0][Y + n - 1] = 0;
				}
				else {
					G1[1][Y + n - 1] = 0;
				}
				//находим единственную вершину смежную с I1 -- Z
				Z = -1;
				if (G1[0][I1] > 0) {
					Z = G1[0][I1];
				}
				else {
					Z = G1[1][I1];
				}
				//помещаем Z в текущий блок J
				//b[Z+n-1]=J;
				//удаляем вершину I1 из графа G1
				if (G1[0][Z + n - 1] == I1 + 1) { // у I1 только одна смежная вершина осталась -- Z
					G1[0][Z + n - 1] = 0;// удаляем у Z ссылку на I1
				}
				else {
					G1[1][Z + n - 1] = 0;
				}
				if (G1[0][I1] > 0) {// удаляем у I1 ссылку на Z
					G1[0][I1] = 0;
				}
				else {
					G1[1][I1] = 0;
				}
				if (Z == X) {//замкнулись: блок построен
							 //удаляем вершину Z из графа G1: все ссылки уже удалены при
							 // удалении I и I1
					J = J + 1;
					f = true;//переходим к построению следующего блока
				}
				else {
					Y = Z;
				}// продолжаем построение текущего блока
			}//while(f==false) конец одной итерации построения блока J
		}//конец построения одного блока
		I++;
	}//конец процедуры построения блоков
	 //вычисляем общее число блоков
	q = J - 1;
	//        file.write("\r\nq(G)=" + q + "\r\n");
	////////////////////////////////////////////////////////////////////////
	 //q -- число блоков в ЗОР
					   //вычисляем число совершенных паросочетаний в графе G
	w = (int)pow(2, q);
	int numberSolutionInRec = w;//число допустимых решений в ЗОР 
	//**********************************************************************
	//строим максимальные паросочетания в блоках
	vector<vector<int>> matching(2, vector<int>(n));//для паросочетаний блоков
									 // в первой строке хранятся первые паросочетания блоков
									 // во второй строке хранятся вторые паросочетания блоков
	for (int i = 0; i < n; ++i) {
		matching[0][i] = p1[i];
		matching[1][i] = p2[i];
	}
	
	//***********************************************************************
	//***********************************************************************
	//вычисляем веса переключений между блоками	
	vector<vector<vector<int>>> S00(2, vector<vector<int>>(q,vector<int>(q,0)));//веса между первыми паросочетаниями блоков
	vector<vector<vector<int>>> S01(2, vector<vector<int>>(q, vector<int>(q,0)));//веса между первым и вторым паросочетаниями блоков
	vector<vector<vector<int>>> S10(2, vector<vector<int>>(q, vector<int>(q,0)));//веса между вторым и первым паросочетаниями блоков
	vector<vector<vector<int>>> S11(2, vector<vector<int>>(q, vector<int>(q,0)));//веса между вторыми паросочетаниями блоков
								// задаем начальные значения весов: 0
	/*for (int i = 0; i < q; ++i) {
		for (int j = 0; j < q; ++j) {
			for (int k = 0; k < 2; ++k) {
				S00[k][i][j] = S01[k][i][j] = S10[k][i][j] = S11[k][i][j] = 0;
			}
		}
	}*/

	//цикл по позициям
	for (int i = 0; i < n - 1; ++i) {
		//вычисляем стоимости переключений для блоков
		if (b[i] == b[i + 1] && b[i] > 0) {//внутри блока
			for (int k = 0; k < 2; ++k) {
				S00[k][b[i] - 1][b[i + 1] - 1] = S00[k][b[i] - 1][b[i + 1] - 1] + s[k][matching[0][i] - 1][matching[0][i + 1] - 1];
				S11[k][b[i] - 1][b[i + 1] - 1] = S11[k][b[i] - 1][b[i + 1] - 1] + s[k][matching[1][i] - 1][matching[1][i + 1] - 1];
			}
		}
		if (b[i] == 0 && b[i + 1] > 0) {//между особым ребром и блоком
			for (int k = 0; k < 2; ++k) {
				S00[k][b[i + 1] - 1][b[i + 1] - 1] = S00[k][b[i + 1] - 1][b[i + 1] - 1] + s[k][matching[0][i] - 1][matching[0][i + 1] - 1];
				S11[k][b[i + 1] - 1][b[i + 1] - 1] = S11[k][b[i + 1] - 1][b[i + 1] - 1] + s[k][matching[1][i] - 1][matching[1][i + 1] - 1];
			}
		}
		if (b[i] > 0 && b[i + 1] == 0) {//между блоком и особым ребром
			for (int k = 0; k < 2; ++k) {
				S00[k][b[i] - 1][b[i] - 1] = S00[k][b[i] - 1][b[i] - 1] + s[k][matching[0][i] - 1][matching[0][i + 1] - 1];
				S11[k][b[i] - 1][b[i] - 1] = S11[k][b[i] - 1][b[i] - 1] + s[k][matching[1][i] - 1][matching[1][i + 1] - 1];
			}
		}
		if (b[i] > 0 && b[i + 1] > 0 && b[i] != b[i + 1]) {//между различными блоками
			for (int k = 0; k < 2; ++k) {
			S00[k][b[i] - 1][b[i + 1] - 1] = S00[k][b[i] - 1][b[i + 1] - 1] + s[k][matching[0][i] - 1][matching[0][i + 1] - 1];
			S11[k][b[i] - 1][b[i + 1] - 1] = S11[k][b[i] - 1][b[i + 1] - 1] + s[k][matching[1][i] - 1][matching[1][i + 1] - 1];
			S01[k][b[i] - 1][b[i + 1] - 1] = S01[k][b[i] - 1][b[i + 1] - 1] + s[k][matching[0][i] - 1][matching[1][i + 1] - 1];
			S10[k][b[i] - 1][b[i + 1] - 1] = S10[k][b[i] - 1][b[i + 1] - 1] + s[k][matching[1][i] - 1][matching[0][i + 1] - 1];
			}
		}
	}
	//    //!!!для коммивояжера!!!//+переналадка с последней позиции на первую
	if (b[n - 1] == b[0] && b[0] > 0) {//внутри блока
		for (int k = 0; k < 2; ++k) {
			S00[k][b[n - 1] - 1][b[0] - 1] = S00[k][b[n - 1] - 1][b[0] - 1] + s[k][matching[0][n - 1] - 1][matching[0][0] - 1];
			S11[k][b[n - 1] - 1][b[0] - 1] = S11[k][b[n - 1] - 1][b[0] - 1] + s[k][matching[1][n - 1] - 1][matching[1][0] - 1];
		}
	}
	if (b[n - 1] == 0 && b[0] > 0) {//между особым ребром и блоком
		for (int k = 0; k < 2; ++k) {
			S00[k][b[0] - 1][b[0] - 1] = S00[k][b[0] - 1][b[0] - 1] + s[k][matching[0][n - 1] - 1][matching[0][0] - 1];
			S11[k][b[0] - 1][b[0] - 1] = S11[k][b[0] - 1][b[0] - 1] + s[k][matching[1][n - 1] - 1][matching[1][0] - 1];
		}
	}
	if (b[n - 1] > 0 && b[0] == 0) {//между блоком и особым ребром
		for (int k = 0; k < 2; ++k) {
			S00[k][b[n - 1] - 1][b[n - 1] - 1] = S00[k][b[n - 1] - 1][b[n - 1] - 1] + s[k][matching[0][n - 1] - 1][matching[0][0] - 1];
			S11[k][b[n - 1] - 1][b[n - 1] - 1] = S11[k][b[n - 1] - 1][b[n - 1] - 1] + s[k][matching[1][n - 1] - 1][matching[1][0] - 1];
		}
	}
	if (b[n - 1] > 0 && b[0] > 0 && b[n - 1] != b[0]) {//между различными блоками
		for (int k = 0; k < 2; ++k) {
			S00[k][b[n - 1] - 1][b[0] - 1] = S00[k][b[n - 1] - 1][b[0] - 1] + s[k][matching[0][n - 1] - 1][matching[0][0] - 1];
			S11[k][b[n - 1] - 1][b[0] - 1] = S11[k][b[n - 1] - 1][b[0] - 1] + s[k][matching[1][n - 1] - 1][matching[1][0] - 1];
			S01[k][b[n - 1] - 1][b[0] - 1] = S01[k][b[n - 1] - 1][b[0] - 1] + s[k][matching[0][n - 1] - 1][matching[1][0] - 1];
			S10[k][b[n - 1] - 1][b[0] - 1] = S10[k][b[n - 1] - 1][b[0] - 1] + s[k][matching[1][n - 1] - 1][matching[0][0] - 1];
		}
	}
	//    //!!!для коммивояжера!!!//+переналадка с последней позиции на первую     
	//***********************************************************************
	//***********************************************************************
	//алгоритм перебора совершенных паросочетаний в блоках и вычисление для них
	// целевых функций.
	/////////////////////////////////////////////////////////
	//I,I1,X,Y,Z,f
	vector<int> f_old(2);
	vector<int> f_new(2);//значения целевых функций (предыдущее, следующее)
	for (int k = 0; k < 2; ++k) {
		f_old[k] = f_new[k] = 0;
	}
	vector<bool> delta_old(q);//индикатор паросочетаний в блоках (предыдущий)
	vector<bool> delta_new(q);//индикатор паросочетаний в блоках  (следующий)
	vector<bool> delta_opt(q);//индикатор паросочетаний в блоках  (оптимальный)
	vector<int> delta(q);//для построения текущего индикатора
	/////////////////////////////////////////////////////////////////////////
	//шаг 0
	for (int i = 0; i < q; ++i) {
		delta[i] = 0;
		delta_opt[i] = delta_new[i] = delta_old[i] = false;
	}
	//начинаем с выбора парососетания w_{0j} в каждом блоке
	// false -> 0; true-> 1
	//вычисляем значение целевой функции, ему соответствующее
	for (int i = 0; i < n - 1; ++i) {
		for(int k = 0; k < 2; ++k) {
			f_old[k] = f_old[k] + s[k][matching[0][i] - 1][matching[0][i + 1] - 1];
		}
	}
	////////////////////////////////////////////////////////////////////
	//!!!для коммивояжера!!!//+переналадка с последней позиции на первую
	for (int k = 0; k < 2; ++k) {
		f_old[k] = f_old[k] + s[k][matching[0][n - 1] - 1][matching[0][0] - 1];
	}
	//!!!для коммивояжера!!!//+переналадка с последней позиции на первую
	////////////////////////////////////////////////////////////////////
	//задаем начальное значение для f_new
	for (int k = 0; k < 2; ++k) {
		f_new[k] = f_old[k];
	}
	printf("%d%c%d%c\n", f_new[0], ' ', f_new[1], ' ');
	/////////////////////////////////////////////////////////////////////////
	printf("%d%c%d%c\n", q, ' ', w, ' ');
	//основные шаги алгоритма
	for (int i = 1; i < w; ++i) {
		/////////////////////////////////////////////////////////////////////////              
		////////////////////////////////////////////////////////////////////////            
		//перебираем двоичные строки в порядке лексикографического возрастания
		/////////////////////////////////////////////////////////////////////////
		// сопоставляем i его двоичное представление длины q --  delta
		I = i;
		for (int j = q - 1; j > 0; --j) {
			delta[j] = i - (i / 2) * 2;
			i /= 2;
		}
		delta[0] = i;
		i = I;
		// сопоставляем delta его представление в кодировке Грея -- delta_new
		// это и будет текущий индикатор паросочетаний
		if (delta[0] == 1) {
			delta_new[0] = true;
		}
		else {
			delta_new[0] = false;
		}
		for (int j = 1; j < q; ++j) {
			if ((delta[j - 1] + delta[j]) % 2 == 0) {//сумма по модулю два
				delta_new[j] = false;
			}
			else {
				delta_new[j] = true;
			}
		}
		/////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////
		//находим блок, в котором произошли изменения
		I = -1;
		for (int j = 0; j < q; ++j) {
			if (delta_old[j] != delta_new[j]) {
				I = j + 1;
				break;
			}
		}
		/////////////////////////////////////////////////////////////////////////
		//вычисляем значение целевой функции для построенной комбинации паросочетаний в блоках
		for (int k = 0; k < 2; ++k) {
			f_new[k] = f_old[k];
		}
		if (delta_old[I - 1] == true) {//обновляем веса контактов внутри блока I
			for (int k = 0; k < 2; ++k) {
				f_new[k] = f_new[k] - S11[k][I - 1][I - 1] + S00[k][I - 1][I - 1];
			}
		}
		else {
			for (int k = 0; k < 2; ++k) {
				f_new[k] = f_new[k] - S00[k][I - 1][I - 1] + S11[k][I - 1][I - 1];
			}
		}
		for (int j = 0; j < q; ++j) {//обновляем веса контактов между блоками
			if (j != I - 1) {
				//обновляем веса на блок I и с блока I
				if (delta_old[I - 1] == true) {
					if (delta_old[j] == true) {
						for (int k = 0; k < 2; ++k) {
							f_new[k] = f_new[k] - S11[k][j][I - 1] + S10[k][j][I - 1];//на блок I
							f_new[k] = f_new[k] - S11[k][I - 1][j] + S01[k][I - 1][j];//с блока I
						}
					}
					else {
						for (int k = 0; k < 2; ++k) {
							f_new[k] = f_new[k] - S01[k][j][I - 1] + S00[k][j][I - 1];//на блок I
							f_new[k] = f_new[k] - S10[k][I - 1][j] + S00[k][I - 1][j];//с блока I
						}
					}
				}
				else {
					if (delta_old[j] == true) {
						for (int k = 0; k < 2; ++k) {
							f_new[k] = f_new[k] - S10[k][j][I - 1] + S11[k][j][I - 1];//на блок I
							f_new[k] = f_new[k] - S01[k][I - 1][j] + S11[k][I - 1][j];//с блока I
						}
					}
					else {
						for (int k = 0; k < 2; ++k) {
							f_new[k] = f_new[k] - S00[k][j][I - 1] + S01[k][j][I - 1];//на блок I
							f_new[k] = f_new[k] - S00[k][I - 1][j] + S10[k][I - 1][j];//с блока I
						}
					}
				}//else
			}//  if(j!=I-1)
		}
		/////////////////////////////////////////////////////////////////////////
		//сравниваем потомка с архивом
		printf("%d%c%d%c\n", f_new[0], ' ', f_new[1], ' ');
		if (arch_ga.check_new(f_new)){
			vector<int> child(n);//результат оптимальной рекомбинации (потомок)
			for (int i = 0; i < n; ++i) {
				if (b[i] >0 && delta_new[b[i] - 1] == true) {// если позиция соответствует блоку со вторым парососетанием
					child[i] = matching[1][i];
				}
				if (b[i] >0 && delta_new[b[i] - 1] == false) {// если позиция соответствует блоку с первым парососетанием
					child[i] = matching[0][i];
				}
				if (b[i] == 0) {// если позиция соответствует особому ребру
					child[i] = matching[0][i];
				}
			}
			arch_ga.arch_modify(child, f_new);
		}

		/////////////////////////////////////////////////////////////////////////
		//переходим к следующей итерации
		for (int j = 0; j < q; ++j) {
			delta_old[j] = delta_new[j];
		}
		for (int k = 0; k < 2; ++k) {
			f_old[k] = f_new[k];
		}
	}//конец перебора совершенных паросочетаний в блоках
	 /////////////////////////////////////////////////////////////////////////
	vector<vector<int>> childs;// (arch_ga.ar_index_not_cons_lst.size(), vector<int>(n));
	list<unsigned>::const_iterator it_lst;
	//int i_arch = 0;
	for (it_lst = arch_ga.ar_index_not_cons_lst.cbegin(); it_lst != arch_ga.ar_index_not_cons_lst.cend(); it_lst++)
	{
		childs.push_back(arch_ga.archive[*it_lst]);
	   // printf("Element: %d%c%d%c\n", arch_ga.val_crit_archive[*it_lst][0], ' ', arch_ga.val_crit_archive[*it_lst][1], ' ');
		//for (int k = 0; k < n; ++k) {
		//	childs[i_arch][k] = arch_ga.archive[*it_lst][k];
			//printf("%d%c", arch_ga.archive[*it_lst][k], ' ');
		//}
		//printf("\n"); 
		//i_arch++;
	}
	/*for (int j=0;j<arch_ga.ar_index_not_cons_lst.size();j++)
	{
		printf("Child %d%c",(j+1),':');
		for (int k = 0; k < n; ++k) {
			printf("%d%c", childs[j][k], ' ');
		}
		printf("\n");
	}*/
	return childs;
	////////////////////////////////////////////////////////////////////////
}
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!///
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!///


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
// Обучение с подкрепелением в операторах ГА
////////////////////////////////////////////////////////////////////////////////
int GA_path::update_rate_crossover(vector<int> p1, vector<int> p2, vector<int> ch)
{
	//один из родителей доминирует другого
	rate_crossover en_rate;
	vector<int> phi_ch = this->multi_phitness(ch);
	vector<int> phi_p1 = this->multi_phitness(p1);
	vector<int> phi_p2 = this->multi_phitness(p2);

	bool ch_p1 = Pareto_pref(phi_ch, phi_p1);
	bool p1_ch = Pareto_pref(phi_p1, phi_ch);
	bool ch_p2 = Pareto_pref(phi_ch, phi_p2);
	bool p2_ch = Pareto_pref(phi_p2, phi_ch);
	bool p1_p2 = Pareto_pref(phi_p1, phi_p2);
	bool p2_p1 = Pareto_pref(phi_p2, phi_p1);

	//
	if ( p1_p2 )
	{
		if ( ch_p1 )
			return VERY_GOOD;

		if ( ch_p2 && !ch_p1 && !p1_ch )
			return VERY_GOOD;

		if ( !ch_p1 && !p1_ch && !ch_p2 && !p2_ch )
			return GOOD;

		if ( phi_ch == phi_p1 || phi_ch == phi_p2 )
			return MEDIUM;

		if (p1_ch && ((!ch_p2 && !p2_ch) || ch_p2))
			return BAD;

		if (p1_ch && p2_ch)
			return VERY_BAD;
	}
		
	if ( p2_p1 )
	{
		if (ch_p2)
			return VERY_GOOD;

		if (ch_p1 && !ch_p2 && !p2_ch)
			return VERY_GOOD;

		if (!ch_p1 && !p1_ch && !ch_p2 && !p2_ch)
			return GOOD;

		if (phi_ch == phi_p1 || phi_ch == phi_p2)
			return MEDIUM;	

		if (p2_ch && ((!ch_p1 && !p1_ch) || ch_p1))
			return BAD;

		if (p1_ch && p2_ch)
			return VERY_BAD;
	}

	
	//родители не доминируют друг друга
	if (!p1_p2 && !p2_p1 )
	{
		if (ch_p1 && ch_p2)
			return VERY_GOOD;
// через return	
		if ( (ch_p1 && !ch_p2 && !p2_ch) || (ch_p2 && !ch_p1 && !p1_ch) )
			return VERY_GOOD;

		if (!ch_p1 && !p1_ch && !ch_p2 && !p2_ch)
			return GOOD;

		if (phi_ch == phi_p1 || phi_ch == phi_p2)
			return MEDIUM;

		if ( (p2_ch && !ch_p1 && !p1_ch) ||
			 (p1_ch && !ch_p2 && !p2_ch) )
			return BAD;

		if (p1_ch && p2_ch)
			return VERY_BAD;
	}
	
	return 0; // по идее недостижимо
}