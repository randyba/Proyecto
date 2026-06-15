#include <ncurses.h>

int main() {
    int size = 32;
    char fname[size];
    char lname[size];

    initscr();   
    
    addstr("Enter your first name: ");
    refresh();
    getnsts(fname, size-1);

    addstr("Enter your last name: ");
    refresh();
    getnsts(lname, size-1);

    printw("Hello, %s %s!\n", fname, lname);
    getch();

    endwin();

    addstr("Press any key to see its value, press 'q' to quit.\n");
}
