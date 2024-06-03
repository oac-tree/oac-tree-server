# Test automation server application

To test the current automation server application, run the executable:

```bash
sup-automation-server -s test-auto
```

This will run an automation server with a single simple procedure. Using the `sequencer-gui`, one can then try the different choices in `test/resources/auto-client.xml` to check that the server contains a single job, start it, monitor its published PVs and stop it.
