#include <ProducerConsumer.h>
#include <thread>

// TODO: add BoundedBuffer, locks and any global variables here
BoundedBuffer buffer(5);
int limitp = 0;
int limitc = 0;
pthread_mutex_t prodcon;
pthread_mutex_t consume;
pthread_mutex_t mutex;
ofstream file;
int csleeps;
int psleeps;
int item;
auto start = std::chrono::high_resolution_clock::now();


void InitProducerConsumer(int p, int c, int psleep, int csleep, int items) {
  // TODO: constructor to initialize variables declared
  //       also see instructions on the implementation
  pthread_t prods[p];
  pthread_t cons[c];
  csleeps = csleep;
  psleeps = psleep;
  item = items;
  int i = 0;
  int pids[p];
  int cids[c];

  for(int i = 0; i < p; i++) {
    pids[i] = i;
  }
  for(int i = 0; i < c; i++) {
    cids[i] = i;
  }
  file.open("output.txt");
  while(i < p || i < c) {
    if(i < p) {
      pthread_create(&prods[i], NULL, producer, &(pids[i]));
    }
    if(i < c) {
      pthread_create(&cons[i], NULL, consumer, &(cids[i]));
    }
    i += 1;
  }
  i = 0;
  while(i < p || i < c) {
    if(i < p) {
      pthread_join(prods[i], NULL);
    }
    if(i < c) {
      pthread_join(cons[i], NULL);
    }
    i += 1;
  }
  file.close();
}

void* producer(void* threadID) {
  // TODO: producer thread, see instruction for implementation
  int i = 0;
  int id = *((int*)(threadID));
  while(limitp < item) {
    sleep(.001 * psleeps);
    pthread_mutex_lock(&prodcon);
    if(limitp >= item) {
      pthread_mutex_unlock(&prodcon);
      break;
    }
    buffer.append(i);
    i += 1;
    limitp += 1;
    pthread_mutex_lock(&mutex);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    file << "Producer #" << id << " time = " << elapsed.count() << ", producing data item #" << i << ", item value=" << i << "\n";
    pthread_mutex_unlock(&mutex);
    pthread_mutex_unlock(&prodcon);
  }
  pthread_exit(NULL);
}

void* consumer(void* threadID) {
  // TODO: consumer thread, see instruction for implementation
  int i = 0;
  int id = *((int*)(threadID));
  while(limitc < item) {
    sleep(.001 * csleeps);
    pthread_mutex_lock(&consume);
    if(limitc >= item) {
      pthread_mutex_unlock(&consume);
      break;
    }
    int temp = buffer.remove();
    i += 1;
    limitc += 1;
    pthread_mutex_lock(&mutex);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    file << "Consumer #" << id << " time = " << elapsed.count() << ", consuming data item with value=" << temp << "\n";
    pthread_mutex_unlock(&mutex);
    pthread_mutex_unlock(&consume);
  }
  pthread_exit(NULL);
}
