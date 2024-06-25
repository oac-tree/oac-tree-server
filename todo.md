# Todo

* Refactor injection of AnyValueManager into server protocol, app, etc.
* Refactor server side regarding Instruction/Variable maps, user interfaces, etc.
* Look for bottlenecks: e.g. using std::map with string keys to identify the name server for EPICS
* Use factories (or factory methods) to manage the lifetime of multiple AnyValueManagerInterfaces?
* Implement client side
* Validate uniqueness and absence of gaps in variable/instruction indices when creating Info objects.
