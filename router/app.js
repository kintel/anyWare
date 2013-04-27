var net = require("net");
var debug = require("debug")("router");
var util = require("util");
var _ = require("underscore");
var serialport = require("serialport");
var SerialPort = serialport.SerialPort;
var child = require('child_process');

function jsonToArduinoCmd(jsonstr) {
  var cmdstr = ""
  try {
    var json = JSON.parse(jsonstr);
    // FIXME: We really only support one string, but we could return multiple commands
    for (key in json) {
      switch (key) {
        case "knockpattern":
        cmdstr += "KNOCK";
        var pattern = json[key];
        for (var i=0;i<pattern.length;i++) {
          cmdstr += " " + pattern[i];
        }
        break;
        case "cmd":
        cmdstr += "CMD " + json[key];
        break;
        default:
        console.log("Unknown JSON command: " + jsonstr);
        break;
      }
    }
  }
  catch (err) {
    console.log("JSON parse error: " + err + "\n     '" + jsonstr + "'");
  }
  return cmdstr;
}

function arduinoCmdToJson(cmdstr) {
  var array = cmdstr.split(" ");
  var json = {};
  switch (array[0]) {
    case "LOGIN":
    json["login"] = array[1];
    break;
    case "PROX":
    json["proximity"] = parseInt(array[1], 10);
    break;
    case "KNOCK":
    var knockpattern = [];
    for (var i=1;i<array.length;i++) knockpattern[i-1] = parseInt(array[i], 10);
    json["knockpattern"] = knockpattern;
    break;
  }
  return json
}

/*
 * Detect an Arduino board
 * Loop through all USB devices and try to connect
 * This should really message the device and wait for a correct response
 */
function connectToArduino(callback) {
  debug('attempting to find Arduino board');
  child.exec('ls /dev | grep usb', function(err, stdout, stderr) {
    var usb = stdout.slice(0, -1).split('\n');
    var found = false;
    var err = null;
    var possible;
    var temp;

    while (usb.length) {
      possible = usb.pop();
      debug("Trying " + possible);
      if (possible.slice(0, 2) !== 'cu') {
        try {
          temp = new SerialPort('/dev/' + possible, {
            baudrate: 9600,
            parser: serialport.parsers.readline('\r\n')
          });
        } catch (e) {
          err = e;
        }
        if (!err) {
          found = temp;
          debug('found board at ' + temp.path);
          break;
        } else {
          err = new Error('Could not find Arduino');
        }
      }
    }

    callback(err, found);
  });
}


console.log("Opening serial port");
var serial;
connectToArduino(function(err, foundserial) {
  if (err) {
    console.log("Error opening serial port: " + err);
  }
  else {
    serial = foundserial;
    serial.on("data", function(data) {
      console.log("from Arduino: " + data);
      var json = arduinoCmdToJson(data);
      console.log("  ->: " + JSON.stringify(json));
      socketclient.write(JSON.stringify(json) + "\n");
    });
  }
});

console.log("Connecting to server");
var socketclient = net.connect(7000, "evol.local", function() {
  debug("Connected to server");
});

socketclient.on("data", function(data) {
  console.log("From server: '" + data.toString() + "'");
  var cmdstr = jsonToArduinoCmd(data.toString())
  console.log("  ->: " + cmdstr);
//  var sending = "KNOCK 398 303 141 437\n"; 
//  console.log("To Arduino: '" + sending + "'");
  serial.write(cmdstr + "\n");
});

socketclient.on("end", function() {
  debug("Disconnected");
});

var stdin = process.openStdin();
stdin.on("data", function(chunk) {
//  serial.write(chunk);
  socketclient.write(chunk);
});
