.. _WebSocket API:


WebSocket API
=============

General considerations
----------------------

The WebSocket API uses different endpoints. For a server hosted at ``<HOST>`` and port ``<PORT>``:

* For each job there is an endpoint ``ws://<HOST>:<PORT>/job/<INDEX>``, where ``<INDEX>`` is the job index (0 to N-1). Clients connecting to this endpoint receive update messages:

  * Status updates of job, instructions, or variables.
  * Generic messages to be displayed to the user (e.g. for logging).

* A single endpoint ``ws://<HOST>:<PORT>/info`` that handles:

  * Information queries about the server: how many jobs are hosted and what is the static structure of each job.
  * Messages that control job execution: start, stop, pause, resume, set breakpoint, etc.

All messages are JSON-encoded.

Server-side publishing of job updates
--------------------------------------

Updates are published on the job-specific endpoint and are sent to all connected clients. Requests for user input are handled in the same way, except that the client is expected to send a reply back; the WebSocket server is not blocked while waiting.

An outstanding user-input request is re-published periodically until it is answered or interrupted. Every re-publication carries the same ``req_id``, so clients must treat a repeated ``req_id`` as the same request. Once a request has been answered or cancelled, the server ignores any further replies for that ``req_id``.

The ``update`` field holds an integer that describes the type of the update message:

.. list-table::
   :header-rows: 1

   * - Value
     - Message type
   * - 0
     - job state update
   * - 1
     - current breakpoint
   * - 2
     - instruction state update
   * - 3
     - variable state update
   * - 4
     - display generic value
   * - 5
     - display message
   * - 6
     - log message
   * - 7
     - get user value
   * - 8
     - get user choice
   * - 9
     - user input cancelled
   * - 10
     - job snapshot

Job state update
~~~~~~~~~~~~~~~~

.. code-block:: json

   {
     "update": 0,
     "state": 3
   }

The ``state`` field encodes the job state:

.. list-table::
   :header-rows: 1

   * - Value
     - State
   * - 0
     - initial
   * - 1
     - paused
   * - 2
     - stepping
   * - 3
     - running
   * - 4
     - succeeded
   * - 5
     - failed
   * - 6
     - halted

Each job starts in the ``initial`` state. ``stepping`` means a single instruction is executing; the state returns to ``paused`` once that instruction finishes. ``running`` indicates continuous execution. The last three states are final.

Current breakpoint
~~~~~~~~~~~~~~~~~~

.. code-block:: json

   {
     "update": 1,
     "id": 8
   }

The ``id`` field is the index of the instruction that currently hit a breakpoint, or the maximum value of a 32-bit unsigned integer when no breakpoint is currently hit.

Instruction state update
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: json

   {
     "update": 2,
     "id": 5,
     "breakpoint": false,
     "state": 3
   }

``id`` is the instruction index. ``breakpoint`` is ``true`` when an active breakpoint is set on this instruction (not necessarily hit). ``state`` encodes the execution state:

.. list-table::
   :header-rows: 1

   * - Value
     - State
   * - 0
     - initial / not started
   * - 1
     - started and waiting to be ticked
   * - 2
     - running asynchronously
   * - 3
     - finished with success
   * - 4
     - finished with failure
   * - 5
     - halted

Variable updated
~~~~~~~~~~~~~~~~

.. code-block:: json

   {
     "update": 3,
     "id": 5,
     "connected": true,
     "value": "<JSON rep>",
     "type": "<JSON rep>"
   }

``id`` is the variable index. ``connected`` is ``false`` for disconnected network variables or to signal an error condition. ``value`` and ``type`` are strings holding the JSON representation of the value and its type respectively.

Display generic value
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: json

   {
     "update": 4,
     "description": "Please note this value:",
     "value": "<JSON rep>",
     "type": "<JSON rep>"
   }

``value`` and ``type`` are encoded the same way as for *variable updated*.

Display message
~~~~~~~~~~~~~~~

.. code-block:: json

   {
     "update": 5,
     "message": "Hello, there"
   }

Log message
~~~~~~~~~~~

.. code-block:: json

   {
     "update": 6,
     "severity": 2,
     "message": "Something went wrong!"
   }

``severity`` follows the syslog severity levels:

.. list-table::
   :header-rows: 1

   * - Value
     - Severity
   * - 0
     - Emergency
   * - 1
     - Alert
   * - 2
     - Critical
   * - 3
     - Error
   * - 4
     - Warning
   * - 5
     - Notification
   * - 6
     - Informational
   * - 7
     - Debug
   * - 8
     - Trace

Get user value
~~~~~~~~~~~~~~

Message to client:

.. code-block:: json

   {
     "update": 7,
     "req_id": 33,
     "description": "Please provide your name",
     "value": "<JSON rep>",
     "type": "<JSON rep>"
   }

``value`` and ``type`` encode the default value for the user.

Reply from client:

.. code-block:: json

   {
     "update": 7,
     "req_id": 33,
     "ok": true,
     "value": "<JSON rep>",
     "type": "<JSON rep>"
   }

``ok`` is ``false`` if the user cancelled; this is still a valid response and the request is considered finished.

Get user choice
~~~~~~~~~~~~~~~

Message to client:

.. code-block:: json

   {
     "update": 8,
     "req_id": 34,
     "choices": ["normal", "degraded", "fault"],
     "value": "<JSON rep>",
     "type": "<JSON rep>"
   }

``value`` and ``type`` allow user interfaces to customise the display (e.g. for OK/Cancel dialogs).

Reply from client:

.. code-block:: json

   {
     "update": 8,
     "req_id": 34,
     "choice": 1
   }

``choice`` is the index of the selected option, or ``-1`` if the user cancelled.

User input cancelled
~~~~~~~~~~~~~~~~~~~~

.. code-block:: json

   {
     "update": 9,
     "req_id": 34
   }

Signals that the request with the given ``req_id`` is cancelled and no longer expects a reply. Clients showing a dialog for this ``req_id`` should dismiss it. There is no reply.

Job snapshot
~~~~~~~~~~~~

When a client connects to a job endpoint while the job is already running it would otherwise miss all state changes that occurred before it connected. To address this, the server sends a single snapshot message to each client immediately after it connects. The snapshot conveys the full current state: job state, current breakpoint, and the latest state of every instruction and variable.

.. code-block:: json

   {
     "update": 10,
     "state": 3,
     "id": 8,
     "instructions": [
       { "id": 0, "breakpoint": false, "state": 3 },
       { "id": 1, "breakpoint": true,  "state": 1 }
     ],
     "variables": [
       { "id": 0, "connected": true,  "value": "<JSON rep>", "type": "<JSON rep>" },
       { "id": 1, "connected": false, "value": "<JSON rep>", "type": "<JSON rep>" }
     ]
   }

``state`` and ``id`` are encoded as in the *job state update* and *current breakpoint* messages. The ``instructions`` and ``variables`` arrays are encoded as in *instruction state update* and *variable updated* respectively. Instructions and variables that have not been updated yet appear with their default state (instructions: not started, no breakpoint; variables: not connected, empty value and type).

Because a client may receive live update messages before or interleaved with the snapshot, it must treat the snapshot only as a baseline and give precedence to update messages. This guarantees the client never misses a state change, while tolerating duplicate or out-of-order delivery.

Client-side requests
---------------------

Client requests are handled on the ``info`` endpoint and are always one-to-one — the server replies only to the requesting client.

Client messages contain an integer ``request`` field:

.. list-table::
   :header-rows: 1

   * - Value
     - Request type
   * - 0
     - get number of jobs
   * - 1
     - get static job info
   * - 2
     - edit breakpoint
   * - 3
     - send job command

Get number of jobs
~~~~~~~~~~~~~~~~~~

Message to server:

.. code-block:: json

   {
     "request": 0,
     "req_id": 42
   }

``req_id`` is a numeric id used to associate the reply with the request.

Reply from server:

.. code-block:: json

   {
     "request": 0,
     "req_id": 42,
     "n_jobs": 3
   }

Get static job information
~~~~~~~~~~~~~~~~~~~~~~~~~~

Message to server:

.. code-block:: json

   {
     "request": 1,
     "req_id": 42,
     "job_index": 2
   }

Reply from server:

.. code-block:: json

   {
     "request": 1,
     "req_id": 42,
     "job_index": 2,
     "value": "<JSON value rep>",
     "type": "<JSON type rep>"
   }

The ``value`` field is a JSON-encoded representation of the static job information. As an example, the following procedure

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <Procedure xmlns="http://codac.iter.org/sup/oac-tree" version="1.0"
              name="Common header"
              xmlns:xs="http://www.w3.org/2001/XMLSchema-instance"
              xs:schemaLocation="http://codac.iter.org/sup/oac-tree oac-tree.xsd">
     <Sequence>
       <Message text="Hello"/>
       <Output fromVar="var1" description="Variable is"/>
       <Copy inputVar="one" outputVar="var1"/>
       <Copy inputVar="one" outputVar="var2"/>
     </Sequence>
     <Workspace>
       <Local name="one"  type='{"type":"uint32"}' value='1'/>
       <Local name="var1" type='{"type":"uint32"}' value='0'/>
       <Local name="var2" type='{"type":"uint32"}' value='0'/>
     </Workspace>
   </Procedure>

would produce the following ``value`` (shown with indentation for readability):

.. code-block:: json

   {
     "full_name": "Common header",
     "ws_info": {
       "one":  { "type": "Local", "index": 0, "attributes": { "name": "one",  "type": "{\"type\":\"uint32\"}", "value": "1" } },
       "var1": { "type": "Local", "index": 1, "attributes": { "name": "var1", "type": "{\"type\":\"uint32\"}", "value": "0" } },
       "var2": { "type": "Local", "index": 2, "attributes": { "name": "var2", "type": "{\"type\":\"uint32\"}", "value": "0" } }
     },
     "instr_tree_info": {
       "type": "Sequence", "category": 2, "index": 0, "attributes": {},
       "children": {
         "child_0": { "type": "Message", "category": 0, "index": 1, "attributes": { "text": "Hello" } },
         "child_1": { "type": "Output",  "category": 0, "index": 2, "attributes": { "fromVar": "var1", "description": "Variable is" } },
         "child_2": { "type": "Copy",    "category": 0, "index": 3, "attributes": { "inputVar": "one", "outputVar": "var1" } },
         "child_3": { "type": "Copy",    "category": 0, "index": 4, "attributes": { "inputVar": "one", "outputVar": "var2" } }
       }
     }
   }

Edit breakpoint
~~~~~~~~~~~~~~~

Message to server:

.. code-block:: json

   {
     "request": 2,
     "job_index": 2,
     "instr_id": 5,
     "active": true
   }

``active`` is ``true`` to set the breakpoint or ``false`` to clear it. There is no reply; the client will receive an instruction state update if the action was successful.

Send job command
~~~~~~~~~~~~~~~~

Message to server:

.. code-block:: json

   {
     "request": 3,
     "job_index": 2,
     "command": 0
   }

``command`` encodes the requested action:

.. list-table::
   :header-rows: 1

   * - Value
     - Command
   * - 0
     - Start
   * - 1
     - Step
   * - 2
     - Pause
   * - 3
     - Reset
   * - 4
     - Halt

There is no reply; the client will receive a job state update if the command was handled successfully.
