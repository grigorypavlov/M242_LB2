const Sensors = {
    HumTemp: 'hum_temp',
    Tilt: 'tilt',
    Pressure: 'pressure'
};

async function fetchSensorData(sensor) {
    const resp = await fetch('http://192.168.65.55:8080/api/v1/data?sensor=' + sensor);
    const dec = await resp.json();

    console.log(dec);
    return dec;
}

const intervalID = setInterval(function() {
    fetchSensorData(Sensors.HumTemp).then(data => {
        const p = document.getElementById('hum');
        p.innerText = "Temperature: " + data[0] + ", Humidity: " + data[1];
    });

    fetchSensorData(Sensors.Tilt).then(data => {
        const p = document.getElementById('tilt');
        p.innerText = "Tilt xl: " + data[0] + " xh: " + data[1] + " yl: " +
            data[2] + " yh: " + data[3] + " zl: " + data[4] + " zh: " + data[5];
    });

    fetchSensorData(Sensors.Pressure).then(data => {
        const p = document.getElementById('air');
        p.innerText = "Air Pressure: " + data;
    });
}, 5000);

// clearInterval(intervalID);
