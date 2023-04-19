```C++
#include <isotream>
#include <thread>

void func()
{
    cout << "hello \n";
}

int main()
{
    // 创建一个 线程对象，同时启动线程
    thread t(func);

    t.join(); // 

    return 0;
}

```