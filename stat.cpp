#include "GA_for_MTSP_source.h"

using namespace std;

Statistics::Statistics()
{

}

Statistics::Statistics(double cur_val)
{
	total_val = cur_val;
}

void Statistics::set_to_zero()
{
	total_val = max_val = min_val = cnt_val = 0;
}

void Statistics::refresh(double new_val)
{
	total_val += new_val;
	if (max_val < new_val)
		max_val = new_val;
	if (min_val > new_val)
		min_val = new_val;
	cnt_val++;
}

void Statistics::init_max_min(double val)
{
	max_val = min_val = val;
}

double Statistics::get_aver()
{
	return total_val / cnt_val;
}

double Statistics::get_max()
{
	return max_val;
}

double Statistics::get_min()
{
	return min_val;
}