const express = require('express');
const bodyParser = require('body-parser');

const app = express();
app.use(bodyParser.json());

const ipAddress = '192.168.93.210'; // Replace with the desired IP address
const port = 3000; // Specify the port number

// Route to handle the POST request from the master node
app.post('/receive-data', (req, res) => {
  const receivedData = req.body;
  console.log('Received data from node:', receivedData.node_id);
  console.log('Message Data:', receivedData.msg_data);

  // You can perform any further processing or store the data in a database here

  res.sendStatus(200); // Send a success response
});

// Start the server
app.listen(port, ipAddress, () => {
  console.log(`Server listening on ${ipAddress}:${port}`);
});