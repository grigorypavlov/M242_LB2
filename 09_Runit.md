# Build and Run the Project

Start the go web-service:

```sh
cd Server
go run .
```

Then start the web interface using any http server, for example python:

```sh
cd Web
python3 -m http.server
```

Finally, open the `IoTKit` folder in mbed-studio. **Ensure the Wi-fi
credentials in `mbed_app.json` are correct.*** Run a `Clean-Build` once. After,
connect the IoTKit, and deploy the application by pressing the `Run` button.

The IoTKit should now be sending data to the webservice.

Console output IoTKit:

<sample>
Connecting to LERNKUBE...SUCCESS
MAC: C4:7F:51:8D:13:58
IP: 192.168.104.23
Humidity & Temperature Sensor = 0xBC
HttpRequest status code: 200
</sample>

Console output web service:

<sample>
Listening on :8080 ... 2022/03/25 11:29:51 Request received: &{POST
/api/v1/hello_world HTTP/1.1 1 1 map[Content-Length:[54]
Content-Type:[application/json]] 0xc0000801c0 <nil> 54 [] false
192.168.104.29:8080 map[] map[] <nil> map[] 192.168.104.23:5669
/api/v1/hello_world <nil> <nil> <nil> 0xc000080200} 2022/03/25 11:29:56 Request
received: &{POST /api/v1/hello_world HTTP/1.1 1 1 map[Content-Length:[54]
Content-Type:[application/json]] 0xc000080400 <nil> 54 [] false
192.168.104.29:8080 map[] map[] <nil> map[] 192.168.104.23:5670
/api/v1/hello_world <nil> <nil> <nil> 0xc000080440}
</sample>

Navigate to the website in your browser. Periodic requests should automatically
update the information displayed.
