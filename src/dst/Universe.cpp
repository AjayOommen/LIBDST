#include "dst/Universe.h"

#include <stdarg.h>

namespace CPPDST
{
    CUniverse::CUniverse() : m_lastHypothesisNumber(0)
    {}

    void CUniverse::addHypotheseses(const VariablesSet& hypotheseses)
    {
        for (VariablesSet::iterator i = hypotheseses.begin(); i != hypotheseses.end(); ++i) {
            if (m_lastHypothesisNumber >= MAX_HYPOTHESESES - 1) {
                // Only MAX_HYPOTHESESES hypotheseses allowed in this version.
                throw 1;
            }

            this->m_hypotheseses[m_lastHypothesisNumber++] = *i;
        }
    }

    void CUniverse::addHypotheseses(const void* hypothesis, ...)
    {
        va_list argumentList;
        const void* current;
        VariablesSet hypotheseses;

        // translate the var-arg list to a set
        va_start (argumentList, hypothesis);
        for (current = hypothesis; current != nullptr; current = va_arg(argumentList, const void*)) {
            hypotheseses.insert(current);
        }
        va_end(argumentList);

        // and delegate to the set variant of this method
        addHypotheseses(hypotheseses);
    }

    void CUniverse::addHypotheseses(const size_t& hypothesis)
    {
        VariablesSet hypotheseses;
        hypotheseses.insert(hypothesisToIdentifier(hypothesis));
        addHypotheseses(hypotheseses);
    }

    CEvidence CUniverse::addEvidence() const
    {
        // the evidence needs a link to the universe to have access to all hypotheseses
        return CEvidence(this);
    }

    HypothesesSet CUniverse::bitsetRepresentation(const VariablesSet& members) const
    {
        HypothesesSet newSet;
        // iterate over all set entries and all known hypotheseses and set the bitmap accordingly
        for (VariablesSet::iterator  i = members.begin(); i != members.end(); ++i) {
            for (int j=0; j < m_lastHypothesisNumber; ++j) {
                if (m_hypotheseses[j] == *i) {
                    newSet.set(j, true);
                }
            }
        }

        return newSet;
    }

    HypothesesSet CUniverse::bitsetRepresentation(const void* member, ...) const
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
        return bitsetRepresentation(members);
    }

    const void* CUniverse::hypothesisToIdentifier(const size_t& hypothesis) const
    {
        return reinterpret_cast<const void*>(&hypothesis);
    }
}
