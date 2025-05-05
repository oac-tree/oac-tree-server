.. _Introduction:


============
Introduction
============

The *oac-tree-server* is a framework designed to serve and manage *oac-tree* procedures. It acts as the backbone for executing, monitoring, and controlling *oac-tree* jobs, providing a robust infrastructure for automation and operational efficiency.

Key Features of *oac-tree-server*
=================================

+ **Serving** *oac-tree* **Procedures**:

  - The server is responsible for hosting and executing *oac-tree* procedures.

  - Procedures are encapsulated as jobs, each with its own workspace, instruction trees, and execution flow.

+ **Job Management**:

  - Each procedure is treated as a *job*, with a unique identifier and associated resources.

  - The server manages multiple jobs simultaneously, ensuring efficient resource utilization.

+ **Information Servers**:

  The *oac-tree-server* provides *information servers* that allow clients to query details about the jobs being served.

  Information includes:

  1. Job metadata (e.g., name, number of instructions, variables).
  2. Current job state (e.g., running, paused, halted).
  3. Instruction statuses and variable values.

+ **Control Servers**:
  
  In addition to information servers, the *oac-tree-server* provides *control servers* for interacting with jobs.

  Clients can:

  1. Start, pause, reset, or halt jobs.
  2. Set or remove breakpoints on specific instructions.

+ **EPICS Integration**:

  The server integrates with EPICS (Experimental Physics and Industrial Control System) for communication.

  It uses EPICS PVs (Process Variables) to publish job states, instruction statuses, and variable values.

Architecture of *oac-tree-server*
=================================

1. **Job Execution**:
   
   - Each job is executed based on its *oac-tree* procedure definition.
   - The server ensures deterministic execution and provides feedback on the job's progress.

2. **Information Flow**:
   
   - The server uses *information servers* to expose job-related data.
   - Clients can subscribe to updates or query specific details.

3. **Control Flow**:
   
   - *Control servers* handle commands from clients to manage job execution.
   - Commands include starting, pausing, or resetting jobs, as well as modifying breakpoints.

4. **Input Handling**:
   - The server supports user input requests during job execution.
   - Input requests are published via dedicated input servers, and clients can respond with the required data.

Example: Serving a Procedure
============================

Below is an example of how the *oac-tree-server* serves a procedure:

.. code-block:: xml
                
  <Procedure xmlns="http://codac.iter.org/sup/oac-tree" version="1.0"
             name="example-procedure">
    <Plugin>liboac-tree-ca.so</Plugin>
    <Sequence name="MainSequence">
      <Wait timeout="1.0"/>
      <Include name="SubProcedure" path="SubProcedure"/>
    </Sequence>
    <Workspace>
      <Local name="counter" type='{"type":"uint32"}' value="0"/>
    </Workspace>
  </Procedure>

The server will:

1. Load the procedure and create a job.
2. Expose job-related information via an information server.
3. Allow clients to control the job via a control server.

Benefits of *oac-tree-server*
=============================

+ Centralized management of *oac-tree* procedures.
+ Real-time access to job information and statuses.
+ Modular and extensible architecture for serving multiple jobs.
+ Seamless integration with EPICS for industrial control systems.

In summary, the *oac-tree-server* is a powerful framework for serving and managing *oac-tree* procedures, providing both information and control interfaces to ensure efficient and reliable automation.

