# Todo

* Handle interrupts for EPICSAnyValueManager: new user input requests should not even start if an interrupt was requested.
* Provide client feedback on refused user input (e.g. because other client already responded)
* Add more extensive unit tests
* Move all server stuff in a separate folder and only expose the absolute necessary (for main.cpp) in the public API
* Look for bottlenecks: e.g. using std::map with string keys to identify the name server for EPICS
