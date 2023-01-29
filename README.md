# Homework3

## Task1 & Task3:

We took into account from the beginning that the connection between process 0 and process 1 is canceled.
At the beginning, each coordinator reads its own topology from the file assigned to it.
Process 0 will send its topology to process 3, process 3 will send to process 2
the topology of process 0 and its own topology. Similarly process 2 will send the 3 topologies
(his and the 2 received from process 3) to
process 1. When process 1 receives the three topologies, it will send it back to process 2
its topology. Process 2 will send its topology and the topology of process 1 to process 3. Analogous to process 3
will send the 3 topologies to process 0, and at this moment all the coordinators will know all the topology of the structure,
following that then each coordinator should inform it's workers and send them the final topology.

## Task2

Process 0 generates the vector, and then calculates the start and end positions for each worker.
It sends the entire vector to all its workers and waits until it receives the calculations made by the workers.
The next step is to send the modified vector following the calculations made by the workers to process 3.
Process 3 executes the same steps and sends the vector modified by both its workers and the workers
of process 0. The same steps are executed by process 2, and when the workers of process 1 finish performing the calculations
process 1 will have the vector in its final form after performing all the calculations. Process 1 will send the vector
in the final form to process 0 through processes 2 and 3, and when process 0 receives the vector it displays it.
