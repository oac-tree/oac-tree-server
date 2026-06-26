.. _Application:


Running the Server
==================

The *oac-tree-server* application provides a set of command-line options to configure its behavior and specify the procedures to be served. Below is a description of the available options and an example of how to use them.

CLI Options
-----------

Exactly one of ``-s`` or ``-p`` must be provided to select the network transport.

+ ``-s`` or ``--service``: Starts the server using the **EPICS** transport with the given automation service name.
+ ``-p`` or ``--port``: Starts the server using the **WebSocket** transport on the given TCP port.
+ ``-d`` or ``--dir``: Specifies a directory containing XML files to be parsed and run. All XML files in the directory will be loaded.
+ Positional arguments: Specifies individual XML files to be parsed and run as procedures.
+ ``-h``, ``--help``: Displays the help message with a summary of all available options.


**WebSocket example**

The following command starts the server on port 8080, loads all procedures from a given directory, and adds one further procedure from an explicit path:

.. code-block:: sh

   oac-tree-server -p 8080 -d /path/to/procedures extra.xml

**EPICS example**

.. code-block:: sh

   oac-tree-server -s MyAutomationServer -d "/path/to/procedures" procedure1.xml procedure2.xml
