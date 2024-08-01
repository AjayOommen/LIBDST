#pragma once

#include <list>

#include "FocalSet.h"

namespace CPPDST
{
    class CUniverse;

    class CEvidence
    {
    public:
        void addFocalSet(double mass, const HypothesesSet& members);
        void addFocalSet(double mass, const VariablesSet& members);
        void addFocalSet(double mass, const void* member, ...);
        void addFocalSet(double mass, const size_t& member);
        void addOmegaSet();

        double conflict(const CEvidence& other);

        double belief(const HypothesesSet& members);
        double belief(const VariablesSet& members);
        double belief(const void* member, ...);
        double belief(const size_t& member);

        double plausability(const HypothesesSet& members);
        double plausability(const VariablesSet& members);
        double plausability(void* member, ...);

        const void* mostBelievable();
        const void* mostPlausible();
        const void* bestMatch();

        CEvidence operator&(const CEvidence& other);

    private:
        CEvidence(const CUniverse* universe);
        void addFocalSet(const FocalSet& set);

        const CUniverse* m_universe;
        std::list<FocalSet> m_focalSets;

    friend class CUniverse;
    };
}
