// Import dependencies
const http = require("http");
const fs = require("fs");
const WebSocketServer = require('websocket').server;

// Create a HTTP server
const server = http.createServer((req, res) => {
    // When we get a request, read the contents of "index.html"
    fs.readFile("./index.html", (err, data) => {
        // The contents of "index.html" is in `data`,
        // send it to the client and close the connection.
        res.writeHead(200, { 'Content-Type': 'text/html' });
        res.end(data);
    });
});

// Set up the HTTP server to listen on port 8080
// it will then be available on http://localhost:8080
const port = 8080;
server.listen(port, (err) => {
    if (err) {
        console.error("Something is wrong", err);
        return;
    }

    console.log("Server is listening on port", port);
})

// Create a web socket server and attach it to our HTTP server.
// This will allow us to establish a WebSocket on
// ws://localhost:8080
const ws = new WebSocketServer({
    httpServer: server,
    autoAcceptConnections: true
});

// Set up the initial state of our app. By keeping a reference
// to it, we can easily reset it when we click the "reset" button
const INITIAL_STATE = {
    button1: "on",
    button2: "off",
    sensitivity: 33
};

// Copy the initial state to the "state" variable.
// We copy it to ensure we never change the values of INITIAL_STATE
let state = Object.assign({}, INITIAL_STATE);

// Set up an array which will hold all the active web socket connections.
// Multiple clients can connect to the server, so we need to send
// updates to all of them
let connections = [];

// Function which will update the `state` variable given an action.
// Example action:
//  { type: "button1", value "off" };
// will set `state.button1` to "off". 
function updateState(action) {
    switch (action.type) {
        // Example: { type: "button1", value: "on" }
        case "button1": {
            state.button1 = action.value;
            break;
        }
        // Example: { type: "button2", value: "off" }
        case "button2": {
            state.button2 = action.value;
            break;
        }
        // Example: { type: "sensitivity", value: 10 }
        case "sensitivity": {
            state.sensitivity = action.value;
            break;
        }
        // Example: { type: "reset"}
        case "reset": {
            state = Object.assign({}, INITIAL_STATE);
            break;
        }
    }
}

// This will send the given state to all the clients.
function broadcastState(state) {
    connections.forEach(connection => {
        // We "stringify" the current state as JSON, this ensures
        // we just send a string to the client.
        connection.send(JSON.stringify({
            type: "state",
            state: state
        }));
    })
}

// Called when a new client connects
ws.on("connect", (connection) => {
    console.log("New connection");

    // Add the connection to our `connections` array
    connections.push(connection);

    // When a client sends us a message
    connection.on("message", message => {
        // Parse the message from the client as JSON.
        const action = JSON.parse(message.utf8Data);

        // Update the current state of the app given the action sent
        updateState(action);

        // Send the updated state to all our connected clients
        broadcastState(state);

        // Output the action and new state to the console
        console.log("Action:", action, "New state:", state);
    });

    // When a client disconnects, we remove it from our
    // array of connections.
    connection.on("close", () => {
        // Remove the connection from our list of connection,
        // this prevents any memory leaks.
        connections = connections.filter(conn => {
            return conn !== connection;
        });
        console.log("clients connected", connections.length);
    });

    // When a new client has connected, we send the current
    // state of the application to the client.
    connection.send(JSON.stringify({
        type: "state",
        state: state
    }));
});

