# Общая информация по научной работе.
## 1. Описание научной работы
LOD's Generator - научная работа по генерации уровней детализации
для геометрических моделей. 
## 2. Требования
<p> <b> Обязательно: </b> </p>
 Реализация пересчета нормалей, регуляция степени оптимизации, а также реализация различных алгоритмов оптимизации.<br>

<p> <b> Реализация: </b> </p>
 Гибридная (CPU + GPU, CPU, GPU). То есть поддержка работы как на системах чисто с процессором, так и на системах с процессором и видеокартой. Необходимо для возможной работы на серверном оборудовании. <br>

<p> <b> Стек: </b> </p> 
С/С++, Shaders, OpenGL, Многопоточность, Кроссплатформенность

## 3. Алгоритмическая база
### 3.1. Удаление групп плоскостей
Анализ вертексов моделей для определения зон, находящихся в "общей" плоскоксти с учетом погрешности. С заменой данной плоскости на общую единую. (Создание множества вертексом одной "общей" плоскости)
Решает проблему протяженных плоскостей с небольшими искажениями в вертексах.
### 3.2. Замена ребер
Выбор ребра с последующим анализом плоскостей принадлежащих данному ребру. Анализ на основе нормалей вертексов/нормалей принадлежащих плоскостей. 

Математическое описание алгоритма:
1) Выбор ребра
2) Поиск плоскостей с данным ребром
3) Определение нормалей плоскостей (их получение)
4) Расчет куммулятивной нормали плоскостей
5) Расчет нормалей для 3-х вариантов (цетр ребра, один из концов ребра) ближайщих плоскостей при замене ребра на вертекс
6) Сравнение исходной куммулятивной нормали с потенциальными заменами. 
7) Выбор оптимального варианта с учетом заданной величины погрешности/ошибки. Итог: Замена или отказ. Дальнейший переход к следующему ребру

Примечание: Более "гладкий" алгоритм. Модель не должна сильно искажаться относительно исходной версии.
### 3.3. Удаление треугольников
Поиск треугольников, удаление которых не приводит к сильным искажениям модели. Работает схоже с удалением групп плоскостей.
## 4. Список материалов
1) http://mkrus.free.fr/CG/LODS/xrds/ - примеры алгоритмов + общие данные + список книг (стоит изучить)