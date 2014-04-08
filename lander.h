/*
name:               Xuping Fang
ONE Card number:    1365512
Unix id:            xuping
lecture section:    A2
instructor's name:  Davood Rafiei
lab section:        D06
TA's name:          Ali Yaclollahi           
*/
#ifndef LANDER_H
#define LANDER_H

typedef struct point{
    double x;
    double y;
}point;

void open_pipe(const char* exec_name);

void close_pipe();

void set_green();

void set_blue();

void set_purple();

void draw_line(point A,point B);

void erase_line(point A,point B);

double angle_to_radian(long angle);

void rotate_point_by_ten_degree_CW(point* P);

void rotate_point_by_ten_degree_CCW(point* P);

void draw_ship();

void draw_thrust();

void erase_ship();

void erase_thrust();

void draw_landscape(FILE* landscape);

//stdio function prototypes

FILE *popen(const char *command, const char *type);

int pclose(FILE *stream);

//curses function prototypes

void setup_curses();

void unset_curses();

//timer function prototypes

void handle_timeout(int signal);

void blocking_signal_for_rotate(int direction);

//intersection function prototypes

int check_intersection(point A,point B,point C,point D);

//improvement function prototypes

void draw_fuel_rec();

void erase_fuel_rec(double fuel);

#endif
