#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

struct Node;
struct Rib;

struct Node{
    char Key;   // имя узла
    bool Flag = 0; // флаг посещения узла
    std::vector<Rib> Rib_list; // вектор исходящих ребер
    int parent_index;
};

struct Rib{
    unsigned int index; // индекс вершины в которую приходит ребро
    float wt;   // вес ребра
};

struct Tree_Node{
    int list_index;     // индекс элемента в списке вершин графа
    int cut;            // кратчайший путь
};


class BinaryHeap
{
private:
    std::vector<Node>& list;
    std::vector<Tree_Node> tree;

public:
    BinaryHeap(std::vector<Node>& list) : list(list) {

    }

    int heapsize(){
        return tree.size();
    }

    bool comp(const int parent, const int i){       // сравнение двух элементов для определения их приоритета в очереди
        if(abs(list[tree[parent].list_index].Key - list[1].Key) + tree[parent].cut  > abs(list[tree[i].list_index].Key - list[1].Key) + tree[i].cut){
            return 1;
        }
        if(abs(list[tree[parent].list_index].Key - list[1].Key) + tree[parent].cut  == abs(list[tree[i].list_index].Key - list[1].Key) + tree[i].cut){
            if(list[tree[parent].list_index].Key < list[tree[i].list_index].Key){
                return 1;
            }
        }
        return 0;
    }

    void add_elem(int index, int cut){       // добавление элемента в очередь

        if(list[index].Flag){       // если вершина посещалась, её не нужно добавлять в очередь на посещение
            return;
        }
        Tree_Node cur;
        cur.list_index = index;
        cur.cut = cut;
        tree.push_back(cur);

        int i = heapsize()-1;
        int parent = (i-1)/2;
        while( i > 0 && comp(parent, i)){
            int temp = tree[i].list_index;
            tree[i].list_index = tree[parent].list_index;
            tree[parent].list_index = temp;

            temp = tree[i].cut;
            tree[i].cut = tree[parent].cut;
            tree[parent].cut = temp;

            i = parent;
            parent = (i-1)/2;
        }

    }

    void heapify(int i){
        int leftChild;
        int rightChild;
        int largestChild;

        for(;;){
            leftChild = 2 * i + 1;
            rightChild = 2 *i + 2;
            largestChild = i;

            if (leftChild < heapsize() && comp(largestChild, leftChild)){
                largestChild = leftChild;
            }

            if (rightChild < heapsize() && comp(largestChild, rightChild)){
                largestChild = rightChild;
            }

            if (largestChild == i){
                break;
            }

            int temp = tree[i].list_index;
            tree[i].list_index = tree[largestChild].list_index;
            tree[largestChild].list_index = temp;

            temp = tree[i].cut;
            tree[i].cut = tree[largestChild].cut;
            tree[largestChild].cut = temp;

            i = largestChild;
        }
    }

    Tree_Node get_max(){
        Tree_Node rezult = tree[0];
        tree[0].list_index = tree[heapsize()-1].list_index;
        tree[0].cut = tree[heapsize()-1].cut;
        tree.pop_back();
        heapify(0);
        return rezult;
    }
};

bool find_path(std::vector<Node>& list, BinaryHeap& heap){
    bool flag = 0;
    if(heap.heapsize()<=0){     // если очередь на посещение пуста
        return 0;
    }
    Tree_Node index = heap.get_max();         // удаляем самый приорететный элемент из очереда
//    std::cout << "Посещаем вершину " << list[index.list_index].Key << std::endl;
    if(list[index.list_index].Flag == 1){
        return 0;
    }

    if(index.list_index == 1){             // если взятый из очереди элемент равен конечной вершине пути
        return 1;                 // возвращаемся
    }

    list[index.list_index].Flag = 1;       // помечаем взятую из очереди вершину как пройденную

    for(int i=0; i < list[index.list_index].Rib_list.size(); i++){
        heap.add_elem(list[index.list_index].Rib_list[i].index, list[index.list_index].Rib_list[i].wt+index.cut);
        list[list[index.list_index].Rib_list[i].index].parent_index = index.list_index;
    }
    while(heap.heapsize()>0 && flag==0){
        flag = find_path(list, heap);
    }
    if(flag==1){
        return 1;
    }
    return 0;
}


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

void find_result(std::vector<Node>& list, std::string& result){   // восстанавливает путь от начальной вершины к конечной
    std::string back_result;
    int i = 1;
    while(i >= 0){
        back_result.push_back(list[i].Key);
        i = list[i].parent_index;
    }
    while(!back_result.empty()){
        result.push_back(back_result[back_result.size()-1]);
        back_result.pop_back();
    }
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

    std::cout << "Введите информацию о ребрах в формате <вершина из которой выходит ребро> <вершин а в которую ходит ребро> <вес вершины>" << std::endl;

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

    BinaryHeap heap = BinaryHeap(list);
    heap.add_elem(0, 0);
    list[0].parent_index = -1;
    int flag = find_path(list, heap);

    if(flag == 0){
        std::cout << "Не существует такого пути!" << std::endl;
        return 0;
    }
    find_result(list, result);
    std::cout << "Результат : " << result << std::endl;
    return 0;
}


