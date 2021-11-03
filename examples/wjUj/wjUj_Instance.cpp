//
// Created by henri on 01/07/21.
//

#include "wjUj_Instance.h"

#include <utility>
#include <iostream>
#include <algorithm>
#include <functional>
#include <fstream>
#include <sstream>

unsigned int Job::_id_iterator = 0;
unsigned int JobOccurence::_id_iterator = 0;

Job::Job(unsigned int weight, unsigned int release_date, unsigned int deadline, unsigned int processing_time,
         unsigned int additional_time, unsigned int max_degradation, unsigned int outsourcing_cost)
        : Job(_id_iterator++, weight, release_date, deadline, processing_time, additional_time, max_degradation, outsourcing_cost)
{}

std::ostream &operator<<(std::ostream& os, const Job& job) {
    return os << "job " << job.id << " [r = " << job.release_date << ", d = " << job.deadline << "], p = " << job.processing_time
              << "(" << job.additional_time << "), w = " << job.weight << "(" << job.max_degradation << "), f = " << job.outsourcing_cost;
}

Job::Job(unsigned int id, unsigned int weight, unsigned int release_date, unsigned int deadline,
         unsigned int processing_time, unsigned int additional_time, unsigned int max_degradation,
         unsigned int outsourcing_cost) :
        weight(weight), release_date(release_date), deadline(deadline), processing_time(processing_time),
        additional_time(additional_time), max_degradation(max_degradation), outsourcing_cost(outsourcing_cost), id(id)
{}

JobOccurence::JobOccurence(const Job &parent_job, unsigned int release_date, unsigned int deadline, bool is_tardy)
        : JobOccurence(_id_iterator++, parent_job, release_date, deadline, is_tardy)
{}

JobOccurence::JobOccurence(unsigned int id, const Job &parent_job, unsigned int release_date, unsigned int deadline, bool is_tardy)
        : id(id), parent_job(parent_job), parent_job_id(parent_job.id), weight(is_tardy ? parent_job.weight : *new unsigned int(0)), release_date(release_date),
          deadline(deadline), processing_time(is_tardy ? *new unsigned int(0) : parent_job.processing_time), additional_time(is_tardy ? *new unsigned int(0) :parent_job.additional_time),
          max_degradation(is_tardy ? *new unsigned int(0) : parent_job.max_degradation), outsourcing_cost(is_tardy ? *new unsigned int(0) : parent_job.outsourcing_cost), is_tardy_occurence(is_tardy)
{}

JobOccurence::~JobOccurence() {
    if (is_tardy_occurence) {
        delete &processing_time;
        delete &max_degradation;
        delete &additional_time;
        delete &outsourcing_cost;
    }
    else delete &weight;
}

std::ostream &operator<<(std::ostream &os, const JobOccurence &job) {
    return os << "occurence " << job.id << " from job " << job.parent_job_id << " [r = " << job.release_date << ", d = " << job.deadline << "], p = " << job.processing_time
              << "(" << job.additional_time << "), w = " << job.weight << "(" << job.max_degradation << "), f = " << job.outsourcing_cost << " ["
              << (job.is_tardy_occurence ? "TARDY" : "ON_TIME") << "] ";
}

const std::string &wjUj_Instance::instance_filename() const { return _instance_filename; }

wjUj_Instance::wjUj_Instance(std::string filename, float gamma) : _instance_filename(std::move(filename)), _gamma(gamma) {
    load_jobs_from_instance();
    build_occurences_from_jobs();
    apply_edf_rule();
}

void read_file_by_line(const std::string& t_filename, const std::function<void(const std::vector<std::string>&)>& t_cb) {
    std::ifstream file(t_filename);

    if (!file.is_open()) {
        throw std::runtime_error("could not open file");
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string word;
        std::stringstream ss(line);
        std::vector<std::string> words;
        while (std::getline(ss, word, ' ')) {
            words.emplace_back(word);
        }
        t_cb(words);
    }

    file.close();
}

void wjUj_Instance::load_jobs_from_instance() {
    enum COLUMNS { WEIGHT, RELEASE_DATE, DEADLINE, PROCESSING_TIME, ADDITIONAL_TIME, MAX_DEGRADATION, OUTSOURCING_COST };

    read_file_by_line(_instance_filename, [this](const std::vector<std::string>& line){
        if (line.empty()) return;
        if (line.size() == 1) {
            if (!_jobs.empty()) throw std::runtime_error("could not parse file");
        } else if (line.size() != 7) {
            throw std::runtime_error("could not parse file");
        } else {
            auto job = new Job(std::stoi(line[WEIGHT]), std::stoi(line[RELEASE_DATE]),
                               std::stoi(line[DEADLINE]), std::stoi(line[PROCESSING_TIME]),
                               std::stoi(line[ADDITIONAL_TIME]), std::stoi(line[MAX_DEGRADATION]),
                               std::stoi(line[OUTSOURCING_COST]));
            _jobs.emplace_back(job);
            //std::cout << "Created " << *job << std::endl;
        }
    });
}

void wjUj_Instance::build_occurences_from_jobs() {
    std::vector<Job*> jobs_copied = _jobs;
    sort(jobs_copied.begin(), jobs_copied.end(), [](const Job* A, const Job* B) { return A->release_date < B->release_date; });

    for (unsigned long int i = 0, n_jobs = jobs_copied.size() ; i < n_jobs ; i += 1) {
        const Job& job_i = *jobs_copied[i];
        if (_max_deadline < job_i.deadline) _max_deadline = job_i.deadline;

        for (unsigned long int j = i + 1 ; j < n_jobs ; j += 1) {
            const Job& job_j = *jobs_copied[j];

            if (job_i.deadline > job_j.deadline && job_i.release_date + job_i.processing_time + job_j.processing_time < job_j.deadline) {
                const unsigned int from = job_i.release_date;
                const unsigned int to = job_j.deadline;
                auto new_occurence = new JobOccurence(job_i, from, to, false);
                _occurences.emplace_back(new_occurence);
                //std::cout << "Created " << *new_occurence << std::endl;
            } else if (job_i.deadline <= job_j.release_date) {
                break;
            }

        }

        auto new_occurence = new JobOccurence(job_i, job_i.release_date, job_i.deadline, false);
        _occurences.emplace_back(new_occurence);
        //std::cout << "Created " << *new_occurence << std::endl;

        new_occurence = new JobOccurence(job_i, 0, 0, true);
        _occurences.emplace_back(new_occurence);
        //std::cout << "Created " << *new_occurence << std::endl;
    }
}

void wjUj_Instance::apply_edf_rule() {
    std::sort(_occurences.begin(), _occurences.end(), [](const JobOccurence* A, const JobOccurence* B){
        return std::pair<int, int>(A->deadline, A->release_date) < std::pair<int, int>(B->deadline, B->release_date);
    });
}

wjUj_Instance::~wjUj_Instance() {
    for (const Job* job : _jobs) delete job;
    for (const JobOccurence* occ : _occurences) delete occ;
}

wjUj_Instance wjUj_Instance::reversed() const {
    return wjUj_Instance(*this, true);
}

wjUj_Instance::wjUj_Instance(const wjUj_Instance &src, bool)
        : _gamma(src._gamma), _instance_filename(src._instance_filename)
{
    std::cout << "Building reversed instance..." << std::endl;

    const unsigned int dmax = src._max_deadline;

    for (const Job* job : src._jobs) {
        auto& new_job = *new Job(job->id, job->weight, dmax - job->deadline,dmax - job->release_date, job->processing_time, job->additional_time, job->max_degradation, job->outsourcing_cost);
        std::cout << "Reversed " << new_job << std::endl;
        _jobs.emplace_back(&new_job);
    }

    unsigned long id = 0;
    for (const JobOccurence* job_occ : src._occurences) {
        auto& new_occ = *new JobOccurence(id++, job_occ->parent_job, dmax - job_occ->deadline, dmax - job_occ->release_date, job_occ->is_tardy_occurence);
        std::cout << "Reversed " << new_occ << std::endl;
        _occurences.emplace_back(&new_occ);
    }

    _max_deadline = 0;
    for (const auto occ : _occurences)
        if (_max_deadline < occ->deadline) _max_deadline = occ->deadline;
}

const std::vector<Job *>& wjUj_Instance::jobs() const {
    return _jobs;
}

const std::vector<JobOccurence *>& wjUj_Instance::occurences() const {
    return _occurences;
}

unsigned int wjUj_Instance::max_deadline() const {
    return _max_deadline;
}

double wjUj_Instance::gamma() const {
    return _gamma;
}
