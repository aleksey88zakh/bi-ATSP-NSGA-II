//#include "GA_for_MTSP_source.h"

using namespace std;


template <typename T>
class Statistics
{
public:
	Statistics();
//	Statistics(T cur_val);

//	T total_val;
	double aver_val;
	T max_val;
	T min_val;
	int cnt_val;

	void set_to_zero();
	void refresh(T new_val);
	void init_max_min(T val);
	double get_aver();
//	T get_total_val();
	T get_min();
	T get_max();
	int get_cnt_val();
};


template <typename T>
Statistics<T>::Statistics()
{
//	total_val = 0;
	max_val = min_val = cnt_val = aver_val = 0;
}

//template <typename T>
//Statistics<T>::Statistics(T cur_val)
//{
//	total_val = cur_val;
//}

template <typename T>
void Statistics<T>::set_to_zero()
{
//	total_val = 0;
	max_val = min_val = cnt_val = aver_val = 0;
}

template <typename T>
void Statistics<T>::refresh(T new_val)
{
//	total_val += new_val;
	if (max_val < new_val)
		max_val = new_val;
	if (min_val > new_val)
		min_val = new_val;
	cnt_val++;

	if (1 == cnt_val)
		aver_val = new_val;
	else
		aver_val = aver_val*(cnt_val - 1)/cnt_val + new_val/((double)cnt_val);
}

template <typename T>
void Statistics<T>::init_max_min(T val)
{
	max_val = min_val = val;
}

template <typename T>
double Statistics<T>::get_aver()
{
	return aver_val;
}

//template <typename T>
//T Statistics<T>::get_total_val()
//{
//	return total_val;
//}

template <typename T>
T Statistics<T>::get_max()
{
	return max_val;
}

template <typename T>
T Statistics<T>::get_min()
{
	return min_val;
}

template <typename T>
int Statistics<T>::get_cnt_val()
{
	return cnt_val;
}
