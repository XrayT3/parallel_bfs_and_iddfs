#include "bfs.h"
#include <iostream>
#include <set>
#include <bits/stdc++.h> 
#include <omp.h>
#include <vector>

std::shared_ptr<const state> bfs(std::shared_ptr<const state> root) {

    if (root->is_goal())
        return root;

    std::unordered_set<unsigned long long> visited;
    std::vector< std::shared_ptr<const state> >results;
    std::shared_ptr<std::vector<std::shared_ptr<const state>>> q_ptr(new std::vector<std::shared_ptr<const state>>);
    std::shared_ptr<std::vector<std::shared_ptr<const state>>> q_next_ptr(new std::vector<std::shared_ptr<const state>>);

    q_ptr->push_back(root);
    int count_q = 1;
    while(count_q && results.empty()){
        #pragma omp parallel for
        for(int i = 0; i < count_q; i++){
            std::vector<std::shared_ptr<const state>> next_s = q_ptr->at(i)->next_states();
            for(std::shared_ptr<const state> childe : next_s)
                if(visited.find(childe->get_identifier()) == visited.end()){
                    #pragma omp critical
                    {
                        visited.insert(childe->get_identifier());
                        if(childe->is_goal()) results.push_back(childe);
                        q_next_ptr->push_back(childe);
                    }
                }
        }
        count_q = q_next_ptr->size();
        q_ptr.swap(q_next_ptr);
        q_next_ptr->clear();
    }

    if(results.empty())
        return root;
    std::shared_ptr<const state> best_goal = results[0];
    for(const std::shared_ptr<const state> &result : results){
        if(result->get_identifier() < best_goal->get_identifier())
            best_goal = result;
    }
    return best_goal;
}
