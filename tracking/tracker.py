import cv2
import sys
import numpy as np

(major_ver, minor_ver, subminor_ver) = (cv2.__version__).split('.')

if __name__ == '__main__':

    # типы трекинга в openCV, чтобы не искать по 100 раз
    tracker_types = ['BOOSTING', 'MIL', 'KCF', 'TLD', 'MEDIANFLOW', 'GOTURN']
    tracker = cv2.TrackerBoosting_create()

    # читаем видео
    video = cv2.VideoCapture("video/video.mp4")

    # выходим, если все плохо
    if not video.isOpened():
        print("Не открывается видео")
        sys.exit()

    # читаем первое изображение
    ok, frame = video.read()
    if not ok:
        print('Невозможно прочитать видео')
        sys.exit()

    # начальная рамка
    bbox = (287, 23, 86, 320)

    # другая рамка
    bbox = cv2.selectROI(frame, False)

    # инициализация трекинга для первого изображения
    ok = tracker.init(frame, bbox)

    while True:
        # читаем новое изображение
        ok, frame = video.read()
        if not ok:
            break

        # старт таймера
        timer = cv2.getTickCount()

        # обновляем изображение
        ok, bbox = tracker.update(frame)

        # считаем ФПС
        fps = cv2.getTickFrequency() / (cv2.getTickCount() - timer)

        #  рисуем квадрат
        if ok:
            p1 = (int(bbox[0]), int(bbox[1]))
            p2 = (int(bbox[0] + bbox[2]), int(bbox[1] + bbox[3]))
            cv2.rectangle(frame, p1, p2, (255, 0, 0), 2, 1)
        else:
            # ошибка трекинга
            cv2.putText(frame, "Ошибка трекера", (100, 80), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 255), 2)
        cv2.putText(frame, "FPS : " + str(int(fps)), (100, 50), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (50, 170, 50), 2)
        # kernel = np.array([[0, 1, 0], [1, -4, 1], [0, 1, 0]])
        # output = cv2.filter2D(frame, -1, kernel)
        cv2.imshow("Track", frame)
        k = cv2.waitKey(1) & 0xff
        if k == 27:
            break