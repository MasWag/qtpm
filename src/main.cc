#include <iostream>
#include <sstream>
#include <cstdio>
#include <boost/program_options.hpp>

#include "quantitative_timed_pattern_matching.hh"
#include "robustness.hh"

using namespace boost::program_options;

// fgets without file lock/unlock. We have to lock/unlock manually.
static inline char *fgets_unsafe(char * chr, int n, FILE *stream) {
  char * init_chr = chr;
  int c;
  char * ret = nullptr;
  if (--n <= 0) {
    return nullptr;
  }
  while ((c = getc_unlocked(stream)) != EOF) {
    ret = init_chr;
    if (c == '\n') {
      *chr = '\0';
      return init_chr;
    }
    *chr = c;
    chr++;
    if (--n <= 0) {
      *chr = '\0';
      return init_chr;
    }
  }
  return ret;
}

template<class Value>
static inline int getOne(FILE* file, double &absTime, std::vector<Value> &valuation) {
  std::array<char, 100> str;
  if (!fgets_unsafe(str.data(), 100, file)) {
    return EOF;
  }
  std::stringstream ss(std::string(str.data()));
  ss >> absTime;
  
  while (!ss.eof()) {
    Value v;
    ss >> v;
    if (ss.fail()) {
      return valuation.size() + 1;
    }
    valuation.push_back(std::move(v));
  }
  return valuation.size() + 1;
}

template<class SignalVariables, class ClockVariables, class Weight, class Value>
static inline void QTPM(QuantitativeTimedPatternMatching<SignalVariables, ClockVariables, Weight, Value> &qtpm, FILE* fin, FILE* fout, bool quiet) {
  flockfile(fin);
  double absTime;
  std::vector<Value> valuation;
  while(getOne(fin, absTime, valuation) != EOF) {
    qtpm.feed(valuation, absTime);

    auto result = qtpm.getResultRef();
    if (!quiet) {
      std::array<Bounds, 6> arr;
      Weight weight;
      BOOST_FOREACH(std::tie(arr, weight), result) {
        fprintf(fout, "----- Weight: %lf -----\n", weight.data);
        fprintf(fout, "%10lf %8s t %s %10lf\n", -arr[0].first,
               (arr[0].second ? "<=" : "<"),
               (arr[1].second ? "<=" : "<"),
               arr[1].first);
        fprintf(fout, "%10lf %8s t' %s %10lf\n", -arr[2].first,
               (arr[2].second ? "<=" : "<"),
               (arr[3].second ? "<=" : "<"),
               arr[3].first);
        fprintf(fout, "%10lf %8s t' - t %s %10lf\n", -arr[4].first,
               (arr[4].second ? "<=" : "<"),
               (arr[5].second ? "<=" : "<"),
               arr[5].first);
        fputs("=============================\n", fout);
      }
    }
    result.clear();
  }
}

int main(int argc, char *argv[])
{
  constexpr const auto programName = "qtpm";

  // visible options
  options_description visible("description of options");
  std::string timedWordFileName;
  std::string timedAutomatonFileName;
  visible.add_options()
    ("help,h", "help")
    ("quiet,q", "quiet")
    ("version,V", "version")
    ("input,i", value<std::string>(&timedWordFileName)->default_value("stdin"),"input file of Timed Words")
    ("automaton,f", value<std::string>(&timedAutomatonFileName)->default_value(""),"input file of Timed Automaton")
    ("maxmin", "use maxmin semiring space robustness (default)")
    ("minplus", "use minplus semiring space robustness");

  command_line_parser parser(argc, argv);
  parser.options(visible);
  variables_map vm;
  const auto parseResult = parser.run();
  store(parseResult, vm);
  notify(vm);

  if (timedAutomatonFileName.empty() || vm.count("help")) {
    std::cout << programName << " [OPTIONS] -f FILE [FILE]\n" 
              << visible << std::endl;
    return 0;
  }
  if (vm.count("version")) {
    std::cout << "QTPM (an implementation of Quantitative Timed Pattern Matching) 0.0.0\n"
              << visible << std::endl;
    return 0;
  }

  using SignalVariables = uint8_t;
  using ClockVariables = uint8_t;

  BoostTimedAutomaton<SignalVariables, ClockVariables> TA;
  std::vector<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor> initStates;

  // parse TA
  std::ifstream taStream(timedAutomatonFileName);
  parseBoostTA(taStream, TA, initStates);

  FILE* file = timedWordFileName == "stdin" ? stdin : fopen(timedWordFileName.c_str(), "r");
  using Value = double;

  // TODO: branching by semirings
  if (vm.count("minplus")) {
    using Weight = MinPlusSemiring<Value>;
    std::function<Weight(const std::vector<Constraint<ClockVariables>> &,const std::vector<std::vector<Value>> &)> cost = multipleSpaceRobustness<Weight, Value, ClockVariables>;
    QuantitativeTimedPatternMatching<SignalVariables, ClockVariables, Weight, Value> qtpm(TA, initStates, cost);
    QTPM(qtpm, file, stdout, vm.count("quiet"));
  } else {
    using Weight = MaxMinSemiring<Value>;
    std::function<Weight(const std::vector<Constraint<ClockVariables>> &,const std::vector<std::vector<Value>> &)> cost = multipleSpaceRobustness<Weight, Value, ClockVariables>;
    QuantitativeTimedPatternMatching<SignalVariables, ClockVariables, Weight, Value> qtpm(TA, initStates, cost);
    QTPM(qtpm, file, stdout, vm.count("quiet"));
  }

  return 0;
}
