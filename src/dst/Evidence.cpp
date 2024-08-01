#include "dst/Evidence.h"
#include "dst/Universe.h"

#include <stdarg.h>
#include <list>

namespace CPPDST
{
    CEvidence::CEvidence(const CUniverse* universe) : m_universe(universe)
    {}

    void CEvidence::addFocalSet(const FocalSet& set)
    {
        m_focalSets.push_back(set);
    }

    void CEvidence::addFocalSet(double mass, const HypothesesSet& members)
    {
        m_focalSets.push_back({mass, members});
    }

    void CEvidence::addFocalSet(double mass, const VariablesSet& members)
    {
        m_focalSets.push_back({
            mass,
            m_universe->bitsetRepresentation(members)
        });
    }

    void CEvidence::addFocalSet(double mass, const void* member, ...)
    {
        va_list argumentList;
        const void* current;
        VariablesSet members;

        // translate the var-arg list to a set
        va_start(argumentList, member);
        for (current = member; current != nullptr; current = va_arg(argumentList, const void*)) {
            members.insert(current);
        }
        va_end(argumentList);

        // and delegate to set-variant of this method
        addFocalSet(mass, members);
    }

    void CEvidence::addFocalSet(double mass, const size_t& member)
    {
        VariablesSet members;
        members.insert(m_universe->hypothesisToIdentifier(member));
        addFocalSet(mass, members);
    }

    void CEvidence::addOmegaSet()
    {
        // calculate how much mass is already distributed
        double usedMass = 0.0;
        for (std::list<FocalSet>::iterator i = m_focalSets.begin(); i != m_focalSets.end(); ++i) {
            usedMass += i->m_mass;
        }

        // and add the rest to the omega set containing all hypotheseses
        if(usedMass < 1.0) {
            m_focalSets.push_back({
                1.0 - usedMass,
                {}
            });
        }
    }

    CEvidence CEvidence::operator&(const CEvidence& other)
    {
        CEvidence combinedEvidence(m_universe);
        std::list<FocalSet> focalSetsBuffer;
        double conflict = 0.0;

        // build the pairwise intersections of the focal sets of both evidences
        for (std::list<FocalSet>::iterator i = m_focalSets.begin(); i != m_focalSets.end(); ++i) {
            for (std::list<FocalSet>::const_iterator j = other.m_focalSets.begin(); j != other.m_focalSets.end(); ++j) {
                FocalSet combined {
                    i->m_mass * j->m_mass,
                    i->m_set & j->m_set
                };

                if (combined.m_set.any()) {
                    // add focal set to new evidence if intersection is not empty
                    focalSetsBuffer.push_back(combined);
                } else {
                    // add the mass to te conflict otherwise
                    conflict += combined.m_mass;
                }
            }
        }

        // normalize the masses
        for(std::list<FocalSet>::iterator i = focalSetsBuffer.begin(); i != focalSetsBuffer.end(); ++i) {
            i->m_mass *= 1.0 / (1.0 - conflict);
            combinedEvidence.addFocalSet(*i);
        }

        return combinedEvidence;
    }

    double CEvidence::conflict(const CEvidence& other)
    {
        double conflict = 0.0;

        // iterate pair-wise over all focal-sets of both evidences
        for (std::list<FocalSet>::iterator i = m_focalSets.begin(); i != m_focalSets.end(); ++i) {
            for (std::list<FocalSet>::const_iterator j = other.m_focalSets.begin(); j != other.m_focalSets.end(); ++j) {
                if ((i->m_set & j->m_set).none()) {
                    // and add the combined mass to the conflict if the intersection is empty
                    conflict += i->m_mass * j->m_mass;
                }
            }
        }

        return conflict;
    }

    double CEvidence::belief(const HypothesesSet& members)
    {
        // search for focal sets that are real subsets of the given hypotheseses
        double belief = 0.0;
        for (std::list<FocalSet>::iterator i = m_focalSets.begin(); i != m_focalSets.end(); ++i) {
            if ((members & i->m_set).count() == i->m_set.count() ) {
                belief += i->m_mass;
            }
        }

        return belief;
    }

    double CEvidence::belief(const VariablesSet& members)
    {
        // translate set to bit-field
        HypothesesSet givenItems = m_universe->bitsetRepresentation(members);
        return belief(givenItems);
    }

    double CEvidence::belief(const void* member, ...)
    {
        va_list argumentList;
        const void* current;
        VariablesSet givenItems;
        // translate the var-arg list to a set
        va_start(argumentList, member);
        for (current = member; current != nullptr; current = va_arg(argumentList, const void*)) {
            givenItems.insert(current);
        }
        va_end(argumentList);

        // and delegate to set-variant of this method
        return belief(givenItems);
    }

    double CEvidence::belief(const size_t& member)
    {
        VariablesSet givenItems;
        givenItems.insert(m_universe->hypothesisToIdentifier(member));
        return belief(givenItems);
    }

    double CEvidence::plausability(const HypothesesSet& members)
    {
        // search for focal sets where intersection with given items is not empty and add mass to plausability
        double plausability = 0.0;
        for (std::list<FocalSet>::iterator i = m_focalSets.begin(); i != m_focalSets.end(); ++i) {
            if ((members & i->m_set).any() ) {
                plausability += i->m_mass;
            }
        }

        return plausability;
    }

    double CEvidence::plausability(const VariablesSet& members)
    {
        return plausability(m_universe->bitsetRepresentation(members));
    }

    double CEvidence::plausability(void* member, ...)
    {
        va_list argumentList;
        void *current;
        VariablesSet givenItems;
        // translate var-arg list to set
        va_start(argumentList, member);
        for (current = member; current != nullptr; current = va_arg(argumentList, void*)) {
            givenItems.insert(current);
        }
        va_end(argumentList);

        // and delegate to set-variant of this method
        return plausability(givenItems);
    }

    const void* CEvidence::mostBelievable()
    {
        double largestBelief = -1.0;
        int largestBeliefIdx = 0;

        for (int j = 0; j < m_universe->m_lastHypothesisNumber; ++j) {
            double belief = 0.0;
            for (std::list<FocalSet>::iterator i = m_focalSets.begin(); i != m_focalSets.end(); ++i) {
                if (i->m_set.test(j) && i->m_set.count() == 1) {
                    // current item is the only on in focal set
                    belief += i->m_mass;
                }
            }

            // keep track of the item with the maximum belief
            if (belief > largestBelief) {
                largestBelief = belief;
                largestBeliefIdx = j;
            }
        }

        return m_universe->m_hypotheseses[largestBeliefIdx];
    }

    const void* CEvidence::mostPlausible()
    {
        double largestPlausability = -1.0;
        int largestPlausabilityIdx = 0;

        for (int j = 0; j < m_universe->m_lastHypothesisNumber; ++j) {
            double plausability = 0.0;
            for (std::list<FocalSet>::iterator i = m_focalSets.begin(); i != m_focalSets.end(); ++i) {
                if (i->m_set.test(j)) {
                    // current item is contained in hypotheses
                    plausability += i->m_mass;
                }
            }

            // keep track of the item with maximum plausability
            if(plausability > largestPlausability) {
                largestPlausability = plausability;
                largestPlausabilityIdx = j;
            }
        }

        return m_universe->m_hypotheseses[largestPlausabilityIdx];
    }

    const void* CEvidence::bestMatch()
    {
        double largestBelief = -1.0;
        double largestPlausability = -1.0;
        int bestMatchIdx = 0;

        for (int j = 0; j < m_universe->m_lastHypothesisNumber; ++j) {
            double belief = 0.0;
            double plausability = 0.0;
            for (std::list<FocalSet>::iterator i = m_focalSets.begin(); i != m_focalSets.end(); ++i) {
                if (i->m_set.test(j)) {
                    // current item is contained in focal set -> add to plausability
                    plausability += i->m_mass;
                    if(i->m_set.count() == 1) {
                        // and is the only one in set -> add to belief
                        belief += i->m_mass;
                    }
                }
            }

            // keep track of item with maximum belief and maximum plausability
            if (belief > largestBelief || (belief == largestBelief && plausability > largestPlausability)) {
                largestBelief = belief;
                largestPlausability= plausability;
                bestMatchIdx = j;
            }
        }

        return m_universe->m_hypotheseses[bestMatchIdx];
    }
}
