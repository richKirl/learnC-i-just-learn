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

export std::string_view hello_static(std::string_view v) {
    return v;
}

export void print_it(std::string_view sv) {
    std::println("{}", sv);
}
