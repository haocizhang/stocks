create table portfolio(company_code varchar(10), quantity int, unitvalue DOUBLE(15,2),totalvalue DOUBLE(15,2), PRIMARY KEY(company_code));
create table frozen(company_code varchar(10), quantity int, transactiontime DATETIME, unitvalue DOUBLE(15,2), totalvalue DOUBLE(15,2));
create table transactions(transactiontime DATETIME, company_code varchar(10), quantity int, unitvalue DOUBLE(15,2), totalvalue DOUBLE(15,2), transactiontype int);
