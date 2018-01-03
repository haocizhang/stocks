#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <mysql/mysql.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include "mysql_connection.h"
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
//#include <folly/SpinLock.h>
#include <vector>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace std;

#define READ_FD     0
#define WRITE_FD    1


//std::mutex db_lock;
string doubletostring(double a) {
	stringstream ss;
	ss<<a;
	string r;
	ss>>r;
	return r;
}
double stringtodouble(string a) {
	stringstream ss;
	ss<<a;
	double r;
	ss>>r;
	return r;
}
string inttostring(int a) {
	stringstream ss;
	ss<<a;
	string r;
	ss>>r;
	return r;
}
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}


int execute_python_file(std::string &result, std::string filename,
						std::vector<string> params = std::vector<string>()) {

	std::string command = "/usr/bin/python3 " + filename;
	int fd[2], savestdout, res, ret, message_size = 5000;
	pid_t pid;

	for(unsigned int i = 0; i < params.size(); i++)
		command += (" " + params[i]);

	pipe(fd);
	//forking 1 process and communicating through pipes. Child process executes python file.
	if ((pid = fork()) == -1) {
        	cout << "Fork Failed :: execute_python_file()" << endl;
        	exit(1);
    	}
	if (pid == 0) {
    		close(fd[READ_FD]);
    		savestdout = dup(1);
    		dup2(fd[WRITE_FD], 1);
		res = system(command.c_str());
		close(fd[WRITE_FD]);
		dup2(savestdout, 1);
		if (res != 0) {
			cout<<"Python file execution failed :: execute_python_file()"<<endl;
			exit(1);
		}
		exit(0);
	} else {
		close(fd[WRITE_FD]);
		char *message = new char[message_size];
		ret = read(fd[READ_FD], message, message_size); //5000 is sure bigger than size.
		std::string str(message);
		result = str;
		if (ret == 0) {
			cout<<"Reading for file descriptor unsuccessful :: execute_python_file()"<<endl;
			exit(1);
		}
		close(fd[READ_FD]);
		wait(NULL);
		free(message);
	}
	return 0;
}

int db_connector(std::string host, std::string user, std::string pass,
		std::string dbname, int port) {
	return 0;
}


int get_inventory_status(double &cash, double &value){
	std::string json;
	int ret = execute_python_file(json, "../api_python/getvalue.py");
	cout<<json.length()<<endl;
	if (json.length() == 0){
		cout<<"Error in get_inventory_status()::json invalid(length 0)."<<endl;
		return 1;
	}	
	int pos = -1;
	for (int i = 0; i < json.length(); i++) {
		if (json[i] == '}') {
			pos = i;
			break;
		}
	}
	if (pos == -1) {
		cout<<"Error in get_inventory_status()::json invalid."<<endl;
		return -1;
	}
	json = json.substr(0, pos + 1);
	rapidjson::Document d;
	d.Parse(json.c_str());
	rapidjson::Value& cash_value = d["cash"];
	rapidjson::Value& account_val = d["account_val"];
	cash = cash_value.GetDouble();
	value = account_val.GetDouble();
	cout<<"Available Cash:(Including Frozen Cash) "<<fixed<<cash_value.GetDouble()<<endl;
	cout<<"Account Total Value: "<<fixed<<value<<endl;
	return 0;
}

int get_stock_price(string company_code, double &stock_price){
	string price_str;
	vector<string> param;
	param.push_back(company_code);
	int ret = execute_python_file(price_str, "../api_python/stockprice.py", param);
	if (ret != 0) {
		cout<<"Error in get_stock_price()"<<endl;
		return 1;
	}
	stock_price = stringtodouble(price_str);
	return 0;
}

int buy(string company_code, int volume) {
	double cash, value;
	if (get_inventory_status(cash, value) != 0) {
		cout<<"Error in buy() :: get inventory status failed"<<endl;
		return 1;	
	}
	double stock_price = 0;
	if (get_stock_price(company_code, stock_price) != 0) {
		cout<<"Error in buy():: get stock price failed"<<endl;
		return 1;	
	}
	if (cash < stock_price * volume * 1.1 || volume <= 0) {
		cout<<"Buy Failed :: Available cash low or volume invalid"<<endl;
		return 1;	
	}
	vector<string> params;
	params.push_back(company_code);
	params.push_back(inttostring(volume));
	string date_time = currentDateTime();
	int ret = 0;//execute_python_file(json, "../api_python/buystock.py", params);
	if (ret != 0) {
		cout<<"Error in buy() :: Fail calling python file."<<endl;
		return 1;	
	}
	try {
		sql::Driver *driver;
		sql::Connection *con;
		sql::Statement *stmt;
		sql::ResultSet *res;
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root", "1234");
		con->setSchema("stocks");
		stmt = con->createStatement();
		string sql = "INSERT INTO frozen VALUES ('" + company_code + "'," + inttostring(volume)
				+ ",'" + date_time + "'," + doubletostring(stock_price) + ","
				+ doubletostring(stock_price * volume) + ");";
		stmt->execute(sql);
		sql = "INSERT INTO transactions VALUES ('" + date_time + "','" + company_code + "',"
			+ inttostring(volume) + "," + doubletostring(stock_price) + ","
			+ doubletostring(stock_price * volume) + ",1);";
		stmt->execute(sql);
		//string result;
		//vector<string> params;
		//params.push_back(company_code);
		//params.push_back(inttostring(volume));
		//execute_python_file(result, "../api_python/buy.py", params);
		delete res;
		delete stmt;
		delete con;
	} catch(std::exception& e) {
		cout<<"Error in buy() :: Database error"<<endl;
		return 1;
	}
	return 0;
}

int sell(std::string company_code, int volume) {
	try {
		std::string sql = "SELECT quantity, unitvalue FROM portfolio WHERE company_code = '" + company_code + "';";
		sql::Driver *driver;
		sql::Connection *con;
		sql::Statement *stmt;
		sql::ResultSet *res;
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root", "1234");
		con->setSchema("stocks");
		stmt = con->createStatement();
		res = stmt->executeQuery(sql);
		int count = 0, larger = 1;
		double unitvalue = 0;
		while(res->next()) {
			count++;
			int v = res->getInt(1);
			unitvalue = res->getDouble(2);
			if (v < volume) {
				cout<<"Error in sell() :: quantity too large"<<endl;
				delete res;
				delete stmt;
				delete con;
				return 1;			
			}
			//string result;
			//vector<string> params;
			//params.push_back(company_code);
			//params.push_back(inttostring(volume));
			//execute_python_file(result, "../api_python/sell.py", params);
			sql = "DELETE FROM portfolio WHERE company_code = '" + company_code + "';";
			stmt->execute(sql);
			if (v - volume > 0) {
				sql = "INSERT INTO portfolio VALUES ('" + company_code + "'," + 
						inttostring(v - volume) + "," + doubletostring(unitvalue) + "," +
						doubletostring((v - volume) * unitvalue) + ");";
				stmt->execute(sql);
			}
			delete res;
			delete stmt;
			delete con;
			break;
		}
		if (count == 0) {
			cout<<"Error in sell() :: Company not found in portfolio"<<endl;
			return 1;
		}
	} catch(std::exception& e) {
		cout<<"Error in sell() :: Database error"<<endl;
		return 1;
	}
	return 0;
	
}


int main() {
	std::string result;
	cout<<currentDateTime()<<endl;
	//db_connector("localhost", "root", "1234", "stocks", 3306);
	//execute_python_file(result, "../api_python/login.py");
	double a;
	//get_inventory_status(a,a);
	//buy("GOOG", 10);
	sell("GOOG", 1);
	//cout<<result<<endl;
	return 0;
}
