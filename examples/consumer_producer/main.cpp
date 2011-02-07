//
// c9y - concurrency
// Copyright 2011 Sean Farell
//
// This file is part of c9y.
//
// c9y is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// c9y is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with c9y. If not, see <http://www.gnu.org/licenses/>.
//

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
