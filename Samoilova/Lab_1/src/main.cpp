#include <iostream>
#include <algorithm>
#include <vector>


struct SquareList
{
    int count = 0;
    int* arr;
};

struct Coord
{
    int x;
    int y;
};


struct SquareData
{
    Coord pos;  // координаты
    int w;   // размер квадрата

    friend std::ostream& operator<<(std::ostream& os, const SquareData& sq){
        os << sq.pos.x + 1 << " " << sq.pos.y + 1 << " " << sq.w << std::endl;
    }
};


class Square
{
private:
    int **iBestConfiguration; // лучшее расположение квадратов на поле
    int res; // количество квадратов в лучшем расположении

    const int iSize; //размер большого квадрата

    int **iArray; // поле для поиска расположения квадратов
    int iColors = 0; //количество квадратов на поле


    int optimize();
    void div2();
    void div3();
    void div5();
    SquareData findPotentialSquare() const;
    SquareData findSquare_(int color, int** Array);
    void rec_backtrack(int iCurSize);
    int rec_backtrack(int iCurSize, SquareList* list);
    void setConfiguration();


public:
    Square(int iSize):
        iSize(iSize)
    {
        iBestConfiguration = new int*[iSize];
        iArray = new int*[iSize];
        for(int i = 0; i < iSize; i++){
            iBestConfiguration[i] = new int[iSize];
            iArray[i] = new int[iSize];
            for(int j = 0; j < iSize; j++){
                iBestConfiguration[i][j] = 0;
                iArray[i][j] = 0;
            }
        }
    }

    void proceed();
    int proceed(SquareList* list);
    int getSize() const;
    void setSquare(int x, int y, int w);
    void printConfiguration(std::ostream &os);
    void delSquare(int x, int y);

    ~Square(){
        for(int i = 0; i < iSize; i++){
            delete [] iBestConfiguration[i];
            delete [] iArray[i];
        }
        delete [] iBestConfiguration;
        delete [] iArray;
    }
};



void Square::proceed(){
    if(!optimize()){
        res = iSize*iSize;
        rec_backtrack(0);
        iColors = res;
    }
}


int Square::proceed(SquareList* list){
    int find_flag = 0;
    if(optimize()){
        for(int j = 0; j<list->count; j++){
            int flag = 0;
            for(int i = 1; i <= iColors; i++){
                auto sq = findSquare_(i, iBestConfiguration);
                if(list->arr[j] == sq.w){
                    flag = 1;
                    find_flag = 1;
                    break;
                }
            }
            if(flag == 0){
                res = iSize*iSize;
                iColors = 0;
                for(int i = 0; i < iSize; i++){
                    for(int j = 0; j < iSize; j++){
                        iBestConfiguration[i][j] = 0;
                        iArray[i][j] = 0;
                    }
                }
                find_flag = rec_backtrack(0, list);
                return find_flag;
            }
        }
    }
    else{
        find_flag = rec_backtrack(0, list);
    }
    return find_flag;
}

//_____________OPTIMIZE____________________________________________

int Square::optimize(){
    if (iSize % 2 == 0){
        div2();
        setConfiguration();
        return 1;
    }
    else if (iSize % 3 == 0){
        div3();
        setConfiguration();
        return 1;
    }
    else if (iSize % 5 == 0){
        div5();
        setConfiguration();
        return 1;
    }
    return 0;
}

void Square::div2(){
    setSquare(0, 0, iSize/2);
    setSquare(iSize/2, 0, iSize/2);
    setSquare(0, iSize/2, iSize/2);
    setSquare(iSize/2, iSize/2, iSize/2);
}

void Square::div3(){
    setSquare(0, 0, iSize/3*2);
    setSquare(iSize/3*2, 0, iSize/3);
    setSquare(iSize/3*2, iSize/3, iSize/3);
    setSquare(0, iSize/3*2, iSize/3);
    setSquare(iSize/3, iSize/3*2, iSize/3);
    setSquare(iSize/3*2, iSize/3*2, iSize/3);
}

void Square::div5(){
    setSquare(0, 0, iSize/5*3);
    setSquare(iSize/5*3, 0, iSize/5*2);
    setSquare(iSize/5*3, iSize/5*2, iSize/5*2);
    setSquare(0, iSize/5*3, iSize/5*2);
    setSquare(iSize/5*2, iSize/5*3, iSize/5);
    setSquare(iSize/5*2, iSize/5*4, iSize/5);
    setSquare(iSize/5*3, iSize/5*4, iSize/5);
    setSquare(iSize/5*4, iSize/5*4, iSize/5);
}

//___________________________________________________________________________________________

SquareData Square::findPotentialSquare() const{ //поск свободного места для квадратика на поле, возвращает позицию и размер
    int x = -1;
    int y = -1;
    for(int i = 0; i < iSize; i++){
        bool b = 0; // флаг для выхода из цикла
        for(int j = 0; j < iSize; j++){
            if(iArray[i][j] == 0){
                y = i;
                x = j;
                b = 1;
                break;
            }
        }
        if(b) break;
    }
    if(x == -1 && y == -1){
        return SquareData{Coord{0, 0}, 0};
    }
    int s;
    for(s = 0; s <= iSize - std::max(x, y); s++){
        bool b = 0; // флаг для выхода из цикла
        for(int i = y; i < y+s; i++){
            if(iArray[i][x+s-1] != 0){
                b = 1;
                break;
            }
        }
        for(int i = x; i < x+s; i++){
            if(iArray[y+s-1][i] != 0){
                b = 1;
                break;
            }
        }
        if(b) break;
    }
    s--;
    return SquareData{Coord{x, y}, s}; // максимально возможный для вмещения квадрат
}


SquareData Square::findSquare_(int color, int** Array){
    int x = -1;
    int y = -1;
    for(int i = 0; i < iSize; i++){ //находим верхний левый угол с заданным цветом
        bool b = 0;
        for(int j = 0; j < iSize; j++){
            if(Array[i][j] == color){
                y = i;
                x = j;
                b = 1;
                break;
            }
        }
        if(b) break;
    }
    if(x == -1 && y == -1)
        return SquareData{Coord{0, 0}, 0};
    int s;
    for(s = x; s < iSize; s++){  // находим размер квадрата
        if(Array[y][s] != color)
            break;
    }
    s -= x;
    return SquareData{Coord{x, y}, s};
}

void Square::rec_backtrack(int iCurSize){
    if(iCurSize >= res)  // если количество уже найденных квадратов больше чем
                        // лучшая конфигурация =>
        return;
    auto emptySquare = findPotentialSquare();
    if(emptySquare.w == iSize){
        setSquare(0, 0, iSize/2+1);
        setSquare(0, iSize/2+1, iSize/2);
        setSquare(iSize/2+1, 0, iSize/2);
        rec_backtrack(3);
        return;
    }
    if(emptySquare.w == 0){   // ура!, мы нашли какую-то конфигурацию
        if(iCurSize < res){
            res = iCurSize;
            setConfiguration();
        }
    }
    else{
        int w = emptySquare.w;
        while(w > 0){
            setSquare(emptySquare.pos.x, emptySquare.pos.y, w);  // записываем цифры в найденный квадрат
            rec_backtrack(iCurSize+1);
            delSquare(emptySquare.pos.x, emptySquare.pos.y); // удаляем квадрат текущего этапа рекурсии
            w--; // уменьшаем размер вставляемого квадрата
        }
    }
}

int Square::rec_backtrack(int iCurSize, SquareList* list){
    int find_flag = 0;
    if(iCurSize >= res)  // если количество уже найденных квадратов больше чем
                        // лучшая конфигурация =>
        return find_flag;
    auto emptySquare = findPotentialSquare();
    if(emptySquare.w == 0){   // ура!, мы нашли какую-то конфигурацию
        //проверка подходит ли нам эта конфигурация
        bool f = true;
        for(int j = 0; j<list->count; j++){
            int flag = 0;
            for(int i = 1; i <= iColors; i++){
                auto sq = findSquare_(i, iArray);
                if(list->arr[j] == sq.w){
                    flag = 1;
                    break;
                }
            }
            if(flag == 0){
                f = false;
                return find_flag;
            }
        }
        if(iCurSize < res && f == true){
            res = iCurSize;
            setConfiguration();
            find_flag = 1;
        }
    }
    else{
        int w = emptySquare.w;
        while(w > 0){
            setSquare(emptySquare.pos.x, emptySquare.pos.y, w);  // записываем цифры в найденный квадрат
            find_flag = rec_backtrack(iCurSize+1, list);
            if(find_flag == 1)
                return find_flag;
            delSquare(emptySquare.pos.x, emptySquare.pos.y); // удаляем квадрат текущего этапа рекурсии
            w--; // уменьшаем размер вставляемого квадрата
        }
    }
    return find_flag;
}



int Square::getSize() const {
    return iSize;
}

void Square::setSquare(int x, int y, int w){
    for(int i = y; i < y + w && i < iSize; i++){
        for(int j = x; j < x + w && j < iSize; j++){
            iArray[i][j] = iColors+1;
        }
    }
    iColors++;
}

void Square::setConfiguration(){  //записываем нынешний вариант, как лучший
    for(int i = 0; i < iSize; i++){
        for(int j = 0; j < iSize; j++){
            iBestConfiguration[i][j] = iArray[i][j];
        }
    }
//    std::cout << "Вариант лучшего расположения квадратов:" << std::endl;       // вывод частичных решений
//    printConfiguration(std::cout);                                             // вывод частичных решений
}


void Square::printConfiguration(std::ostream &os){
    os << iColors << std::endl;
    for(int i = 1; i <= iColors; i++){
        auto sq = findSquare_(i, iBestConfiguration);
        os << sq;
    }
}

void Square::delSquare(int x, int y){
    int color = iArray[y][x];
    for(int i = y; i < iSize; i++){
        for(int j = x; j < iSize; j++){
            if(iArray[i][j] == color)
                iArray[i][j] = 0;
            else break;
        }
    }
    for(int i = 0; i < iSize; i++){
        for(int j = 0; j < iSize; j++){
            if(iArray[i][j] > color)
                iArray[i][j]--;
        }
    }
    iColors--;
}



void read_sizes(int size, SquareList *list){
    std::cout << "Введите количество квадратов, которые необходимо разместить:" << std::endl;
    std::cin >> list->count;
    if(list->count <= 0)
        return;
    std::cout << "Введите размеры квадратов, которые необходимо разместить:" << std::endl;
    list->arr = new int[list->count];
    for(int i = 0; i<list->count; i++){
        std::cin >> list->arr[i];
        if(list->arr[i]>size){
            list->count = -1;
            return;
        }
        if(list->arr[i]==size){
            list->count = -1;
            return;
        }
    }
}

int main(){
    int size, flag;
    SquareList *list = new SquareList;
    std::cout << "Введите размер столешницы:" << std::endl;
    std::cin >> size;
    read_sizes(size, list);
    if(list->count < 0){
        std::cout << "Невозможно заполнить столешницу заданными квадратами!" << std::endl;
        return 0;
    }
    else if(list->count == 0){
        if(size == 0 || size == 1)
            std::cout << 0 << std::endl;
        else{
            Square square(size);
            square.proceed();
            std::cout << "Решение:" << std::endl;
            square.printConfiguration(std::cout);
        }
    }
    else{
        Square square(size);
        flag = square.proceed(list);
        if(flag == 1){
            std::cout << "Решение:" << std::endl;
            square.printConfiguration(std::cout);
        }
        else
            std::cout << "Невозможно заполнить столешницу заданными квадратами!" << std::endl;
    }
    return 0;
}
