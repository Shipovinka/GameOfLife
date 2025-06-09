#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 80
#define HEIGHT 25

// stdin array reading
int read_array(int **field, int n, int m);
// count alive neighbors
int count_neighbors(int **field, int y, int x);
// update field
void update_field(int **field);
// ncurses fiend drawing
void draw_field(int **field, int delay, int generation);
// start the game
void turn_on(int **field);
// creating array
int **create_array(int height, int width);
// memory free
void free_array(int **field, int height);

int main() {
    int **field = create_array(HEIGHT, WIDTH);
    if (read_array(field, HEIGHT, WIDTH)) {
        // off EOF and keyboard input on
        FILE *terminal_on_again = fopen("/dev/tty", "r");  // file which connect current terminal and keybord
        if (!terminal_on_again) {
            printf("Error. Can not open file");
        } else {
            stdin = terminal_on_again;  // switch stdin to keybord instead of file

            initscr();    // ncurses initialization
            noecho();     // off keys printing
            cbreak();     // do not wait enter to continue running the program
            curs_set(0);  // hide cursor
            timeout(10);  // waiting for input
            turn_on(field);
            endwin();  // off ncurses
        }
    }
    free_array(field, HEIGHT);  // memory free
    return 0;
}

// readfile
int read_array(int **field, int n, int m) {
    int flag = 1;
    for (int i = 0; (i < n && flag == 1); i++) {
        for (int j = 0; (j < m && flag == 1); j++) {
            if (scanf("%d", &field[i][j]) != 1) {
                flag = 0;
            }
        }
    }
    return flag;
}

// alife neighbors
int count_neighbors(int **field, int y, int x) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;

            int ny = (y + dy + HEIGHT) % HEIGHT;  // if -1 diagonal
            int nx = (x + dx + WIDTH) % WIDTH;    // if -1 diagonal

            if (field[ny][nx]) count++;
        }
    }
    return count;
}

void update_field(int **field) {
    int **new_field = create_array(HEIGHT, WIDTH);

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int neighbors = count_neighbors(field, y, x);

            if (field[y][x] == 1) {
                // rules for alife, if 2 or 3 neighbors still alife then true(1)
                new_field[y][x] = (neighbors == 2 || neighbors == 3);
            } else {
                // for dead if 3 neighbors then alife
                new_field[y][x] = (neighbors == 3);
            }
        }
    }

    // copy each element of new_field array to field_array fir it's printing
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            field[y][x] = new_field[y][x];
        }
    }
    free_array(new_field, HEIGHT);
}

// ncurses drawing field
void draw_field(int **field, int delay, int generation) {
    clear();  // cursor position 0;0
    printw("Speed: %dms (A/a - faster, Z/z - slower, SPACE - exit)\n", delay);
    printw("Generation: %d \n", generation);
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (field[y][x] == 1) {
                mvaddch(y + 4, x,
                        'O');  // move cursor and add char O for alife (+4 for not to hide two prints)
            } else {
                mvaddch(y + 4, x, ' ');  // no live - empty space
            }
        }
    }
    refresh();
}

// start
void turn_on(int **field) {
    if (LINES < HEIGHT || COLS < WIDTH) {  // librarie's special vars for rows and columns
        endwin();
        printf("Your terminal too small %dx%d\n", WIDTH, HEIGHT);
    } else {
        int delay = 100;
        int generation = 0;
        while (1) {
            // check for key pressed
            int ch = getch();
            if (ch != ERR) {  // getch return error if thehe was not pressed any key (ncurses behavior)
                if (ch == 'a' || ch == 'A') {
                    // speed up
                    if (delay >= 20) {
                        delay = delay - 10;
                    }
                } else if (ch == 'z' || ch == 'Z') {
                    // sdeed down
                    delay = delay + 10;
                } else if (ch == ' ') {
                    // end game
                    break;
                }
            }
            if (generation != 2147483647) {
                generation = generation + 1;
            }
            draw_field(field, delay, generation);
            update_field(field);
            timeout(delay);
        }
    }
}

// memory allocation
int **create_array(int height, int width) {
    int **field = (int **)malloc(height * sizeof(int *));
    if (!field) {
        fprintf(stderr, "Memory allocation failed!\n");
        field = NULL;
    }

    for (int i = 0; i < height; i++) {
        field[i] = (int *)malloc(width * sizeof(int));
        if (!field[i]) {
            fprintf(stderr, "Memory allocation failed!\n");
            field = NULL;
        }
    }

    return field;
}
// memory free
void free_array(int **field, int height) {
    for (int i = 0; i < height; i++) {
        free(field[i]);
    }
    free(field);
}
