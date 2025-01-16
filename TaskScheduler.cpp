//Objective: Design a task scheduler for assigning jobs to Caterpillar equipment efficiently.
#include <iostream>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <map>

// Base class for Jobs
class Job {
public:
    virtual void execute() = 0;
    virtual ~Job() = default;
};

// Derived classes for specific job types
class DiggingJob : public Job {
public:
    void execute() override {
        std::cout << "Executing Digging Job" << std::endl;
    }
};

class HaulingJob : public Job {
public:
    void execute() override {
        std::cout << "Executing Hauling Job" << std::endl;
    }
};

class LiftingJob : public Job {
public:
    void execute() override {
        std::cout << "Executing Lifting Job" << std::endl;
    }
};

class DrillingJob : public Job {
public:
    void execute() override {
        std::cout << "Executing Drilling Job" << std::endl;
    }
};

class PavingJob : public Job {
public:
    void execute() override {
        std::cout << "Executing Paving Job" << std::endl;
    }
};

// Factory Pattern to create jobs
class JobFactory {
public:
    static std::unique_ptr<Job> createJob(int jobType) {
        switch (jobType) {
            case 1: 
            std::cout<<"Scheduled Job: Digging"<<std::endl;
            return std::make_unique<DiggingJob>();
            case 2: 
            std::cout<<"Scheduled Job: Hauling"<<std::endl;
            return std::make_unique<HaulingJob>();
            case 3: 
            std::cout<<"Scheduled Job: Lifting"<<std::endl;
            return std::make_unique<LiftingJob>();
            case 4: 
            std::cout<<"Scheduled Job: Drilling"<<std::endl;
            return std::make_unique<DrillingJob>();
            case 5: 
            std::cout<<"Scheduled Job: Paving"<<std::endl;
            return std::make_unique<PavingJob>();
            default: return nullptr;
        }
    }
};

// Task Queue Management
class TaskQueue {
private:
    std::queue<std::unique_ptr<Job>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop = false;

public:
    void addTask(std::unique_ptr<Job> job) {
        std::lock_guard<std::mutex> lock(mtx);
        tasks.push(std::move(job));
        cv.notify_one();
    }

    std::unique_ptr<Job> getTask() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !tasks.empty() || stop; });
        if (tasks.empty()) return nullptr;
        auto job = std::move(tasks.front());
        tasks.pop();
        return job;
    }

    void stopProcessing() {
        std::lock_guard<std::mutex> lock(mtx);
        stop = true;
        cv.notify_all();
    }
};

// Hierarchical Pattern for task dependencies
class TaskHierarchy {
private:
    std::map<int, std::vector<int>> dependencies;

public:
    void addDependency(int taskId, int dependentTaskId) {
        dependencies[taskId].push_back(dependentTaskId);
    }

    const std::vector<int>& getDependencies(int taskId) const {
        return dependencies.at(taskId);
    }
};

// Concurrency for job execution
void worker(TaskQueue& taskQueue) {
    while (true) {
        auto job = taskQueue.getTask();
        if (!job) break;
        job->execute();
    }
}

int main() {
    TaskQueue taskQueue;
    TaskHierarchy taskHierarchy;

    int choice;
    while (true) {
        std::cout << "Enter job type to add to the queue"<<std::endl<<
        "1: Digging"<<std::endl<<"2: Hauling"<<std::endl<<"3: Lifting"<<std::endl<<
        "4: Drilling"<<std::endl<<"5: Paving"<<std::endl<<"0: Exit: ";
        std::cin >> choice;
        if (choice == 0) break;
        auto job = JobFactory::createJob(choice);
        if (job) {
            taskQueue.addTask(std::move(job));
        } else {
            std::cout << "Invalid job type!" << std::endl;
        }
    }

    // Adding dependencies (example)
    taskHierarchy.addDependency(1, 2);

    // Starting worker threads
    std::thread worker1(worker, std::ref(taskQueue));
    std::thread worker2(worker, std::ref(taskQueue));

    // Stop processing when user exits
    taskQueue.stopProcessing();

    worker1.join();
    worker2.join();

    std::cout << "All the jobs have been scheduled and executed."<<std::endl<<"Exit the program." << std::endl;

    return 0;
}
