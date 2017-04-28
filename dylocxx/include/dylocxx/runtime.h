#ifndef DYLOCXX__RUNTIME_H__INCLUDED
#define DYLOCXX__RUNTIME_H__INCLUDED

#include <dylocxx/host_topology.h>
#include <dylocxx/unit_mapping.h>
#include <dylocxx/locality_domain.h>

#include <dyloc/common/types.h>

#include <dash/dart/if/dart_types.h>

#include <vector>


namespace dyloc {

class runtime {
  std::unordered_map<dart_team_t, host_topology>   _host_topologies;
  std::unordered_map<dart_team_t, unit_mapping>    _unit_mappings;
  std::unordered_map<dart_team_t, locality_domain> _locality_domains;

 public:
  void initialize_locality(dart_team_t team);
  void finalize_locality(dart_team_t team);
};

} // namespace dyloc

#endif // DYLOCXX__RUNTIME_H__INCLUDED