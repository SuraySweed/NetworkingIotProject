const express = require('express');
const bodyParser = require('body-parser');
const fs = require('fs');

const app = express();
app.use(bodyParser.json());

const ipAddress = '192.168.249.210'; // Replace with the desired IP address
const port = 3000; // Specify the port number
const filename = 'real_received_data_1000_300.txt';

// Route to handle the POST request from the master node
app.post('/receive-data', (req, res) => {
  const receivedData = req.body;
  console.log('Received data from node:', receivedData.node_id);
  console.log('Message Data:', receivedData.msg_data);

  // Write the message to a text file
  const fileData = `Node ID: ${receivedData.node_id}\nMessage Data: ${receivedData.msg_data}\n\n`;

  fs.appendFile(filename, fileData, { flag: 'a+' }, (err) => {
    if (err) {
      console.error('Error writing to file:', err);
      res.sendStatus(500);
    } else {
      console.log('Message written to file:', filename);
      res.sendStatus(200);
    }
  });
});

// Start the server
app.listen(port, ipAddress, () => {
  console.log(`Server listening on ${ipAddress}:${port}`);
});