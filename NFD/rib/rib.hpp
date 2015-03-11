/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2015,  Regents of the University of California,
 *                           Arizona Board of Regents,
 *                           Colorado State University,
 *                           University Pierre & Marie Curie, Sorbonne University,
 *                           Washington University in St. Louis,
 *                           Beijing Institute of Technology,
 *                           The University of Memphis.
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NFD_RIB_RIB_HPP
#define NFD_RIB_RIB_HPP

#include "rib-entry.hpp"
#include "fib-update.hpp"
#include "common.hpp"
#include <ndn-cxx/management/nfd-control-command.hpp>
#include <ndn-cxx/util/signal.hpp>

namespace nfd {
namespace rib {

/** \brief represents the RIB
 */
class Rib : noncopyable
{
public:
  typedef std::list<shared_ptr<RibEntry>> RibEntryList;
  typedef std::map<Name, shared_ptr<RibEntry>> RibTable;
  typedef RibTable::const_iterator const_iterator;
  typedef std::map<uint64_t, std::list<shared_ptr<RibEntry>>> FaceLookupTable;
  typedef bool (*RouteComparePredicate)(const Route&, const Route&);
  typedef std::set<Route, RouteComparePredicate> RouteSet;
  typedef std::list<shared_ptr<const FibUpdate>> FibUpdateList;

  Rib();

  const_iterator
  find(const Name& prefix) const;

  Route*
  find(const Name& prefix, const Route& route) const;

  void
  insert(const Name& prefix, const Route& route);

  void
  erase(const Name& prefix, const Route& route);

  void
  erase(const uint64_t faceId);

  const_iterator
  begin() const;

  const_iterator
  end() const;

  size_t
  size() const;

  bool
  empty() const;

  shared_ptr<RibEntry>
  findParent(const Name& prefix) const;

  /** \brief finds namespaces under the passed prefix
   *  \return{ a list of entries which are under the passed prefix }
   */
  std::list<shared_ptr<RibEntry>>
  findDescendants(const Name& prefix) const;

  const std::list<shared_ptr<const FibUpdate>>&
  getFibUpdates() const;

  void
  clearFibUpdates();

private:
  RibTable::iterator
  eraseEntry(RibTable::iterator it);

  void
  insertFibUpdate(shared_ptr<FibUpdate> update);

  void
  createFibUpdatesForNewRibEntry(RibEntry& entry, const Route& route);

  void
  createFibUpdatesForNewRoute(RibEntry& entry, const Route& route,
                              const bool captureWasTurnedOn);

  void
  createFibUpdatesForUpdatedRoute(RibEntry& entry, const Route& route,
                                  const uint64_t previousFlags, const uint64_t previousCost);
  void
  createFibUpdatesForErasedRoute(RibEntry& entry, const Route& route,
                                 const bool captureWasTurnedOff);

  void
  createFibUpdatesForErasedRibEntry(RibEntry& entry);

  RouteSet
  getAncestorRoutes(const RibEntry& entry) const;

  void
  modifyChildrensInheritedRoutes(RibEntry& entry, const Rib::RouteSet& routesToAdd,
                                                  const Rib::RouteSet& routesToRemove);

  void
  traverseSubTree(RibEntry& entry, Rib::RouteSet routesToAdd,
                                   Rib::RouteSet routesToRemove);

  /** \brief Adds passed routes to the entry's inherited routes list
   */
  void
  addInheritedRoutesToEntry(RibEntry& entry, const Rib::RouteSet& routesToAdd);

  /** \brief Removes passed routes from the entry's inherited routes list
   */
  void
  removeInheritedRoutesFromEntry(RibEntry& entry, const Rib::RouteSet& routesToRemove);

public:
  ndn::util::signal::Signal<Rib, Name> afterInsertEntry;
  ndn::util::signal::Signal<Rib, Name> afterEraseEntry;

private:
  RibTable m_rib;
  FaceLookupTable m_faceMap;
  FibUpdateList m_fibUpdateList;

  size_t m_nItems;
};

inline Rib::const_iterator
Rib::begin() const
{
  return m_rib.begin();
}

inline Rib::const_iterator
Rib::end() const
{
  return m_rib.end();
}

inline size_t
Rib::size() const
{
  return m_nItems;
}

inline bool
Rib::empty() const
{
  return m_rib.empty();
}

inline const Rib::FibUpdateList&
Rib::getFibUpdates() const
{
  return m_fibUpdateList;
}

inline void
Rib::clearFibUpdates()
{
  m_fibUpdateList.clear();
}

std::ostream&
operator<<(std::ostream& os, const Rib& rib);

} // namespace rib
} // namespace nfd

#endif // NFD_RIB_RIB_HPP
