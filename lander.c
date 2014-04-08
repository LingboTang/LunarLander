/*
name:               Xuping Fang
ONE Card number:    1365512
Unix id:            xuping
lecture section:    A2
instructor's name:  Davood Rafiei
lab section:        D06
TA's name:          Ali Yaclollahi           
*/
#define _POSIX_C_SOURCE 200112L

#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#include<math.h>
#include<assert.h>
#include<curses.h>
#include<signal.h>
#include<sys/time.h>
#include"lander.h"

#define PI acos(-1.0)

//pipe
FILE* executable;

// landscape file
FILE* landscape;

const char exec_name[] = "java -jar Sketchpad.jar";

//ship points
point ship[5];

//ship degree
long ship_degree;

//landscape_points

point LANDSCAPE[25];

int point_cnt;
//signal blocker
sigset_t block_mask_g;

//switches:

int thrust_sw,over,I;

//fuel
double fuel;

//velocity&acceleration:

double xV,yV;

//g,t

double gravity,thrust;

int main(int argc,char *argv[]){
    I=0;
    int argv_index=1;
    for(argv_index=1;argv_index<argc;argv_index++){
	if(argv[argv_index][0]=='-'){
		if(strlen(argv[argv_index])==2){
			if((argv[argv_index][1])=='g'){
				gravity=atof(argv[argv_index+1]);
			}
			else if((argv[argv_index][1])=='t'){
				thrust=atof(argv[argv_index+1]);
			}
			else if((argv[argv_index][1])=='f'){
				landscape=fopen(argv[argv_index+1],"r");
			}
                        else if((argv[argv_index][1])=='i'){
                                I=1;
                        }
		}
		else if(strlen(argv[argv_index])>2){
			if((argv[argv_index][1])=='g'){
				gravity=atof(argv[argv_index]+2);
			}
			else if((argv[argv_index][1])=='t'){
				thrust=atof(argv[argv_index]+2);
			}
			else if((argv[argv_index][1])=='f'){
				landscape=fopen(argv[argv_index]+2,"r");
			}
		}
	}
    }
    fuel=10;
    ship_degree=90;
    // curses set up------------------------------------------
    setup_curses();
    open_pipe(exec_name);
    move(5, 10);
    printw("Press any key to start.\n");
    printw("          (Then press arrow keys to rotate, space for thust, 'q' to quit)");
    refresh();
    int cmd = getch();
	
    nodelay(stdscr, true);
    erase();
    move(5, 0);
    printw("left arrow key rotate counter-clockwise, right clockwise, space for thust, q to quit.");
	
    //set initial location of the ship-----------------
	
    //left_up point
    ship[0].x=315;
    ship[0].y=20;
    
    //right_up point
    ship[1].x=325;
    ship[1].y=20;
    
    //left_down point
    ship[2].x=305;
    ship[2].y=50;
    
    //right_down point
    ship[3].x=335;
    ship[3].y=50;
    
    //thrust point
    ship[4].x=320;
    ship[4].y=60;

    // set all switches-----------------------------
    over=0;
    thrust_sw=0;
    //set_velocity--------------------------------------
    xV=0;yV=0;
    //draw the landscape
    draw_landscape(landscape);
    if(I==1){
        draw_fuel_rec();
    }
    //set_timer-------------------------------------
    sigemptyset(&block_mask_g);
    sigaddset(&block_mask_g, SIGALRM);
    struct sigaction handler;
    handler.sa_handler = handle_timeout;
    sigemptyset(&handler.sa_mask);
    handler.sa_flags = 0;
    if (sigaction(SIGALRM, &handler, NULL) == -1){
        exit(EXIT_FAILURE);
    }
    struct itimerval timer;
        struct timeval time_delay;
        time_delay.tv_sec = 0;
        time_delay.tv_usec = 50000;
        timer.it_interval = time_delay;
        struct timeval start;
        start.tv_sec = 0;
        start.tv_usec = 50000;
        timer.it_value = start;
    if (setitimer(ITIMER_REAL, &timer, NULL) == -1){
        exit(EXIT_FAILURE);
    }
    while(1){
        cmd = getch();
        if (cmd != ERR){
            if (cmd == KEY_LEFT){
                if(over==0){
                    blocking_signal_for_rotate(1);  //rotate left
                }
            }
            else if(cmd == KEY_RIGHT){
                if(over==0){
                    blocking_signal_for_rotate(0);  //rotate right
                }
            }
            else if(cmd == ' '){
                if(I==0){
                    thrust_sw=1;
                }
                else if(I==1){
                    if(fuel<=0){
                        move(8,10);
                        refresh();
                        printw("Empty fuel tank ! ! ! !");
                    }
                    else if(fuel>0){
                        thrust_sw=1;
                    }
                }
            }
            else if(cmd == 'q'){
                // block the signal
                sigset_t old_mask;
                if (sigprocmask(SIG_BLOCK, &block_mask_g, &old_mask) < 0){
                    exit(EXIT_FAILURE);
                }
                break;
            }
        }
    }
    if(executable!=NULL&&over==0){
        fprintf(executable,"end\n");
    }
    unset_curses();
    //-----------------------------------------------------------
    fclose(landscape);
    close_pipe();
    exit(EXIT_SUCCESS);
}
