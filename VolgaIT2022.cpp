#include <iostream>
#include <fstream>
#include <array>
#include <list>
#include <stack>
#include <clocale>
#include <vector>

#include "fairy_tail.hpp"


using std::array;
using std::vector;
using std::stack;
using std::list;
using std::find;
using std::reverse;
using std::cout;
using std::endl;
using std::ofstream;


struct Point {
    int x = 0;
    int y = 0;

    bool operator==(Point other) {
        return this->x == other.x && this->y == other.y;
    }
    bool operator!=(Point other) {
        return this->x != other.x || this->y != other.y;
    }
    bool operator>(Point other) {
        if (this->x > other.x) {
            return true;
        }
        else if (this->x == other.x) {
            if (this->y > other.y) {
                return true;
            }
        }
        return false;
    }
    bool operator>=(Point other) {
        return *this > other || *this == other;
    }
    bool operator<(Point other) {
        if (this->x < other.x) {
            return true;
        }
        else if (this->x == other.x) {
            if (this->y < other.y) {
                return true;
            }
        }
        return false;
    }
    bool operator<=(Point other) {
        return *this < other || *this == other;
    }
};

void checkWay(Character name, array<array<char, 30>, 30>& map, Point position, Fairyland& world);
Direction Go(array<array<char, 30>, 30>& map, Point& position, stack<Point>& way);
/*
    Функция создана для того, чтобы сравнивать локальные карты персонажей. Если карты
    совпадают, то возвращается истина, иначе - ложь.
*/
bool comparison_maps(const array<array<char, 30>, 30>& first, const array<array<char, 30>, 30>& second);

array<array<char, 10>, 10> compound_map(const array<array<char, 30>, 30>& first, const array<array<char, 30>, 30>& second, Point first_position, Point second_position);

array<array<char, 10>, 10> generate_map(const array<array<char, 30>, 30>& first, const array<array<char, 30>, 30>& second);
/*
    Функция проверяет на изолированность персонажа и на изведанность области, в которой
    он находится.
*/
bool isolation(const array<array<char, 30>, 30>& arr);

/*
    Функция будет искать кратчайший путь и возвращать путь по координатам.
*/
list<Point> find_way(const array<array<char, 10>, 10>& arr);

int main() {

    setlocale(LC_ALL, "Russian");

    // Локальные карты для персонажей
    array<array<char, 30>, 30> Ivan;
    array<array<char, 30>, 30> Elena;

    // Переменные для исследования пути
    stack<Point> Ivans_way;
    stack<Point> Elenas_way;

    Point Ivans_position;
    Point Elenas_position;

    Direction Ivans_direction;
    Direction Elenas_direction;

    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            Ivan[i][j] = '?';
            Elena[i][j] = '?';
        }
    }

    Ivan[11][11] = '@';
    Elena[11][11] = '&';

    Ivans_position.x = 11; Ivans_position.y = 11;
    Elenas_position.x = 11; Elenas_position.y = 11;

    Ivans_way.push(Ivans_position);
    Elenas_way.push(Elenas_position);

    /*
        getTurnCount() - Кол-во ходов
        canGo() - Проверка на наличие препядствий
        go() - переход
    */
    Fairyland world;

    bool meet = false;


    // Здесь происходит ходьба, заполнение локальных карт, путей их.
    while (!Ivans_way.empty() && !Elenas_way.empty()) {

        //Проверка на возможность передвижения
        checkWay(Character::Ivan, Ivan, Ivans_position, world);
        checkWay(Character::Elena, Elena, Elenas_position, world);

        meet = world.go(Go(Ivan, Ivans_position, Ivans_way), Go(Elena, Elenas_position, Elenas_way));
        
    }


    // Если они неожиданно встретились. Здесь строится путь, выводится он. 
    if (meet) {
        array<array<char, 10>, 10> map = compound_map(Ivan, Elena, Ivans_position, Elenas_position);
        ofstream file("./map.txt");

        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (map[i][j] == ',')
                    map[i][j] = '.';

                file << map[i][j];
            }
            file << endl;
        }
    }
    // Если они не встретились и они обошли, все, что возможно.
    // Если локальные карты совпадают, то выводятся карты.
    else if (comparison_maps(Elena, Elena)) {

        array<array<char, 10>, 10> map = generate_map(Ivan, Elena);

        list<Point> way = find_way(map);

        ofstream file("./map.txt");

        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (map[i][j] == ',')
                    map[i][j] = '.';
                
                file << map[i][j];
            }
            file << endl;
        }

        
        
    }
    // Они не встретились. Выводится карта Ивана
    else {

        array<array<char, 10>, 10> map = generate_map(Ivan, Ivan);

        ofstream file("./map.txt");

        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (map[i][j] == ',')
                    map[i][j] = '.';

                file << map[i][j];
            }
            file << endl;
        }
    }
    return 0;
}

void checkWay(Character name, array<array<char, 30>, 30>& map, Point position, Fairyland& world) {
    if (world.canGo(name, Direction::Down)) {
        if(map[position.y + 1][position.x] == '?')
            map[position.y + 1][position.x] = '.';
    }
    else {
        map[position.y + 1][position.x] = '#';
    }
    if (world.canGo(name, Direction::Up)) {
        if (map[position.y - 1][position.x] == '?')
            map[position.y - 1][position.x] = '.';
    }
    else {
        map[position.y - 1][position.x] = '#';
    }
    if (world.canGo(name, Direction::Left)) {
        if (map[position.y][position.x - 1] == '?')
            map[position.y][position.x - 1] = '.';
    }
    else {
        map[position.y][position.x - 1] = '#';
    }
    if (world.canGo(name, Direction::Right)) {
        if (map[position.y][position.x + 1] == '?')
            map[position.y][position.x + 1] = '.';
    }
    else {
        map[position.y][position.x + 1] = '#';
    }

}

Direction Go(array<array<char, 30>, 30>& map, Point& position, stack<Point>& way) {
    if (map[position.y + 1][position.x] == '.') {
        map[position.y + 1][position.x] = ',';

        position.y += 1;
        way.push(position);
        return Direction::Down;
    }
    else if (map[position.y - 1][position.x] == '.') {
        map[position.y - 1][position.x] = ',';

        position.y -= 1;
        way.push(position);
        return Direction::Up;
    }
    else if (map[position.y][position.x - 1] == '.') {
        map[position.y][position.x - 1] = ',';

        position.x -= 1;
        way.push(position);
        return Direction::Left;
    }
    else if (map[position.y][position.x + 1] == '.') {
        map[position.y][position.x + 1] = ',';

        position.x += 1;
        way.push(position);
        return Direction::Right;
    }

    way.pop();
    if (!way.empty()) {
        if (way.top().y - position.y == -1) {
            position = way.top();
            return Direction::Up;
        }
        else if (way.top().y - position.y == 1) {
            position = way.top();
            return Direction::Down;
        }
        else if (way.top().x - position.x == -1) {
            position = way.top();
            return Direction::Left;
        }
        else if (way.top().x - position.x == 1) {
            position = way.top();
            return Direction::Right;
        }
    }
}

// Что-то сделал тут не так
bool isolation(const array<array<char, 30>, 30>& arr) {
    
    stack<Point> way;
    list<Point> wall;
    
    Point position;
    for (int i = 0; i < 30 && !way.empty(); i++) {
        for (int j = 0; j < 30 && !way.empty(); j++) {
            if (arr[i][j] == '#') {
                position.x = j;
                position.y = i;
                way.push(position);
            }
        }
    }

    array<array<char, 30>, 30> tmp_arr = arr;
    tmp_arr[position.y][position.x] = '%';

    if (way.empty())
        return false;
    
    while (!way.empty() && find(wall.begin(), wall.end(), position) == wall.end()) {
        wall.push_back(position);

        if (tmp_arr[position.y - 1][position.x - 1] == '#') {
            tmp_arr[position.y - 1][position.x - 1] = '%';
            position.y -= 1;
            position.x -= 1;
            way.push(position);
        }
        else if (tmp_arr[position.y - 1][position.x] == '#') {
            tmp_arr[position.y - 1][position.x] = '%';
            position.y -= 1;
            way.push(position);
        }
        else if (tmp_arr[position.y - 1][position.x + 1] == '#') {
            tmp_arr[position.y - 1][position.x + 1] = '%';
            position.y += 1;
            position.x += 1;
            way.push(position);
        }
        else if (tmp_arr[position.y][position.x - 1] == '#') {
            tmp_arr[position.y][position.x - 1] = '%';
            position.x -= 1;
            way.push(position);
        }
        else if (tmp_arr[position.y][position.x + 1] == '#') {
            tmp_arr[position.y][position.x + 1] = '%';
            position.x += 1;
            way.push(position);
        }
        else if (tmp_arr[position.y + 1][position.x - 1] == '#') {
            tmp_arr[position.y + 1][position.x - 1] = '%';
            position.y += 1;
            position.x -= 1;
            way.push(position);
        }                           
        else if (tmp_arr[position.y + 1][position.x] == '#') {
            tmp_arr[position.y + 1][position.x] = '%';
            position.y += 1;
            way.push(position);
        }                           
        else if (tmp_arr[position.y + 1][position.x + 1] == '#') {
            tmp_arr[position.y + 1][position.x + 1] = '%';
            position.y += 1;
            position.x += 1;
            way.push(position);
        }
        else if (way.empty()) {
            break;
        }
        else {
            position = way.top();
            way.pop();
        }
    }

    if (way.empty())
        return false;

    return true;
}

bool comparison_maps(const array<array<char, 30>, 30>& first, const array<array<char, 30>, 30>& second) {
    int Ivan_min_X = 30;
    int Ivan_max_X = -1;
    int Ivan_min_Y = 30;
    int Ivan_max_Y = -1;

    int Elena_min_X = 30;
    int Elena_max_X = -1;
    int Elena_min_Y = 30;
    int Elena_max_Y = -1;

    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            if (first[i][j] == '#') {
                if (Ivan_min_X > j)
                    Ivan_min_X = j;
                if (Ivan_max_X < j)
                    Ivan_max_X = j;
                if (Ivan_min_Y > i)
                    Ivan_min_Y = i;
                if (Ivan_max_Y < i)
                    Ivan_max_Y = i;
            }
            if (second[i][j] == '#') {
                if (Elena_min_X > j)
                    Elena_min_X = j;
                if (Elena_max_X < j)
                    Elena_max_X = j;
                if (Elena_min_Y > i)
                    Elena_min_Y = i;
                if (Elena_max_Y < i)
                    Elena_max_Y = i;
            }
        }
    }

    if (Ivan_max_X - Ivan_min_X != Elena_max_X - Elena_min_X || Ivan_max_Y - Ivan_min_Y != Elena_max_Y - Elena_min_Y)
        return false;

    int width = Ivan_max_X - Ivan_min_X + 1;
    int height = Ivan_max_Y - Ivan_min_Y + 1;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (!((first[Ivan_min_Y + i][Ivan_min_X + j] == ',' || first[Ivan_min_Y + i][Ivan_min_X + j] == '@') && (second[Elena_min_Y + i][Elena_min_X + j] == ',' || second[Elena_min_Y + i][Elena_min_X + j] == '&') || first[Ivan_min_Y + i][Ivan_min_X + j] == second[Elena_min_Y + i][Elena_min_X + j]))
                return false;
        }
    }
    return true;
}

array<array<char, 10>, 10> generate_map(const array<array<char, 30>, 30>& first, const array<array<char, 30>, 30>& second) {
    int Ivan_min_X = 30;
    int Ivan_max_X = -1;
    int Ivan_min_Y = 30;
    int Ivan_max_Y = -1;

    int Elena_min_X = 30;
    int Elena_max_X = -1;
    int Elena_min_Y = 30;
    int Elena_max_Y = -1;

    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            if (first[i][j] == '#') {
                if (Ivan_min_X > j)
                    Ivan_min_X = j;
                if (Ivan_max_X < j)
                    Ivan_max_X = j;
                if (Ivan_min_Y > i)
                    Ivan_min_Y = i;
                if (Ivan_max_Y < i)
                    Ivan_max_Y = i;
            }
            if (second[i][j] == '#') {
                if (Elena_min_X > j)
                    Elena_min_X = j;
                if (Elena_max_X < j)
                    Elena_max_X = j;
                if (Elena_min_Y > i)
                    Elena_min_Y = i;
                if (Elena_max_Y < i)
                    Elena_max_Y = i;
            }
        }
    }

    int width = Ivan_max_X - Ivan_min_X + 1;
    int height = Ivan_max_Y - Ivan_min_Y + 1;

    array<array<char, 10>, 10> map;

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            map[i][j] = '?';
        }
    }

    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            map[i - 1][j - 1] = first[Ivan_min_Y + i][Ivan_min_X + j];
            if (second[Elena_min_Y + i][Elena_min_X + j] == '&')
                map[i - 1][j - 1] = '&';
        }
    }

    return map;
}

array<array<char, 10>, 10> compound_map(const array<array<char, 30>, 30>& first, const array<array<char, 30>, 30>& second, Point first_position, Point second_position) {
    array<array<char, 30>, 30> tmp;
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            tmp[i][j] = '?';
        }
    }
    int Ivan_min_X = 30;
    int Ivan_max_X = -1;
    int Ivan_min_Y = 30;
    int Ivan_max_Y = -1;

    int Elena_min_X = 30;
    int Elena_max_X = -1;
    int Elena_min_Y = 30;
    int Elena_max_Y = -1;

    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            if (first[i][j] == '#') {
                if (Ivan_min_X > j)
                    Ivan_min_X = j;
                if (Ivan_max_X < j)
                    Ivan_max_X = j;
                if (Ivan_min_Y > i)
                    Ivan_min_Y = i;
                if (Ivan_max_Y < i)
                    Ivan_max_Y = i;
            }
            if (second[i][j] == '#') {
                if (Elena_min_X > j)
                    Elena_min_X = j;
                if (Elena_max_X < j)
                    Elena_max_X = j;
                if (Elena_min_Y > i)
                    Elena_min_Y = i;
                if (Elena_max_Y < i)
                    Elena_max_Y = i;
            }
        }
    }

    for (int i = Ivan_min_X; i <= first_position.x; i++) {
        for (int j = Ivan_min_Y; j <= first_position.y; j++) {
            tmp[15 + j - Ivan_min_Y][15 + i - Ivan_min_X] = first[j][i];
        }
        for (int j = Ivan_max_Y; j >= first_position.y; j--) {
            tmp[15 + j - Ivan_min_Y][15 + i - Ivan_min_X] = first[j][i];
        }
    }

    for (int i = Ivan_max_X; i >= first_position.x; i--) {
        for (int j = Ivan_min_Y; j <= first_position.y; j++) {
            tmp[15 + j - Ivan_min_Y][15 + i - Ivan_min_X] = first[j][i];
        }
        for (int j = Ivan_max_Y; j >= first_position.y; j--) {
            tmp[15 + j - Ivan_min_Y][15 + i - Ivan_min_X] = first[j][i];
        }
    }

    for (int i = Elena_min_X; i <= second_position.x; i++) {
        for (int j = Elena_min_Y; j <= second_position.y; j++) {
            if(tmp[15 + j - Elena_min_Y][15 + i - Elena_min_X] == '?' || second[j][i] == '&')
                tmp[15 + j - Elena_min_Y][15 + i - Elena_min_X] = second[j][i];
        }
        for (int j = Elena_max_Y; j >= second_position.y; j--) {
            if (tmp[15 + j - Elena_min_Y][15 + i - Elena_min_X] == '?' || second[j][i] == '&')
                tmp[15 + j - Elena_min_Y][15 + i - Elena_min_X] = second[j][i];
        }
    }

    for (int i = Elena_max_X; i >= second_position.x; i--) {
        for (int j = Elena_min_Y; j <= second_position.y; j++) {
            if (tmp[15 + j - Elena_min_Y][15 + i - Elena_min_X] == '?' || second[j][i] == '&')
                tmp[15 + j - Elena_min_Y][15 + i - Elena_min_X] = second[j][i];
        }
        for (int j = Elena_max_Y; j >= second_position.y; j--) {
            if (tmp[15 + j - Elena_min_Y][15 + i - Elena_min_X] == '?' || second[j][i] == '&')
                tmp[15 + j - Elena_min_Y][15 + i - Elena_min_X] = second[j][i];
        }
    }

    for (int i = Ivan_min_X; i <= first_position.x; i++) {
        for (int j = Ivan_min_Y; j <= first_position.y; j++) {
            if(first[j][i] == '@')
                tmp[15 + j - Ivan_min_Y][15 + i - Ivan_min_X] = first[j][i];
        }
        for (int j = Ivan_max_Y; j >= first_position.y; j--) {
            if (first[j][i] == '@')
                tmp[15 + j - Ivan_min_Y][15 + i - Ivan_min_X] = first[j][i];
        }
    }

    for (int i = Ivan_max_X; i >= first_position.x; i--) {
        for (int j = Ivan_min_Y; j <= first_position.y; j++) {
            if (first[j][i] == '@')
                tmp[15 + j - Ivan_min_Y][15 + i - Ivan_min_X] = first[j][i];
        }
        for (int j = Ivan_max_Y; j >= first_position.y; j--) {
            if (first[j][i] == '@')
                tmp[15 + j - Ivan_min_Y][15 + i - Ivan_min_X] = first[j][i];
        }
    }



    int min_X = 30;
    int max_X = -1;
    int min_Y = 30;
    int max_Y = -1;

    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            if (tmp[i][j] == '#' || tmp[i][j] == ',' || tmp[i][j] == '@' || tmp[i][j] == '&') {
                if (min_X > j)
                    min_X = j;
                if (max_X < j)
                    max_X = j;
                if (min_Y > i)
                    min_Y = i;
                if (max_Y < i)
                    max_Y = i;
            }
        }
    }

    int width = max_X - min_X + 1;
    int height = max_Y - min_Y + 1;

    array<array<char, 10>, 10> map;

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            map[i][j] = '?';
        }
    }


    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            map[i - 1][j - 1] = tmp[min_Y + i][min_X + j];
        }
    }



    return map;
}

list<Point> find_way(const array<array<char, 10>, 10>& arr) {

    list<Point> way;

    int n = 0;
    vector<Point> points;
    Point Ivan_position;
    Point Elena_position;
    Point tmp;
    Point tmp1;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (arr[i][j] == ',' || arr[i][j] == '@' || arr[i][j] == '&') {
                n++;
                tmp.x = j;
                tmp.y = i;
                points.push_back(tmp);
            }
            if (arr[i][j] == '@') {
                Ivan_position = tmp;
            }
            if (arr[i][j] == '&') {
                Elena_position = tmp;
            }
            
        }
    }
    
    int st = find(points.begin(), points.end(), Ivan_position) - points.begin();
    int fi = find(points.begin(), points.end(), Elena_position) - points.begin();
    
    way.push_front(points[fi]);

    if (points[st].x - points[fi].x == 0 && abs(points[st].y - points[fi].y) == 1 || abs(points[st].x - points[fi].x) == 1 && points[st].y - points[fi].y == 0) {
        return way;
    }

    vector<vector<int >> w;
    w.resize(n);
    for (int i = 0; i < n; i++)
        w[i].resize(n);
    


    for (int i = 0; i < n; i++) {
        tmp = points[i];
        for (int j = 0; j < n; j++) {
            w[i][j] = 20000;
            if (i == j)
                w[i][j] = 0;
            else
                tmp1 = points[j];

            if (tmp.x - tmp1.x == 0 && abs(tmp.y - tmp1.y) == 1 || abs(tmp.x - tmp1.x) == 1 && tmp.y - tmp1.y == 0)
                w[i][j] = 1;
        }
    }

    vector<vector<list<int>>> ways;
    ways.resize(n);
    for (int i = 0; i < n; i++)
        ways[i].resize(n);

    way.clear();

    for (int i = 0; i < n; i++) {
        vector<vector<int>> w1;
        vector<vector<list<int>>> ways1;
        w1.resize(n);
        ways1.resize(n);
        for (int j = 0; j < n; j++) {
            w1[j].resize(n);
            ways1[j].resize(n);
        }
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                if (w[j][i] + w[i][k] < w[j][k]) {
                    w1[j][k] = w[j][i] + w[i][k];
                    ways1[j][k] = ways[j][i];
                    ways1[j][k].push_back(i);
                    for (auto c : ways[i][k]) {
                        ways1[j][k].push_back(c);
                    }
                }
                else {
                    w1[j][k] = w[j][k];
                    ways1[j][k] = ways[j][k];
                }
            }
        }
        w = w1;
        ways = ways1;
    }

    for (auto i = ways[st][fi].begin(); i != ways[st][fi].end(); i++) {
        way.push_back(points[*i]);
    }

    return way;
    
}
