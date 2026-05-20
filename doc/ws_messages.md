# Websocket messages

## General considerations

JSON is probably the best way of encoding messages over websockets, as it is very well supported in Javascript and can also be easily handled in other programming languages.

This has some implications:

Transmission of instruction state updates could be very simple JSON messages, for example:

```json
{
  "type": 2,
  "id": 5,
  "breakpoint": false,
  "state": 3
}
```

where `"type": 2` denotes that this is an update of instruction status (see further), the `id` field encodes the numeric id of the instruction and `state` is the numeric value of the `ExecutionStatus` enumeration.

Note that this breaks the current API for such updates in `IAnyValueManager`.

The same logic applies for almost all messages that are sent to the client to notify some state update (breakpoint hit, job state, etc.) The only exceptions would be updates that explicitly need to encode an `AnyValue`, e.g. `VariableUpdated`.

## Global server URL

The automation server will be hosted on a URL: `ws://<HOST>:<PORT>`. This endpoint will only be used to get global information about the host, like the list of served procedures with their prefixes.

For each specific procedure, the URL will be `ws://<HOST>:<PORT>/<PREFIX>`, where `<PREFIX>` is the prefix of the procedure.

## Server side publishing of procedure updates

The content for server side publishing of updates related to a single procedure is determined by the member functions in `ServerJobInfoIO`.

The `type` field in the message structures will be the integer value of the following enumeration:

```c++
enum class MessageType
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
  "type": 0,
  "state": 3
}
```

where the `state` field encodes the `JobState` enumeration as an integer.

### BreakpointInstructionUpdated

```json
{
  "type": 1,
  "id": 8
}
```
where the `id` field encodes the index of the instruction that currently hit a breakpoint, or `std::numeric_limits<sup::dto::uint32>::max()` if no breakpoint is hit.

### InstructionStateUpdated

The message structure for instruction state updates was alread mentioned above, but is repeated here for convenience:

```json
{
  "type": 2,
  "id": 5,
  "breakpoint": false,
  "state": 3
}
```

### VariableUpdated

```json
{
  "type": 3,
  "id": 5,
  "connected": true,
  "value": <JSON rep>
}
```

where `id` denotes the numeric id of the variable. The `value` field should hold some representation of the underlying AnyValue. This could be the direct JSON from sup-dto, or some other representation (note that these values only need to be displayed and that type information is not important here).

### PutValue

```json
{
  "type": 4,
  "description": "Please note this value:",
  "value": <JSON rep>
}
```

The same issue appears here as in `VariableUpdated`, i.e how to encode the value.

### Message

```json
{
  "type": 5,
  "message": "Hello, there"
}
```
### Log

```json
{
  "type": 6,
  "severity": 2,
  "message": "Something went wrong!"
}
```

where the `severity` field directly encodes the integer severity level.

### User input

`GetUserValue`, `GetUserChoice` and `Interrupt` are all related to providing a mechanism for user input. This will be delayed to a further stage of the implementation.

## Control protocol server

TBD

## Info protocol server

TBD

## Open issues

* JSON representation for `VariableUpdated` values;
* JSON representation for `PutValue` values (similar to the above);
* Handle user input messages
