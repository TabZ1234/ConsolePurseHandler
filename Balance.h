#pragma once
#include"commonfunctions.h"

using namespace std;


#define ERR error(convert_cstr_to_wstring(err.what()));

#define u16_to_u8 wstring_convert<codecvt_utf8_utf16<wchar_t>>
#define u8_to_u16 wstring_convert<codecvt_utf8<wchar_t>>


template<class type1, class type2, class type3>
class Balance
{
private:
	tm time;
	type1 balance;
	type2 debt;
	type3 description;

	fstream file;
	const wstring file_name;

	void parseTime(const int values_count,
		vector<wstring>& init_vect_str, 
		vector<int>& init_vect, wstring data_str)
	{
		wstring::iterator wstr_it;
		vector<wstring>::iterator vect_it;
		vector<int>::iterator init_vect_it;


		for (wstr_it = data_str.begin(),
			vect_it = init_vect_str.begin();
			wstr_it != data_str.end(); wstr_it++)
		{
			if (*wstr_it == L'.')vect_it++;
			else {
				*vect_it += *wstr_it;
			}
		}

		try
		{
			for (init_vect_it = init_vect.begin(),
				vect_it = init_vect_str.begin(); 

				vect_it != init_vect_str.end() && init_vect_it != init_vect.end();

				vect_it++,
				init_vect_it++)
			{
				*init_vect_it = stoi(*vect_it);
			handled: {}
			}
		}
		catch (const invalid_argument&)
		{
			*init_vect_it = 0;
			goto handled;
		}
	}
	void set_scalar(const wstring& arg, int& scalar)
	{
		try
		{
			scalar = stoi(arg);
		}
		catch (const invalid_argument&)
		{
			scalar = 0;
		}
	}
	void initTime()
	{
		__time64_t long_time;
		_time64(&long_time);
		_localtime64_s(&time, &long_time);

		time.tm_mon++;//1-12
	}
	void initTime(const tm& obj_time)
	{
		time = obj_time;
	}
public:
	static constexpr char block_delimiter{ u8'~' };
	static constexpr char field_delimiter{ u8'|' };

	Balance(type1 bal, type2 db, type3 dscrptn, wstring fname)
		:balance{ bal },
		debt{ db },
		description{ dscrptn },
		file_name{ fname }
	{

		initTime();
	}
	Balance()
	{

	}
	Balance(const Balance& obj)
		:balance{ obj.balance },
		debt{ obj.debt },
		description{ obj.description },
		file_name{ obj.file_name },
		time{obj.get_time()}
	{

	}
	void print()
	{
		wostringstream ss;

	  	 ss << L"Д : " << field_delimiter << time.tm_mon << L'.' << time.tm_mday << L'.' << time.tm_year - 100 << field_delimiter << L'\n'
			<< L"В : " << field_delimiter << time.tm_hour << L'.' << time.tm_min << L'.' << time.tm_sec << field_delimiter << L'\n'
			<< L"Б : " << field_delimiter << balance << field_delimiter << L'\n'
			<< L"Д : " << field_delimiter << debt << field_delimiter << L'\n'
			<< L"К : " << field_delimiter << description << field_delimiter << L'\n' << block_delimiter << L'\n';

		wstring u16buff = ss.str();
		u16_to_u8 convert_out;
		string u8buff = convert_out.to_bytes(u16buff);

		try
		{
			file.open(file_name, ios::binary | ios::out | ios::app);
			file.write(u8buff.c_str(), u8buff.size());
			file.close();
		}
		catch (const ios_base::failure& err)
		{
			ERR
#ifdef NDEBUG
			throw FATAL_ERROR;
#endif
		}
	}
	void set_date(wstring date_str)
	{
		constexpr int values_count = 3;
		vector<wstring> init_vect_str(values_count);
		vector<int> init_vect(values_count);

		parseTime(values_count, init_vect_str, init_vect, date_str);
		
		time.tm_mon  = init_vect[0];
		time.tm_mday = init_vect[1];
		time.tm_year = init_vect[2] + 100;
	}
	void set_time(wstring time_str)
	{
		constexpr int values_count = 3;
		vector<wstring> init_vect_str(values_count);
		vector<int> init_vect(values_count);

		parseTime(values_count, init_vect_str, init_vect, time_str);

		time.tm_hour = init_vect[0];
		time.tm_min  = init_vect[1];
		time.tm_sec  = init_vect[2];
	}
	void  set_balance(wstring bal_str)
	{
		set_scalar(bal_str, balance);
	}
	void set_debt(wstring db_str)
	{
		set_scalar(db_str, debt);
	}
	void  set_description(wstring dscrptn_str)
	{
		description = dscrptn_str;
	}
	wstring get_date_str()const
	{
		wostringstream ss;
		ss << time.tm_mon << L'.' << time.tm_mday << L'.' << time.tm_year - 100;
		return ss.str();
	}
	wstring get_time_str()const
	{
		wostringstream ss;
		ss << time.tm_hour << L'.' << time.tm_min << L'.' << time.tm_sec;
		return ss.str();
	}
	tm get_time()const
	{
		return time;
	}
	type1 get_balance()const
	{
		return balance;
	}
	type2  get_debt()const
	{
		return debt;
	}
	type3  get_description()const
	{
		return description;
	}

	bool operator <(const Balance& obj)const
	{
		if (time.tm_year < obj.time.tm_year)
			return true;

		if (time.tm_year == obj.time.tm_year &&
			time.tm_mon < obj.time.tm_mon)
			return true;

		if (time.tm_year == obj.time.tm_year &&
			time.tm_mon == obj.time.tm_mon &&
		    time.tm_mday < obj.time.tm_mday)
			return true;

		if (time.tm_year == obj.time.tm_year &&
			time.tm_mon == obj.time.tm_mon &&
			time.tm_mday == obj.time.tm_mday && 
			time.tm_hour < obj.time.tm_hour
			)return true;

		if (time.tm_year == obj.time.tm_year &&
			time.tm_mon == obj.time.tm_mon &&
			time.tm_mday == obj.time.tm_mday &&
			time.tm_hour == obj.time.tm_hour &&
			time.tm_min < obj.time.tm_min
			)return true;

		if (time.tm_year == obj.time.tm_year &&
			time.tm_mon == obj.time.tm_mon &&
			time.tm_mday == obj.time.tm_mday &&
			time.tm_hour == obj.time.tm_hour &&
			time.tm_min == obj.time.tm_min &&
			time.tm_sec < obj.time.tm_sec
			)return true;

		return false;
	}
	
	Balance& operator=(const Balance& obj)
	{
		time = obj.time;
		balance = obj.balance;
		debt = obj.debt;
		description = obj.description;

		return *this;
	}
};

