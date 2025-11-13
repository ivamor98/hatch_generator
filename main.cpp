
/*!
\file
\brief Тестовое задание

Данный файл содержит в себе модуль для генерации штриховки (hatch) внутри
заданного контура квадратной или прямоугольной детали. Стороны квадрата параллельны осям.
Программа принимает на вход массив точек, описывающий простой квадрат или прямоугольник,
угол наклона штриховки относительно горизонтальной оси в градусах (0–180°) и шаг между линиями штриховки в мм (положительное число).
Программа запускается командой: ./hatch_generator --angle 45 --step 1
где --angle - угол наклона штриховки, --step - шаг.
\warning Неверный формат вызыва программы может привести к ее аварийному завершению
*/

#include <iostream>
#include <vector>
#include <cmath>

/// \brief Структура, хранящая координату точки на двухмерной плостости
struct Point_2
{
    double x;
    double y;

    Point_2() {};
    Point_2(double inX, double inY) : x(inX), y(inY) {};
};

/*!
    \brief Структура, хранящая координаты двух точек одной линии

    Точки объявлены структурами типа Point_2
*/
struct Line_2
{
    Point_2 firstPoint;
    Point_2 secondPoint;

    Line_2() {};
    Line_2(Point_2 inFirstPoint, Point_2 inSecondPoint) : firstPoint(inFirstPoint), secondPoint(inSecondPoint) {};
};

/*!
Функция находит верхний левый угол четырехугольника
\param inContoursPoints вектор точек, описывающий простой квадрат или прямоугольник
\return координаты верхнего левый угла четырехугольника типа Point_2
*/
Point_2 findTopLeftCorner(const std::vector<Point_2> &inContoursPoints)
{
    auto it = inContoursPoints.begin();
    Point_2 topLeftCorner = *it;
    it++;
    for (; it != inContoursPoints.end(); ++it)
    {
        if (it->y > topLeftCorner.y && it->x <= topLeftCorner.x)
            topLeftCorner = *it;
    }
    return topLeftCorner;
}

/*!
Функция находит нижний правый угол четырехугольника
\param inContoursPoints вектор точек, описывающий простой квадрат или прямоугольник
\return координаты нижнего правого угла четырехугольника типа Point_2
*/
Point_2 findBottomRightCorner(const std::vector<Point_2> &inContoursPoints)
{
    auto it = inContoursPoints.begin();
    Point_2 bottomRightCorner = *it;
    it++;
    for (; it != inContoursPoints.end(); ++it)
    {
        if (it->y <= bottomRightCorner.y && it->x > bottomRightCorner.x)
            bottomRightCorner = *it;
    }
    return bottomRightCorner;
}

/*!
Функция переводит урадусы угла в радианы
\param degrees градусы
\return значение в радианах
*/
double degreesToRadians(double degrees)
{
    return degrees * M_PI / 180;
}

/*!
    \brief Класс, генерирующий штриховку по заданным координатам четырехуголника


    \warning Данный класс не производит проверку входных данных. Координаты четырехуголника, шаг и угол штриховки должны быть заданы корректно.
*/

class HatchGenerator
{
private:
    std::vector<Line_2> hatch; ///<  Вектор линий штриховки

public:
    HatchGenerator() = default;

    /// \brief Функция печатает содержание вектора линий штриховки в формате Line 1: (0,0) -> (10,10)
    void printHatch()
    {
        for (int i = 0; i < hatch.size(); ++i)
        {
            std::cout << "Line " << i + 1 << ": (" << hatch[i].firstPoint.x << "," << hatch[i].firstPoint.y << ") -> ("
                      << hatch[i].secondPoint.x << "," << hatch[i].secondPoint.y << ")" << std::endl;
        }
    }

    // генерируем штриховку, angel угол (0 - 180), step - шаг в миллиметрах, положительное число

    /*!
        Вычисляет точки пересечения линий штриховки со сторонами прямоугольника
        \param inContoursPoints координаты четырех углов прямоугольника
        \param angel угол в градусах (0 - 180)
        \param step шаг в миллиметрах
        \return  Вектор линий штриховки std::vector<Line_2> hatch
    */
    std::vector<Line_2> generateHatch(const std::vector<Point_2> &inContoursPoints, const double &angel, const double &step)
    {
        Point_2 firstPoint;
        Point_2 secondPoint;
        Point_2 topLeftCorner = findTopLeftCorner(inContoursPoints); // находим границы квадрата по двум противоположным углам
        Point_2 bottomRightCorner = findBottomRightCorner(inContoursPoints);
        if (angel == 90) // при угле 90 градусов x = const
        {

            for (double Xi = topLeftCorner.x + step; Xi < bottomRightCorner.x; Xi += step)
            {

                firstPoint.x = Xi;
                firstPoint.y = topLeftCorner.y;
                secondPoint.x = Xi;
                secondPoint.y = bottomRightCorner.y;

                hatch.emplace_back(firstPoint, secondPoint);
            }
        }
        else if (angel == 0) // при угле 0 градусов y = const
        {
            for (double Yi = bottomRightCorner.y + step; Yi < topLeftCorner.y; Yi += step)
            {

                firstPoint.x = topLeftCorner.x;
                firstPoint.y = Yi;
                secondPoint.x = bottomRightCorner.x;
                secondPoint.y = Yi;

                hatch.emplace_back(firstPoint, secondPoint);
            }
        }
        else if (angel > 0 && angel < 90) // вычисляем функцию каждой линии y = k*x+b + stepY, где k = tg(angel), stepY -  шаг штриховки по оси y.
        {
            double angelInRadians = degreesToRadians(angel);
            double k = tan(angelInRadians);
            double stepY;

            stepY = step / cos(angelInRadians);
            double b = topLeftCorner.y - k * topLeftCorner.x;            // найдем b первого штриха в верхнем левом углу прямоугольника, от которого начнем отчет
            double maxB = bottomRightCorner.y - k * bottomRightCorner.x; // найдем b последнего возможного штриха
            while (b > maxB)
            {
                b -= stepY;
                firstPoint.y = topLeftCorner.y;
                firstPoint.x = (firstPoint.y - b) / k;
                if (firstPoint.x > bottomRightCorner.x)
                {
                    firstPoint.x = bottomRightCorner.x;
                    firstPoint.y = k * firstPoint.x + b;
                    if (firstPoint.y < bottomRightCorner.y)
                        break;
                }

                secondPoint.x = topLeftCorner.x;
                secondPoint.y = k * secondPoint.x + b;
                if (secondPoint.y < bottomRightCorner.y)
                {
                    secondPoint.y = bottomRightCorner.y;
                    secondPoint.x = (secondPoint.y - b) / k;
                    if (secondPoint.x > bottomRightCorner.x)
                        break;
                }
                hatch.emplace_back(firstPoint, secondPoint);
            }
        }
        else if (angel > 90 && angel < 180)
        {
            double angelInRadians = degreesToRadians(angel);
            double k = tan(angelInRadians);
            double stepY;

            stepY = step / cos(degreesToRadians(180 - angel));
            double b = topLeftCorner.y - k * bottomRightCorner.x;    // найдем b первого штриха в верхнем правом углу прямоугольника, от которого начнем отчет
            double maxB = bottomRightCorner.y - k * topLeftCorner.x; // найдем b последнего возможного штриха
            while (b > maxB)
            {
                b -= stepY;
                firstPoint.y = topLeftCorner.y;
                firstPoint.x = (firstPoint.y - b) / k;
                if (firstPoint.x < topLeftCorner.x)
                {
                    firstPoint.x = topLeftCorner.x;
                    firstPoint.y = k * firstPoint.x + b;
                    if (firstPoint.y < bottomRightCorner.y)
                        break;
                }

                secondPoint.x = bottomRightCorner.x;
                secondPoint.y = k * secondPoint.x + b;
                if (secondPoint.y < bottomRightCorner.y)
                {
                    secondPoint.y = bottomRightCorner.y;
                    secondPoint.x = (secondPoint.y - b) / k;
                    if (secondPoint.x < topLeftCorner.x)
                        break;
                }
                hatch.emplace_back(firstPoint, secondPoint);
            }
        }
        return hatch;
    }
};

int main(int argc, char *argv[])
{

    std::vector<Point_2> contoursPoints = {
        {0, 0}, {10, 0}, {10, 10}, {0, 10}};

    double angel = atof(argv[2]);
    double step = atof(argv[4]);

    HatchGenerator testHatch;

    testHatch.generateHatch(contoursPoints, angel, step);
    testHatch.printHatch();

    return 0;
}