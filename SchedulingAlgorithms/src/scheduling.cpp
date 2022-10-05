#include <scheduling.h>
#include <fstream>
#include <iostream>
#include <list>
#include <queue>
#include <string>

using namespace std;

pqueue_arrival read_workload(string filename) {
  pqueue_arrival workload;
  std::ifstream infile(filename);
  int arrival, duration;
  while(infile >> arrival >> duration) {
    Process pro;
    pro.arrival = arrival;
    pro.duration = duration;
    workload.push(pro);
  }
  return workload;
}

void show_workload(pqueue_arrival workload) {
  pqueue_arrival xs = workload;
  cout << "Workload:" << endl;
  while (!xs.empty()) {
    Process p = xs.top();
    cout << '\t' << p.arrival << ' ' << p.duration << endl;
    xs.pop();
  }
}

void show_processes(list<Process> processes) {
  list<Process> xs = processes;
  cout << "Processes:" << endl;
  while (!xs.empty()) {
    Process p = xs.front();
    cout << "\tarrival=" << p.arrival << ", duration=" << p.duration
         << ", first_run=" << p.first_run << ", completion=" << p.completion
         << endl;
    xs.pop_front();
  }
}

list<Process> fifo(pqueue_arrival workload) {
  int time = 0;
  list<Process> complete;
  while(!workload.empty()) {
    Process pro = workload.top();
    pro.first_run = time;
    time += pro.duration;
    pro.completion = time;
    complete.push_back(pro);
    workload.pop();
  }
  return complete;
}

list<Process> sjf(pqueue_arrival workload) {
  list<Process> complete;
  pqueue_duration arrived;
  int time = 0;
  while(!workload.empty() || !arrived.empty()) {
    while(!workload.empty()) {
      Process pro = workload.top();
      if(pro.arrival <= time) {
        arrived.push(pro);
        workload.pop();
      } else {
        break;
      }
    }
    Process pro = arrived.top();
    pro.first_run = time;
    time += pro.duration;
    pro.completion = time;
    complete.push_back(pro);
    arrived.pop();
  }
  return complete;
}

list<Process> stcf(pqueue_arrival workload) {
  list<Process> complete;
  pqueue_duration arrived;
  int time = 0;
  while(!workload.empty() || !arrived.empty()) {
    while(!workload.empty()) {
      Process pro = workload.top();
      if(pro.arrival <= time) {
        pro.first_run = -1;
        arrived.push(pro);
        workload.pop();
      } else {
        break;
      }
    }
    Process pro = arrived.top();
    if(pro.first_run == -1) {
      pro.first_run = time;
    }
    time += 1;
    pro.duration -= 1;
    arrived.pop();
    arrived.push(pro);
    if(pro.duration != 0) {
      while(!workload.empty()) {
        Process pro = workload.top();
        if(pro.arrival <= time) {
          arrived.push(pro);
          workload.pop();
        } else {
          break;
        }
      }
      continue;
    } else {
      pro.completion = time;
      complete.push_back(pro);
      arrived.pop();
    }
  }
  return complete;
}

list<Process> rr(pqueue_arrival workload) {
  list<Process> complete;
  queue<Process> arrived;
  int time = 0;
  while(!workload.empty() || !arrived.empty()) {
    while(!workload.empty()) {
      Process pro = workload.top();
      if(pro.arrival <= time) {
        pro.first_run = -1;
        arrived.push(pro);
        workload.pop();
      } else {
        break;
      }
    }
    Process pro = arrived.front();
    if(pro.first_run == -1) {
      pro.first_run = time;
    }
    time += 1;
    pro.duration -= 1;
    if(pro.duration == 0) {
      pro.completion = time;
      complete.push_back(pro);
      arrived.pop();
    } else {
      arrived.pop(); 
      arrived.push(pro);
    }
  }
  return complete;
}

float avg_turnaround(list<Process> processes) {
  float tot = 0.0;
  for(Process pro : processes) {
    tot += pro.completion - pro.arrival;
  }
  return tot / processes.size();
}

float avg_response(list<Process> processes) {
  float tot = 0.0;
  for(Process pro : processes) {
    tot += pro.first_run - pro.arrival;
  }
  return tot / processes.size();
}

void show_metrics(list<Process> processes) {
  float avg_t = avg_turnaround(processes);
  float avg_r = avg_response(processes);
  show_processes(processes);
  cout << '\n';
  cout << "Average Turnaround Time: " << avg_t << endl;
  cout << "Average Response Time:   " << avg_r << endl;
}
