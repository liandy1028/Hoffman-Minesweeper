# https://massaioli.wordpress.com/2013/01/12/solving-minesweeper-with-matricies/

import minesweeper
import numpy as np
import random
from time import sleep

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
    g = minesweeper.Game()
    if g.mine(7,9,1):
        return 'win'

    global ROWS, COLS
    ROWS, COLS = g.ROWS, g.COLS
    board = np.zeros((g.ROWS, g.COLS), dtype=int)
    moves = np.zeros((g.ROWS, g.COLS), dtype=int)

    go = True
    while go:
        generate_board(g, board)
        reset_moves(moves)
        for x in range(ROWS):
            for y in range(COLS):
                calculate(x, y, board, moves)
        
        if check_moves(moves):
            for x in range(ROWS):
                for y in range(COLS):
                    if moves[x][y] == M_MINE:
                        result = g.mine(x, y)
                        if result:
                            win = result
                            go = False
                    elif moves[x][y] == M_FLAG:
                        g.flag(x, y)
        else:
            facts = {}
            make_facts(board, facts)
            for x in range(ROWS):
                for y in range(COLS):
                    calculate_facts(x, y, board, moves, facts)

            if check_moves(moves):
                for x in range(ROWS):
                    for y in range(COLS):
                        if moves[x][y] == M_MINE:
                            win = g.mine(x, y)
                            if win:
                                go = False
                        elif moves[x][y] == M_FLAG:
                            g.flag(x, y)
            else:
                guesses += 1

                if len(facts):
                    squares = set()
                    for key in facts:
                        for square in key:
                            squares.add(square)
                    square = random.choice(list(squares))

                    win = g.mine(square[0], square[1])
                    if win:
                        go = False
                else:
                    brk = False
                    for x in range(ROWS):
                        for y in range(COLS):
                            if board[x][y] == UNKNOWN:
                                win = g.mine(x, y)
                                if win:
                                    go = False
                                brk = True
                            if brk:
                                break
                        if brk:
                            break


    # g.display()
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
    
def generate_board(g, board):
    for x in range(ROWS):
        for y in range(COLS):
            if g.display_board[x][y] in range(1,9):
                counter = 0
                for dx in (-1,0,1):
                    if 0 <= x + dx < ROWS:
                        for dy in (-1,0,1):
                            if 0 <= y + dy < COLS:
                                if g.display_board[x + dx][y + dy] == g.FLAG:
                                    counter += 1
                board[x][y] = g.display_board[x][y] - counter
            elif g.display_board[x][y] == 0:
                board[x][y] = UNKNOWN
            elif g.display_board[x][y] == g.FLAG:
                board[x][y] = FLAG
            elif g.display_board[x][y] == g.EMPTY:
                board[x][y] = EMPTY
            
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


wins = 0
losses = 0
for _ in range(1000):
    if play_game() == 1:
        wins += 1
    else:
        losses += 1
print(wins, losses)