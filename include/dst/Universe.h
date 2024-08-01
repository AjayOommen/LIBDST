#pragma once

#include "FocalSet.h"
#include "Evidence.h"

namespace CPPDST
{
    class CUniverse
    {
    public:
        CUniverse();

        void addHypotheseses(const VariablesSet& hypotheseses);
        void addHypotheseses(const void* hypothesis, ...);
        void addHypotheseses(const size_t& hypothesis);
        CEvidence addEvidence() const;

        HypothesesSet bitsetRepresentation(const VariablesSet& members) const;
        HypothesesSet bitsetRepresentation(const void* member, ...) const;

        const void* hypothesisToIdentifier(const size_t& hypothesis) const;

    private:
        const void* m_hypotheseses[MAX_HYPOTHESESES];
        int m_lastHypothesisNumber;

    friend class CEvidence;
    };
}
