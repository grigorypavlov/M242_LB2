# Test Protocol

Requirement | Test Description | Expected Result | Result OK? | Comment
----------- | ---------------- | --------------- | ---------- | -------
Motion sensor | Lower or wave hand over MCU. | MCU starts sending data to server, website displays updates. | OK | -
Time\*      | Compare time displayed on webpage with actual time. | The time is correct. | FAILED | Incorrect time on MCU, would neet be by synchronized first, e.g., with NTP.
Tilt\*      | Tilt the board in any direction. | Website displays the angle of tilt. | OK | Direction of tilt can be measured, but not the exact degree.
Temperature\* | Compare displayed temperature with actual temperature. | Temperature is correct. | OK | -
Air Pressure\* | Compare displayed air pressure with actual air pressure. | Air pressure is correct. | OK | -
Humidity\*  | Compare displayed humidity with actual humidity. | Humidity is correct. | OK | -

\* = Depends on motion sensor requirement.
