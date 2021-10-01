//
// Created by henri on 01/07/21.
//

#ifndef K_ADAPTABILITY_WJUJ_INSTANCE_H
#define K_ADAPTABILITY_WJUJ_INSTANCE_H

#include <string>
#include <vector>
#include <limits>

struct Job {
    static unsigned int _id_iterator;
    const unsigned int id = _id_iterator++;
    const unsigned int release_date;
    const unsigned int deadline;
    const unsigned int weight;
    const unsigned int processing_time;
    const unsigned int additional_time;
    const unsigned int max_degradation;
    const unsigned int outsourcing_cost;
    Job(unsigned int weight, unsigned int release_date, unsigned int deadline, unsigned int processing_time, unsigned int additional_time, unsigned int max_degradation, unsigned int outsourcing_cost);
    Job(unsigned int id, unsigned int weight, unsigned int release_date, unsigned int deadline, unsigned int processing_time, unsigned int additional_time, unsigned int max_degradation, unsigned int outsourcing_cost);
    friend std::ostream& operator<<(std::ostream& os, const Job& job);
};

struct JobOccurence {
    static unsigned int _id_iterator;
    const Job& parent_job;
    const unsigned int id;
    const unsigned int release_date;
    const unsigned int deadline;
    const unsigned int& parent_job_id;
    const unsigned int& weight;
    const unsigned int& processing_time;
    const unsigned int& additional_time;
    const unsigned int& max_degradation;
    const unsigned int& outsourcing_cost;
    bool is_tardy_occurence;
    JobOccurence(const Job& parent_job, unsigned int release_date, unsigned int  deadline, bool is_tardy);
    JobOccurence(unsigned int id, const Job& parent_job, unsigned int release_date, unsigned int  deadline, bool is_tardy);
    virtual ~JobOccurence();
    friend std::ostream& operator<<(std::ostream& os, const JobOccurence& job);
};

class wjUj_Instance {
    const std::string _instance_filename;
    const float _gamma;
    unsigned int _max_deadline = 0;
    std::vector<Job*> _jobs;
    std::vector<JobOccurence*> _occurences;

    void load_jobs_from_instance();
    void build_occurences_from_jobs();
    void apply_edf_rule();
    wjUj_Instance(const wjUj_Instance& src, bool);
public:
    const std::string& instance_filename() const;
    wjUj_Instance(std::string  filename, float gamma);
    virtual ~wjUj_Instance();
    wjUj_Instance reversed() const;
    const std::vector<Job*>& jobs() const;
    const std::vector<JobOccurence*>& occurences() const;
    unsigned int max_deadline() const;
    double gamma() const;
};


#endif //K_ADAPTABILITY_WJUJ_INSTANCE_H
