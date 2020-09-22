from cv2 import cv2

def test_1(img):
    if not img[0][0]:
        return False
    elif img[1][-2]:
        return False
    elif img[3][2]:
        return False
    elif any(img[1:-2, -2]):
        return False
    elif not all(img[8:, 2]):
        return False
    return True

def test_2(img):
    if any(img[-2, 1:-1]):
        return False
    return True

def test_3(img):
    if img[3,2]:
        return False
    elif not img[9,0]:
        return False
    elif img[1,6]:
        return False
    elif not img[9,2]:
        return False
    elif img[9,6]:
        return False
    elif not img[-1,-1]:
        return False
    elif not img[9,-1]:
        return False
    return True

def test_4(img):
    if any(img[1:-2, -3]):
        return False
    elif any(img[-5, 1:-2]):
        return False
    return True

def test_5(img):
    # col = img[:,2]
    # if col[0]:
    #     return False
    # change = 0
    # for pxl in col:
    #     if bool(pxl) != change % 2:
    #         change += 1
    # if change != 3:
    #     return False
    
    for i in range(5):
        if img[8 + i, 2]:
            if all(img[8 + i, 0:5]):
                return True
    return False

    # return True

def test_6(img):
    # cv2.imwrite('python/images/6.png', img)
    return True

def test_7(img):
    return False


# print(test_5(cv2.cvtColor(cv2.imread(f'python/images/6_2_6.png'), cv2.COLOR_BGR2GRAY)))