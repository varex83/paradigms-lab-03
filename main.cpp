#include <iostream>
#include <thread>
#include <vector>
#include <latch>

using namespace std;

void f(char actionSet, int actionNumber) {
    string actionSetStr = "Дія " + string(1, actionSet) + to_string(actionNumber) + "\n";
    cout << actionSetStr;
}

atomic_int total(0);

void executeActionSet(char actionSet, int action, latch& latch) {
    for (int i = 1; i <= action; ++i) {
        f(actionSet, i);
        ++total;
    }
    latch.count_down();
}


int main() {
    cout << "Обчислення розпочато.\n";

    const int nt = 5; // кількість потоків

    // Кількість дій для кожної дуги
    const int a = 7, b = 7, c = 6, d = 6, e = 7, f = 4, g = 4, h = 7, i = 4, j = 9, k = 5;

    latch latchDE(1);
    latch latchF(2);
    latch latchGH(1);
    latch latchI(2);
    latch latchJ(1);
    latch latchK(2);
    latch latchFinal(2);

    vector<thread> threads;

    auto joinThreads = [&](int nt) {
        while (!threads.empty() && threads.size() >= nt) {
            if (threads.back().joinable()) {
                threads.back().join();
                threads.pop_back();
            }
        }
    };

    // Дуга a
    threads.emplace_back([&]() {
        executeActionSet('a', a, latchDE);
    });

    cout << "a" << endl;

    // Дуга b
    threads.emplace_back([&]() {
        executeActionSet('b', b, latchF);
    });

    cout << "b" << endl;
    joinThreads(nt);

    // Дуга c
    threads.emplace_back([&]() {
        executeActionSet('c', c, latchF);
    });

    cout << "c" << endl;
    joinThreads(nt);

    // Дуга d і дуга e
    threads.emplace_back([&]() {
        latchDE.wait();
        executeActionSet('d', d, latchGH);
        executeActionSet('e', e, latchI);
    });

    cout << "d" << endl;

    joinThreads(nt);

    // Дуга f
    threads.emplace_back([&]() {
        latchF.wait();
        executeActionSet('f', f, latchI);
    });

    cout << "f" << endl;

    // Дуга g і дуга h
    threads.emplace_back([&]() {
        latchGH.wait();
        executeActionSet('g', g, latchJ);
        executeActionSet('h', h, latchK);
    });

    cout << "g" << endl;
    joinThreads(nt);

    // Дуга i
    threads.emplace_back([&]() {
        latchI.wait();
        executeActionSet('i', i, latchK);
    });

    // Дуга j
    threads.emplace_back([&]() {
        latchJ.wait();
        executeActionSet('j', j, latchFinal);
    });

    // Дуга k
    threads.emplace_back([&]() {
        latchK.wait();
        executeActionSet('k', k, latchFinal);
    });

    latchFinal.wait();
    joinThreads(0);
    cout << "Обчислення завершено.\n";
    cout << "Всього виконано " << total << " дій.\n";
    cout << "Очікувалось виконання " << a + b + c + d + e + f + g + h + i + j + k << " дій.\n";
    return 0;
}
