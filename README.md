# OS-Homework-1
Homework 1 for Operating systems 

The two files in my repository are producer.c and consumer.c.

Producer.c creates shared memory and named semaphores, then produces 10 items into the shared buffer. It uses semaphores to wait if the buffer is full and to ensure mutual exclusion while inserting items.

Consumer.c maps the existing shared memory and semaphores, and consumes 10 items from the buffer. It waits when the buffer is empty and uses mutual exclusion with semaphores.

Compilation instructions:

gcc producer.c -pthread -o producer

gcc consumer.c -pthread -o consumer

./producer & ./consumer
