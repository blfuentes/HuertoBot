# HuertoBot
Smart orchard project with espressif and esp32 following guidance of the course from [Laboratorio Gluon](https://github.com/LaboratorioGluon)

- :hammer_and_pick: Hardware:
    - **BME280**: Implement the interface for the Bosch library.
    - **ADS1115**: Create the ADC library to read the sensors.
    - **LDR** and **Humidity**: Connect and understand how to read the sensors.
    - **MOSFETS**: Motor control for irrigation. 
    - **Hx711** and **Load Cell**: To measure the water weight in the reservoir.
    - **Solar Panels** and **Batteries**: How to manage battery charging via solar panel and charge balancers.
- :computer: ESP32:
    - Use **I2C** to communicate with the sensors.
    - Send data over **Wifi** using **MQTT**.
    - Learn about **low power** modes.
    - Use **FreeRTOS** and **multithreading**.
- :desktop_computer: Server:
    - Configure **proxmox**
    - Install and configure **Telegraf** + **InfluxDB** + **Grafana**
    - Receive and display data from **MQTT**

## Scheme
![Design Huertobot](/Course/Design/system-scheme.png)

## Lessons

- [Project setup workflow](Course/03%20-%20Project%20setup/workflow-definition.md)
- [I2C + BME280](Course/04%20-%20I2C%20+%20BME280/BME280.md)
- [ADC ADS1115](Course/07%20-%20ADC%20ADS1115/ADS1115.md)
