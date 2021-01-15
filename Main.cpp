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

bool game_running = true;
bool game_won = false;

sf::RenderWindow window(sf::VideoMode(1712, 963), "Minesweeper");
sf::Color background(139, 190, 255);
sf::Color lightGreen(51, 255, 44);
sf::Color lightRed(255, 67, 67);
sf::Color lightYellow(224, 255, 65);


int boardx = 50;
int boardy = 50;
int tileSize = 30;


void create_board();
void generate_board(int x, int y);
char display_map(int x);
sf::Color display_map_colored();
bool mine(int x, int y);
bool flag(int x, int y);
bool calculate_win();
void reset_board();
void choose_difficultyUI();
void update_board_info(sf::Event event, int x, int y);
bool play_againUI();
void draw_tile(int x, int y, int data);
void displayUI(int** board);
void text_Formatter(sf::Text& text, std::string str, sf::Color color, float x, float y, int char_size);
void println(std::string s)
{
    std::cout << s << std::endl;
}

// TODO run game
int main()
{
    std::ios_base::sync_with_stdio(false);
    bool playing = true;
    while (playing)
    {
        window.setPosition(sf::Vector2i(0, 0));
        choose_difficultyUI();
        window.display();
        create_board();
        int x = 0;
        int y = 0;

        displayUI(display_board);
        sf::Event event;
        while (window.isOpen() && game_running)
        {
            while ((FIRST_MOVE || !calculate_win()) && window.pollEvent(event))
            {
                sf::Vector2i pos = sf::Mouse::getPosition(window);
                update_board_info(event, pos.x, pos.y);
            }
        }
        reset_board();
        playing = play_againUI();


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

char display_map(int x)
{
    if (x >= 1 && x <= 8)
    {
        return '0' + x;
    }
    if (x == 9 || x == 0)
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

sf::Color display_map_colored(int x)
{
    if (x == 0)
    {
        return sf::Color::Green;
    }
    else if (x == 9)
    {
        sf::Color brown(210, 105, 30);
        return brown;
    }
    else if (x == 10)
    {
        return sf::Color::Red;
    }
    return sf::Color::White;
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
                game_running = false;
                game_won = false;
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
    game_running = false;
    game_won = true;
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
    FIRST_MOVE = true;
    game_running = true;
    window.clear(background);
}

void text_Formatter(sf::Text& text, std::string str, sf::Color color, float x, float y, int char_size)
{
    text.setString(str);
    text.setFillColor(color);
    text.setPosition(x, y);
    text.setCharacterSize(char_size);
}

void choose_difficultyUI()
{
    int currButton = 0;
    sf::Font font;
    font.loadFromFile("ArialCE.ttf");

    sf::Text welcome;
    welcome.setFont(font);
    text_Formatter(welcome, "Welcome to Minesweeper", sf::Color::White, 300, 100, 100);
    window.draw(welcome);

    sf::RectangleShape EasyButton(sf::Vector2f(600.f, 150.f));
    EasyButton.setPosition(530.f, 300.f);
    window.draw(EasyButton);

    sf::Text EasyText;
    EasyText.setFont(font);
    text_Formatter(EasyText, "Easy", sf::Color::Black, 765, 340, 60);
    window.draw(EasyText);

    sf::RectangleShape MediumButton(sf::Vector2f(600.f, 150.f));
    MediumButton.setPosition(530.f, 500.f);
    window.draw(MediumButton);

    sf::Text MediumText;
    MediumText.setFont(font);
    text_Formatter(MediumText, "Medium", sf::Color::Black, 725, 540, 60);
    window.draw(MediumText);

    sf::RectangleShape HardButton(sf::Vector2f(600.f, 150.f));
    HardButton.setPosition(530.f, 700.f);
    window.draw(HardButton);

    sf::Text HardText;
    HardText.setFont(font);
    text_Formatter(HardText, "Hard", sf::Color::Black, 765, 740, 60);
    window.draw(HardText);

    window.display();

    sf::Event event;
    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            sf::Vector2i pos = sf::Mouse::getPosition(window);

            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseMoved)
            {
                if (pos.x < 530 || pos.x > 1130)
                {
                    if (currButton != 0)
                    {
                        EasyButton.setFillColor(sf::Color::White);
                        MediumButton.setFillColor(sf::Color::White);
                        HardButton.setFillColor(sf::Color::White);
                        currButton = 0;
                    }
                }
                else if (pos.y > 300 && pos.y <= 450)
                {
                    if (currButton != 1)
                    {
                        EasyButton.setFillColor(lightGreen);
                        currButton = 1;
                    }
                }
                else if (pos.y > 500 && pos.y <= 650)
                {
                    if (currButton != 2)
                    {
                        MediumButton.setFillColor(lightYellow);
                        currButton = 2;
                    }
                }
                else if (pos.y > 700 && pos.y <= 850)
                {
                    if (currButton != 3)
                    {
                        HardButton.setFillColor(lightRed);
                        currButton = 3;
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
                    }

                }
                window.clear(background);
                window.draw(EasyButton);
                window.draw(MediumButton);
                window.draw(HardButton);
                window.draw(EasyText);
                window.draw(MediumText);
                window.draw(HardText);
                window.draw(welcome);
                window.display();
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (pos.x < 550 || pos.x > 1150)
                {
                    return choose_difficultyUI();
                }
                else if (pos.y > 300 && pos.y <= 450)
                {
                    std::cout << "Easy\n";
                    ROWS = 9;
                    COLS = 9;
                    MINES = 10;
                    tileSize = 800 / 9;
                }
                else if (pos.y > 500 && pos.y <= 650)
                {
                    std::cout << "Medium\n";
                    ROWS = 16;
                    COLS = 16;
                    MINES = 40;
                    tileSize = 800 / 16;
                }
                else if (pos.y > 700 && pos.y <= 850)
                {
                    std::cout << "Hard\n";
                    ROWS = 16;
                    COLS = 30;
                    MINES = 99;
                    tileSize = 1600 / 30;
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

void draw_tile(int x, int y, int numCode)
{
    sf::Font font;
    font.loadFromFile("calibri.ttf");
    char out = display_map(numCode);
    sf::Color tileCol = display_map_colored(numCode);
    if ((x + y) % 2 == 0)
    {
        tileCol.a -= 30;
    }

    int xPosition = x * tileSize + boardx;
    int yPosition = y * tileSize + boardy;

    sf::RectangleShape tile(sf::Vector2f(tileSize, tileSize));
    tile.setOutlineColor(sf::Color::Black);
    tile.setOutlineThickness(2);
    tile.setFillColor(tileCol);
    tile.setPosition(xPosition, yPosition);
    window.draw(tile);

    std::string s(1, out);
    sf::Text num;
    num.setFont(font);
    text_Formatter(num, s, sf::Color::Black, xPosition + tileSize / 3, yPosition + tileSize / 7, tileSize / 2);
    window.draw(num);
}

void displayUI(int** board)
{
    window.clear(background);
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            draw_tile(j, i, board[i][j]);
        }
    }
    window.display();

}


void update_board_info(sf::Event event, int x, int y)
{
    int tilex = (x - boardx) / tileSize;
    int tiley = (y - boardy) / tileSize;
    if (event.type == sf::Event::Closed)
    {
        window.close();
    }
    else if (event.type == sf::Event::MouseButtonPressed)
    {
        std::cout << tilex << ", " << tiley << std::endl;
        if (x >= boardx && tilex < COLS &&
            y >= boardy && tiley < ROWS)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                std::cout << "mining" << std::endl;
                mine(tiley, tilex);
                displayUI(display_board);
            }
            else if (event.mouseButton.button == sf::Mouse::Right)
            {
                std::cout << "flagging" << std::endl;

                flag(tiley, tilex);
                displayUI(display_board);
            }

        }
    }
    else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F)
    {
        if (x >= boardx && x <= tilex <= COLS &&
            y >= boardy && y <= tiley <= ROWS)
        {
            flag(tiley, tilex);
            displayUI(display_board);
        }
    }
}

//asks user to play again, creating 2 buttons; if yes, return true; if no, close window, return false
bool play_againUI()
{
    int hovered = 0;
    sf::Font font;
    font.loadFromFile("calibri.ttf");

    sf::Text WinLoss;
    WinLoss.setFont(font);
    if (game_won)
    {
        text_Formatter(WinLoss, "You won!", sf::Color::White, 530, 2, 175);
    }
    else
    {
        text_Formatter(WinLoss, "You lost!", sf::Color::White, 530, 2, 175);
    }
    window.draw(WinLoss);

    sf::Text Prompt;
    Prompt.setFont(font);
    text_Formatter(Prompt, "Play again?", sf::Color::White, 625, 170, 100);
    window.draw(Prompt);

    sf::RectangleShape YesButton(sf::Vector2f(800.f, 250.f));
    YesButton.setFillColor(sf::Color::White);
    YesButton.setPosition(450.f, 300.f);
    window.draw(YesButton);

    sf::Text YesText;
    YesText.setFont(font);
    text_Formatter(YesText, "Yes", sf::Color::Black, 775, 360, 100);
    window.draw(YesText);

    sf::RectangleShape NoButton(sf::Vector2f(800.f, 250.f));
    NoButton.setFillColor(sf::Color::White);
    NoButton.setPosition(450.f, 600.f);
    window.draw(NoButton);

    sf::Text NoText;
    NoText.setFont(font);
    text_Formatter(NoText, "No", sf::Color::Black, 795, 660, 100);
    window.draw(NoText);

    window.display();

    sf::Event event;

    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            sf::Vector2i pos = sf::Mouse::getPosition(window);

            //window closed
            if (event.type == sf::Event::Closed)
            {
                window.close();
                return false;
            }
            //left click pressed, test for position and button press
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                //clicked yes
                if (pos.y >= 300 && pos.y <= 550 && pos.x > 450 && pos.x < 1250)
                {
                    println("clicked yes");
                    window.clear(background);
                    return true;
                }
                //clicked no
                else if (pos.y >= 600 && pos.y <= 850 && pos.x > 450 && pos.x < 1250)
                {
                    println("clicked no");
                    window.close();
                    return false;
                }
            }
            //highlight when hover
            else if (event.type == sf::Event::MouseMoved)
            {
                if (pos.y >= 300 && pos.y <= 550 && pos.x > 450 && pos.x < 1250)
                {
                    if (hovered != 1)
                    {
                        YesButton.setFillColor(lightGreen);
                        hovered = 1;
                    }
                }
                else if (pos.y >= 600 && pos.y <= 850 && pos.x > 450 && pos.x < 1250)
                {
                    if (hovered != 2)
                    {
                        NoButton.setFillColor(lightRed);
                        hovered = 2;
                    }
                }
                else
                {
                    if (hovered != 0)
                    {
                        YesButton.setFillColor(sf::Color::White);
                        NoButton.setFillColor(sf::Color::White);
                        hovered = 0;
                    }
                }
                window.clear(background);
                window.draw(WinLoss);
                window.draw(Prompt);
                window.draw(YesButton);
                window.draw(NoButton);
                window.draw(NoText);
                window.draw(YesText);
                window.display();
            }
        }
    }
}
