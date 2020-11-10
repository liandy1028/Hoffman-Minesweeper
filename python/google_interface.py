from cv2 import cv2
import pyautogui
import numpy as np
from time import sleep
import digit_recognition
from pynput import keyboard

# sleep(3)

DELAY = 1
# pyautogui.PAUSE = 0

BOARD_THRESH = 185

display_board = []
FLAG = -2
EMPTY = -3
UNKNOWN = 0
MINE = -1

TILES = {
    'unknown' : UNKNOWN,
    'blank' : EMPTY,
    'bomb' : MINE,
    'flag' : FLAG
}

BBOX = 0
HEIGHT = 0
WIDTH = 0

def mine(x, y):
    pyautogui.click((y + 0.5) * BBOX[2] / WIDTH + BBOX[0], (x + 0.5) * BBOX[3] / HEIGHT + BBOX[1], button='left')
    pyautogui.click(button='middle')
    # sleep(DELAY)

def flag(x, y):
    pyautogui.click((y + 0.5) * BBOX[2] / WIDTH + BBOX[0], (x + 0.5) * BBOX[3] / HEIGHT + BBOX[1], button='right')
    # sleep(DELAY)

def update_board():
    pyautogui.moveTo(BBOX[0] - 10, BBOX[1] - 10)
    sleep(DELAY)

    win = 0

    screenshot = pyautogui.screenshot(region=BBOX)
    screenshot = cv2.cvtColor(np.array(screenshot), cv2.COLOR_RGB2BGR)

    t_width = BBOX[2] / WIDTH
    t_height = BBOX[3] / HEIGHT

    global display_board
    display_board = np.zeros((HEIGHT, WIDTH), dtype=int)
    brk = False
    for x in range(HEIGHT):
        for y in range(WIDTH):
            img = screenshot[int(x * t_height):int((x + 1) * t_height), int(y * t_width):int((y + 1) * t_width)]
            img = cv2.resize(img, (30,30))
            # cv2.imwrite(f'python/im{x}_{y}.png', img)
            val = digit_recognition.process_img(img, f'{x}_{y}')
            if val in range(9):
                display_board[x][y] = val
            else:
                if val == 'bomb':
                    win = -1
                    brk = True
                display_board[x][y] = TILES[val]
            if brk:
                break
        if brk:
            break

    # display(display_board)
    return win
            

def find_board_size(bbox):
    img = pyautogui.screenshot(region=bbox)
    img = cv2.cvtColor(np.array(img), cv2.COLOR_RGB2GRAY)

    _, img = cv2.threshold(img, BOARD_THRESH, 255, cv2.THRESH_BINARY)
    # cv2.imwrite('python/board.png', img)

    test_row = int(bbox[3] / 2)
    old_pxl0 = img[test_row][0]
    old_pxl1 = img[test_row + 10][0]
    width0 = 1
    width1 = 1
    for pxl in img[test_row]:
        if pxl != old_pxl0:
            width0 += 1 
            old_pxl0 = pxl
    for pxl in img[test_row + 10]:
        if pxl != old_pxl1:
            width1 += 1
            old_pxl1 = pxl 
    width = max(width0, width1)

    test_col = int(bbox[2] / 2)
    old_pxl0 = img[0][test_col]
    old_pxl1 = img[0][test_col + 10]
    height0 = 1
    height1 = 1
    for pxl in range(bbox[3]):
        if img[pxl][test_col] != old_pxl0:
            height0 += 1
            old_pxl0 = img[pxl][test_col]
    for pxl in range(bbox[3]):
        if img[pxl][test_col + 10] != old_pxl1:
            height1 += 1
            old_pxl1 = img[pxl][test_col + 10]
    height = max(height0, height1)

    global BBOX, HEIGHT, WIDTH
    BBOX, HEIGHT, WIDTH = (bbox, height, width)

    global display_board
    display_board = np.zeros((HEIGHT, WIDTH), dtype=int)

    return (height, width)

def find_bbox():
    img = pyautogui.screenshot()
    img = cv2.cvtColor(np.array(img), cv2.COLOR_RGB2BGR)

    lowerb = np.array([0, 150, 0])
    upperb = np.array([200, 255, 200])
    img = cv2.inRange(img, lowerb, upperb)

    contours, _ = cv2.findContours(img, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    max_area = 0
    for cnt in contours:
        pos = cv2.boundingRect(cnt)
        if pos[2] * pos[3] > max_area:
            max_area = pos[2] * pos[3]
            final_cnt = cnt

    bbox = cv2.boundingRect(final_cnt)
    return bbox

def display(board):
    print("INTERFACE")
    for row in board:
        for col in row:
            if col == UNKNOWN:
                print('@', end=' ')
            elif col == MINE:
                print('X', end=' ')
            elif col == FLAG:
                print('^', end=' ')
            elif col == EMPTY:
                print(' ', end=' ')
            else:
                print(col, end=' ')
        print()
    print()

def speed(fast):
    if fast:
        pyautogui.PAUSE = 0
    else:
        pyautogui.PAUSE = 0.1

# def on_press(key):
#     if key == keyboard.Key.esc:
#         return False
#     elif key == keyboard.Key.enter:
#         update_board()
#         print(display_board)

# find_board_size(find_bbox())

# with keyboard.Listener(on_press=on_press) as k:
#     k.join()
