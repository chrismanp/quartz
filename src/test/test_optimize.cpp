#include <parlay/alloc.h>
#include <parlay/parallel.h>
#include <parlay/primitives.h>
#include <parlay/sequence.h>


#include "quartz/tasograph/substitution.h"
#include "quartz/tasograph/tasograph.h"
#include "test/gen_ecc_set.h"

using namespace quartz;

void parse_args(char **argv, int argc, bool &simulated_annealing,
                bool &early_stop, bool &disable_search, bool &serial_search,
                std::string &input_filename, std::string &output_filename,
                std::string &eqset_filename) {
  assert(argv[1] != nullptr);
  input_filename = std::string(argv[1]);
  early_stop = true;
  for (int i = 2; i < argc; i++) {
    if (!std::strcmp(argv[i], "--output")) {
      output_filename = std::string(argv[++i]);
      continue;
    }
    if (!std::strcmp(argv[i], "--eqset")) {
      eqset_filename = std::string(argv[++i]);
      continue;
    }
    if (!std::strcmp(argv[i], "--disable_search")) {
      disable_search = true;
      continue;
    }
    if (!std::strcmp(argv[i], "--serial")) {
      serial_search = true;
      continue;
    }

  }
}

int main(int argc, char **argv) {
  std::string input_fn, output_fn;
  std::string eqset_fn = "../Nam_6_3_complete_ECC_set.json";
  bool simulated_annealing = false;
  bool early_stop = false;
  bool disable_search = false;
  bool serial_search = false;
  parse_args(argv, argc, simulated_annealing, early_stop, disable_search, serial_search,
             input_fn, output_fn, eqset_fn);

  Context ctx({GateType::input_qubit, GateType::input_param, GateType::cx,
               GateType::h, GateType::rz, GateType::x, GateType::add});

  std::cout << "parsing file"<< std::endl;
  auto graph = Graph::from_qasm_file(&ctx, input_fn);
  std::cout << "parsing done"<< std::endl;
  assert(graph);

  std::vector<Context*> context_array;
  std::vector<std::vector<GraphXfer *>> xfers_array;
  if(serial_search) {
    graph->create_xfers(eqset_fn, 1, context_array, xfers_array);
    std::cout << "number of xfers: " << xfers_array[0].size() << std::endl;
    auto new_graph = graph->optimize(xfers_array[0], graph->gate_count() * 1.05, input_fn, "", true);
  } else {
    graph->create_xfers(eqset_fn, parlay::num_workers(), context_array, xfers_array);
    std::cout << "number of xfers: " << xfers_array[0].size() << std::endl;
    auto new_graph = graph->par_optimize(xfers_array, context_array, graph->gate_count() * 1.05, input_fn, "", true);
  }
  // new_graph->to_qasm(output_fn, false, false);
  return 0;
}
