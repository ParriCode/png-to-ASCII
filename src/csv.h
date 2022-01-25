/*
 * csv.h
 *      Author: theparri@protonmail.com
 */
#ifndef CSV_H_
#define CSV_H_
int size_csv_file(char * path,int* n_lines);                                 //Get data file size
int readcsv(char * datapath,int * colors_num,char **characters);             //Read data from data file
int writecsv(char *datapath,char **characters,int colors_num[],int n_lines); //write data on datafile

#endif /* CSV_H_ */
