//Setup function here
//ESP32 > Mqtt > _Nodered_ > Influxdb > Grafana

// Set measurement name
msg.measurement = "sensor3";

//From Mqtt to Payload
let mqtt = msg.payload;
let esp32THI = 0.8 * mqtt.esp32temp + (mqtt.esp32hum * mqtt.esp32temp) / 500;
//THI=0.8*Temp+(Hum*Temp)/500

//Set Payload include debug
msg.payload = {       
    temperature : mqtt.esp32temp,
    humidity : mqtt.esp32hum,
    heatindex: parseFloat(esp32THI.toFixed(2))
};

// Add timestamp (InfluxDB will use this if provided)
// msg.timestamp = new Date().toISOString();

let temp = {payload :mqtt.esp32temp};
let hum  = {payload :mqtt.esp32hum};
let thi  = {payload :esp32THI}; 

return [msg, temp, hum, thi ,
        null];
