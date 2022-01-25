/*
 * dataconfig.c
 *
 *  Created on: 15 ene 2022
 *      Author: luis
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <ncurses.h>
#include <menu.h>
#include <string.h>
#include <stdbool.h>
#include "csv.h"

#define MAX_CHAR_PIXEL_LENGTH 3
#define SIZE_Y 13 //size of the menu on y
#define CONSOLE_RED "\033[0;31m"
#define RESET_COLOR "\033[0m"
WINDOW *create_newwin(int height,int width,int starty,int startx);
int update_menu(WINDOW *menu,char **options,int n_options,int cursor);
int get_options(char *datapath,char **characters,int *colors_num,char **options,int n_options);
void errors(int errno);
void dataconfig(char *datapath){
	FILE *file;
	file = fopen(datapath,"r");
	if(file != NULL){
		fclose(file);
	}
	else{
		file = fopen(datapath,"w");
		fprintf(file,"char,min");
		fclose(file);
	}

	WINDOW *menu_win;
	int  i,ch,cursor,n_options;
	char newchar[MAX_CHAR_PIXEL_LENGTH];
	char firewall[50];
	i = cursor = n_options = 0;
	//Get the number of options of data file
	size_csv_file(datapath,&n_options);
    /* reserve memory for variables */
	int *colors_num;   //colors_num its the brightness of each pixel that is compared with the image
	colors_num = (int *  )malloc(n_options*sizeof(int));

	char **characters;	//this list of characters are the substitutes of each pixel on image
	characters = (char **)malloc(n_options*sizeof(char *));
	for(i = 0;i< n_options;i++){
		characters[i] = (char *)malloc(MAX_CHAR_PIXEL_LENGTH*sizeof(char));
	}
	char **options;    //this is an array list of menu option for this program
	options = (char**)malloc(n_options*sizeof(char *));
	for(i = 0;i<n_options;i++){
		options[i] = (char*)malloc(sizeof(char[8]));
	}
	char buffer[4]; // this variable is used as a bridge
	readcsv(datapath,colors_num,characters);	//get the pixels colors and characters from the data file
	for(i = 0;i<n_options;i++){                 //building the menu options with the data
			strcpy(options[i],characters[i]);
			strcat(options[i],"->");
			sprintf(buffer,"%d",colors_num[i]);
			strcat(options[i],buffer);
	}
	initscr();								//start the ncurses screen
	start_color();
	init_pair(1,COLOR_BLACK,COLOR_WHITE);   //normal console theme
	init_pair(2,COLOR_WHITE,COLOR_BLUE);	//selected on menu theme
	init_pair(3,COLOR_WHITE,COLOR_RED);     //selected exit button theme
	init_pair(4,COLOR_RED,COLOR_BLACK);     //deleted pixel character message
	keypad(stdscr, TRUE);                   //turn on keypad reader
	noecho();                               //turn off screen writing
	menu_win = create_newwin(20,80,0,0);    //create the principal window
	refresh();
	update_menu(menu_win,options,n_options,cursor);
	wrefresh(menu_win);
	refresh();
	while((ch != KEY_F(1))){
		ch = getch();					 	//waiting for keyboard event
		switch(ch){
			case KEY_DOWN:
				cursor++;
				if ( cursor%SIZE_Y == 0){     //if you press keydown in the lower part the cursor must going to "NEW" button
					cursor = n_options;       //n_options always will be the new button because n_option menu started ad 0 not at 1
				}
				break;
			case KEY_UP:
				cursor--;
				break;
			case KEY_LEFT:
				cursor = cursor - SIZE_Y;
				break;
			case KEY_RIGHT:
				cursor = cursor + SIZE_Y;
				if(cursor > n_options){cursor = n_options;}
				break;
			case 10: //10 =  enter key
				echo();
				if(cursor == n_options){
					n_options++;
					if(n_options > 60){
						errors(1);
					}
					//Adding more memory for the new element
					characters = (char **)realloc(characters,n_options*sizeof(char *));
					characters[n_options-1] = (char *)malloc(MAX_CHAR_PIXEL_LENGTH*sizeof(char)); //malloc for the space of the new element

					colors_num = (int *)realloc(colors_num,n_options*sizeof(int));

					options = (char**)realloc(options,n_options*sizeof(char *));
					options[n_options-1] = (char*)malloc(sizeof(char[8]));

					mvprintw(20,0,"Write the new character ->");
					scanw("%[^\n]s",firewall);    //firewall is for detect errors before write in the data file

					if(strlen(firewall)>1){errors(1);}  //the length of new character must be 1

					strcpy(newchar,firewall);
					strcpy(characters[n_options-1],newchar);

					mvprintw(20,0,"Write the brightness num of the new character->");
					scanw("%d",&colors_num[n_options-1]);

					if(colors_num[n_options-1] > 256){      errors(2);}
					if(colors_num[n_options-1] < 0){        errors(3);}

					get_options(datapath,characters,colors_num,options,n_options);
				}
				else if(cursor == n_options+1){
					/*EXIT BUTTON*/
					echo();
					endwin();
					exit(1);
				}
				else{
					mvprintw(20,0,"Write the brightness num of the character->");
					scanw("%d",&colors_num[cursor]);

					if(colors_num[cursor] > 256){      errors(2);}
					if(colors_num[cursor] < 0){        errors(3);}
				}
				writecsv(datapath,characters,colors_num,n_options);
				get_options(datapath,characters,colors_num,options,n_options);
				mvprintw(20,0,"                                                                ");
				mvprintw(0,0,"");

				noecho();
				refresh();
				break;
			case KEY_BACKSPACE:
				n_options--;
				attron(COLOR_PAIR(4));
				mvprintw(20,0,"The character %c has been deleted",characters[cursor][0]);
				mvprintw(20,0,"                                                                ");
				attroff(COLOR_PAIR(4));
				for(i = 0;i<(n_options-cursor);i++){
					characters[cursor+i] =  characters[cursor+i+1];
				    colors_num[cursor+i] =  colors_num[cursor+i+1];
					options[cursor+i]    =  options[cursor+i+1];
				}
				/*update memory for one less item*/
				characters = (char **)realloc(characters,n_options*sizeof(char *));
				colors_num = (int *  )realloc(colors_num,n_options*sizeof(int   ));
				options    = (char** )realloc(options,n_options   *sizeof(char *));

				writecsv(datapath,characters,colors_num,n_options);       //write on the data file
				refresh();
				break;
			default:
				break;
		}
		if(cursor < 0){cursor = 0;}
		if(cursor > n_options+1){cursor = n_options+1;}
		update_menu(menu_win,options,n_options,cursor);
		wrefresh(menu_win);
		mvprintw(0,0,"");
		refresh();
	}
}
WINDOW *create_newwin(int height, int width, int starty, int startx){
	WINDOW *local_win;
	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters		*/
	wrefresh(local_win);		/* Show that box 		*/
	return local_win;
}
int update_menu(WINDOW *menu_win,char **options,int n_options,int cursor){
	/* FUNCTION FOR UPDATE MENU
	 * ========================*/
	int x = 0;
	int y = 0;
	for(int i = 0;i < 18;i++){
		//fill all with spaces
		mvwprintw(menu_win,i+1,1,"                                                                              ");
	}
	mvwprintw(menu_win,1,30,"CARACTERES");
	mvwprintw(menu_win,2,1,"==============================================================================");
	for(int i = 0;i < n_options;i++){
		if(i>(SIZE_Y-1)*(x+1)){
			x++;}
		y = 3+i-(SIZE_Y*x);
		if(cursor == i){
			wattron(menu_win,COLOR_PAIR(2));
			mvwprintw(menu_win,y,2+15*x,"%s  ",options[i]);
			wattroff(menu_win,COLOR_PAIR(2));
		}
		else{

			mvwprintw(menu_win,y,2+15*x,"%s  ",options[i]);
		}
	}
	if(cursor == n_options){
		wattron(menu_win,COLOR_PAIR(2));
		mvwprintw(menu_win,17,0,"                                  NEW                                           ");
		wattroff(menu_win,COLOR_PAIR(2));
	}
	else{
		mvwprintw(menu_win,17,0,"                                  NEW                                           ");
	}
	if(cursor == n_options+1){
		wattron(menu_win,COLOR_PAIR(3));
		mvwprintw(menu_win,18,0,"                                  QUIT                                          ");
		wattroff(menu_win,COLOR_PAIR(3));
	}
	else{
		mvwprintw(menu_win,18,0,"                                  QUIT                                          ");
	}
	wrefresh(menu_win);
	return 0;
}
int get_options(char *datapath ,char **characters,int *colors_num,char **options,int n_options){
	/*Function for read data file and update options array*/
	char buffer[4];
	readcsv(datapath,colors_num,characters);
	for(int i = 0;i<n_options;i++){
		strcpy(options[i],characters[i]);
		strcat(options[i],"->");
		sprintf(buffer,"%d",colors_num[i]);
		strcat(options[i],buffer);
	}
	return 0;
}
void errors(int errno){
	/*Error manager*/
	echo();
	endwin();
	char * errmsg;
	if(errno == 1){
		errmsg = "the entry its too long";
	}
	else if(errno == 2){
		errmsg = "The max brightness is 256";
	}
	else if(errno == 3){
		errmsg = "The minimal brightness is 0";
	}
	else if(errno == 4){
		printf("To much elements\n");
	}
	printf(CONSOLE_RED"ERROR :%s\n"RESET_COLOR,errmsg);
	exit(1);
}









