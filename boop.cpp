#include <iostream>
#include <random>
#include <string>
#include <time.h>
#include <conio.h>
#include <windows.h>

bool FIRST_MOVE;
int ROWS;
int COLS;
int MINES;
int *mines_board = NULL;
int *nums_board = NULL;
int *display_board = NULL;

void setup_terminal();
char get_char_input(std::string prompt);
int index(int row, int col);
int unindexX(int i);
int unindexY(int i);
void choose_difficulty();
void create_board();
void generate_board(int x, int y);
void display(int *board, int x, int y);
char display_map(int x);
std::string display_map_with_colors(int x);
void make_move(int &x, int &y);
void make_move_unbuffered(int &x, int &y);
bool mine(int x, int y);
bool flag(int x, int y);
bool calculate_win();

// TODO run game
int main()
{
    setup_terminal();
    bool playing = true;
    while (playing)
    {
        choose_difficulty();
        create_board();
        int x = 0;
        int y = 0;
        display(display_board, x, y);
        while (FIRST_MOVE || !calculate_win())
        {
            make_move_unbuffered(x, y);
            display(display_board, x, y);
        }
        std::cout << "Play again? (y/n): ";
        bool answered = false;
        while (!answered)
        {
            char input = get_char_input("Invalid answer, please enter y or n: ");
            if (input == 'y')
            {
                answered = true;
            }
            else if (input == 'n')
            {
                playing = false;
                answered = true;
            }
            else
            {
                std::cout << "Invalid answer, please enter y or n: ";
            }
        }
    }
}

void setup_terminal()
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(h, &mode);
    SetConsoleMode(h, mode | 0x0004);
}

char get_char_input(std::string prompt)
{
    while (true)
    {
        std::string input;
        std::getline(std::cin, input);
        if (input.length() != 1)
        {
            std::cout << prompt;
            continue;
        }
        return input[0];
    }
}

int index(int row, int col)
{
    return row * COLS + col;
}

int unindexX(int i)
{
    return i / COLS;
}

int unindexY(int i)
{
    return i - COLS * unindexX(i);
}

void choose_difficulty()
{
    bool gotten_input = false;
    std::cout << "Please select your difficulty (e:easy, m:medium, h:hard): ";
    while (!gotten_input)
    {
        char difficulty = get_char_input("Invalid difficulty, please enter e, m, or h: ");
        gotten_input = true;
        switch (difficulty)
        {
            case 'e':
                ROWS = 9;
                COLS = 9;
                MINES = 10;
                break;
            case 'm':
                ROWS = 16;
                COLS = 16;
                MINES = 40;
                break;
            case 'h':
                ROWS = 16;
                COLS = 30;
                MINES = 99;
                break;
            default:
                gotten_input = false;
                std::cout << "Invalid difficulty, please enter e, m, or h: ";
        }
    }
}

void create_board()
{
    mines_board = new int[ROWS * COLS];
    nums_board = new int[ROWS * COLS];
    display_board = new int[ROWS * COLS];
    for (int i = 0; i < ROWS * COLS; i++)
    {
        mines_board[i] = 0;
        nums_board[i] = 0;
        display_board[i] = 0;
    }
    FIRST_MOVE = true;
}

void generate_board(int x, int y)
{
    srand(time(0));
    for (int i = 0; i < MINES; i++)
    {
        bool placed_mine = false;
        while (!placed_mine)
        {
            int place = rand() % (ROWS * COLS);
            bool valid_place = true;
            for (int dx = -1; dx <= 1; dx++)
            {
                for (int dy = -1; dy <= 1; dy++)
                {
                    if (place == index(x + dx, y + dy))
                    {
                        valid_place = false;
                    }
                }
            }
            if (valid_place)
            {
                if (mines_board[place] == 0)
                {
                    mines_board[place] = 1;
                    placed_mine = true;
                }
            }
        }
    }

    for (int i = 0; i < ROWS * COLS; i++)
    {
        if (mines_board[i])
        {
            nums_board[i] = -1;
            continue;
        }
        int x = unindexX(i);
        int y = unindexY(i);
        int count = 0;
        for (int dx = -1; dx <= 1; dx++)
        {
            if (x + dx >= 0 && x + dx < ROWS)
            {
                for (int dy = -1; dy <= 1; dy++)
                {
                    if (y + dy >= 0 && y + dy < COLS)
                    {
                        if (mines_board[index(x + dx, y + dy)])
                        {
                            count++;
                        }
                    }
                }
            }
        }
        nums_board[i] = count;
    }
}

void display(int *board, int x, int y)
{
    std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
    
    std::cout << '|';
    for (int i = 0; i < COLS; i++)
    {
        std::cout << "--";
    }
    std::cout << "-|";
    std::cout << std::endl;
    for (int i = 0; i < ROWS; i++)
    {
        std::cout << '|';
        for (int j = 0; j < COLS; j++)
        {
            if (x == i && y == j)
            {
                std::cout << '(' << display_map_with_colors(board[index(i, j)]) << ')';
                continue;
            }
            if (x != i || y + 1 != j)
            {
                std::cout << ' ';
            }
            std::cout << display_map_with_colors(board[index(i, j)]);
        }
        if (x != i || y + 1 != COLS)
        {
            std::cout << ' ';
        }
        std::cout << '|';
        std::cout << std::endl;
    }
    std::cout << '|';
    for (int i = 0; i < COLS; i++)
    {
        std::cout << "--";
    }
    std::cout << "-|";
    std::cout << std::endl;
}

char display_map(int x)
{
    if (x >= 1 && x <= 8)
    {
        return '0' + x;
    }
    if (x == 0)
    {
        return '.';
    }
    if (x == 9)
    {
        return ' ';
    }
    if (x == 10)
    {
        return '^';
    }
    if (x == -1)
    {
        return '*';
    }
    return 'O';
}

std::string display_map_with_colors(int x)
{
    switch (x)
    {
        case 0:
            return "\x1B[42m\x1B[30m.\033[0m";
        case 1:
            return "\x1B[34m1\033[0m";
        case 2:
            return "\x1B[32m2\033[0m";
        case 3:
            return "\x1B[31m3\033[0m";
        case 4:
            return "\x1B[36m4\033[0m";
        case 5:
            return "\x1B[33m5\033[0m";
        case 6:
            return "\x1B[35m6\033[0m";
        case 7:
            return "\x1B[36m7\033[0m";
        case 8:
            return "\x1B[30m8\033[0m";
        case 9:
            return " ";
        case 10:
            return "\x1B[41m\x1B[33m^\033[0m";
        case -1:
            return "\x1B[31m*\033[0m";
        default:
            return "\x1B[47mO\033[0m";
    }
}

void make_move(int &x, int &y)
{
    bool made_move = false;
    while (!made_move)
    {
        char move = get_char_input("Invalid move, please enter w, a, s, d, m, or f: ");
        made_move = true;
        switch (move)
        {
            case 'w':
                if (x > 0)
                {
                    x -= 1;
                }
                else
                {
                    std::cout << "Cannot move up. Please enter another move: ";
                    made_move = false;
                }
                break;
            case 'a':
                if (y > 0)
                {
                    y -= 1;
                }
                else
                {
                    std::cout << "Cannot move left. Please enter another move: ";
                    made_move = false;
                }
                break;
            case 's':
                if (x < ROWS - 1)
                {
                    x += 1;
                }
                else
                {
                    std::cout << "Cannot move down. Please enter another move: ";
                    made_move = false;
                }
                break;
            case 'd':
                if (y < COLS - 1)
                {
                    y += 1;
                }
                else
                {
                    std::cout << "Cannot move right. Please enter another move: ";
                    made_move = false;
                }
                break;
            case 'm':
                if (mine(x, y))
                {
                    std::cout << "Cannot mine this square. Please enter another move: ";
                    made_move = false;
                }
                break;
            case 'f':
                if (flag(x,y))
                {
                    std::cout << "Cannot flag this square. Please enter another move: ";
                    made_move = false;
                }
                break;
            default:
                std::cout << "Invalid move, please enter w, a, s, d, m, or f: ";
                made_move = false;
        }
    }
}

void make_move_unbuffered(int &x, int &y)
{
    bool made_move = false;
    while (!made_move)
    {
        char move = getch();
        made_move = true;
        switch (move)
        {
            case 'w':
                if (x > 0)
                {
                    x -= 1;
                }
                else
                {
                    std::cout << "Cannot move up" << std::endl;
                    made_move = false;
                }
                break;
            case 'a':
                if (y > 0)
                {
                    y -= 1;
                }
                else
                {
                    std::cout << "Cannot move left." << std::endl;;
                    made_move = false;
                }
                break;
            case 's':
                if (x < ROWS - 1)
                {
                    x += 1;
                }
                else
                {
                    std::cout << "Cannot move down." << std::endl;
                    made_move = false;
                }
                break;
            case 'd':
                if (y < COLS - 1)
                {
                    y += 1;
                }
                else
                {
                    std::cout << "Cannot move right." << std::endl;
                    made_move = false;
                }
                break;
            case 'm':
                if (mine(x, y))
                {
                    std::cout << "Cannot mine this square." << std::endl;
                    made_move = false;
                }
                break;
            case 'f':
                if (flag(x,y))
                {
                    std::cout << "Cannot flag this square." << std::endl;
                    made_move = false;
                }
                break;
            default:
                made_move = false;
        }
    }
}

bool mine(int x, int y)
{
    if (FIRST_MOVE)
    {
        generate_board(x, y);
        FIRST_MOVE = false;
    }
    if (display_board[index(x, y)] == 0)
    {
        if (nums_board[index(x, y)] == 0)
        {
            display_board[index(x, y)] = 9;
            for (int dx = -1; dx <= 1; dx++)
            {
                if (x + dx >= 0 && x + dx < ROWS)
                {
                    for (int dy = -1; dy <= 1; dy++)
                    {
                        if (y + dy >= 0 && y + dy < COLS)
                        {
                            if (display_board[index(x + dx, y + dy)] == 0)
                            {
                                mine(x + dx, y + dy);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            display_board[index(x, y)] = nums_board[index(x, y)];
        }
        return false;
    }
    return true;
}

bool flag(int x, int y)
{
    if (display_board[index(x, y)] == 0)
    {
        display_board[index(x, y)] = 10;
        return false;
    }
    if (display_board[index(x, y)] == 10)
    {
        display_board[index(x, y)] = 0;
        return false;
    }
    return true;
}

bool calculate_win()
{
    for (int i = 0; i < ROWS * COLS; i++)
    {
        if (display_board[i] == -1)
        {
            std::cout << "Kaboom!! You lose." << std::endl;
            return true;
        }
    }
    for (int i = 0; i < ROWS * COLS; i++)
    {
        if (!mines_board[i])
        {
            if (display_board[i] == 0 || display_board[i] == 10)
            {
                return false;
            }
        }
    }
    std::cout << "Congratuhelatiionsz!!! You win!" << std::endl;
    return true;
}