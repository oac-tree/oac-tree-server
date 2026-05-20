# Websocket messages

The automation server will be hosted on a URL: `ws://<HOST>:<PORT>`. This endpoint will only be used to get global information about the host, like the list of served procedures with their prefixes.

For each specific procedure, the URL will be `ws://<HOST>:<PORT>/<PREFIX>`, where `<PREFIX>` is the prefix of the procedure.

## IAnyValueManager

The API is defined as:

```c++
  virtual bool UpdateAnyValue(const std::string& name, const sup::dto::AnyValue& value) = 0;

  virtual UserInputReply GetUserInput(const std::string& input_server_name, sup::dto::uint64 id,
                                      const UserInputRequest& request) = 0;

  virtual void Interrupt(const std::string& input_server_name, sup::dto::uint64 id) = 0;
```

### Update value

This function is used to publish changes in instruction and variable states.

This needs to be published to all clients connected to the endpoint for the procedure. The message published will be:

```json
{
  "<name_1>": "<JSON representation of value_1>",
  "<name_2>": "<JSON representation of value_2>",
  ...
}
```

Note that the message allows multiple updates to be send at once. It's not clear if we need this.

## Control protocol server

## Info protocol server

