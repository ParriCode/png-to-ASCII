#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE_LENGTH 100
#define MAX_CHAR_PIXEL_LENGTH 3
int cmpfunc(const void *a,const void * b){
	return (*(int *) a - *(int *)b);
}
int size_csv_file(char * path,int* n_lines){
	FILE    *textfile;
	char    line[MAX_LINE_LENGTH];
	textfile = fopen(path, "r");
	if(textfile == NULL){
		return 1;}
	while(fgets(line, MAX_LINE_LENGTH, textfile)){
		*n_lines = *n_lines+1;
	}
	*n_lines = *n_lines-1;
	fclose(textfile);
	return 0;
}
int readcsv(char * datapath,int * colors_num,char **characters) {
    FILE    *textfile;
    char    line[MAX_LINE_LENGTH];
    int 	l = 0;
    textfile = fopen(datapath, "r");
    if(textfile == NULL){return 1;}
    char *token ;
    while(fgets(line, MAX_LINE_LENGTH, textfile)){
    	if(l > 0){
    		token = strtok(line,",");
    		strcpy(characters[l-1],token);
			token = strtok(NULL,",");
			colors_num[l-1] = atoi(token);
		}
    l++;
	}
    fclose(textfile);
    return 0;
}
int writecsv(char *datapath ,char **characters,int *colors_num,int n_lines){
	FILE *datafile;
	int locallist[n_lines];
	char localarr[n_lines][MAX_CHAR_PIXEL_LENGTH];
	for(int i = 0;i<n_lines;i++){
		locallist[i] = colors_num[i];
	}
	qsort(colors_num,n_lines,sizeof(int),cmpfunc);
	for(int i = 0;i<n_lines;i++){
		for(int j = 0;j<n_lines;j++){
			if(locallist[i] == colors_num[j]){
				strcpy(localarr[j],characters[i]);
			}
		}
	}
	for(int i = 0;i<n_lines;i++){
		strcpy(characters[i],localarr[i]);
	}

	datafile = fopen(datapath,"w");
	if(datafile == NULL){
		exit(1);
	}
	fprintf(datafile,"char,min");
	for(int i = 0;i<n_lines;i++){
		fprintf(datafile,"\n%s,%d",characters[i],colors_num[i]);
	}
	fclose(datafile);
	return 0;
}



