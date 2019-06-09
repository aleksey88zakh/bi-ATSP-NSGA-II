#include "GA_for_MTSP_source.h"


void GA_path::output_pop()
{
	for (int i = 0; i < this->get_N(); i++)
	{
		for (int j = 0; j < this->get_n(); j++)
		{
			//sw->Write("{0};", this->pop[i][j]);
			printf("%d\t", this->pop[i][j]);
		}
		//sw->WriteLine();
		printf("\n");
	}

	//sw->WriteLine();
	
}

//вывод значений критерия
void  GA_path::output_phi()
{
	for (int j = 0; j < this->get_m(); j++)
	{
		for (int i = 0; i < this->get_N(); i++)
		{
			//sw->Write("{0};", ga.phi[i][j]);
			printf("%d\t", this->phi[i][j]);
		}
		//sw->WriteLine();
		printf("\n");
	}
	printf("\n");
	//sw->WriteLine();
}

void  GA_path::output_ranks()
{
	for (int i = 0; i < this->get_N(); i++)
	{
		//sw->Write("{0};", ga.i_rank[i]);
		//sw_1->Write("{0};", ga.i_rank[i]);
		printf("%d\t", this->i_rank[i]);
	}
	//sw->WriteLine();
	//sw_1->WriteLine();
	printf("\n");
}

void  GA_path::output_dist()
{
	for (int i = 0; i < this->get_N(); i++)
	{
		//sw->Write("{0:F2};", ga.i_dist[i]);
		printf("%.2f\n", this->i_dist[i]);
	}
	//sw->WriteLine();
}

void  GA_path::output_P_approx()
{
	for (int j = 0; j < this->get_m(); j++)
	{
		for (int i = 0; i < this->phi_P_approx.size(); i++)
		{
			//sw->Write("{0};", ga.phi_P_approx[i][j]);
			printf("%d\t", this->phi_P_approx[i][j]);
		}
		//sw->WriteLine();
	}
}

void write_to_file(StreamWriter^ sw, String^ st)
{
	sw->Write(st);
}

//void write_to_file(StreamWriter^ sw, array<wchar_t> ^buffer)
//{
//	sw->Write(buffer);
//}

//void write_to_file(StreamWriter^ sw, array<wchar_t> ^buffer, int index, int count)
//{
//	sw->Write(buffer, index, count);
//}

void write_to_file(StreamWriter^ sw, wchar_t val)
{
	sw->Write(val);
}

void write_to_file_DEBUG(StreamWriter^ sw, String^ st)
{
#ifdef _DEBUG_
		sw->Write(st);
#endif
}

void write_to_file_endl(StreamWriter^ sw, String^ st)
{
	sw->WriteLine(st);
}

void write_to_file_endl_DEBUG(StreamWriter^ sw, String^ st)
{
#ifdef _DEBUG_
	sw->WriteLine(st);
#endif
}