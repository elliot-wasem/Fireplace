/**
 * @Author: Elliot Wasem
 * @Github: http://github.com/elliot-wasem
 * This project: http://github.com/elliot-wasem/Fireplace
 */

#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>

// quick color definitions, for ease of use.
// colors declared inside main
#define GREEN                 1
#define RED                   2
#define BLUE                  3
#define YELLOW                4
#define WHITE                 5
#define CYAN                  6
#define BACKWALL_COLOR        7
#define BRICK_COLOR           8
#define FIREPLACE_BACK_COLOR  9
#define FLAME_BLUE_COLOR     10
#define FLAME_YELLOW_COLOR   11
#define FLAME_RED_COLOR      12
#define SKY_COLOR            13

#define FLOOR_WIDTH       3
#define FIREPLACE_HEIGHT 15
#define FIREPLACE_START  (8)
#define FIREPLACE_WIDTH  (31) // FIREPLACE_HEIGHT * 2 + 1
#define WINDOW_LEFT_EDGE (8 + 42)
#define WINDOW_BOTTOM_EDGE (screen_size.y - 6)
#define WINDOW_SIDE_LENGTH 13
#define NUM_FLAKES (72) // number of flakes is 3 flakes per column in window

typedef struct {
    int x;
    int y;
} point;

typedef enum {
    FLAME_0 = 0,
    FLAME_1 = 1
} FLAME_STATE;

typedef point dimensions;

// time used to set delay between each frame
static int time = 250;

// stores size of screen
static dimensions screen_size = (dimensions){0, 0};

// stores which flame is to be drawn next
static FLAME_STATE flame_state = FLAME_0;

// stores snowflakes
static point flakes[NUM_FLAKES]; // 2 * 3, 2 for the number of pixels
                                             // per width unit, and 3 for 3 flakes
                                             // per pixel

// wreathe position, used for offset of wreathe in draw_wreathe().
static point wreathe_pos = (point){.y=-27, .x=10};

void initialize_program();
void initialize_colors();
void cleanup_program();
void draw_scene();
void draw_flame();
void draw_lights();
void draw_wreathe();
void draw_floor();
void draw_window();
void draw_wall();
void draw_fireplace();
void draw_greeting();

int main(int argc, char *argv[]) {
    initialize_program();
    char c = '\0';
    while (c != 'q') {
        getmaxyx(stdscr, screen_size.y, screen_size.x);
        if (screen_size.y > 30 && screen_size.x > 80) {
            draw_scene();
        } else {
            erase();
            mvprintw(0, 0, "Please increase screen size");
            refresh();
        }
        c = getch();
    }
    cleanup_program();
}

void initialize_program() {

    // initialize screen
    initscr();

    // initializes color
    start_color();

    // initially refreshes screen, emptying it
    refresh();

    // keypresses will not be displayed on screen
    noecho();

    // sets no visible cursor
    curs_set(0);

    /*
     * sets getch timeout (and I assume other hanging operations' timeouts, but
     * I haven't confirmed that). This is used for the timing functionality
     */
    timeout(time);

    initialize_colors();

    // fills initial snowflake array
    for (int i = 0; i < NUM_FLAKES; i++) {
        flakes[i].x = (WINDOW_LEFT_EDGE+(i/3)+2);
        flakes[i].y = (WINDOW_BOTTOM_EDGE - WINDOW_SIDE_LENGTH + 1);
    }

    // stores the maximum y and x coordinates of the screen at the time.
    getmaxyx(stdscr, screen_size.y, screen_size.x);
}

void initialize_colors() {

    // sets color pairs to numbers defined above
    init_pair(GREEN,                COLOR_GREEN, COLOR_BLACK);
    init_pair(RED,                  COLOR_RED, COLOR_BLACK);
    init_pair(BLUE,                 COLOR_BLUE, COLOR_BLACK);
    init_pair(YELLOW,               COLOR_YELLOW, COLOR_BLACK);
    init_pair(WHITE,                COLOR_WHITE, COLOR_BLACK);
    init_pair(CYAN,                 COLOR_CYAN, COLOR_BLACK);
    init_pair(BACKWALL_COLOR,       COLOR_RED, COLOR_BLACK);
    init_pair(BRICK_COLOR,          COLOR_WHITE, COLOR_RED);
    init_pair(FIREPLACE_BACK_COLOR, COLOR_YELLOW, COLOR_BLACK);
    init_pair(FLAME_BLUE_COLOR,     COLOR_BLACK, COLOR_CYAN);
    init_pair(FLAME_YELLOW_COLOR,   COLOR_BLACK, COLOR_YELLOW);
    init_pair(FLAME_RED_COLOR,      COLOR_BLACK, COLOR_RED);
    init_pair(SKY_COLOR,            COLOR_WHITE, COLOR_BLUE);

}

void cleanup_program() {
    endwin();
}

void draw_scene() {
    getmaxyx(stdscr, screen_size.y, screen_size.x);
    draw_wall();
    draw_floor();
    draw_fireplace();
    draw_lights();
    draw_window();
    draw_wreathe();
    draw_greeting();
    refresh();
}

void draw_wall() {
    int brick_width = 8;
    int shift = 0;
    attron(COLOR_PAIR(BACKWALL_COLOR));
    for (int y = 0; y < screen_size.y; y++) {
        for (int x = 0; x < screen_size.x; x++) {
            if (y % 2 == 0) {
                mvprintw(y, x, "-");
            } else {
                if ((x+shift) % brick_width == 0) {
                    mvprintw(y, x, "|");
                } else {
                    mvprintw(y, x, " ");
                }
            }
        }
        shift = (shift + (brick_width / 4)) % brick_width;
    }
    attroff(COLOR_PAIR(BACKWALL_COLOR));
}

void draw_floor() {
    attron(COLOR_PAIR(BRICK_COLOR));
    for (int y = screen_size.y-1; y > screen_size.y-1-FLOOR_WIDTH; y--) {
        for (int x = 0; x < screen_size.x; x++) {
            mvprintw(y, x, " ");
        }
    }
    attroff(COLOR_PAIR(BRICK_COLOR));
}

void draw_window() {
    int y, x;

    // print window outer frame
    attron(COLOR_PAIR(BRICK_COLOR));
    for (y = WINDOW_BOTTOM_EDGE; y >= WINDOW_BOTTOM_EDGE-WINDOW_SIDE_LENGTH; y--) {
        for (x = WINDOW_LEFT_EDGE; x <= WINDOW_LEFT_EDGE+(WINDOW_SIDE_LENGTH*2)+1; x++) {
            mvprintw(y, x, " ");
        }
    }
    attroff(COLOR_PAIR(BRICK_COLOR));

    // print sky
    attron(COLOR_PAIR(SKY_COLOR));
    for (y = WINDOW_BOTTOM_EDGE-1; y >= WINDOW_BOTTOM_EDGE-WINDOW_SIDE_LENGTH+1; y--) {
        for (x = WINDOW_LEFT_EDGE+2; x <= WINDOW_LEFT_EDGE+(WINDOW_SIDE_LENGTH*2)-1;x++) {
            mvprintw(y, x, " ");
        }
    }

    // print snow
    for (int i = 0; i < NUM_FLAKES; i++) {
	    int random = rand() % 3;
        if (flakes[i].y >= WINDOW_BOTTOM_EDGE - WINDOW_SIDE_LENGTH + 1) {
        mvprintw(flakes[i].y, flakes[i].x, "%c", (random == 2 ? '*' : (random == 1 ? '+' : 'x')));
        }

        if (flakes[i].y < WINDOW_BOTTOM_EDGE-1) {
            flakes[i].y++;
        } else {
            flakes[i].y = WINDOW_BOTTOM_EDGE-1;
        }
    }
    flakes[rand() % NUM_FLAKES].y = WINDOW_BOTTOM_EDGE - WINDOW_SIDE_LENGTH + 1;
    flakes[rand() % NUM_FLAKES].y = WINDOW_BOTTOM_EDGE - WINDOW_SIDE_LENGTH + 1;
    flakes[rand() % NUM_FLAKES].y = WINDOW_BOTTOM_EDGE - WINDOW_SIDE_LENGTH + 1;
    attroff(COLOR_PAIR(SKY_COLOR));

    // print cross on window
    attron(COLOR_PAIR(BRICK_COLOR));
    x = WINDOW_LEFT_EDGE + (WINDOW_SIDE_LENGTH);
    y = WINDOW_BOTTOM_EDGE;
    for (; y > WINDOW_BOTTOM_EDGE - WINDOW_SIDE_LENGTH; y--) {
        mvprintw(y, x, "  ");
    }
    x = WINDOW_LEFT_EDGE;
    y = WINDOW_BOTTOM_EDGE - (WINDOW_SIDE_LENGTH/2+1);
    for (; x < WINDOW_LEFT_EDGE + (WINDOW_SIDE_LENGTH*2); x++) {
        mvprintw(y, x, " ");
    }

    attroff(COLOR_PAIR(BRICK_COLOR));
}

void draw_fireplace() {
    attron(COLOR_PAIR(BRICK_COLOR));
    int y, x;
    // draw sides and back
    for (y = screen_size.y-1-FLOOR_WIDTH;
         y > screen_size.y-1-FLOOR_WIDTH-FIREPLACE_HEIGHT;
         y--) {
        x = FIREPLACE_START;
        // draw side piece
        mvprintw(y, x, "  ");
        attroff(COLOR_PAIR(BRICK_COLOR));
        attron(COLOR_PAIR(FIREPLACE_BACK_COLOR));
        // draw back pieces
        for (x += 2; x < FIREPLACE_START+FIREPLACE_WIDTH; x++) {
            mvprintw(y, x, x % 2 == 0 ? " " : ".");
        }
        attroff(COLOR_PAIR(FIREPLACE_BACK_COLOR));
        attron(COLOR_PAIR(BRICK_COLOR));
        // draw other side piece
        mvprintw(y, x, "  ");
    }
    y = screen_size.y-1-FLOOR_WIDTH-FIREPLACE_HEIGHT;
    for (x = FIREPLACE_START-4; x < FIREPLACE_START+FIREPLACE_WIDTH+6; x++) {
        mvprintw(y, x, " ");
    }
    attroff(COLOR_PAIR(BRICK_COLOR));

    draw_flame();
}

void draw_lights() {
    int light_colors[] = {
        GREEN, RED, BLUE, YELLOW, WHITE
    };

    int fireplace_bot = screen_size.y - 1 - FLOOR_WIDTH;
    point wreathe = (point){.y = screen_size.y+wreathe_pos.y, .x=FIREPLACE_START+wreathe_pos.x};
    point lights[] = {
        // fireplace
        (point){.y=fireplace_bot, .x=FIREPLACE_START-1},
        (point){.y=fireplace_bot-1, .x=FIREPLACE_START-1},
        (point){.y=fireplace_bot-2, .x=FIREPLACE_START-1},
        (point){.y=fireplace_bot-3, .x=FIREPLACE_START-1},
        (point){.y=fireplace_bot-4, .x=FIREPLACE_START-1},
        (point){.y=fireplace_bot-5, .x=FIREPLACE_START-1},
        (point){.y=fireplace_bot-6, .x=FIREPLACE_START-1},
        (point){.y=fireplace_bot-7, .x=FIREPLACE_START-1},
        (point){.y=fireplace_bot-8, .x=FIREPLACE_START-1},
        (point){.y=fireplace_bot-9, .x=FIREPLACE_START-1},
        (point){.y=fireplace_bot-10, .x=FIREPLACE_START-1},
        (point){.y=fireplace_bot-11, .x=FIREPLACE_START-1},
        (point){.y=fireplace_bot-12, .x=FIREPLACE_START-1},
        (point){.y=fireplace_bot-13, .x=FIREPLACE_START-1},
        (point){.y=fireplace_bot-14, .x=FIREPLACE_START-1},
        (point){.y=fireplace_bot-14, .x=FIREPLACE_START-3},
        (point){.y=fireplace_bot-14, .x=FIREPLACE_START-5},
        (point){.y=fireplace_bot-15, .x=FIREPLACE_START-5},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START-5},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START-3},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START-1},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+1},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+3},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+5},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+7},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+9},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+11},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+13},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+15},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+17},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+19},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+21},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+23},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+25},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+27},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+29},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+31},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+33},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+35},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+37},
        (point){.y=fireplace_bot-15, .x=FIREPLACE_START+37},
        (point){.y=fireplace_bot-14, .x=FIREPLACE_START+37},
        (point){.y=fireplace_bot-14, .x=FIREPLACE_START+35},
        (point){.y=fireplace_bot-14, .x=FIREPLACE_START+33},
        (point){.y=fireplace_bot-13, .x=FIREPLACE_START+33},
        (point){.y=fireplace_bot-12, .x=FIREPLACE_START+33},
        (point){.y=fireplace_bot-11, .x=FIREPLACE_START+33},
        (point){.y=fireplace_bot-10, .x=FIREPLACE_START+33},
        (point){.y=fireplace_bot-9, .x=FIREPLACE_START+33},
        (point){.y=fireplace_bot-8, .x=FIREPLACE_START+33},
        (point){.y=fireplace_bot-7, .x=FIREPLACE_START+33},
        (point){.y=fireplace_bot-6, .x=FIREPLACE_START+33},
        (point){.y=fireplace_bot-5, .x=FIREPLACE_START+33},
        (point){.y=fireplace_bot-4, .x=FIREPLACE_START+33},
        (point){.y=fireplace_bot-3, .x=FIREPLACE_START+33},
        (point){.y=fireplace_bot-2, .x=FIREPLACE_START+33},
        (point){.y=fireplace_bot-1, .x=FIREPLACE_START+33},
        (point){.y=fireplace_bot, .x=FIREPLACE_START+33},

        // window
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+41},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+43},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+45},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+47},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+49},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+51},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+53},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+55},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+57},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+59},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+61},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+63},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+65},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+67},
        (point){.y=fireplace_bot-16, .x=FIREPLACE_START+69},
        (point){.y=fireplace_bot-15, .x=FIREPLACE_START+70},
        (point){.y=fireplace_bot-14, .x=FIREPLACE_START+70},
        (point){.y=fireplace_bot-13, .x=FIREPLACE_START+70},
        (point){.y=fireplace_bot-12, .x=FIREPLACE_START+70},
        (point){.y=fireplace_bot-11, .x=FIREPLACE_START+70},
        (point){.y=fireplace_bot-10, .x=FIREPLACE_START+70},
        (point){.y=fireplace_bot-9, .x=FIREPLACE_START+70},
        (point){.y=fireplace_bot-8, .x=FIREPLACE_START+70},
        (point){.y=fireplace_bot-7, .x=FIREPLACE_START+70},
        (point){.y=fireplace_bot-6, .x=FIREPLACE_START+70},
        (point){.y=fireplace_bot-5, .x=FIREPLACE_START+70},
        (point){.y=fireplace_bot-4, .x=FIREPLACE_START+70},
        (point){.y=fireplace_bot-3, .x=FIREPLACE_START+70},
        (point){.y=fireplace_bot-2, .x=FIREPLACE_START+70},
        (point){.y=fireplace_bot-1, .x=FIREPLACE_START+70},
        (point){.y=fireplace_bot-1, .x=FIREPLACE_START+68},
        (point){.y=fireplace_bot-1, .x=FIREPLACE_START+66},
        (point){.y=fireplace_bot-1, .x=FIREPLACE_START+64},
        (point){.y=fireplace_bot-1, .x=FIREPLACE_START+62},
        (point){.y=fireplace_bot-1, .x=FIREPLACE_START+60},
        (point){.y=fireplace_bot-1, .x=FIREPLACE_START+58},
        (point){.y=fireplace_bot-1, .x=FIREPLACE_START+56},
        (point){.y=fireplace_bot-1, .x=FIREPLACE_START+54},
        (point){.y=fireplace_bot-1, .x=FIREPLACE_START+52},
        (point){.y=fireplace_bot-1, .x=FIREPLACE_START+50},
        (point){.y=fireplace_bot-1, .x=FIREPLACE_START+48},
        (point){.y=fireplace_bot-1, .x=FIREPLACE_START+46},
        (point){.y=fireplace_bot-1, .x=FIREPLACE_START+44},
        (point){.y=fireplace_bot-1, .x=FIREPLACE_START+42},
        (point){.y=fireplace_bot-2, .x=FIREPLACE_START+41},
        (point){.y=fireplace_bot-3, .x=FIREPLACE_START+41},
        (point){.y=fireplace_bot-4, .x=FIREPLACE_START+41},
        (point){.y=fireplace_bot-5, .x=FIREPLACE_START+41},
        (point){.y=fireplace_bot-6, .x=FIREPLACE_START+41},
        (point){.y=fireplace_bot-7, .x=FIREPLACE_START+41},
        (point){.y=fireplace_bot-8, .x=FIREPLACE_START+41},
        (point){.y=fireplace_bot-9, .x=FIREPLACE_START+41},
        (point){.y=fireplace_bot-10, .x=FIREPLACE_START+41},
        (point){.y=fireplace_bot-11, .x=FIREPLACE_START+41},
        (point){.y=fireplace_bot-12, .x=FIREPLACE_START+41},
        (point){.y=fireplace_bot-13, .x=FIREPLACE_START+41},
        (point){.y=fireplace_bot-14, .x=FIREPLACE_START+41},
        (point){.y=fireplace_bot-15, .x=FIREPLACE_START+41},

        // wreathe
        (point){.y=wreathe.y+0, .x=wreathe.x-1},
        (point){.y=wreathe.y+1, .x=wreathe.x-1},
        (point){.y=wreathe.y+2, .x=wreathe.x-1},
        (point){.y=wreathe.y+3, .x=wreathe.x+1},
        (point){.y=wreathe.y+4, .x=wreathe.x+3},
        (point){.y=wreathe.y+5, .x=wreathe.x+6},
        (point){.y=wreathe.y+4, .x=wreathe.x+10},
        (point){.y=wreathe.y+3, .x=wreathe.x+12},
        (point){.y=wreathe.y+2, .x=wreathe.x+14},
        (point){.y=wreathe.y+1, .x=wreathe.x+14},
        (point){.y=wreathe.y+0, .x=wreathe.x+14},
        (point){.y=wreathe.y-1, .x=wreathe.x+14},
        (point){.y=wreathe.y-2, .x=wreathe.x+12},
        (point){.y=wreathe.y-3, .x=wreathe.x+10},
        (point){.y=wreathe.y-4, .x=wreathe.x+7},
        (point){.y=wreathe.y-3, .x=wreathe.x+3},
        (point){.y=wreathe.y-2, .x=wreathe.x+1},
        (point){.y=wreathe.y-1, .x=wreathe.x+0},
    };

    int len = sizeof(lights) / sizeof(lights[0]);

    static int start_color = 0;

    for (int i = 0, color = start_color; i < len; i++) {
        attron(COLOR_PAIR(color));
        mvprintw(lights[i].y, lights[i].x, "o");
        attroff(COLOR_PAIR(color));
        color = (color + 1) % (sizeof(light_colors) / sizeof(light_colors[0]));
    }
    start_color = (start_color + 1) % (sizeof(light_colors) / sizeof(light_colors[0]));
}

void draw_flame() {
    int x, y;
    int base_x, base_y;
    base_y = screen_size.y-1-FLOOR_WIDTH-1;
    base_x = FIREPLACE_START+(FIREPLACE_WIDTH/2)+2;
    // blue flame
    attron(COLOR_PAIR(FLAME_BLUE_COLOR));
    y = base_y;
    x = base_x;
    mvprintw(y, x, " ");
    y -= 1;
    x -= 1;
    mvprintw(y, x, "   ");
    y -= 1;
    x += 1;
    mvprintw(y, x, " ");
    attroff(COLOR_PAIR(FLAME_BLUE_COLOR));
    switch (flame_state) {
        case FLAME_0: {
            // yellow flame
            attron(COLOR_PAIR(FLAME_YELLOW_COLOR));
            y = base_y;
            x = base_x - 1;
            mvprintw(y, x, " ");
            y = base_y - 1;
            x = base_x + 2;
            mvprintw(y, x, "  ");
            y = base_y - 2;
            x = base_x + 1;
            mvprintw(y, x, " ");
            y = base_y - 2;
            x = base_x + 3;
            mvprintw(y, x, "  ");
            y = base_y - 1;
            x = base_x - 3;
            mvprintw(y, x, "  ");
            y = base_y - 2;
            x = base_x - 4;
            mvprintw(y, x, "   ");
            y = base_y - 3;
            x = base_x - 4;
            mvprintw(y, x, "  ");
            y = base_y - 3;
            x = base_x - 0;
            mvprintw(y, x, "  ");
            y = base_y - 3;
            x = base_x + 4;
            mvprintw(y, x, "  ");
            y = base_y - 4;
            x = base_x + 5;
            mvprintw(y, x, "  ");
            y = base_y - 4;
            x = base_x - 1;
            mvprintw(y, x, "   ");
            y = base_y - 5;
            x = base_x - 0;
            mvprintw(y, x, "  ");
            y = base_y - 5;
            x = base_x + 5;
            mvprintw(y, x, " ");
            y = base_y - 6;
            x = base_x + 1;
            mvprintw(y, x, " ");
            attroff(COLOR_PAIR(FLAME_YELLOW_COLOR));

            // red flame
            attron(COLOR_PAIR(FLAME_RED_COLOR));
            y = base_y;
            x = base_x + 1;
            mvprintw(y, x, " ");
            y = base_y - 2;
            x = base_x + 2;
            mvprintw(y, x, " ");
            y = base_y - 2;
            x = base_x - 1;
            mvprintw(y, x, " ");
            y = base_y - 3;
            x = base_x - 2;
            mvprintw(y, x, "  ");
            y = base_y - 3;
            x = base_x + 2;
            mvprintw(y, x, "  ");
            y = base_y - 4;
            x = base_x + 2;
            mvprintw(y, x, "   ");
            y = base_y - 4;
            x = base_x - 3;
            mvprintw(y, x, "  ");
            y = base_y - 5;
            x = base_x - 2;
            mvprintw(y, x, "  ");
            y = base_y - 5;
            x = base_x + 2;
            mvprintw(y, x, "   ");
            y = base_y - 6;
            x = base_x - 1;
            mvprintw(y, x, "  ");
            y = base_y - 6;
            x = base_x + 2;
            mvprintw(y, x, "   ");
            y = base_y - 7;
            x = base_x - 0;
            mvprintw(y, x, "     ");
            y = base_y - 8;
            x = base_x + 1;
            mvprintw(y, x, "   ");
            attroff(COLOR_PAIR(FLAME_RED_COLOR));
            break;
        }
        case FLAME_1: {
            // yellow flame
            attron(COLOR_PAIR(FLAME_YELLOW_COLOR));
            y = base_y;
            x = base_x + 1;
            mvprintw(y, x, " ");
            y = base_y - 1;
            x = base_x + 3;
            mvprintw(y, x, " ");
            y = base_y - 1;
            x = base_x - 2;
            mvprintw(y, x, " ");
            y = base_y - 2;
            x = base_x - 2;
            mvprintw(y, x, "  ");
            y = base_y - 2;
            x = base_x + 3;
            mvprintw(y, x, "  ");
            y = base_y - 3;
            x = base_x + 3;
            mvprintw(y, x, "  ");
            y = base_y - 3;
            x = base_x - 3;
            mvprintw(y, x, "   ");
            y = base_y - 4;
            x = base_x + 2;
            mvprintw(y, x, "  ");
            y = base_y - 4;
            x = base_x - 4;
            mvprintw(y, x, "   ");
            y = base_y - 5;
            x = base_x + 2;
            mvprintw(y, x, " ");
            y = base_y - 5;
            x = base_x - 3;
            mvprintw(y, x, "  ");
            y = base_y - 6;
            x = base_x - 2;
            mvprintw(y, x, " ");
            attroff(COLOR_PAIR(FLAME_YELLOW_COLOR));

            // red flame
            attron(COLOR_PAIR(FLAME_RED_COLOR));
            y = base_y;
            x = base_x - 1;
            mvprintw(y, x, " ");
            y = base_y - 1;
            x = base_x + 2;
            mvprintw(y, x, " ");
            y = base_y - 1;
            x = base_x - 3;
            mvprintw(y, x, " ");
            y = base_y - 2;
            x = base_x - 4;
            mvprintw(y, x, "  ");
            y = base_y - 2;
            x = base_x + 1;
            mvprintw(y, x, "  ");
            y = base_y - 3;
            x = base_x + 0;
            mvprintw(y, x, "   ");
            y = base_y - 3;
            x = base_x - 5;
            mvprintw(y, x, "  ");
            y = base_y - 4;
            x = base_x - 6;
            mvprintw(y, x, "  ");
            y = base_y - 4;
            x = base_x - 1;
            mvprintw(y, x, "   ");
            y = base_y - 5;
            x = base_x - 5;
            mvprintw(y, x, "  ");
            y = base_y - 5;
            x = base_x - 1;
            mvprintw(y, x, "   ");
            y = base_y - 6;
            x = base_x - 1;
            mvprintw(y, x, "   ");
            y = base_y - 6;
            x = base_x - 4;
            mvprintw(y, x, "  ");
            y = base_y - 7;
            x = base_x - 4;
            mvprintw(y, x, "     ");
            y = base_y - 8;
            x = base_x - 3;
            mvprintw(y, x, "   ");
            y = base_y - 9;
            x = base_x - 2;
            mvprintw(y, x, " ");
            attroff(COLOR_PAIR(FLAME_RED_COLOR));
            break;
        }
    }

    if (flame_state == FLAME_0) {
        flame_state = FLAME_1;
    } else if (flame_state == FLAME_1) {
        flame_state = FLAME_0;
    }
}

void draw_wreathe() {
    point wreathe = (point){.y = screen_size.y+wreathe_pos.y, .x=FIREPLACE_START+wreathe_pos.x};
    attron(COLOR_PAIR(GREEN));
    mvprintw(wreathe.y,   wreathe.x, "###");
    mvprintw(wreathe.y+1, wreathe.x, "###");
    mvprintw(wreathe.y+2, wreathe.x, "####");
    mvprintw(wreathe.y+3, wreathe.x+2, "##########");
    mvprintw(wreathe.y+4, wreathe.x+4, "######");
    mvprintw(wreathe.y+2, wreathe.x+10, "####");
    mvprintw(wreathe.y+1, wreathe.x+11, "###");
    mvprintw(wreathe.y+0, wreathe.x+11, "###");
    mvprintw(wreathe.y-2, wreathe.x+2, "##########");
    mvprintw(wreathe.y-3, wreathe.x+4, "######");
    mvprintw(wreathe.y-1, wreathe.x, "####");
    mvprintw(wreathe.y-1, wreathe.x+10, "####");
    attroff(COLOR_PAIR(GREEN));
}

void draw_greeting() {
    int y_pos = screen_size.y-31;
    int x_pos = 38;
    mvprintw(y_pos++, x_pos, "+---------------------------------------+");
    mvprintw(y_pos++, x_pos, "| Merry Christmas and a Happy New Year! |");
    mvprintw(y_pos++, x_pos, "+---------------------------------------+");
    y_pos = screen_size.y-2;
    x_pos = 0;
    attron(COLOR_PAIR(BRICK_COLOR));
    mvprintw(y_pos++, x_pos, " Author: Elliot Wasem");
    mvprintw(y_pos++, x_pos, " https://github.com/elliot-wasem");
    y_pos = screen_size.y-1;
    x_pos = screen_size.x-8;
    mvprintw(y_pos++, x_pos, "q: quit");
    attroff(COLOR_PAIR(BRICK_COLOR));
}
