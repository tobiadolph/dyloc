
#include "domain_group_test.h"
#include "test_globals.h"

#include <dylocxx/init.h>
#include <dylocxx/unit_locality.h>
#include <dylocxx/hwinfo.h>

#include <dylocxx/topology.h>
#include <dylocxx/utility.h>
#include <dylocxx/adapter/dart.h>
#include <dylocxx/internal/logging.h>

#include <boost/graph/graph_utility.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graphviz.hpp>

#include <algorithm>


namespace dyloc {
namespace test {

TEST_F(DomainGroupTest, GroupSubdomains) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);

  auto & topo = dyloc::query_topology();
  locality_domain_dfs_output_visitor<typename topology::domain_map>
    vis(topo.domains());

  DYLOC_LOG_DEBUG("DomainGroupTest.GroupSubdomains",
                  "total domain hierarchy:");
  if (dyloc::myid().id == 0) {
    topo.depth_first_search(vis);
    graphviz_out(
      topo.graph(), "DomainGroupTest.GroupSubdomains.original.dot");
  }
  dart_barrier(DART_TEAM_ALL);

  // Tags of locality domains in NUMA scope:
  auto numa_domain_tags = topo.scope_domain_tags(
                            DYLOC_LOCALITY_SCOPE_NUMA);
  for (const auto & numa_domain_tag : numa_domain_tags) {
    DYLOC_LOG_DEBUG_VAR("DomainGroupTest.GroupSubdomains", numa_domain_tag);
  }

  topo.group_domains(
    numa_domain_tags.begin(),
    numa_domain_tags.end());

  dart_barrier(DART_TEAM_ALL);

  DYLOC_LOG_DEBUG("DomainGroupTest.GroupSubdomains",
                  "total domain hierarchy:");
  if (dyloc::myid().id == 0) {
    topo.depth_first_search(vis);
    graphviz_out(
      topo.graph(), "DomainGroupTest.GroupSubdomains.grouped.dot");
  }
  dart_barrier(DART_TEAM_ALL);

  dyloc::finalize();
}

TEST_F(DomainGroupTest, GroupAsymmetric) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);

  auto & topo = dyloc::query_topology();
  locality_domain_dfs_output_visitor<typename topology::domain_map>
    vis(topo.domains());

  if (dyloc::myid().id == 0) {
    topo.depth_first_search(vis);
    graphviz_out(
      topo.graph(), "DomainGroupTest.GroupAsymmetric.original.dot");
  }
  dart_barrier(DART_TEAM_ALL);



  if (dyloc::myid().id == 0) {
    topo.depth_first_search(vis);
    graphviz_out(
      topo.graph(), "DomainGroupTest.GroupAsymmetric.grouped.dot");
  }
  dart_barrier(DART_TEAM_ALL);

  dyloc::finalize();
}

} // namespace dyloc
} // namespace test

