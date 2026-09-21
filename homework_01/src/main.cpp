#include <iostream>
#include <cmath>
#include <numbers>
#include <iomanip>

constexpr double GRAVITY_MPS2 = 9.81;
constexpr double DROP_THRESHOLD_M = 2.0;

int main() {

    double altitude_m = 120.0;
    double target_x = 35.0;
    double target_y = 60.0;

    const double distance_to_target = std::hypot(target_x, target_y);   
    const double angle_to_target = std::atan2(target_x, target_y) * 180.0 / std::numbers::pi;
    const double free_fall_time = std::sqrt(2.0 * altitude_m / GRAVITY_MPS2);
    const double hit_velocity = GRAVITY_MPS2 * free_fall_time;
    const bool is_attack = distance_to_target <= DROP_THRESHOLD_M;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Дистанція до точки скиду: " << distance_to_target << " м\n"
              << "Курс на ціль: " << angle_to_target << " град\n"
              << "Час падіння: " << free_fall_time << " с\n"
              << "Швидкість зустрічі: " << hit_velocity << " м/с\n"
              << "Статус (1 = ATTACK, 0 = MOVE): " << is_attack  << "\n";

    return 0;
}
