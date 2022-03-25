const Sensors = {
    HumTemp: 'hum_temp'
};

function fetchSensorData(sensor) {
    fetch('http://192.168.104.29:8080/api/v1/data?sensor=' + sensor).then((res) => {
        res.json().then((dec) => {
            console.log(dec);
            const p = document.getElementById('info');
            p.innerText = "Temperature: " + dec[0] + ", Humidity: " + dec[1];
        });
    });
}

const intervalID = setInterval(function() {
    fetchSensorData(Sensors.HumTemp);
}, 5000);

// clearInterval(intervalID);
