g++ -g -o0 -I/usr/local/include -I/usr/local/boost/include -c test.cpp -o test.o
g++ -g -o0 -L/usr/local/lib -L/usr/local/mysql/lib test.o -o test -lmysqlcppconn
