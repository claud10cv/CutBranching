#include "jlwrapper.h"
#include "branch_and_reduce_algorithm.h"
#include <vector>

static void max_indep_set_cb(jlcxx::ArrayRef<int> _src, jlcxx::ArrayRef<int> _dst, jlcxx::ArrayRef<int> _sol)
{
    branch_and_reduce_algorithm::BRANCHING = 2;
    branch_and_reduce_algorithm::USE_DEPENDENCY_CHECKING = true;

    int nnodes = std::max(*std::max_element(std::begin(_src), std::end(_src)), *std::max_element(std::begin(_dst), std::end(_dst)));
    int nedges = (int) _src.size();
    std::vector<std::vector<int> > adj(nnodes, std::vector<int>());
    for(int e = 0; e < nedges; ++ e)
    {
        int i = _src[e] - 1;
        int j = _dst[e] - 1;
        adj[i].push_back(j);
        adj[j].push_back(i);
    }
    
    branch_and_reduce_algorithm::resetStatistics();
    branch_and_reduce_algorithm algo = branch_and_reduce_algorithm(adj, nnodes);
    timer t;
    int opt = algo.solve(t, 86400);
    std::vector<bool> mis(nnodes, false);
    algo.get_solved_is(mis);
    for(int i = 0; i < (int)mis.size(); ++ i) _sol[i] = mis[i];
    return;
}
JLCXX_MODULE define_julia_module(jlcxx::Module& mod)
{
  mod.method("max_indep_set_cb", &max_indep_set_cb);
}
