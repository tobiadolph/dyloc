#ifndef DYLOCXX__TOPOLOGY_H__INCLUDED
#define DYLOCXX__TOPOLOGY_H__INCLUDED

#include <dylocxx/host_topology.h>
#include <dylocxx/unit_mapping.h>
#include <dylocxx/locality_domain.h>

#include <dyloc/common/types.h>

#include <dash/dart/if/dart.h>

#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/graph_traits.hpp>

#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <string>


namespace dyloc {


/**
 * Extension to the hwloc topology data structure.
 */
class topology {
  typedef topology self_t;

 public:
  enum class edge_type : int {
    unspecified  = 0,
    contains     = 100,
    sibling,
    alias
  };

  enum class vertex_state : int {
    unspecified     = 0,
    hidden_vertex   = 100,
    selected_vertex,
  };

  struct vertex_properties {
    std::string       domain_tag;
    locality_domain * domain;
    vertex_state      state;
  };

  struct edge_properties {
    edge_type type;
    int       distance;
  };

  /*
   * Using boost graph with domain data as external properties, see:
   *   http://programmingexamples.net/wiki/CPP/Boost/BGL/GridGraphProperties
   */
  typedef boost::adjacency_list<
            boost::listS,          // out-edges stored in a std::list
            boost::vecS,           // vertex set stored here
            boost::directedS,      // directed graph (parent -> sub)
            vertex_properties,     // vertex properties
            edge_properties,       // edge properties
            boost::no_property,    // graph properties
            boost::listS           // edge storage
          >
    graph_t;

  typedef boost::property_map<
            graph_t,
            boost::vertex_index_t
          >::const_type
    index_map_t;

  typedef boost::graph_traits<graph_t>::vertex_descriptor
    graph_vertex_t;

 private:
  host_topology    & _host_topology;
  unit_mapping     & _unit_mapping;
  locality_domain  & _root_domain;

  std::unordered_map<std::string, locality_domain> _domains;

  graph_t _graph;
  
 public:
  topology() = delete;

  topology(
    host_topology   & host_topo,
    unit_mapping    & unit_map,
    locality_domain & team_global_dom)
  : _host_topology(host_topo)
  , _unit_mapping(unit_map)
  , _root_domain(team_global_dom) {
    build_hierarchy();
  }

#if 0
  topology(const topology & other)
  : _host_topology(other._host_topology)
  , _unit_mapping(other._unit_mapping)
  , _root_domain(other._root_domain)
  , _domains(other._domains) {
    boost::copy_graph(other._graph, _graph);
  }

  topology & operator=(const topology & rhs) {
    _host_topology = rhs._host_topology;
    _unit_mapping  = rhs._unit_mapping;
    _root_domain   = rhs._root_domain;
    _domains       = rhs._domains;
    boost::copy_graph(rhs._graph, _graph);
    return *this;
  }
#endif

  const std::unordered_map<std::string, locality_domain> & domains() const {
    return _domains;
  }

  locality_domain & operator[](const std::string & tag) {
    return _domains[tag];
  }

  const locality_domain & operator[](const std::string & tag) const {
    return _domains.at(tag);
  }

  inline const graph_t & graph() const noexcept {
    return _graph;
  }

  template <class Visitor>
  void depth_first_search(const Visitor & vis) {
    boost::depth_first_search(_graph, vis);
  }

  template <class Visitor>
  void depth_first_search(const Visitor & vis) const {
    boost::depth_first_search(_graph, vis);
  }

  template <class FilterPredicate>
  void filter(
         const FilterPredicate & filter) {
    // filtered_graph<graph_t, filter> fg(_graph, filter);
  }

  template <class Iterator, class Sentinel>
  const locality_domain & ancestor(
         const Iterator & domain_tag_first,
         const Sentinel & domain_tag_last) const {
    for (int prefix_len = 0;
         prefix_len < std::distance(domain_tag_first, domain_tag_last);
         ++prefix_len) {
      for (auto it = domain_tag_first; it != domain_tag_last; ++it) {
        // TODO
      }
    }
    return _root_domain;
  }

  template <class Iterator, class Sentinel>
  void group_domains(
         const Iterator & group_domain_tag_first,
         const Sentinel & group_domain_tag_last) {
    // TODO
  }

  template <class Iterator, class Sentinel>
  void exclude_domains(
         const Iterator & domain_tag_first,
         const Sentinel & domain_tag_last) {
    // TODO
  }

  template <class Iterator, class Sentinel>
  void select_domains(
         const Iterator & domain_tag_first,
         const Sentinel & domain_tag_last) {
    // TODO
  }

 private:
  void build_hierarchy();

  void build_node_level(
         locality_domain & node_domain,
         graph_vertex_t  & node_domain_vertex);

  void build_module_level(
         locality_domain & module_domain,
         graph_vertex_t  & node_domain_vertex,
         int               module_scope_level);

};

} // namespace dyloc

#endif // DYLOCXX__TOPOLOGY_H__INCLUDED
