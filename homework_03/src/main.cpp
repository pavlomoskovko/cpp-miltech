#include <iostream>
#include <cmath>
#include <numbers>
#include <iomanip>
#include <string>
#include <optional>
#include <fstream>

constexpr double GRAVITY_MPS2 = 9.81;
constexpr int MAX_TARGETS = 64;

enum class TargetType { PERSONNEL, LOGISTICS, APC, TANK, RADAR };

int target_value(TargetType type)
{
  switch (type) {
    case TargetType::PERSONNEL:
      return 10;
    case TargetType::LOGISTICS:
      return 20;
    case TargetType::APC:
      return 40;
    case TargetType::TANK:
      return 60;
    case TargetType::RADAR:
      return 90;
  }
  return 0;
}

struct Target {
  double x;
  double y;
  int hp;
  TargetType type;
};

struct Ammo {
  const char* name;
  double radius;
  int damage;
  int price;
};

struct Scenario {
  double altitude_m;
  double wind_x;
  double wind_y;
  int count;
  Target targets[MAX_TARGETS];
};

constexpr Ammo AMMO_TYPES[] = {{"Frag", 60.0, 25, 35}, {"HE", 35.0, 55, 20}, {"HEAT", 15.0, 130, 45}};

std::optional<TargetType> parse_target_type(const std::string& word)
{
  if (word == "PERSONNEL")
    return TargetType::PERSONNEL;
  if (word == "LOGISTICS")
    return TargetType::LOGISTICS;
  if (word == "APC")
    return TargetType::APC;
  if (word == "TANK")
    return TargetType::TANK;
  if (word == "RADAR")
    return TargetType::RADAR;
  return std::nullopt;
}

bool read_scenario(const char* path, Scenario& out)
{
  std::ifstream in(path);
  if (!in) {
    std::cerr << "cannot open " << path << "\n";
    return false;
  }

  if (!(in >> out.altitude_m >> out.wind_x >> out.wind_y >> out.count)) {
    std::cerr << "bad scenario header\n";
    return false;
  }
  if (out.altitude_m <= 0.0) {
    std::cerr << "altitude must be positive\n";
    return false;
  }
  if (out.count < 1 || out.count > MAX_TARGETS) {
    std::cerr << "target count must be in 1.." << MAX_TARGETS << "\n";
    return false;
  }

  for (int i = 0; i < out.count; ++i) {
    Target& target = out.targets[i];
    std::string type_word;
    if (!(in >> target.x >> target.y >> target.hp >> type_word)) {
      std::cerr << "bad target line " << i << "\n";
      return false;
    }
    const std::optional<TargetType> type = parse_target_type(type_word);
    if (!type) {
      std::cerr << "unknown target type: " << type_word << "\n";
      return false;
    }
    target.type = *type;
  }
  return true;
}

double compute_free_fall_time(double altitude_m)
{
  return std::sqrt(2.0 * altitude_m / GRAVITY_MPS2);
}

double compute_distance_to_target(double target_ax, double target_ay, double target_bx, double target_by)
{
  return std::hypot(target_ax - target_bx, target_ay - target_by);
}

int compute_net(const Scenario& s, int candidate, const Ammo& ammo, int& destroyed, int& value_sum)
{
  const Target& impact = s.targets[candidate];
  destroyed = 0;
  value_sum = 0;
  for (int j = 0; j < s.count; ++j) {
    const Target& target = s.targets[j];
    const double distance = compute_distance_to_target(impact.x, impact.y, target.x, target.y);
    if (distance <= ammo.radius && ammo.damage >= target.hp) {
      ++destroyed;
      value_sum += target_value(target.type);
    }
  }
  return value_sum - ammo.price;
}

int main()
{
  Scenario scenario;
  if (!read_scenario("scenario.txt", scenario)) {
    return 1;
  }

  int best_candidate = 0;
  const Ammo* best_ammo = &AMMO_TYPES[0];
  int best_destroyed = 0;
  int best_value_sum = 0;
  int best_net = compute_net(scenario, best_candidate, *best_ammo, best_destroyed, best_value_sum);

  for (int i = 0; i < scenario.count; ++i) {
    for (const Ammo& ammo : AMMO_TYPES) {
      int destroyed = 0;
      int value_sum = 0;
      const int net = compute_net(scenario, i, ammo, destroyed, value_sum);
      const bool better = net > best_net || (net == best_net && ammo.price < best_ammo->price);
      if (better) {
        best_candidate = i;
        best_ammo = &ammo;
        best_destroyed = destroyed;
        best_value_sum = value_sum;
        best_net = net;
      }
    }
  }

  const Target& impact = scenario.targets[best_candidate];
  const double fall_time_s = compute_free_fall_time(scenario.altitude_m);
  const double release_x = impact.x - scenario.wind_x * fall_time_s;
  const double release_y = impact.y - scenario.wind_y * fall_time_s;

  std::ofstream out("result.txt");
  if (!out) {
    std::cerr << "cannot open result.txt for writing\n";
    return 1;
  }
  out << std::fixed << std::setprecision(2);
  out << impact.x << " " << impact.y << "\n"
      << release_x << " " << release_y << "\n"
      << best_ammo->name << "\n"
      << best_destroyed << " " << best_value_sum << " " << best_ammo->price << " " << best_net << "\n";

  return 0;
}
