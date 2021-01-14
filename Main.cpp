#include <SFML/Graphics.hpp>
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
int** mines_board = NULL;
int** nums_board = NULL;
int** display_board = NULL;
sf::RenderWindow window(sf::VideoMode(1000, 1000), "Minesweeper");

void setup_terminal();
char get_char_input(std::string prompt);
void choose_difficulty();
void create_board();
void generate_board(int x, int y);
void display(int** board, int x, int y);
char display_map(int x);
std::string display_map_with_colors(int x);
void make_move(int& x, int& y);
void make_move_unbuffered(int& x, int& y);
bool mine(int x, int y);
bool flag(int x, int y);
bool calculate_win();
void reset_board();
void choose_difficultyUI();

// TODO run game
int main()
{
    setup_terminal();
    std::ios_base::sync_with_stdio(false);
    bool playing = true;
    while (playing)
    {
        choose_difficultyUI();
        create_board();
        int x = 0;
        int y = 0;
        display(display_board, x, y);
        while (FIRST_MOVE || !calculate_win())
        {
            make_move_unbuffered(x, y);
            display(display_board, x, y);
        }
        reset_board();
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
    mines_board = new int* [ROWS];
    nums_board = new int* [ROWS];
    display_board = new int* [ROWS];
    for (int i = 0; i < ROWS; i++)
    {
        mines_board[i] = new int[COLS];
        nums_board[i] = new int[COLS];
        display_board[i] = new int[COLS];
        for (int j = 0; j < COLS; j++)
        {
            mines_board[i][j] = 0;
            nums_board[i][j] = 0;
            display_board[i][j] = 0;
        }
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
            int mine_x = rand() % ROWS;
            int mine_y = rand() % COLS;
            bool valid_place = true;
            if (abs(mine_x - x) <= 1 && abs(mine_y - y) <= 1)
            {
                valid_place = false;
            }
            if (valid_place)
            {
                if (mines_board[mine_x][mine_y] == 0)
                {
                    mines_board[mine_x][mine_y] = 1;
                    placed_mine = true;
                }
            }
        }
    }

    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (mines_board[i][j])
            {
                nums_board[i][j] = -1;
                continue;
            }
            int count = 0;
            for (int dx = -1; dx <= 1; dx++)
            {
                if (i + dx >= 0 && i + dx < ROWS)
                {
                    for (int dy = -1; dy <= 1; dy++)
                    {
                        if (j + dy >= 0 && j + dy < COLS)
                        {
                            if (mines_board[i + dx][j + dy])
                            {
                                count++;
                            }
                        }
                    }
                }
            }
            nums_board[i][j] = count;
        }
    }
}

void display(int** board, int x, int y)
{
    std::string display_txt;
    display_txt += "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

    display_txt += '|';
    for (int i = 0; i < COLS; i++)
    {
        display_txt += "--";
    }
    display_txt += "-|";
    display_txt += '\n';
    for (int i = 0; i < ROWS; i++)
    {
        display_txt += '|';
        for (int j = 0; j < COLS; j++)
        {
            if (x == i && y == j)
            {
                display_txt += '(' + display_map_with_colors(board[i][j]) + ')';
                continue;
            }
            if (x != i || y + 1 != j)
            {
                display_txt += ' ';
            }
            display_txt += display_map_with_colors(board[i][j]);
        }
        if (x != i || y + 1 != COLS)
        {
            display_txt += ' ';
        }
        display_txt += '|';
        display_txt += '\n';
    }
    display_txt += '|';
    for (int i = 0; i < COLS; i++)
    {
        display_txt += "--";
    }
    display_txt += "-|";
    display_txt += '\n';
    std::cout << display_txt;
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

void make_move(int& x, int& y)
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
        case 'k':
        case 'f':
            if (flag(x, y))
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

void make_move_unbuffered(int& x, int& y)
{
    bool made_move = false;
    while (!made_move)
    {
        char move = _getch();
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
        case 'k':
        case 'f':
            if (flag(x, y))
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
        PlaySound(L"miningsound.wav", NULL, SND_ASYNC);
        generate_board(x, y);
        FIRST_MOVE = false;
    }
    if (display_board[x][y] == 0)
    {
        if (nums_board[x][y] == 0)
        {
            PlaySound(L"miningsound.wav", NULL, SND_ASYNC);
            display_board[x][y] = 9;
            for (int dx = -1; dx <= 1; dx++)
            {
                if (x + dx >= 0 && x + dx < ROWS)
                {
                    for (int dy = -1; dy <= 1; dy++)
                    {
                        if (y + dy >= 0 && y + dy < COLS)
                        {
                            if (display_board[x + dx][y + dy] == 0)
                            {
                                PlaySound(L"miningsound.wav", NULL, SND_ASYNC);
                                mine(x + dx, y + dy);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            PlaySound(L"miningsound.wav", NULL, SND_ASYNC);
            display_board[x][y] = nums_board[x][y];
        }
        return false;
    }
    return true;
}

bool flag(int x, int y)
{
    if (display_board[x][y] == 0)
    {
        PlaySound(L"flag.wav", NULL, SND_ASYNC);
        display_board[x][y] = 10;
        return false;
    }
    if (display_board[x][y] == 10)
    {
        PlaySound(L"flag.wav", NULL, SND_ASYNC);
        display_board[x][y] = 0;
        return false;
    }
    return true;
}

bool calculate_win()
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (display_board[i][j] == -1)
            {
                PlaySound(L"losingsound.wav", NULL, SND_ASYNC);
                std::cout << "Kaboom!! You lose." << std::endl;
                return true;
            }
        }
    }
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (!mines_board[i][j])
            {
                if (display_board[i][j] == 0 || display_board[i][j] == 10)
                {
                    return false;
                }
            }
        }
    }
    PlaySound(L"winningmusic.wav", NULL, SND_ASYNC);
    std::cout << "Congratuhelatiionsz!!! You win!" << std::endl;
    return true;
}

void reset_board()
{
    for (int i = 0; i < ROWS; i++)
    {
        delete[] mines_board[i];
        delete[] nums_board[i];
        delete[] display_board[i];
    }
    delete[] mines_board;
    mines_board = NULL;
    delete[] nums_board;
    nums_board = NULL;
    delete[] display_board;
    display_board = NULL;
}



void choose_difficultyUI()
{
    int currButton = 0;
    sf::Font font;
    font.loadFromFile("ArialCE.ttf");

    sf::RectangleShape EasyButton(sf::Vector2f(800.f, 250.f));
    EasyButton.setOutlineColor(sf::Color::Green);
    EasyButton.setOutlineThickness(2);
    EasyButton.setPosition(100.f, 100.f);
    window.draw(EasyButton);

    sf::Text EasyText;
    EasyText.setFont(font);
    EasyText.setString("Easy");
    EasyText.setFillColor(sf::Color::Black);
    EasyText.setPosition(390, 170);
    EasyText.setCharacterSize(100);
    window.draw(EasyText);

    sf::RectangleShape MediumButton(sf::Vector2f(800.f, 250.f));
    MediumButton.setOutlineColor(sf::Color::Yellow);
    MediumButton.setOutlineThickness(2);
    MediumButton.setPosition(100.f, 400.f);
    window.draw(MediumButton);

    sf::Text MediumText;
    MediumText.setFont(font);
    MediumText.setString("Medium");
    MediumText.setFillColor(sf::Color::Black);
    MediumText.setPosition(340, 470);
    MediumText.setCharacterSize(100);
    window.draw(MediumText);

    sf::RectangleShape HardButton(sf::Vector2f(800.f, 250.f));
    HardButton.setOutlineColor(sf::Color::Red);
    HardButton.setOutlineThickness(2);
    HardButton.setPosition(100.f, 700.f);
    window.draw(HardButton);

    sf::Text HardText;
    HardText.setFont(font);
    HardText.setString("Hard");
    HardText.setFillColor(sf::Color::Black);
    HardText.setPosition(390, 770);
    HardText.setCharacterSize(100);
    window.draw(HardText);

    window.display();

    sf::Event event;
    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseMoved)
            {
                sf::Vector2i pos = sf::Mouse::getPosition(window);
                if (pos.y > 100 && pos.y <= 350)
                {
                    if (currButton != 1)
                    {
                        EasyButton.setFillColor(sf::Color::Green);
                        currButton = 1;
                        window.clear();
                        window.draw(EasyButton);
                        window.draw(MediumButton);
                        window.draw(HardButton);
                        window.draw(EasyText);
                        window.draw(MediumText);
                        window.draw(HardText);
                        window.display();
                    }
                }
                else if (pos.y > 400 && pos.y <= 650)
                {
                    if (currButton != 2) 
                    {
                        MediumButton.setFillColor(sf::Color::Yellow);
                        currButton = 2;
                        window.clear();
                        window.draw(EasyButton);
                        window.draw(MediumButton);
                        window.draw(HardButton);
                        window.draw(EasyText);
                        window.draw(MediumText);
                        window.draw(HardText);
                        window.display();
                    }
                }
                else if (pos.y > 700 && pos.y <= 950)
                {
                    if (currButton != 3)
                    {
                        HardButton.setFillColor(sf::Color::Red);
                        currButton = 3;
                        window.clear();
                        window.draw(EasyButton);
                        window.draw(MediumButton);
                        window.draw(HardButton);
                        window.draw(EasyText);
                        window.draw(MediumText);
                        window.draw(HardText);
                        window.display();
                    }
                }
                else
                {
                    if (currButton != 0)
                    {
                        EasyButton.setFillColor(sf::Color::White);
                        MediumButton.setFillColor(sf::Color::White);
                        HardButton.setFillColor(sf::Color::White);
                        currButton = 0;
                        window.clear();
                        window.draw(EasyButton);
                        window.draw(MediumButton);
                        window.draw(HardButton);
                        window.draw(EasyText);
                        window.draw(MediumText);
                        window.draw(HardText);
                        window.display();
                    }

                }
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2i pos = sf::Mouse::getPosition(window);
                if (pos.y > 100 && pos.y <= 350)
                {
                    std::cout << "Easy\n";
                    ROWS = 9;
                    COLS = 9;
                    MINES = 10;
                }
                else if (pos.y > 400 && pos.y <= 650)
                {
                    std::cout << "Medium\n";
                    ROWS = 16;
                    COLS = 16;
                    MINES = 40;
                }
                else if (pos.y > 700 && pos.y <= 950)
                {
                    std::cout << "Hard\n";
                    ROWS = 16;
                    COLS = 30;
                    MINES = 99;
                }
                else
                {
                    return choose_difficultyUI();
                }
                return;
            }
        }
    }
}