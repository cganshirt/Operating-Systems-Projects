#include <BoundedBuffer.h>

BoundedBuffer::BoundedBuffer(int N) {
  // TODO: constructor to initiliaze all the varibales declared in
  buffer = (int*) malloc(N * sizeof(int));
  buffer_size = N;
  buffer_cnt = 0;
  buffer_first = 0;
  buffer_last = 0;
  pthread_mutex_init(&buffer_lock, NULL);
  pthread_cond_init(&buffer_not_full, NULL);
  pthread_cond_init(&buffer_not_empty, NULL);
}

BoundedBuffer::~BoundedBuffer() {
  // TODO: destructor to clean up anything necessary
  delete[] buffer;
}

void BoundedBuffer::append(int data) {
  // TODO: append a data item to the circular buffer
  pthread_mutex_lock(&buffer_lock);
  while(buffer_cnt == buffer_size) {
    pthread_cond_wait(&buffer_not_full, &buffer_lock);
  }
  buffer[buffer_last] = data;
  buffer_cnt += 1;
  buffer_last = (buffer_last + 1) % buffer_size;
  pthread_cond_signal(&buffer_not_empty);
  pthread_mutex_unlock(&buffer_lock);
}

int BoundedBuffer::remove() {
  // TODO: remove and return a data item from the circular buffer
  pthread_mutex_lock(&buffer_lock);
  while(isEmpty()) {
    pthread_cond_wait(&buffer_not_empty, &buffer_lock);
  }
  int temp = buffer[buffer_first];
  buffer_first = (buffer_first + 1) % buffer_size;
  buffer_cnt -= 1;
  pthread_cond_signal(&buffer_not_full);
  pthread_mutex_unlock(&buffer_lock);
  return temp;
}

bool BoundedBuffer::isEmpty() {
  // TODO: check is the buffer is empty
  return (buffer_cnt == 0);
}
