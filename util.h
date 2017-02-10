#ifndef UTIL_H
#define UTIL_H
#include <chrono>
#include <string>
#include <iostream>
#include <random>
#include <cassert>
#include <unordered_map>
#include <experimental/optional>
#include "json.hpp"
namespace util {
  using Clock = std::chrono::high_resolution_clock;
  using std::chrono::time_point;
  using std::chrono::duration_cast;
  using timeunit = std::chrono::duration<double>;
  using namespace std::literals::chrono_literals;
  class scoped_timer {
    public:
      scoped_timer(const std::string& s = ""): start_time(Clock::now()),
        name(s) {}
      ~scoped_timer() {
        auto end_time = Clock::now();
        auto diff = duration_cast<timeunit>(end_time - start_time);
        std::cout << "Time elapsed " << (name.empty() ? "" : "for " + name)
          << ": " << diff.count() << " seconds" << std::endl;
      }
    private:
      time_point<Clock> start_time; 
      std::string name;
  };

  // returns a random real number between 0 .. 1
  double random_real(std::experimental::optional<int> seed = {});

  // flag stuff
  void add_int_flag(const std::string& flagname, const std::string& desc,
      int default_val);
  void add_string_flag(const std::string& flagname, const std::string& desc,
      std::string default_val);
  void add_bool_flag(const std::string& flagname, const std::string& desc,
      bool default_val);
  void add_double_flag(const std::string& flagname, const std::string& desc,
      double default_val);
  bool get_bool_flag(const std::string& fname);
  double get_double_flag(const std::string& fname);
  std::string get_string_flag(const std::string& fname);
  int get_int_flag(const std::string& fname);
  void parse_flags(int argc, char** argv);
  bool is_flag_set(const std::string& fname);


  // Stuff for experiments.
  struct experiment_info {
    int num_vars = 1;
    int num_clauses = 1;
    int how_many_runs = 100;

    operator nlohmann::json(){
      nlohmann::json j;
      j["num_vars"] = num_vars;
      j["num_clauses"] = num_clauses;
      j["how_many_runs"] = how_many_runs;
      return j;
    }
    experiment_info(const nlohmann::json& j):
      num_vars(j["num_vars"]),
      num_clauses(j["num_clauses"]),
      how_many_runs(j["how_many_runs"]) {}

    experiment_info() : num_vars(0), num_clauses(0), how_many_runs(0) {}
  };
} // namespace util.
#endif
