import cv2
import yaml
import numpy as np

markerLength = 0.0064

# НАСТРОЙКИ ДЛЯ КАЛИБРОВКИ КАМЕРЫ
criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
objp = np.zeros((7*7, 3), np.float32)
objp[:, :2] = np.mgrid[0:7, 0:7].T.reshape(-1, 2)
objpoints = []  # 3d point in real world space
imgpoints = []  # 2d points in image plane.

# ОТКРЫВАЕМ ВИДЕО ПОТОК С КАМЕРЫ
"""
cap = cv2.VideoCapture('video/calibration.mp4')
found = 0

# ИДЕТ КАЛИБРОВКА: Пока не наберет 10 схваченных изображений шахатной доски, будет работать цикл
while found < 10:
    ret, img = cap.read()  # Capture frame-by-frame
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # НАХОЖДЕНИЕ С ПОМОЩЬЮ МЕТОДА OpenCV шахматной доски на изображении
    ret, corners = cv2.findChessboardCorners(gray, (7, 7), None)  # ПОСТАВИЛ РАЗМЕР 7 на ?
    # ДОСКА БЫЛА НАЙДЕНА
    if ret:
        objpoints.append(objp)
        corners2 = cv2.cornerSubPix(gray, corners, (11, 11), (-1, -1), criteria)
        imgpoints.append(corners2)
        # ОТРИСОВЫВАЕТ ДОСКУ
        img = cv2.drawChessboardCorners(img, (7, 7), corners2, ret)
        found += 1
    cv2.imshow('img', img)
    cv2.waitKey(10)
# ИСПОЛЬЗУЕТ ДАННЫЕ С ПОЛУЧЕННЫХ ИЗОБРАЖЕНИЙ ШАХМАТНОЙ ДОСКИ
# ДЛЯ КАЛИБРОВКИ ИЗОБРАЖЕНИЯ
ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, gray.shape[::-1], None, None)

# ЗАПИСЫВАЕМ ДАННЫЕ КАЛИБРОВКИ В ФАЙЛ
data = {'camera_matrix': np.asarray(mtx).tolist(), 'dist_coeff': np.asarray(dist).tolist()}
with open("calibration.yaml", "w") as f:
    yaml.dump(data, f)

cap.release()
cv2.destroyAllWindows()
"""


# САМА ДЕТЕКЦИЯ МАРКЕРА

# ПОДТЯГИВАЕМ СЛОВАРЬ МАРКЕРОВ, ИСПОЛЬЗУЕМ МАРКЕРЫ 6 на 6 из 50 штук
dictionary = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_6X6_100)

# ОТКРЫВАЕМ ВИДЕО ПОТОК С КАМЕРЫ

cap = cv2.VideoCapture(0)
while True:
    ret, frame = cap.read()
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # OpenCV функция для нахождения маркера из выбранного словаря
    res = cv2.aruco.detectMarkers(gray, dictionary)
    print(res[0], res[1], len(res[2]))

    # ОТРИСОВЫВАЕТ
    if len(res[0]) > 0:
        cv2.aruco.drawDetectedMarkers(gray, res[0], res[1])

    cv2.imshow('frame', gray)

    # ВЫХОД С ПОМОЩЬЮ КЛАВИШИ 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()