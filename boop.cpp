#include <iostream>
#include <random>
#include <string>

bool FIRST_MOVE;
int ROWS;
int COLS;
int MINES;
int *mines_board = NULL;
int *nums_board = NULL;
int *display_board = NULL;

char get_char_input(std::string prompt);
int index(int row, int col);
void choose_difficulty();
void create_board();
void display(int *board, int x, int y);
char display_map(int x);
void make_move(int &x, int &y);
bool mine(int x, int y);
bool flag(int x, int y);

// TODO run game
int main()
{
    choose_difficulty();
    create_board();
    int x = 0;
    int y = 0;
    for (int i = 0; i < 20; i++)
    {
        display(display_board, x, y);
        make_move(x, y);
    }
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

void display(int *board, int x, int y)
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (x == i && y == j)
            {
                std::cout << '(' << display_map(board[index(i, j)]) << ')';
                continue;
            }
            if (x != i || y + 1 != j)
            {
                std::cout << ' ';
            }
            std::cout << display_map(board[index(i, j)]);
        }
        std::cout << std::endl;
    }
}

// TODO finish mapping
char display_map(int x)
{
    if (x > 0 && x < 9)
    {
        return '0' + x;
    }
    if (x == 0)
    {
        return '.';
    }
    return 'O';
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

// TODO work out first move
bool mine(int x, int y)
{
    return true;
}

// TODO 
bool flag(int x, int y)
{
    return true;
}