#include <sys/time.h>
#include <time.h>

struct timespec tsAdd(struct timespec ts1, struct timespec ts2);
struct timespec tsSub(struct timespec ts1, struct timespec ts2);
struct timespec tsDiv(struct timespec ts, int i);
int tsComp(struct timespec ts1, struct timespec ts2);
