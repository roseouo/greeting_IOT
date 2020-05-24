const express = require('express');
const app = express();
const server = require('http').Server(app);
const io = require('socket.io')(server);


var fs = require('fs'); //require filesystem module

var port = 8080;

var SerialPort = require("serialport");
var arduinoCOMPort = "COM6";    //"/dev/ttyACM0"

var arduinoSerialPort = new SerialPort(arduinoCOMPort, 
	{baudRate: 9600}  );

arduinoSerialPort.on('open',function() {
  console.log('Serial Port ' + arduinoCOMPort + ' is opened.');
});

app.get('/', function (req, res) {
   handler (req, res);
})

app.get('/:action', function (req, res) {
   var action = req.params.action || req.param('action');
    
    if(action == 'led'){
        arduinoSerialPort.write("o");
        return res.send('Led light is on!');
    } 
    if(action == 'off') {
        arduinoSerialPort.write("f");
        return res.send("Led light is off!");
    }
    return res.send('Action: ' + action);
 
});

app.listen(port, function () {
   ///記�?將此檔�?以UTF-8?��?存�?，否?��??��?輸出?�是亂碼
   console.log("http伺�??�已??+port"+"?�口?��?");  
});

process.on('SIGINT', function () { //on ctrl+c
  console.log('Process is terminated');
  process.exit(); //exit completely
});


io.on('connection', (socket) => {
  console.log('a user connected');

  socket.on("disconnect", () => {
    console.log("a user go out");
  });

});

function handler (req, res) { //create server
  fs.readFile(__dirname + '/public/index2.html',   ///public/index.html
	  function(err, data) { //read file index.html in public folder
		if (err) {
		  res.writeHead(404, {'Content-Type': 'text/html'}); //display 404 on error
		  return res.end("404 Not Found");
		} 
		res.writeHead(200, {'Content-Type': 'text/html'}); //write HTML
		res.write(data); //write data from index.html
		return res.end();
      }
  );
}