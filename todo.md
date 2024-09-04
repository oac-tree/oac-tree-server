# Todo

* Implement AddInputServer in all IAnyValueManager implementations
* Adapt Listener accordingly (and call the appropriate methods of the member IAnyValueManager)
* Reorganise the stuff in sup_auto_protocol.h and anyvalue_input_request.h
* Make user input interruptable
* Add more extensive unit tests
* Add output methods over the network: PutValue, Message and Log
* Expose next instruction leaves on each tick
* Move all server stuff in a separate folder and only expose the absolute necessary (for main.cpp) in the public API
* Look for bottlenecks: e.g. using std::map with string keys to identify the name server for EPICS
