Installation Of MYSQL on Linux:

sudo apt-get install mysql-server mysql-client libmysqlclient-dev
Important: When initializing database, set a password!!!

start db: sudo service mysql start



Connecting to MYSQL Database:

1. Installing Boost
	a. sudo apt-get install libboost-dev

2. Installing CMake
	a. Download CMake: wget http://www.cmake.org/files/v2.8/cmake-2.8.3.tar.gz
	b. Extraction: 
		tar xzf cmake-2.8.3.tar.gz
		cd cmake-2.8.3
	c. Compile and install: 
		./bootstrap
		make
		sudo make install
	d. Verify that cmake is installed succesfully: cmake --version, the output should be something like cmake version 2.8.3

3. Installing MYSQL Connector
	a. Download https://dev.mysql.com/downloads/connector/cpp/ (For Operating System, Choose Source Code instead of linux (this is important!))
	b. Extract it in the target folder
	c. cd /path/to/mysql-connector-cpp
	d. cmake .
	e. make clean
	f. make
	g. sudo make install
	h. export LD_LIBRARY_PATH=/usr/local/lib
	

script to verify if installation of connector is successfull:


#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>

static char *host = "localhost";
static char *user = "root";
static char *pass = "pass";
static char *dbname = "stocks";

unsigned int port = 3306;
static char *unix_socket = NULL;
unsigned int flag = 0;

int main() {
	MYSQL *conn;
	conn = mysql_init(NULL);

	if (!(mysql_real_connect(conn, host, user, pass, dbname, port, unix_socket, flag))) {
		fprintf(stderr, "\nError: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
		exit(1);
	}
	printf("Connection Successful!\n\n");

	return EXIT_SUCCESS;
}


Compile Command: gcc -o program $(mysql_config --cflags) Connect.c $(mysql_config --libs)
Execute:		 ./program

