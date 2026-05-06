import Task1;
//1
void teststatic(){
    auto p = hello_static("Hello Static");
    println("{}",p);
}
//2
void testhello(){
    print_it("Hello World!");
}
//3
void testph(){
    mutex my_mutex;
    auto _ = lock_guard(my_mutex);
}
//4
void testdph(){
    tuple<char,int,char> geek;

    // Assigning values to tuple using make_tuple()
    geek = std::make_tuple('a', 10, 15.5);

    auto& [a, _, _] = geek;
}
//5
template <typename... Args>
auto first_arg(Args... args) {
    return args...[0];
}
int main(){


    return 0;
}
