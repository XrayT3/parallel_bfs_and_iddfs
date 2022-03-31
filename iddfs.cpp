#include "iddfs.h"
#include <iostream>
#include <set>
#include <bits/stdc++.h> 
#include <omp.h>
#include <vector>

std::vector< std::shared_ptr<const state> > results;

void dfs(std::shared_ptr<const state> &node, int max_depth){
    if(node->is_goal()){
        #pragma omp critical
        {
            results.push_back(node);
        }
        return;
    }
    if(max_depth <= 0)
        return;

    for(auto &adjacent : node->next_states()){
        dfs(adjacent, max_depth-1);
    }
}

std::shared_ptr<const state> iddfs(std::shared_ptr<const state> root) {

    std::unordered_set<unsigned long int> visited;
    std::vector<std::shared_ptr<const state>> q_ptr;
    std::vector<std::shared_ptr<const state>> q_next_ptr;

    q_ptr.push_back(root);
    while(visited.size() < 300000 && results.empty()){
        #pragma omp parallel for
        for(int i = 0; i < q_ptr.size(); i++){
            std::vector<std::shared_ptr<const state>> next_s = q_ptr[i]->next_states();
            for(std::shared_ptr<const state> &childe : next_s){
                if(visited.find(childe->get_identifier()) == visited.end()){
                    #pragma omp critical
                    {
                        visited.insert(childe->get_identifier());
                        if(childe->is_goal()) results.push_back(childe);
                        q_next_ptr.push_back(childe);
                    }
                }
            }
        }
        q_ptr.swap(q_next_ptr);
        q_next_ptr.clear();
    }
    int i = 1;
    while(results.empty()){
        #pragma omp parallel for
        for(int idx = 0; idx < q_ptr.size(); idx++){
            dfs(q_ptr[idx], i);
        }
        i++;
    }

    std::shared_ptr<const state> best_result = results[0];
    for(const auto &result : results){
        if(result->current_cost() < best_result->current_cost() ||
            (result->current_cost() == best_result->current_cost() && result->get_identifier() < best_result->get_identifier()))
            best_result = result;
    }
    return best_result;
}
