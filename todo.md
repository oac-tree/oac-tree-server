# Todo

* Implement AddInputServer in all IAnyValueManager implementations
* Adapt Listener accordingly (and call the appropriate methods of the member IAnyValueManager)
* Make user input interruptable
* Add more extensive unit tests
* Implement other JobInterface methods over the network
* Move all server stuff in a separate folder and only expose the absolute necessary (for main.cpp) in the public API
* Look for bottlenecks: e.g. using std::map with string keys to identify the name server for EPICS
