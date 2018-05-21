import numpy as np
import matplotlib.pyplot as plt
import cv2 as cv

img = cv.imread('map.jpg')
rows, cols, ch = img.shape

pts1 = np.float32(
    [(y, x) for (x, y) in [(340, 734), (595, 770), (668, 249), (882, 1125)]]
)
pts2 = np.float32([(y, x) for (x, y) in
                   [(248 + 35, 1563 - 5), (1101 - 5, 1563 - 10),
                    (1368 - 10, 248 - 10), (1886 - 10, 2705 - 20)]])

# findHomography if npoints > 4
M = cv.getPerspectiveTransform(pts1, pts2)
dst = cv.warpPerspective(img, M, (3148, 1886))
cv.imwrite('project_res.jpg', img=dst)
plt.subplot(121), plt.imshow(img), plt.title('Input')
plt.subplot(122), plt.imshow(dst), plt.title('Output')
plt.show()
