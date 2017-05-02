
#include "init_test.h"
#include "test_globals.h"

#include <dylocxx/init.h>
#include <dylocxx/unit_locality.h>
#include <dylocxx/hwinfo.h>

#include <dylocxx/utility.h>
#include <dylocxx/adapter/dart.h>
#include <dylocxx/internal/logging.h>

#include <boost/graph/graph_utility.hpp>
#include <boost/graph/depth_first_search.hpp>

#include <iostream>
#include <iomanip>


namespace dyloc {
namespace test {


TEST_F(InitTest, ImmediateFinalize) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);
  dyloc::finalize();
}

TEST_F(InitTest, UnitLocality) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);

  if (dyloc::myid().id == 0) {
    for (int u = 0; u < dyloc::num_units(); ++u) {
      const auto & uloc = dyloc::query_unit_locality(u);
      DYLOC_LOG_DEBUG("InitTest.UnitLocality", "unit locality:", uloc);
    }
  }

  dyloc::finalize();
}

class custom_dfs_visitor : public boost::default_dfs_visitor {
public:
  template <typename Vertex, typename Graph>
  void discover_vertex(Vertex u, const Graph & g) const {
    dyloc::locality_domain * ldom     = g[u].domain;
    const std::string &      ldom_tag = g[u].domain_tag;
    std::cout << std::left << std::setw(8)  << ldom->scope
              << std::left << std::setw(15) << ldom_tag << " | "
              << "units:"
              << dyloc::make_range(
                  ldom->unit_ids.begin(),
                  ldom->unit_ids.end())
              << std::endl;
  }
};

TEST_F(InitTest, DomainGraph) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);

  if (dyloc::myid().id == 0) {
    const auto & graph = dyloc::query_locality_graph().graph();
    // boost::print_graph(
    //   graph,
    //   boost::get(
    //     &domain_graph::vertex_properties::domain,
    //     graph));
    custom_dfs_visitor vis;
    boost::depth_first_search(graph, visitor(vis));
  }

  dyloc::finalize();
}

} // namespace dyloc
} // namespace test
