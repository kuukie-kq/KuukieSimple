#include <iostream>
#include "virtual_cpu.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    system("chcp 65001");
    kuu::virtual_cpu_run_main();

    unsigned int a = 1400;
    int b = 106401;

    int c = (int)((b - a + 1) / a);

    std::cout << c << std::endl;

    return 0;
}
