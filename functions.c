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
#include <signal.h>
#include <sys/time.h>
#include <curses.h>
#include"lander.h"

#define PI acos(-1.0)

extern FILE* executable;

extern FILE* landscape;

extern point ship[5];

extern point LANDSCAPE[25];

extern int point_cnt;

extern long ship_degree;

extern int thrust_sw,over,I;

extern double fuel;

extern double xV,yV;

extern double gravity,thrust;

extern sigset_t block_mask_g;

void open_pipe(const char* exec_name){
    executable = popen(exec_name, "w");
    if (executable == NULL){
        fprintf(stderr, "Could not open pipe %s\n", exec_name);
    }
}

void close_pipe(){
    pclose(executable);
}

void set_green(){
    fprintf(executable, "setColor 0 255 0\n");
}

void set_blue(){
    fprintf(executable, "setColor 0 0 255\n");
}

void set_purple(){
    fprintf(executable, "setColor 200 0 255\n");
}

void draw_line(point A,point B){
    fprintf(executable, "drawSegment %ld %ld %ld %ld\n",lround(A.x),lround(A.y),lround(B.x),lround(B.y));
}

void erase_line(point A,point B){
    fprintf(executable, "eraseSegment %ld %ld %ld %ld\n",lround(A.x),lround(A.y),lround(B.x),lround(B.y));
}

double angle_to_radian(long angle){
    double radian;
    radian = angle*PI/180.0;
    return radian;
}

void rotate_point_by_ten_degree_CW(point* P){
    double rotated_x,rotated_y;
    rotated_x = (P->x*cos(angle_to_radian(10)))-(P->y*sin(angle_to_radian(10)));
    rotated_y = (P->x*sin(angle_to_radian(10)))+(P->y*cos(angle_to_radian(10)));
    P->x=rotated_x;
    P->y=rotated_y;
}

void rotate_point_by_ten_degree_CCW(point* P){
    double rotated_x,rotated_y;
    rotated_x = (P->x*cos(angle_to_radian(350)))-(P->y*sin(angle_to_radian(350)));
    rotated_y = (P->x*sin(angle_to_radian(350)))+(P->y*cos(angle_to_radian(350)));
    P->x=rotated_x;
    P->y=rotated_y;
}

void draw_ship(){
    set_blue();
    draw_line(ship[0],ship[1]);
    draw_line(ship[0],ship[2]);
    draw_line(ship[1],ship[3]);
    draw_line(ship[2],ship[3]);
}

void draw_thrust(){
    set_purple();
    draw_line(ship[2],ship[4]);
    draw_line(ship[3],ship[4]);
}

void erase_ship(){
    erase_line(ship[0],ship[1]);
    erase_line(ship[0],ship[2]);
    erase_line(ship[1],ship[3]);
    erase_line(ship[2],ship[3]);
}

void erase_thrust(){
    erase_line(ship[2],ship[4]);
    erase_line(ship[3],ship[4]);
}

//problem in this function
void draw_landscape(FILE* landscape){
    set_green();
    memset(LANDSCAPE,0,25*sizeof(point));
    long index=0;
    long x_value,y_value;
    char line[257];
    memset(line,0,257);
    while (fgets(line,257,landscape) != NULL){
        sscanf(line,"%ld%ld",&x_value,&y_value);
        memset(line,0,257);
        LANDSCAPE[index].x=x_value;
        LANDSCAPE[index].y=y_value;
        index++;
    }
    long prev_index=0;
    point_cnt=index;
    for(prev_index=0;prev_index<index-1;prev_index++){
        draw_line(LANDSCAPE[prev_index],LANDSCAPE[prev_index+1]);
    }
}

// functions for curses:

void setup_curses(){
  // use return values.  see man pages.  likely just useful for error
  // checking (NULL or non-NULL, at least for init_scr)
  initscr();
  cbreak();
  noecho();
  // needed for cursor keys (even though says keypad)
  keypad(stdscr, true);
}

void unset_curses(){
  keypad(stdscr, false);
  nodelay(stdscr, false);
  nocbreak();
  echo();
  endwin();
}

//timer functions

void handle_timeout(int signal){
  static int called = 0;

  called++;

  // called because of SIGALRM
  if (signal == SIGALRM){
    // gets timer, puts it into timer (man 2 getitimer)
    struct itimerval timer;
    if (getitimer(ITIMER_REAL, &timer) == -1){
        exit(EXIT_FAILURE);
    }
    //block the signal before the process--------------------
    sigset_t old_mask;
    if (sigprocmask(SIG_BLOCK, &block_mask_g, &old_mask) < 0){
        exit(EXIT_FAILURE);
    }
    //-------------------------------------------------------
    //warp around the left_right side if necessary:
    int WARP_INDEX=1;
    double MAX_X=ship[0].x;
    double MIN_X=ship[0].x;
    for(WARP_INDEX=1;WARP_INDEX<5;WARP_INDEX++){
        if(ship[WARP_INDEX].x>MAX_X){
            MAX_X=ship[WARP_INDEX].x;
        }
        if(ship[WARP_INDEX].x<MIN_X){
            MIN_X=ship[WARP_INDEX].x;
        }
    }
    WARP_INDEX=0;
    if(MAX_X<=0){
        for(WARP_INDEX=0;WARP_INDEX<5;WARP_INDEX++){
            ship[WARP_INDEX].x+=640;
        }
    }
    else if(MIN_X>=640){
        for(WARP_INDEX=0;WARP_INDEX<5;WARP_INDEX++){
            ship[WARP_INDEX].x-=640;
        }
    }
    //add velocity ; fall due to gravity; addtional to thrust:
    int ship_index=0;
    //erase the ship and draw a new one
    erase_ship();
    erase_thrust();
    for(ship_index=0;ship_index<5;ship_index++){
        if(thrust_sw==0){
            ship[ship_index].y+=(yV*0.05)+(0.5*gravity*0.05*0.05);
        }
        else if(thrust_sw==1){
            ship[ship_index].y+=(yV*0.05)+(0.5*(gravity+(thrust*sin(angle_to_radian(ship_degree))))*0.05*0.05);
        }
        if(xV!=0){
            ship[ship_index].x+=(xV*0.05)+(0.5*(thrust*cos(angle_to_radian(ship_degree)))*0.05*0.05);
        }
    }
    draw_ship();
    //add velocity-----------------------------
    if(thrust_sw==0){
        yV+=gravity*0.05;
    }
    else if(thrust_sw==1){
        draw_thrust();
        yV+=0.05*(gravity+(thrust*sin(angle_to_radian(ship_degree))));
        xV+=0.05*thrust*cos(angle_to_radian(ship_degree));
        thrust_sw=0;
        if(I==1){
            fuel-=0.05;
            erase_fuel_rec(fuel);
        }
    }
    //-----------------------------------------
    fflush(executable);
    erase_thrust();
    //check_intersection-------------------------
    //index
    int l_index=0;
    //check
    int land_index;
    for(l_index=0;l_index<point_cnt-1;l_index++){
        if((check_intersection(LANDSCAPE[l_index],LANDSCAPE[l_index+1],ship[0],ship[1])==1)||(check_intersection(LANDSCAPE[l_index],LANDSCAPE[l_index+1],ship[0],ship[2])==1)||(check_intersection(LANDSCAPE[l_index],LANDSCAPE[l_index+1],ship[1],ship[3])==1)||(check_intersection(LANDSCAPE[l_index],LANDSCAPE[l_index+1],ship[2],ship[3])==1)){
            over=1;
            land_index=l_index;
            break;
        }
    }
    //improvement
    if(I==1){
        move(7,10);
        double TIME_USED=called/20;
        printw("Time used:  %lf",TIME_USED);
        move(8,10);
        if(fuel>0){
            printw("Fuel amount:  %lf",fuel);
        }
        refresh();
    }
    //---------------------
    // game is done (e.g., ship has crashed or landed)
    if (over==1){
        timer.it_interval.tv_usec = 0;
        timer.it_value.tv_usec = 0;
        fprintf(executable,"end\n");
        if(I==0){move(7,10);}
        if(I==1){move(9,10);}
        if((yV<20)&&(ship_degree==90)&&(LANDSCAPE[land_index].y==LANDSCAPE[land_index+1].y)){
            printw("LANDED ! ! !");
        }
        else{
            printw("CRASHED ! ! !");
        }
    }
    //unblock the signal
    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0){
        exit(EXIT_FAILURE);
    }
    //-------------------------------------------------------
    if (setitimer(ITIMER_REAL, &timer, NULL) == -1){
        exit(EXIT_FAILURE);
    }
  }

}

void blocking_signal_for_rotate(int direction){
  sigset_t old_mask;
  // set blocked signal set to block_mask_g (man 2 sigprocmask).
  // so now block SIGALRM.
  if (sigprocmask(SIG_BLOCK, &block_mask_g, &old_mask) < 0){
      exit(EXIT_FAILURE);
  }
  // CRITICAL CODE GOES HERE ... can call other functions here, and
  // they will not be interrupted
  erase_ship();
  // find center point
  int idx=1;
  double min_x=ship[0].x;
  double max_x=ship[0].x;
  double min_y=ship[0].y;
  double max_y=ship[0].y;
  for(idx=1;idx<4;idx++){
      if(ship[idx].x<min_x){
          min_x=ship[idx].x;
      }
      if(ship[idx].x>max_x){
          max_x=ship[idx].x;
      }
      if(ship[idx].y<min_y){
          min_y=ship[idx].y;
      }
      if(ship[idx].y>max_y){
          max_y=ship[idx].y;
      }
  }
  double origin_center_x=(min_x + max_x) / 2.0;
  double origin_center_y=(min_y + max_y) / 2.0;
  //rotate and put it back
  point R_ship[5];
  idx=0;
  for(idx=0;idx<5;idx++){
      R_ship[idx].x=ship[idx].x-origin_center_x;
      R_ship[idx].y=ship[idx].y-origin_center_y;
      if(direction==0){
          rotate_point_by_ten_degree_CW(&R_ship[idx]);
      }
      else if(direction==1){
          rotate_point_by_ten_degree_CCW(&R_ship[idx]);
      }
      ship[idx].x=R_ship[idx].x+origin_center_x;
      ship[idx].y=R_ship[idx].y+origin_center_y;
  }
  if(direction==0){
      if(ship_degree==360){
          ship_degree=0;
      } 
      ship_degree+=10;
  }
  if(direction==1){
      if(ship_degree==0){
          ship_degree=360;
      } 
      ship_degree-=10;
  }
  draw_ship();
  // unblock signal by setting mask to old value (which in our
  // case wasn't blocking anything)
  if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0){
      exit(EXIT_FAILURE);
  }
}

int check_intersection(point A,point B,point C,point D){
    double dis_AB, the_cos, the_sin, new_X, ABpos;
    if ((A.x==B.x && A.y==B.y)||(C.x==D.x && C.y==D.y)){return 0;}
    if ((A.x==C.x && A.y==C.y)||(B.x==C.x && B.y==C.y)||(A.x==D.x && A.y==D.y)||(B.x==D.x && B.y==D.y)){return 0;}
    point cpy_A=A;
    point cpy_B=B;
    point cpy_C=C;
    point cpy_D=D;
    cpy_B.x-=cpy_A.x;
    cpy_B.y-=cpy_A.y;
    cpy_C.x-=cpy_A.x;
    cpy_C.y-=cpy_A.y;
    cpy_D.x-=cpy_A.x;
    cpy_D.y-=cpy_A.y;
    dis_AB=sqrt((cpy_B.x*cpy_B.x)+(cpy_B.y*cpy_B.y));
    the_cos=cpy_B.x/dis_AB;
    the_sin=cpy_B.y/dis_AB;
    new_X=(cpy_C.x*the_cos)+(cpy_C.y*the_sin);
    cpy_C.y=(cpy_C.y*the_cos)-(cpy_C.x*the_sin);
    cpy_C.x=new_X;
    new_X=(cpy_D.x*the_cos)+(cpy_D.y*the_sin);
    cpy_D.y=(cpy_D.y*the_cos)-(cpy_D.x*the_sin);
    cpy_D.x=new_X;
    if ((cpy_C.y<0. && cpy_D.y<0.)||(cpy_C.y>=0. && cpy_D.y>=0.)){return 0;}
    ABpos=cpy_D.x+(cpy_C.x-cpy_D.x)*cpy_D.y/(cpy_D.y-cpy_C.y);
    if (ABpos<0.||ABpos>dis_AB){return 0;}
    return 1;
}
// draw graphical fuel gauge
void draw_fuel_rec(){
    set_purple();
    point REC[2];
    REC[0].x=580;
    REC[1].x=590;
    REC[0].y=30;
    REC[1].y=30;
    long fuel_index=0;
    for(fuel_index=0;fuel_index<50;fuel_index++){
        draw_line(REC[0],REC[1]);
        REC[0].y++;
        REC[1].y++;
    }
    point EDGE[4];
    EDGE[0].x=579;
    EDGE[0].y=29;
    EDGE[1].x=591;
    EDGE[1].y=29;
    EDGE[2].x=579;
    EDGE[2].y=80;
    EDGE[3].x=591;
    EDGE[3].y=80;
    draw_line(EDGE[0],EDGE[1]);
    draw_line(EDGE[0],EDGE[2]);
    draw_line(EDGE[1],EDGE[3]);
    draw_line(EDGE[2],EDGE[3]);
}

void erase_fuel_rec(double fuel){
    point REC[2];
    REC[0].x=580;
    REC[1].x=590;
    REC[0].y=80-(5*fuel);
    REC[1].y=80-(5*fuel);
    erase_line(REC[0],REC[1]);
}
