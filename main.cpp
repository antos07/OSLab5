#include <iostream>
#include <thread>
#include <pthread.h>
#include <chrono>

using namespace std::chrono_literals;

int f(int x) {
    if (x < 1) {
        return 0;
    }
    if (x < 10) {
        return x;
    }
    while (true) {
        std::this_thread::sleep_for(10s);
    }
}


int g(int x) {
    if (x > 5) {
        return 0;
    }
    if (x == 1) {
        return x;
    }
    while (true) {
        std::this_thread::sleep_for(10s);
    }
}


int input_x() {
    int x;
    do {
        std::cin.clear();
        std::cout << "Enter x: ";
        std::cin >> x;
    } while (!std::cin);
    return x;
}

std::optional<int> calculation(std::optional<int> f_res, std::optional<int> g_res) {
    if (f_res && f_res.value()) {
        return f_res.value();
    }
    if (g_res && g_res.value()) {
        return g_res.value();
    }
    if (f_res && g_res) {
        return 0;
    }
    return {};
}

enum class SelectedOption {
    CONTINUE, STOP, CONTINUE_WITHOUT_ASKING
};


SelectedOption user_selects_option() {
    int option;
    while (true) {
        std::cout << "Select the action. Available options:\n1) continue\n2) stop\n3) continue without asking\nEnter the option: ";
        if (!(std::cin >> option)) {
            std::cin.clear();
            continue;
        }
        switch (option) {
            case 1:
                return SelectedOption::CONTINUE;
            case 2:
                return SelectedOption::STOP;
            case 3:
                return SelectedOption::CONTINUE_WITHOUT_ASKING;
            default:
                continue;
        }
    }

}


void stop_thread(std::thread &thread) {
    auto handle = thread.native_handle();
    pthread_cancel(handle);
    thread.detach();
}


int main() {
    int x{input_x()};

    std::optional<int> f_res{}, g_res{};
    std::thread f_thread{[&]{f_res = f(x);}};
    std::thread g_thread{[&]{g_res = g(x);}};

    bool continue_without_asking{false};
    bool running{true};
    while (running) {
        auto res = calculation(f_res, g_res);
        if (res) {
            std::cout << std::format("f(x) || g(x) = {}", res.value()) << std::endl;
            running = false;
            break;
        }
        if (continue_without_asking)
            continue;

        switch (user_selects_option()) {
            case SelectedOption::STOP: {
                std::cout << "Stopped.";
                running = false;
                break;
            }
            case SelectedOption::CONTINUE_WITHOUT_ASKING:
                continue_without_asking = true;
            case SelectedOption::CONTINUE:
                std::this_thread::sleep_for(10s);
        }
    }

    stop_thread(f_thread);
    stop_thread(g_thread);

    return 0;
}
