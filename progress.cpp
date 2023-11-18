// I HAVE NOT COMPILED ANY OF THIS HAHA

std::string read_file (std::string file_path) {

  // Open the file
  std::ifstream file_stream(file_path);

  // Check if the file is open
  if (!file_stream.is_open()) {
      std::cerr << "Error opening file: " << file_path << std::endl;
      return 1;
  }

  // Read the file contents into a string
  std::string file_contents(
      (std::istreambuf_iterator<char>(file_stream)),
      (std::istreambuf_iterator<char>())
  );

  // Close the file
  file_stream.close();
  return file_contents;
}

void create_xfers(std::string eqset_fn, int P, Context* contextArray, std::vector<GraphXfer *> * xferArray) {
  // read the file once and use the string to create P copies of transformations.
  std::string file_contents = read_file(eqset_fn);
  std::istringstream iss(file_contents);

  auto gate_set = {GateType::h, GateType::x, GateType::rz, GateType::add,
                  GateType::cx, GateType::input_qubit, GateType::input_param};
  for (int i = 0; i < P; i ++) {
    Context *ctxt = new Context(gate_set);
    EquivalenceSet* eqs = new EquivalenceSet();
    if (!eqs->load_json(ctxt, iss)) {
      std::cout << "Failed to load equivalence file \"" << "whe"
                << "\"." << std::endl;
      assert(false);
    }

    std::vector<std::vector<CircuitSeq *>> eccs = eqs->get_all_equivalence_sets();

    std::vector<GraphXfer *> xfers;

    for (const auto &ecc : eccs) {
      CircuitSeq *representative = ecc.front();
      for (auto &circuit : ecc) {
        if (circuit != representative) {
          auto xfer =
              GraphXfer::create_GraphXfer(ctxt, circuit, representative, true);
          if (xfer != nullptr) {
            xfers.push_back(xfer);
          }
          xfer = GraphXfer::create_GraphXfer(ctxt, representative, circuit, true);
          if (xfer != nullptr) {
            xfers.push_back(xfer);
          }
        }
      }
    }
    contextArray[i] = *context;
    xferArray[i] =  new std::vector<GraphXfer*>(xfers);
  }
}
