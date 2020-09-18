import numpy as np
import random

class Game:
    ROWS = 14
    COLS = 18
    MINES = 40

    # KEY
    EMPTY = -3
    FLAG = -2
    MINE = -1
    
    def __init__(self):
        self.board = np.zeros((self.ROWS,self.COLS), dtype=int)
        self.mines = np.zeros((self.ROWS,self.COLS), dtype=int)
        self.display_board = np.zeros((self.ROWS,self.COLS), dtype=int)

    def mine(self, x, y, first=0):
        if first:
            self.generate_mines(x, y)
            self.generate_board()
        if self.display_board[x][y] == 0:
            if self.mines[x][y]:
                self.display_board[x][y] = self.MINE
                return -1
            self.display_board[x][y] = self.board[x][y]
            if self.display_board[x][y] == 0:
                self.display_board[x][y] = self.EMPTY
                for dx in (-1,0,1):
                    if 0 <= x + dx < self.ROWS:
                        for dy in (-1,0,1):
                            if 0 <= y + dy < self.COLS:
                                self.mine(x + dx, y + dy)
            if self.calculate_win():
                return 1
        return 0

    def generate_mines(self, x, y):
        for _ in range(self.MINES):
            while True:
                mx = random.randint(0, self.ROWS - 1)
                my = random.randint(0, self.COLS - 1)
                if not self.mines[mx][my]:
                    if abs(mx - x) <= 2 and abs(my - y) <= 2:
                        pass
                    else:
                        self.mines[mx][my] = 1
                        break

    def generate_board(self):
        for x in range(self.ROWS):
            for y in range(self.COLS):
                self.board[x][y] = self.calculate_mines(x, y)
                
    def calculate_mines(self, x, y):
        if self.mines[x][y]:
            return self.MINE
        else:
            counter = 0
            for dx in (-1,0,1):
                if 0 <= x + dx < self.ROWS:
                    for dy in (-1,0,1):
                        if 0 <= y + dy < self.COLS:
                            if self.mines[x + dx][y + dy]:
                                counter += 1
            return counter

    def calculate_win(self):
        for x in range(self.ROWS):
            for y in range(self.COLS):
                if self.mines[x][y] == 0:
                    if self.display_board[x][y] == 0 or self.display_board[x][y] == self.FLAG:
                        return 0
        return 1

    def flag(self, x, y):
        if self.display_board[x][y] == 0:
            self.display_board[x][y] = self.FLAG
        elif self.display_board[x][y] == self.FLAG:
            self.display_board[x][y] = 0

    def display(self):
        for row in self.display_board:
            for col in row:
                if col == 0:
                    print('@', end=' ')
                elif col == self.MINE:
                    print('X', end=' ')
                elif col == self.FLAG:
                    print('^', end=' ')
                elif col == self.EMPTY:
                    print(' ', end=' ')
                else:
                    print(col, end=' ')
            print()
