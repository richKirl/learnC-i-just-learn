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
    geek = std::make_tuple('a', 10, 'r');

    auto& [a, _, _] = geek;
}
//5
template <typename... Args>
auto first_arg(Args... args) {
    return args...[0];
}
//6
template <typename... Args>
auto last_arg(Args... args) {
    return args...[sizeof...(args)-1];
}

//7
void test7(){
    test();
}
//8
void test8(){
    Point<int> x{10,11};
    x.print();
    Point<float> x1{10.000001f,11.0000001f};
    x1.print();
    Point<double> x2{10.000001,11.0000001};
    x2.print();
}
//9
void test9(){
    Point<int> x{10,11};
    println("{}",x.get_value());
    Point<float> x1{10.000001f,11.0000001f};
    println("{}",x1.get_value());
    Point<double> x2{10.000001,11.0000001};
    println("{}",x2.get_value());


}
int main(){
    //teststatic();
    //testhello();
    //testph();
    //testdph();
    //auto temp = first_arg(1,2,3,4,5,6);
    //print_it(to_string(temp));

    //temp = last_arg(1,2,3,4,5,6);
    //print_it(to_string(temp));

    //test7();

    //test8();
    //test9();
    vector<int> p;
    p.reserve(100);
    //p.resize(100);
    int count =0;
    for (int i=0;i<100;i++) {
        p.push_back(i);
        //println("{} {}",p.capacity(),p.size());
    }
    println("{}",p);
    return 0;
}
