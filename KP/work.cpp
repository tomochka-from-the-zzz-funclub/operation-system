#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <set>
#include <vector>
#include <string>
#include <sys/wait.h>
#include <queue>
#include <unordered_set>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

using namespace std;

void working_job(const string& job, const unordered_map<string, vector<string>>& dependencies, unordered_set<string>& visited_jobs) {
    if (visited_jobs.size() == dependencies.size()) { 
        return;
    }

    if (visited_jobs.count(job) == 0) { 
        visited_jobs.insert(job); 
    }

    if (dependencies.count(job) > 0) {
        const vector<string>& current_dependencies = dependencies.at(job); 

        for (const auto& dependency : current_dependencies) {
            if (visited_jobs.count(dependency) == 0) {
                visited_jobs.insert(dependency);
                if (visited_jobs.size() == dependencies.size()) {
                    cout << "Job done: " << dependency << endl;
                    sleep(1);
                } else {
                    working_job(dependency, dependencies, visited_jobs);
                }
            }
        }
    }

    cout << "Job done: " << job << endl;
    sleep(1);
}

bool has_cycle(const YAML::Node& jobs, const string& current_job, set<string>& visited, set<string>& recursion_stack) {
    visited.insert(current_job);
    recursion_stack.insert(current_job);

    const YAML::Node& dependencies = jobs[current_job]["dependencies"];
    for (const auto& dependency : dependencies) {
        const string& dependencyJob = dependency.as<string>();
        if (recursion_stack.count(dependencyJob)) {
            return true;
        }
        if (!visited.count(dependencyJob) && has_cycle(jobs, dependencyJob, visited, recursion_stack)) {
            return true;
        }
    }

    recursion_stack.erase(current_job);
    return false;
}

bool is_valid_DAG(const YAML::Node& jobs) {
    set<string> visited;
    set<string> recursion_stack;

    for (const auto& job : jobs) {
        if (!visited.count(job.first.as<string>()) && has_cycle(jobs, job.first.as<string>(), visited, recursion_stack)) {
            return false;
        }
    }

    return true;
}

bool has_one_component(const YAML::Node& jobs) {
    unordered_map<string, vector<string>> adjacency;
    set<string> visited;

    // Строим список смежности
    for (const auto& job : jobs) {
        const string &jobName = job.first.as<string>();
        const YAML::Node &dependencies = job.second["dependencies"];
        for (const auto& dependency : dependencies) {
            adjacency[dependency.as<string>()].push_back(jobName);
            adjacency[jobName].push_back(dependency.as<string>());
        }
    }

    int component_count = 0;

    for (const auto& job : jobs) {
        const string &job_name = job.first.as<string>();
        if (visited.count(job_name) == 0) {
            queue<string> q;

            q.push(job_name);
            visited.insert(job_name);

            while (!q.empty()) {
                string current_job = q.front();
                q.pop();

                for (const auto& neighbor : adjacency[current_job]) {
                    if (visited.count(neighbor) == 0) {
                        q.push(neighbor);
                        visited.insert(neighbor);
                    }
                }
            }

            component_count++;
        }
    }

    return component_count > 1;
}

bool has_start_end_jobs(const YAML::Node &jobs) {
    set<string> start_jobs;
    set<string> endJobs;
    for (const auto& job : jobs) {
        const string &jobName = job.first.as<string>();
        const YAML::Node &dependencies = job.second["dependencies"];
        if (dependencies.size() == 0) {
            start_jobs.insert(jobName);
        }

        for (const auto& dependency : dependencies) { //если можно прийти в текущую из джоб из списка. то они точно не последние
            endJobs.erase(dependency.as<string>());
        }

        endJobs.insert(jobName); 
    }
    return !start_jobs.empty() && !endJobs.empty();
}

int main(int argc, char* argv[]) {
    string fileName = argv[1];
    ifstream file(fileName);
    YAML::Node data = YAML::Load(file);
    YAML::Node jobs = data["jobs"];

    if (!is_valid_DAG(jobs)) {
        throw runtime_error("DAG contains a cycle");
    }

    if (has_one_component(jobs)) {
        throw runtime_error("DAG has more than one connectivity component");
    }

    if (!has_start_end_jobs(jobs)) {
        throw runtime_error("DAG does not have a start and end job");
    }

    cout << "DAG is valid" << endl;
    sleep(1);

    int n = jobs.size();
    vector<string> job_names;
    for (const auto& job : jobs) {
        job_names.push_back(job.first.as<string>());
    }

    unordered_map<string, vector<string>> dependencies_map;
    unordered_set<string> visitedJobs;

    for (const auto& job : jobs) {
        const string &job_names = job.first.as<string>();
        vector<string> dependencies;
        for (const auto& dependency : job.second["dependencies"]) {
            dependencies.push_back(dependency.as<string>());
        }
        dependencies_map[job_names] = dependencies;
    }

    for (const string& job : job_names) {
        working_job(job, dependencies_map, visitedJobs);
    }
    
    cout << "All jobs done" << endl;
    return 0;
}