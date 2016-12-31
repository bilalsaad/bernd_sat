#include "util.h"
#include <random>
#include <ctime>
namespace util {
  using std::string;
  using std::experimental::optional;
   double random_real(optional<int> seed) {
    
    static std::minstd_rand0 generator((seed ? *seed : std::time(0)));
    static std::uniform_real_distribution<double> dst(0, 1);
    return dst(generator);
  }



   template<typename T>
    struct Flag {
      string description;
      bool is_set;
      T val;
    };
  static std::unordered_map<string, Flag<int>> INT_FLAGS;
  static std::unordered_map<string, Flag<string>>
    STRING_FLAGS;
  static std::unordered_map<string, Flag<bool>> BOOL_FLAGS;
  void add_int_flag(const string& fname, const string& desc, int v) {
    INT_FLAGS.emplace(fname, Flag<int>{desc, false, v});
  }
  void add_string_flag(const string& fname, const string& desc, string v) {
    STRING_FLAGS.emplace(fname, Flag<string>{desc, false, v});
  }
  void add_bool_flag(const string& fname, const string& desc, bool v) {
    BOOL_FLAGS.emplace(fname, Flag<bool>{desc, false, v});
  }

  int get_int_flag(const string& fname) {
    return INT_FLAGS[fname].val;
  }
  string get_string_flag(const string& fname) {
    return STRING_FLAGS[fname].val;
  }
  bool get_bool_flag(const string& fname) {
    return BOOL_FLAGS[fname].val;
  }

  namespace {
    std::pair<string, string> parse_flag_name(char* arg) {
      while(*arg == '-')
        ++arg;
      string name;
      while (*arg != '=' && *arg != 0) {
        name.push_back(*(arg++));
      }
      assert(*arg != 0);
      ++arg;
      string val;
      while(*arg != 0) {
        val.push_back(*(arg++));
      }
      return {name, val};
    }
  }  // namespace

  void parse_flags(int argc, char** argv) {
    // first argv is the program name.
    ++argv;
    for (int i = 1; i < argc; ++i) {
      auto name_and_val = parse_flag_name(*argv);
      const auto& fname = name_and_val.first;
      const auto& fval = name_and_val.second;
      if (INT_FLAGS.count(fname) != 0) {  // int flag
        INT_FLAGS[fname].val = std::stoi(fval); 
        INT_FLAGS[fname].is_set = true;
      } else if (STRING_FLAGS.count(fname) != 0) {  // string flag
        STRING_FLAGS[fname].val = fval;
        STRING_FLAGS[fname].is_set = true;
      } else if (BOOL_FLAGS.count(fname) != 0) {  // bool flag
        BOOL_FLAGS[fname].val = fval == "true";
        BOOL_FLAGS[fname].is_set = true;
      } else {  // unrecognized flag
        std::cout << "Unrecognized flag " << fname << std::endl;
        assert(false);
      }
      ++argv;
    }
  }

  bool is_flag_set(const string& fname) {
    return (INT_FLAGS.count(fname) != 0 && INT_FLAGS[fname].is_set) ||
           (STRING_FLAGS.count(fname) != 0 && STRING_FLAGS[fname].is_set) ||
           (BOOL_FLAGS.count(fname) != 0 && BOOL_FLAGS[fname].is_set);
  }
} // namespace.
