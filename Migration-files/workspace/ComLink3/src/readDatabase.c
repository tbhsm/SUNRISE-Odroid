/*
 * readDatabase.c
 *
 *  Created on: Sep 28, 2015
 *      Author: odroid
 */

#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <stdint.h>

void readData(){
	/*Declaration of variables, be so kind do not EDIT this part*/
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	int num_fields;
	int i;
	char *server = "localhost";
	char *user = "root";
	char *password = "odroid";
	char *database = "odroid";
	conn = mysql_init(NULL);
	/*end of super important stuff*/
	/*CONNECTING*/
	if ( !mysql_real_connect(conn, server, user, password, database, 0, NULL,0)){
		printf(stderr,"%s\n",mysql_error(conn));
		exit(1);
	}
	/*query mysql*/
	if (mysql_query(conn,"SHOW tables")){
		printf(stderr,"%s\n",mysql_error(conn));
		exit(1);
	}
	/*initiate res & show info*/
	res = mysql_use_result(conn);
	printf("MYSQL database victron says: \n");
	while ((row = mysql_fetch_row(res)) != NULL){
		printf("%s \n",row[0]);
	}
	/*show contents of the database*/
	if (mysql_query(conn,"SELECT*FROM VictronTable")){
		printf(stderr,"%s\n",mysql_error(conn));
		exit(1);
	}
	res = mysql_store_result(conn);
	num_fields = mysql_num_fields(res);
	while ((row = mysql_fetch_row(res)) != NULL){
		for (i=0;i<num_fields;i++){
			printf("%s \n",row[i] ? row[i]:"NULL");
		}
	}
	/*mischief managed --> CLOSE CONNECTION*/
//	mysql_free_result(free);
	mysql_close(conn);

	puts("lol");
}
/*!!!!!!!!!!!!!!!!!NEW FUNCTION FOR WRITING!!!!!!!!!!!!!!!!!!!*/
void writeData(int START, int END, uint16_t *tab_register){
	/*Declaration of variables, be so kind do not EDIT this part*/
		MYSQL *conn;
		MYSQL_RES *res;
		MYSQL_ROW row;
		int num_fields;
		int i;
		char *server = "localhost";
		char *user = "root";
		char *password = "odroid";
		char *database = "odroid";
		conn = mysql_init(NULL);
		/*end of super important stuff*/

		/*CONNECTING*/

		if ( !mysql_real_connect(conn, server, user, password, database, 0, NULL,0)){
			printf(stderr,"%s\n",mysql_error(conn));
			exit(1);
		}
		/*query mysql*/
		if (mysql_query(conn,"SHOW tables")){
			printf(stderr,"%s\n",mysql_error(conn));
			exit(1);
		}
		/*initiate res & show info*/
		res = mysql_use_result(conn);
		printf("MYSQL database odroid says: \n");
		while ((row = mysql_fetch_row(res)) != NULL){
			printf("%s \n",row[0]);
		}
		/*write to the DATABASE*/
		char buf[50];
		char buf2[50];

		for(i=0;i<=END-START;i++){

			sprintf(buf,"UPDATE victron SET VALUE= %d", tab_register[i]);
			sprintf(buf2," WHERE Address= %d", START+i);
			strcat(buf,buf2);
			char *m;
			m = strchr(buf,NULL);
			int e = (int)(m-buf);
			char buf3[e];
			for(i=0;i<=e;i++){
				buf3[i]=buf[i];
			}

			printf(" \n %s \n",buf3);

			if (mysql_query(conn,buf3)){
			printf(stderr,"%s\n",mysql_error(conn));
			exit(1);
			}
		}

	/*	res = mysql_store_result(conn);
		num_fields = mysql_num_fields(res);
		while ((row = mysql_fetch_row(res)) != NULL){
			for (i=0;i<num_fields;i++){
				printf("%s \n",row[i] ? row[i]:"NULL");
			}
		}*/
		/*mischief managed --> CLOSE CONNECTION*/
		/*mysql_free_result(free);*/
		mysql_close(conn);

		puts("chaha");
}
