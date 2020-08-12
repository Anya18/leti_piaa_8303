#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

// структура графа
struct Node;
struct Rib;

struct Node{
    char Key;   // имя узла
    bool Flag = 0; // флаг посещения узла
    std::vector<Rib> Rib_list; // вектор исходящих ребер
    Node *next = nullptr; // указатель на следующую вершину в списке
};

struct Rib{
    unsigned int index; // индекс вершины в которую приходит ребро
    float wt;   // вес ребра
};

void add_rib(std::vector<Node>& list, int i, char c2, float f){     // добавление исходящего ребра к массиву исходящих рёбер вершины
    Rib cur;
    cur.wt = f;
    for(int k = 0; k < list.size(); k++){           // поиск вершины в которую приходит ребро
        if(list[k].Key == c2){
            cur.index = k;
            break;
        }
    }
    list[i].Rib_list.push_back(cur);
}

bool my_comp(Rib a, Rib b){         // компаратор сортировки исходящих из вершины ребер по увеличению их веса
    return a.wt < b.wt;
}

void rib_sort(std::vector<Node>& list){  // сортировка исходящих из вершины рёбер по увеличенению их веса для всех вершин графа
    for(int i = 0; i < list.size(); i++){
        if(list[i].Rib_list.size() < 2){ // если из вершины исходят менее 2-х ребер сортировка не нужна
            continue;
        }
        std::sort(list[i].Rib_list.begin(), list[i].Rib_list.end(), my_comp);
    }

}

void find_path(std::vector<Node>& list, unsigned int i, std::string& result){       // поиск пути в графе
    list[i].Flag = 1;                     // изменяем флаг, показывая, что побывали в этой вершине
//    std::cout << "Добавили вершину " << list[i].Key << std::endl;
    result.push_back(list[i].Key);        // добавляем букву в конечный путь
    int r_size = result.size();         // сохраняем размер строки, чтобы потом узнать, найден ли путь в оследующих вызовах функции или нужно искать ещё
    if(list[1].Key == list[i].Key){       // если нашли конечное ребро
        return;                         // возвращаемся
    }
    if(list[i].Rib_list.empty()){         // если некуда идти
//        std::cout << "Удаляем из пути вершину " << result[result.size()-1] << std::endl;
        result.pop_back();              // удаляем  букву добавленную в этой итерации
        return;                         // возвращаемся
    }

    for(int j = 0; j < list[i].Rib_list.size(); j++) {    // для обхода всех ребер которые есть
        if(list[list[i].Rib_list[j].index].Flag == 0) {        // если их ещё не посещали
            find_path(list, list[i].Rib_list[j].index, result);        // вызываем функцию рекурсивно
            if (r_size < result.size()) {
                return;         // если вернувшаяся строка больше пришедшей на этот уровень => путь был найден и очищать строку не нужно
            }
        }
    }
//    std::cout << "Удаляем из пути вершину " << result[result.size()-1] << std::endl;
    result.pop_back(); // удалить  букву добавленную в этой итерации
}

int main() {
    Node Curr;
    std::string s;
    std::string result;

    std::cout << "Введите вершину начала пути и вершину конца пути:" << std::endl;

    std::getline(std::cin, s);
    std::stringstream ss(s);

    std::vector<Node> list;
    ss >> Curr.Key;
    list.push_back(Curr);
    ss >> Curr.Key;
    list.push_back(Curr);

    std::cout << "Введите информацию о ребрах в формате <вершина из которой выходит ребро> <вершин а в которую ходит ребро> <вес вершиныa g>" << std::endl;

    std::getline(std::cin, s);
    while(s!=""){     // считывание строк с информацией о ребрах между вершинами
        char c1, c2;
        float f;
        bool flag = 0;
        std::stringstream ss(s);
        ss >> c1 >> c2 >> f;
        for(int i = 0; i < list.size(); i++){
            if(c2 == list[i].Key){
                flag = 1;
                break;
            }
        }
        if(flag == 0){
            Curr.Key = c2;
            list.push_back(Curr);
        }
        flag = 0;

        for(int i = 0; i < list.size(); i++){
            if(c1 == list[i].Key){
                add_rib(list, i, c2, f);
                flag = 1;
                break;
            }
        }
        if(flag == 0){
            Curr.Key = c1;
            list.push_back(Curr);
            add_rib(list, list.size()-1, c2, f);
        }

        std::getline (std::cin,s);
    }

    rib_sort(list);

    find_path(list, 0, result);
    if(result.empty()){
        std::cout << "Не существует такого пути!" << std::endl;
        return 0;
    }
    std::cout << "Результат : " << result << std::endl;
    return 0;
}

