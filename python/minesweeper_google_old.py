import google_interface
import numpy as np
import random
from time import sleep
import time


# TILES
UNKNOWN = -1
FLAG = -2
EMPTY = -3
ROWS = 0
COLS = 0

# MOVES
M_MINE = 1
M_FLAG = 2

def play_game():
    guesses = 0
    t = time.time()

    global ROWS, COLS
    bbox = google_interface.find_bbox()
    ROWS, COLS = google_interface.find_board_size(bbox)
    board = np.zeros((ROWS, COLS), dtype=int)
    moves = np.zeros((ROWS, COLS), dtype=int)

    if google_interface.mine(int(ROWS / 2), int(COLS / 2)):
        go = False
    else:
        go = True

    while go:
        win = google_interface.update_board()
        if win:
            break

        if t + 300 < time.time():
            break
        
        generate_board(board)
        reset_moves(moves)
        for x in range(ROWS):
            for y in range(COLS):
                calculate(x, y, board, moves)
        
        # if check_moves(moves):
        #     for x in range(ROWS):
        #         for y in range(COLS):
        #             if moves[x][y] == M_MINE:
        #                 google_interface.mine(x, y)
        #             elif moves[x][y] == M_FLAG:
        #                 google_interface.flag(x, y)

        if True:
            facts = {}
            make_facts(board, facts)
            for x in range(ROWS):
                for y in range(COLS):
                    calculate_facts(x, y, board, moves, facts)

            if check_moves(moves):
                for x in range(ROWS):
                    for y in range(COLS):
                        if moves[x][y] == M_MINE:
                            google_interface.mine(x, y)
                        elif moves[x][y] == M_FLAG:
                            google_interface.flag(x, y)
            else:
                guesses += 1

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




    # print(win) 
    # print('Guesses: ' + str(guesses))
    return win

        
def calculate(x, y, board, moves):
    if board[x][y] == UNKNOWN or board[x][y] == FLAG or board[x][y] == EMPTY:
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
    if board[x][y] == UNKNOWN or board[x][y] == FLAG or board[x][y] == EMPTY:
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
            else:
                print(col, end=' ')
        print()
    print()


play_game()

# wins = 0
# losses = 0
# for _ in range(1000):
#     if play_game() == 1:
#         wins += 1
#     else:
#         losses += 1
# print(wins, losses)