.. _Application:


oac-tree-server Application cli
===============================

The *oac-tree-server* application provides a set of command-line options to configure its behavior and specify the procedures to be served. Below is a description of the available options and an example of how to use them.

CLI Options
-----------

   + ``-s`` or ``--service``: Specifies the name of the automation server (required).

   + ``-d`` or ``--dir``: Specifies the directory containing XML files to be parsed and run.

   + Positional arguments: Specifies the files to be parsed and run as procedures.

   + ``-h``, ``--help``: Displays the help message with a summary of all available options.


Below is an example of how to launch the *oac-tree-server* with a specific service name and a set of procedures:

.. code-block:: sh

   oac-tree-server -s MyAutomationServer -d . "/path/to/procedures" procedure1.xml procedure2.xml

