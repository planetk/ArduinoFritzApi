# ArduinoFritzApi
An arduino Library for Automation of Fritz!Box, Fritz!DECT and FRITZ!Powerline devices

# Implemented Functions

Following functions have been implemented:

<dl>
  <dt>setSwitchOn</dt>
  <dd>Switch actor on, return new switch state (true=on, false=off)</dd>
  <dt>setSwitchOff</dt>
  <dd>Switch actor off, return new switch state (true=on, false=off)</dd>
  <dt>setSwitchToggle</dt>
  <dd>Toggle switch state, return new switch state (true=on, false=off)</dd>
  <dt>getSwitchState</dt>
  <dd>Retrieve current switch state (true=on, false=off)</dd>
  <dt>getSwitchPresent</dt>
  <dd>Check for presence of a switch with the given ain (true=present, false=offline)</dd>
  <dt>getSwitchPower</dt>
  <dd>Get current power consumption in Watt</dd>
  <dt>getSwitchEnergy</dt>
  <dd>Get total energy since last reset in Wh</dd>
  <dt>getTemperature</dt>
  <dd>Get temperature in °C</dd>
  <dt>getSwitchName</dt>
  <dd>Get name of an actor</dd>
  <dt>getThermostatNominalTemperature</dt>
  <dd>Get nominal temperature of thermostat (8: &lt;= 8°C, 28: &gt;= 28°C, 100: max, 0: off)</dd>
  <dt>getThermostatComfortTemperature</dt>
  <dd>Get comfort temperature of thermostat (8: &lt;= 8°C, 28: &gt;= 28°C, 100: max, 0: off)</dd>
  <dt>getThermostatReducedTemperature</dt>
  <dd>Get reduced temperature of thermostat (8: &lt;= 8°C, 28: &gt;= 28°C, 100: max, 0: off)</dd>
  <dt>setThermostatNominalTemperature</dt>
  <dd>Set nominal temperature of thermostat (8: &lt;= 8°C, 28: &gt;= 28°C, 100: max, 0: off)</dd>

# Error handling

In case something went wrong one of following error codes will be thrown:

```
   FRITZ_ERR_HTTP_COMMUNICATION  -1001
   FRITZ_ERR_NO_CHALLENGE        -1002
   FRITZ_ERR_NO_SID              -1003
   FRITZ_ERR_EMPTY_SID           -1004
   FRITZ_ERR_INVALID_SID         -1005
   FRITZ_ERR_VALUE_NOT_AVAILABLE -1006
```

In Addition one of the following HTTP STATUS Codes may be thrown:

```
   Bad Request                    400 
   Forbidden                      403
   Internal Server Error          500
```


