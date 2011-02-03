
#include <cstdlib>
#include <ctime>

#include <c9y/utility.h>

#include "Queue.h"
#include "Producer.h"
#include "Consumer.h"

int main()
{
    srand(time(NULL));

    copr::Queue queue;
    copr::Producer producer(queue);
    copr::Consumer consumer(queue);

    producer.start();
    consumer.start();

    // run one minute
    c9y::sleep(60000);

    producer.finish();
    consumer.finish();
}
