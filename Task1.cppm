module;
#include <assert.h>
export module Task1;

export import std;

// Импортируем стандартную библиотеку как модуль
import std;

// Экспортируем нужные сущности
export using std::println;
export using std::lock_guard;
export using std::mutex;
export using std::tuple;
export using std::to_string;
export using std::vector;
export namespace App::UI::Themes{};

export template <typename T>
void debugFunction(T value) {
    // Prints the full signature including the template type
    std::cout << "Location: " << __PRETTY_FUNCTION__ << std::endl;
}
//static assert compile time
export void test(){
    static_assert(std::string_view("d22").size() == 3, "String is empty");
}

export std::string_view hello_static(std::string_view v) {
    return v;
}

export void print_it(std::string_view sv) {
    std::println("{}", sv);
}

export template <typename T>
struct Point {
    T x, y;
    // Метод print() использует "deducing this"
    // Self&& выводит тип (Point&, const Point&, и т.д.)
    template <typename Self>
    void print(this Self&& self) {
        //std::cout << "Point(" << self.x << ", " << self.y << ")" << std::endl;
        println("Point {} {}",self.x,self.y);
    }
    template <typename Self>
    auto&& get_value(this Self&& self) {
        return std::forward<Self>(self).x;
    }
};
