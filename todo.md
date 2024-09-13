# Todo

* Provide client feedback on refused user input (e.g. because other client already responded)
* Make user input interruptable
* EPICSAnyValueManager calls AddAnyValues with a single PV when AddInputServer is called; this may lead to some duplication on the client side. See if this can be handled in a more clean way
* In test/unit/anyvalue_manager_server_client_tests.cpp, there is an asymmetry in the calls to ServerJobInfoIO and the mock IJobInfoIO.
* Add more extensive unit tests
* Add output methods over the network: PutValue, Message and Log
* Expose next instruction leaves on each tick
* Move all server stuff in a separate folder and only expose the absolute necessary (for main.cpp) in the public API
* Look for bottlenecks: e.g. using std::map with string keys to identify the name server for EPICS
