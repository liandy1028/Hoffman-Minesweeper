from cv2 import cv2
import numpy as np
from time import sleep
import numtests as tests

def identify_number(img):
    if tests.test_1(img):
        return 1
    elif tests.test_2(img):
        return 2
    elif tests.test_3(img):
        return 3
    elif tests.test_4(img):
        return 4
    elif tests.test_5(img):
        return 5
    elif tests.test_6(img):
        return 6
    elif tests.test_7(img):
        return 7
    else:
        return 0

def crop_img(img):
    contours, _ = cv2.findContours(img, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    max_area = 0
    for cnt in contours:
        pos = cv2.boundingRect(cnt)
        if pos[2] * pos[3] > max_area:
            max_area = pos[2] * pos[3]
            final_cnt = cnt

    if max_area:
        bbox = cv2.boundingRect(final_cnt)
        dimensions = img.shape
        if bbox[3] == dimensions[0] and bbox[2] == dimensions[1]:
            pass
        else:
            return (1, img[bbox[1]:bbox[1] + bbox[3], bbox[0]:bbox[0] + bbox[2]])
    return (0, 0)


def process_img(img, num=0):
    orig = img
    # Threshold for color between blank and unknown
    lowerb = np.array([150,180,200]) 
    upperb = np.array([165,200,235])
    img = cv2.inRange(img, lowerb, upperb)

    failed_crps = 0
    single_color = 0
    crp = crop_img(img)
    if crp[0]:
        img = crp[1]
    else:
        failed_crps = 1

    img = cv2.bitwise_not(img)
    
    crp = crop_img(img)
    if crp[0]:
        img = crp[1]
    elif failed_crps:
        single_color = 1

    img = cv2.bitwise_not(img)

    output = 0
    if single_color:
        if img[0][0] == 255:
            output = 'blank'
        elif img[0][0] == 0:
            # Green range
            if all([(70, 205, 160)[i] < orig[1][1][i] < (105, 220, 185)[i] for i in range(3)]):
                # for row in orig:
                #     for pxl in row:
                        # if all([abs(pxl[i] - (7, 54, 242)[i]) < 10 for i in range(3)]):
                        #     output = 'flag'
                # if output:
                #     pass
                # else:
                #     output = 'unknown'

                # Flag's red
                if all([abs(orig[10][10][i] - (37, 74, 211)[i]) < 10 for i in range(3)]):
                    output = 'flag'
                else:
                    output = 'unknown'

            else:
                output = 'bomb'
    else:
        output = identify_number(img)
        if output == 4 or output == 3:
            # cv2.imwrite(f'python/images/{output}_{num}.png', img)
            pass

    # cv2.imwrite(f'python/images/processed/{NUM}/{NUM}_{num + START}.png', img)
    # cv2.imshow(str(output), img)
    # cv2.waitKey(0)
    # cv2.destroyAllWindows()
    return output

# process_img(cv2.resize(cv2.imread('python/images/555.png'), (30,30)))
# print(cv2.imread('python/images/images/95.png').shape)

# for x in range(8):
#     for y in range(10):
#         img = cv2.imread(f'python/images/{x}_{y}.png')
#         process_img(img)

# NUM = 3
# START = 7
# for i in range(1,2):
#     img = cv2.imread(f'python/images/{NUM}-{i}.png')
#     process_img(img, i)

# sets = set()
# for i in range(96):
#     img = cv2.imread(f'python/images/images/{i}.png')
#     img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
#     sets.add(img.shape)
#     if identify_number(img) == 1:
#         img = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
#         for j in range(img.shape[0]):
#             for k in range(img.shape[1]):
#                 if not any(img[j][k]):
#                     img[j][k] = np.array([0,255,0])
#         # cv2.imshow(f'1_{i}', cv2.resize(img,(300,300)))
#     elif identify_number(img) == 2:
#         img = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
#         for j in range(img.shape[0]):
#             for k in range(img.shape[1]):
#                 if not any(img[j][k]):
#                     img[j][k] = np.array([255,0,0])
#         # cv2.imshow(f'2_{i}', cv2.resize(img, (300,300)))
#     elif identify_number(img) == 3:
#         img = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
#         for j in range(img.shape[0]):
#             for k in range(img.shape[1]):
#                 if not any(img[j][k]):
#                     img[j][k] = np.array([0,0, 255])
#         cv2.imshow(f'3_{i}', cv2.resize(img, (300,300)))
#     else:
#         cv2.imshow(f'{identify_number(img)}_{i}', cv2.resize(img, (300,300)))
#     print(i, img.shape)
#     sleep(0.1)
# if cv2.waitKey() == ord('q'):
#     cv2.destroyAllWindows()

# print(sets)