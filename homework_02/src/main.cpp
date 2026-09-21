#include <iostream>
#include <cmath>
#include <numbers>
#include <iomanip>
#include <string>

constexpr double GRAVITY_MPS2 = 9.81;
constexpr int MAX_TICKS = 60;
constexpr double ARRIVAL_EPS_M = 1e-6;

double compute_free_fall_time(double altitude_m)
{
  return std::sqrt(2.0 * altitude_m / GRAVITY_MPS2);
}

double compute_distance_to_target(double target_ax, double target_ay, double target_bx, double target_by)
{
  return std::hypot(target_ax - target_bx, target_ay - target_by);
}

double compute_angle_to_target(double target_x, double target_y)
{
  return std::atan2(target_x, target_y) * 180.0 / std::numbers::pi;
}

double compute_hit_velocity(double free_fall_time)
{
  return GRAVITY_MPS2 * free_fall_time;
}

double step_toward(double& bx, double& by, double rx, double ry, double step_m)
{
  const double d = compute_distance_to_target(bx, by, rx, ry);
  if (d <= step_m) {
    bx = rx;
    by = ry;
    return 0.0;
  }
  else {
    bx += step_m * (rx - bx) / d;
    by += step_m * (ry - by) / d;
    return d - step_m;
  }
}

int main(int argc, char* argv[])
{
  if (argc != 7) {
    std::cerr << "usage: " << argv[0] << " <altitude_m> <target_x> <target_y> <wind_x> <wind_y> <step_m>\n";
    return 1;
  }

  const double altitude_m = std::stod(argv[1]);
  const double target_x = std::stod(argv[2]);
  const double target_y = std::stod(argv[3]);
  const double wind_x = std::stod(argv[4]);
  const double wind_y = std::stod(argv[5]);
  const double step_m = std::stod(argv[6]);

  const double free_fall_time = compute_free_fall_time(altitude_m);
  const double release_x = target_x - wind_x * free_fall_time;
  const double release_y = target_y - wind_y * free_fall_time;

  const double angle_to_target = compute_angle_to_target(release_x, release_y);
  const double hit_velocity = compute_hit_velocity(free_fall_time);

  double drone_x = 0.0;
  double drone_y = 0.0;
  int ticks = 0;
  double remaining = compute_distance_to_target(drone_x, drone_y, release_x, release_y);

  while (remaining > ARRIVAL_EPS_M && ticks < MAX_TICKS) {
    remaining = step_toward(drone_x, drone_y, release_x, release_y, step_m);
    ++ticks;
  }

  const bool is_attack = remaining <= ARRIVAL_EPS_M;
  std::cout << std::fixed << std::setprecision(2);
  std::cout << "Точка скиду X: " << release_x << " м\n"
            << "Точка скиду Y: " << release_y << " м\n"
            << "Курс на точку скиду: " << angle_to_target << " град\n"
            << "Час падіння: " << free_fall_time << " с\n"
            << "Швидкість зустрічі: " << hit_velocity << " м/с\n"
            << "Кроків корекції: " << ticks << "\n"
            << "Статус (1 = ATTACK, 0 = MOVE): " << is_attack << "\n";

  return 0;
}
