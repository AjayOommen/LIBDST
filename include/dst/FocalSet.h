#pragma once

#include <bitset>
#include <set>

#define MAX_HYPOTHESESES 64

typedef std::bitset<MAX_HYPOTHESESES> HypothesesSet;
typedef std::set<const void*> VariablesSet;

namespace CPPDST
{
    struct FocalSet
    {
        double m_mass;
        HypothesesSet m_set;
    };
}
