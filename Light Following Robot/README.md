# Arduino Light Following Robot

This project demonstrates how to build a simple autonomous light-following robot using an Arduino Uno, two LDR sensors, and an L298N motor driver.

## Components

- Arduino Uno
- L298N Motor Driver
- 2 × LDR Sensors
- 2 × 10kΩ Resistors
- 2 × DC Geared Motors
- Robot Chassis
- Battery Pack

## Working Principle

The Arduino continuously reads two LDR sensors.

- If both sensors receive similar light intensity, the robot moves forward.
- If the left sensor receives more light, the robot turns left.
- If the right sensor receives more light, the robot turns right.

## Uploading

1. Open `Light_Following_Robot.ino`
2. Select **Arduino Uno**
3. Upload the sketch
4. Power the robot

## License

MIT License

### Project Created: 30 Jun 2026
