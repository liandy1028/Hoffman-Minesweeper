import google_interface
import numpy as np
import random
from time import sleep
import time
from pynput import keyboard



google_interface.DELAY = 1.1



# TILES
UNKNOWN = -1
FLAG = -2
EMPTY = -3
UNCOVERED = -4
ROWS = 0
COLS = 0

# MOVES
M_MINE = 1
M_FLAG = 2

def play_game():
    sleep(0.1)
    guesses = 0
    t = time.time()

    global ROWS, COLS
    bbox = google_interface.find_bbox()
    ROWS, COLS = google_interface.find_board_size(bbox)
    board = np.zeros((ROWS, COLS), dtype=int)
    moves = np.zeros((ROWS, COLS), dtype=int)
    fin_moves = np.zeros((ROWS, COLS), dtype=int)

    google_interface.mine(int(ROWS / 2), int(COLS / 2))

    # for _ in range(5):
    #     google_interface.mine(random.randrange(0,ROWS), random.randrange(0,COLS))

    update = True
    while True:
        if update:
            win = google_interface.update_board()
        if win:
            break

        if t + 300 < time.time():
            break

        if update:
            generate_board(board)

        reset_moves(moves)
        for x in range(ROWS):
            for y in range(COLS):
                calculate(x, y, board, moves)

        facts = {}
        make_facts(board, facts)
        for x in range(ROWS):
            for y in range(COLS):
                calculate_facts(x, y, board, moves, facts)

        if check_moves(moves):
            update_current_board(board, moves)
            update = False
            for x in range(ROWS):
                for y in range(COLS):
                    if moves[x][y]:
                        fin_moves[x][y] = moves[x][y]
           
        else:
            if update:
                guesses += 1
                print(f'Guessing... ({guesses})')

                if len(facts):
                    squares = set()
                    for key in facts:
                        for square in key:
                            squares.add(square)
                    square = random.choice(list(squares))

                    google_interface.mine(square[0], square[1])
                else:
                    brk = False
                    for x in range(ROWS):
                        for y in range(COLS):
                            if board[x][y] == UNKNOWN:
                                google_interface.mine(x, y)
                                brk = True
                            if brk:
                                break
                        if brk:
                            break
            else:
                update = True
                for x in range(ROWS):
                    for y in range(COLS):
                        if fin_moves[x][y] == M_FLAG:
                            google_interface.flag(x, y)
                for x in range(ROWS):
                    for y in range(COLS):
                        if fin_moves[x][y] == M_MINE:
                            google_interface.mine(x, y)
                reset_moves(fin_moves)

                
    # print(win) 
    # print('Guesses: ' + str(guesses))
    return win

def update_current_board(board, moves):
    for x in range(ROWS):
        for y in range(COLS):
            if moves[x][y] == M_FLAG:
                board[x][y] = FLAG
                for dx in (-1,0,1):
                    if 0 <= x + dx < ROWS:
                        for dy in (-1,0,1):
                            if 0 <= y + dy < COLS:
                                if board[x + dx][y + dy] in range(9):
                                    board[x + dx][y + dy] -= 1
            elif moves[x][y] == M_MINE:
                board[x][y] = UNCOVERED
    # display(board)

        
def calculate(x, y, board, moves):
    if board[x][y] == UNKNOWN or board[x][y] == FLAG or board[x][y] == EMPTY or board[x][y] == UNCOVERED:
        return 0
    elif board[x][y] == 0:
        for dx in (-1,0,1):
            if 0 <= x + dx < ROWS:
                for dy in (-1,0,1):
                    if 0 <= y + dy < COLS:
                        if board[x + dx][y + dy] == UNKNOWN:
                            moves[x + dx][y + dy] = M_MINE
    else:
        counter = 0
        for dx in (-1,0,1):
            if 0 <= x + dx < ROWS:
                for dy in (-1,0,1):
                    if 0 <= y + dy < COLS:
                        if board[x + dx][y + dy] == UNKNOWN:
                            counter += 1
        if board[x][y] == counter:
            for dx in (-1,0,1):
                if 0 <= x + dx < ROWS:
                    for dy in (-1,0,1):
                        if 0 <= y + dy < COLS:
                            if board[x + dx][y + dy] == UNKNOWN:
                                moves[x + dx][y + dy] = M_FLAG

def calculate_facts(x, y, board, moves, facts):
    if board[x][y] == UNKNOWN or board[x][y] == FLAG or board[x][y] == EMPTY or board[x][y] == UNCOVERED:
        return 0
    else:
        affected_squares = []
        for dx in (-1,0,1):
            if 0 <= x + dx < ROWS:
                for dy in (-1,0,1):
                    if 0 <= y + dy < COLS:
                        if board[x + dx][y + dy] == UNKNOWN:
                            affected_squares.append((x + dx, y + dy))
        for key, val in facts.items():
            if len(key) > len(affected_squares):
                if all([square in key for square in affected_squares]):
                    more_squares = []
                    for square in key:
                        if square not in affected_squares:
                            more_squares.append(square)
                    if len(more_squares) == val - board[x][y]:
                        for square in more_squares:
                            moves[square[0]][square[1]] = M_FLAG
                    elif val == board[x][y]:
                        for square in more_squares:
                            moves[square[0]][square[1]] = M_MINE
            elif len(key) < len(affected_squares):
                if all([square in affected_squares for square in key]):
                    more_squares = []
                    for square in affected_squares:
                        if square not in key:
                            more_squares.append(square)
                    if len(more_squares) == board[x][y] - val:
                        for square in more_squares:
                            moves[square[0]][square[1]] = M_FLAG
                    elif board[x][y] == val:
                        for square in more_squares:
                            moves[square[0]][square[1]] = M_MINE
            if val == 1 and board[x][y] > 1:
                if any([square in affected_squares for square in key]):
                    more_squares = []
                    for square in affected_squares:
                        if square not in key:
                            more_squares.append(square)
                    if len(more_squares) == board[x][y] - 1:
                        for square in more_squares:
                            moves[square[0]][square[1]] = M_FLAG

def check_moves(moves):
    for x in range(ROWS):
        for y in range(COLS):
            if moves[x][y]:
                return 1
    return 0

def reset_moves(moves):
    for x in range(ROWS):
        for y in range(COLS):
            moves[x][y] = 0    
    
def generate_board(board):
    for x in range(ROWS):
        for y in range(COLS):
            if google_interface.display_board[x][y] in range(1,9):
                counter = 0
                for dx in (-1,0,1):
                    if 0 <= x + dx < ROWS:
                        for dy in (-1,0,1):
                            if 0 <= y + dy < COLS:
                                if google_interface.display_board[x + dx][y + dy] == google_interface.FLAG:
                                    counter += 1
                board[x][y] = google_interface.display_board[x][y] - counter
            elif google_interface.display_board[x][y] == 0:
                board[x][y] = UNKNOWN
            elif google_interface.display_board[x][y] == google_interface.FLAG:
                board[x][y] = FLAG
            elif google_interface.display_board[x][y] == google_interface.EMPTY:
                board[x][y] = EMPTY
    # display(board)
            
def make_facts(board, facts):
    for x in range(ROWS):
        for y in range(COLS):
            if board[x][y] in range(1,9):
                squares = []
                for dx in (-1,0,1):
                    if 0 <= x + dx < ROWS:
                        for dy in (-1,0,1):
                            if 0 <= y + dy < COLS:
                                if board[x + dx][y + dy] == UNKNOWN:
                                    squares.append((x + dx, y + dy))
                facts[tuple(squares)] = board[x][y]
def display(board):
    print("GAME")
    for row in board:
        for col in row:
            if col == UNKNOWN:
                print('@', end=' ')
            # elif col == self.MINE:
            #     print('X', end=' ')
            elif col == FLAG:
                print('^', end=' ')
            elif col == EMPTY:
                print(' ', end=' ')
            elif col == UNCOVERED:
                print('^', end=' ')
            elif col == 0:
                print('.', end=' ')
            else:
                print(col, end=' ')
        print()
    print()

def on_press(key):
    if key == keyboard.Key.esc:
        return False
    elif key == keyboard.KeyCode.from_char('.'):
        google_interface.speed(True)
    elif key == keyboard.KeyCode.from_char(','):
        google_interface.speed(False)
    elif key == keyboard.Key.ctrl_r:
        play_game()

# play_game()

with keyboard.Listener(on_press=on_press) as kL:
    kL.join()

# wins = 0
# losses = 0
# for _ in range(1000):
#     if play_game() == 1:
#         wins += 1
#     else:
#         losses += 1
# print(wins, losses)