# Websocket messages

## General considerations

JSON is probably the best way of encoding messages over websockets, as it is very well supported in Javascript and can also be easily handled in other programming languages.

This has some implications:

Transmission of instruction state updates could be very simple JSON messages, for example:

```json
{
  "update": 2,
  "id": 5,
  "breakpoint": false,
  "state": 3
}
```

where `"update": 2` denotes that this is an update of instruction status (see further), the `id` field encodes the numeric id of the instruction and `state` is the numeric value of the `ExecutionStatus` enumeration.

Note that this breaks the current API for such updates in `IAnyValueManager`.

The same logic applies for almost all messages that are sent to the client to notify some state update (breakpoint hit, job state, etc.) The only exceptions would be updates that explicitly need to encode an `AnyValue`, e.g. `VariableUpdated`.

## Global server URL

The automation server will be hosted on a URL: `ws://<HOST>:<PORT>`. This endpoint will only be used to get global information about the host, like the list of served procedures with their prefixes.

For each specific procedure, the URL will be `ws://<HOST>:<PORT>/<PREFIX>`, where `<PREFIX>` is the prefix of the procedure.

## Server side publishing of procedure updates

The content for server side publishing of updates related to a single procedure is determined by the member functions in `ServerJobInfoIO`.

All such update messages will be sent to all clients that are connected to the endpoint for the job in question.

The `update` field in the message structures will be the integer value of the following enumeration:

```c++
enum class UpdateMessageType
{
  kJobState = 0,
  kBreakPoint,
  kInstructionState,
  KVariableState,
  kPutValue,
  kMessage,
  kLog,
};
```

The following subsections specify how each of the member functions of `ServerJobInfoIO` is encoded in a JSON message.

Note that `InitNumberOfInstructions` and `ProcedureTicked` do not need to send any messages to the client, so we can safely ignore these here.

### JobStateUpdated

```json
{
  "update": 0,
  "state": 3
}
```

where the `state` field encodes the `JobState` enumeration as an integer.

### BreakpointInstructionUpdated

```json
{
  "update": 1,
  "id": 8
}
```
where the `id` field encodes the index of the instruction that currently hit a breakpoint, or `std::numeric_limits<sup::dto::uint32>::max()` if no breakpoint is hit.

### InstructionStateUpdated

The message structure for instruction state updates was alread mentioned above, but is repeated here for convenience:

```json
{
  "update": 2,
  "id": 5,
  "breakpoint": false,
  "state": 3
}
```

### VariableUpdated

```json
{
  "update": 3,
  "id": 5,
  "connected": true,
  "value": <JSON rep>,
  "type": <JSON rep>
}
```

where `id` denotes the numeric id of the variable. The `value` field will hold the JSON representation of the updated value without type information. The `type` field is the JSON representation of its type and may be ignored for webapps that just need to show the updated value.

### PutValue

```json
{
  "update": 4,
  "description": "Please note this value:",
  "value": <JSON rep>,
  "type": <JSON rep>
}
```

The `value` and `type` fields are encoded in the same way as for `VariableUpdated`.

### Message

```json
{
  "update": 5,
  "message": "Hello, there"
}
```
### Log

```json
{
  "update": 6,
  "severity": 2,
  "message": "Something went wrong!"
}
```

where the `severity` field directly encodes the integer severity level.

### User input

`GetUserValue`, `GetUserChoice` and `Interrupt` are all related to providing a mechanism for user input. This will be delayed to a further stage of the implementation.

## Client side requests

The client side requests correspond to the functionality exposed by the control and info protocol servers.

Client messages for getting information about the jobs served are always handled one-to-one, i.e. the server will reply only to the client requesting that information.

Client messages will contain a message type that encodes the type of request that is made and this will be the integer value of the following enumeration:

```c++
enum class ClientMessageType
{
  kGetNumberOfJobs = 0,
  kGetJobInfo,
  kEditBreakPoint,
  kSendCommand
};
```

### GetServerPrefix

This method is not needed for the websocket implementation, as the server URL is all that is needed. Job specific URL are built by adding a path to it.

### GetNumberOfJobs

Message to server:

```json
{
  "request": 0
}
```

Reply message from the server:

```json
{
  "request": 0,
  "n_jobs": 3
}
```

### GetJobInfo

Message to server:

```json
{
  "request": 1
}
```

Reply message from the server:

```json
{
  "request": 1,
  "job_info": <JSON representation of the JobInfo>
}
```

The JSON representation of the JobInfo structure is best shown by example. Assume the procedure is defined by the following XML file:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<Procedure name="My test procedure">
    <Sequence name="My root instruction">
        <Wait timeout="1.0"/>
        <Wait timeout="2.0"/>
    </Sequence>
    <Workspace>
        <Local name="counter" type='{"type":"int32"}' value="42"/>
        <File name="from_file" file="variable.json"/>
    </Workspace>
</Procedure>
```

The JobInfo representation in JSON then becomes:

```json
{
  "name": "My test procedure",
  "workspace": [
    {
      "name": "counter",
      "type": "Local",
      "attributes": [
        {
          "name": "type",
          "value": "{\"type\":\"int32\"}"
        },
        {
          "name": "value",
          "value": "42"
        }
      ]
    },
    {
      "name": "from_file",
      "type": "File",
      "attributes": [
        {
          "name": "file",
          "value": "variable.json"
        }
      ]
    }
  ],
  "root_instruction": {
    "type": "Sequence",
    "category": 2,
    "id": 0,
    "attributes": [
      {
        "name": "name",
        "value": "My root instruction"
      }
    ],
    "children": [
      {
        "type": "Wait",
        "category": 0,
        "id": 1,
        "attributes": [
          {
            "name": "timeout",
            "value": "1.0"
          }
        ]
      },
      {
        "type": "Wait",
        "category": 0,
        "id": 2,
        "attributes": [
          {
            "name": "timeout",
            "value": "2.0"
          }
        ]
      },
    ]
  }
}
```

```c++
enum Category
{
  kAction = 0,
  kDecorator,
  kCompound
};
```


### EditBreakPoint

Message to server:

```json
{
  "request": 2,
  "id": 5,
  "active": true
}
```

where the `id` field encodes the id of the instruction and `active` encodes if the breakpoint should be set (true) or cleared (false).

There is no reply message from the server. The client will receive an update message of the breakpoint if this action was successful.

### SendJobCommand

Message to server:

```json
{
  "request": 3,
  "command": 0
}
```

where the `command` field is the integer representation of the job command enumeration:

```c++
enum class JobCommand
{
  kStart = 0,
  kStep,
  kPause,
  kReset,
  kHalt,
  kTerminate
};
```

Again, there is no reply message from the server. The client will receive an update message for the job status if this command was handled successfully.

## Open issues

* Handle user input messages
