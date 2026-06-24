# Websocket API for oac-tree automation server

## General considerations

The websocket API for the automation server uses different endpoints. For a server, hosted at hostname `<HOST>` and port `<PORT>`, there will be:

* For each job, there will be and enpoint `ws://<HOST>:<PORT>/job/<INDEX>`, where `<INDEX>` is the job index, which can be any value between 0 and N-1, with N the number of jobs on the server. This endpoint will be used to monitor the state of the job and clients connecting to this endpoint will receive update messages:
  * Status updates of job, instructions or variables;
  * Generic messages to be displayed to the user, e.g. for logging.
* A single endpoint `ws://<HOST>:<PORT>/info` that handles:
  * information queries about the server: how many jobs are hosted at this server and what is the static structure of each job;
  * messages that control the execution of jobs: start, stop, pause, resume, set breakpoint, etc.

All messages passed over the websocket interface will be JSON encoded.

## Server side publishing of job updates

These updates are published on the job-specific endpoint and all update messages will be sent to all clients that are connected to this endpoint. Requests for user input are handled in the same way, except that the client is expected to send a reply back. The web socket server is not blocked while waiting for this reply.

An outstanding request for user input is re-published periodically (at a server-configurable interval) until it is answered or interrupted, so that clients connecting after the initial broadcast still receive it. Every re-publication carries the same `req_id`, so clients must treat a repeated `req_id` as the same request. Once a request has been answered (or otherwise cancelled), the server ignores any further replies for that `req_id`.

The `update` field in the message structures holds an integer value that describes the type of the update message:

| Value | Message type             |
|-------|--------------------------|
| 0     | job state update         |
| 1     | current breakpoint       |
| 2     | instruction state update |
| 3     | variable state update    |
| 4     | display generic value    |
| 5     | display message          |
| 6     | log message              |
| 7     | get user value           |
| 8     | get user choice          |
| 9     | user input cancelled     |

The following subsections specify the fields in each of the different types of JSON messages.

### Job state update

```json
{
  "update": 0,
  "state": 3
}
```

where the `state` field encodes the updated job state as an integer:

| Value | State     |
|-------|-----------|
| 0     | initial   |
| 1     | paused    |
| 2     | stepping  |
| 3     | running   |
| 4     | succeeded |
| 5     | failed    |
| 6     | halted    |

Each job starts out in the `initial` state. When the job is executing only a single instruction (in 'step' mode), the state is `stepping` until that instruction is executed, when the state will be `paused`. `running` indicates the job is executing in continuous mode (so no stepping). The last three states indicate final states of a job, corresponding to successful execution, failure or a job that was halted before finishing.

### Current breakpoint

```json
{
  "update": 1,
  "id": 8
}
```
where the `id` field encodes the index of the instruction that currently hit a breakpoint, or an invalid id if no breakpoint is currently hit. The server uses the maximum value of a 32 bit unsigned integer as the invalid id.

### Instruction state update

```json
{
  "update": 2,
  "id": 5,
  "breakpoint": false,
  "state": 3
}
```

where the `id` field encodes the index of the instruction that has an updates state. The `breakpoint` field is a boolean that is `true` when there is an active breakpoint set on this instruction. Note that this does not mean that this breakpoint is currently hit, as that information is communicated via a `current breakpoint` message. Finally, the `state` field encodes the current execution state of the instruction as an integer:

| Value | State                            |
|-------|----------------------------------|
| 0     | initial/not started              |
| 1     | started and waiting to be ticked |
| 2     | running asynchronously           |
| 3     | finished with success            |
| 4     | finished with failure            |
| 5     | halted                           |

### Variable updated

```json
{
  "update": 3,
  "id": 5,
  "connected": true,
  "value": <JSON rep>,
  "type": <JSON rep>
}
```

where `id` denotes the index of the variable. The `connected` field is mostly used for network variables to denote if it is currently connected, but can also be used to specify an error condition by setting it to `false`. The `value` field will hold the JSON representation of the updated value without type information. The `type` field is the JSON representation of its type and may be ignored for webapps that just need to show the updated value. Note that both `value` and `type` are just strings that hold a JSON representation.

### Display generic value

```json
{
  "update": 4,
  "description": "Please note this value:",
  "value": <JSON rep>,
  "type": <JSON rep>
}
```

The `value` and `type` fields are encoded in the same way as for `variable updated`.

### Display message

```json
{
  "update": 5,
  "message": "Hello, there"
}
```
### Log message

```json
{
  "update": 6,
  "severity": 2,
  "message": "Something went wrong!"
}
```

where the `severity` field directly encodes the severity level, which closely follows the syslog severity levels:

| Value | Severity      |
|-------|---------------|
| 0     | Emergency     |
| 1     | Alert         |
| 2     | Critical      |
| 3     | Error         |
| 4     | Warning       |
| 5     | Notification  |
| 6     | Informational |
| 7     | Debug         |
| 8     | Trace         |

### Get user value

Message to client:

```json
{
  "update": 7,
  "req_id": 33,
  "description": "Please provide your name",
  "value": <JSON rep>,
  "type": <JSON rep>
}
```

where `value` and `type` encode the default value for the user.

Reply from client:

```json
{
  "update": 7,
  "req_id": 33,
  "ok": true,
  "value": <JSON rep>,
  "type": <JSON rep>
}
```

where the `ok` field encodes if the user input was successful. User can cancel such a request, which will lead to this field to be `false`. Note that this is still considered a valid response and that the user input request will be considered finished afterwards.

### Get user choice

Message to client:

```json
{
  "update": 8,
  "req_id": 34,
  "choices": ["normal", "degraded", "fault"],
  "value": <JSON rep>,
  "type": <JSON rep>
}
```

where `value` and `type` encode metadata that allows user interfaces to customize the display, e.g. for simple `OK - Cancel` dialogs.

Reply from client:

```json
{
  "update": 8,
  "req_id": 34,
  "choice": 1
}
```

where the `choice` field encodes the index of the choice made by the user, or `-1` if the user cancelled the request.

### User input cancelled

Message to client:

```json
{
  "update": 9,
  "req_id": 34
}
```

This message signals that the user input request with the given `req_id` is cancelled and no longer expects a reply. It is published when the request was answered (by any client) or when the request was interrupted on the server side. Clients that are still showing a dialog for this `req_id` should dismiss it. There is no reply to this message.

## Client side requests

Client side requests are handled on the `info` endpoint and are always one-to-one, i.e. the server will reply only to the client requesting that information.

Client messages will contain an integer `request` field that encodes the type of request:

| Value | Request type        |
|-------|---------------------|
| 0     | get number of jobs  |
| 1     | get static job info |
| 2     | edit breakpoint     |
| 3     | send job command    |

### Get number of jobs

This requests the number of jobs that the server is currently managing. This number can then be used to connect to specific endpoints for each of these jobs.

Message to server:

```json
{
  "request": 0,
  "req_id": 42
}
```

where the request id `req_id` is used to associate a reply to a specific request. This numberic id will be unique for each request/reply pair.

Reply message from the server:

```json
{
  "request": 0,
  "req_id": 42,
  "n_jobs": 3
}
```

### Get static job information

Message to server:

```json
{
  "request": 1,
  "req_id": 42,
  "job_index": 2
}
```

where `job_index` is the index of the job that is queried and `req_id` is as above.

Reply message from the server:

```json
{
  "request": 1,
  "req_id": 42,
  "job_index": 2,
  "value": <JSON value rep>,
  "type": <JSON type rep>
}
```

The `value` and `type` fields are again encoded as in previous cases. However, the exact structure of the static job information is not as unconstrained as for example a variable's value. As an example, the following procedure (as XML)

```xml
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
    <Local name="one" type='{"type":"uint32"}' value='1'/>
    <Local name="var1" type='{"type":"uint32"}' value='0'/>
    <Local name="var2" type='{"type":"uint32"}' value='0'/>
  </Workspace>
</Procedure>
```

would produce the following JSON string as its value representation (displayed here with indentation for readability):

```json
{
  "full_name": "Common header",
  "ws_info": {
    "one": {
      "type": "Local",
      "index": 0,
      "attributes": {
        "name": "one",
        "type": "{\"type\":\"uint32\"}",
        "value": "1"
      }
    },
    "var1": {
      "type": "Local",
      "index": 1,
      "attributes": {
        "name": "var1",
        "type": "{\"type\":\"uint32\"}",
        "value": "0"
      }
    },
    "var2": {
      "type": "Local",
      "index": 2,
      "attributes": {
        "name": "var2",
        "type": "{\"type\":\"uint32\"}",
        "value": "0"
      }
    }
  },
  "instr_tree_info": {
    "type": "Sequence",
    "category": 2,
    "index": 0,
    "attributes": {},
    "children": {
      "child_0": {
        "type": "Message",
        "category": 0,
        "index": 1,
        "attributes": {
          "text": "Hello"
        }
      },
      "child_1": {
        "type": "Output",
        "category": 0,
        "index": 2,
        "attributes": {
          "fromVar": "var1",
          "description": "Variable is"
        }
      },
      "child_2": {
        "type": "Copy",
        "category": 0,
        "index": 3,
        "attributes": {
          "inputVar": "one",
          "outputVar": "var1"
        }
      },
      "child_3": {
        "type": "Copy",
        "category": 0,
        "index": 4,
        "attributes": {
          "inputVar": "one",
          "outputVar": "var2"
        }
      }
    }
  }
}
```

### Edit breakpoint

Message to server:

```json
{
  "request": 2,
  "job_index": 2,
  "instr_id": 5,
  "active": true
}
```

where the `instr_id` field encodes the index of the instruction and `active` encodes if the breakpoint should be set (true) or cleared (false).

There is no reply message from the server. The client will receive an update message of the breakpoint state if this action was successful.

### Send job command

Message to server:

```json
{
  "request": 3,
  "job_index": 2,
  "command": 0
}
```

where the `command` field an integer representing the command:

| Value | Command   |
|-------|-----------|
| 0     | Start     |
| 1     | Step      |
| 2     | Pause     |
| 3     | Reset     |
| 4     | Halt      |

Again, there is no reply message from the server. The client will receive an update message for the job status if this command was handled successfully.

## Open issues

* Get current state of job, instructions and variables. This is required when a client connects to a job that was already running.
* Handle user input messages.
