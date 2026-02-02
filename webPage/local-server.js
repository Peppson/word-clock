// Quick Node test server
// Run the darn thing locally instead of directly on ESP32
// npm install
// node webPage/local-server.js

const path = require('path');
const express = require('express');
const app = express();
const port = 3000;
app.use(express.json());
app.use(express.text());

app.use(express.static('webPage/app'));
app.get('/', (req, res) => {
    res.sendFile('index.html', { root: "webPage/app" });
    log(req);
});

app.get('/favicon.ico', (req, res) => {
    res.sendFile('icons/favicon.ico', { root: "webPage" });
    log(req);
});

app.post('/submit/WiFi-form', (req, res) => {
    res.status(200).send('OK');
    log(req, 1);
});

app.post('/submit/time-form', (req, res) => {
    res.status(200).send('OK');
    log(req, 1);
});

app.get('/ping', (req, res) => {
    res.status(200).send('OK');
    log(req);
});

app.get('/restart', (req, res) => {
    res.status(200).send('OK');
    log(req);
});

app.get('/snake/start', (req, res) => {
    res.status(200).send('OK');
    log(req);
});

app.get('/snake/end', (req, res) => {
    res.status(200).send('OK');
    log(req);
});

app.post('/snake/:button', (req, res) => {
    res.status(200).send('OK');
    log(req);
});

app.get('/color/start', (req, res) => {
    res.status(200).send('OK');
    log(req);
});

app.post('/color', (req, res) => {
    res.status(200).end();
    log(req, 1);
});

app.post('/color/end', (req, res) => {
    res.status(200).send('OK');
    log(req, 1);
});

// 404
app.use((req, res) => {
    res.status(404).send('Not Found');
    log(req);
});

// Start
app.listen(port, () => {
    console.log(`\n################ START @ port ${port} ################\n> http://localhost:3000`);
});

// Shutdown
process.on('SIGINT', () => {
    console.log('################ STOP ################');
    process.exit();
});

function log(req, json = false) {
    console.log(`
        Method: ${req.method}
        Path: ${req.path}`);
    if (json) {
        const json_data = req.body;
        console.log('JSON:', json_data);
    }
}
