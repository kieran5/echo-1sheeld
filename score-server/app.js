import express from 'express';
import mongoose from 'mongoose';
import bodyParser from 'body-parser';
import hiscoresRouter from './src/routes/hiscoreRoutes';
import usersRouter from './src/routes/userRoutes';

const app = express();
const PORT = 5000;
const path = require('path');

app.set("view engine", "pug");
app.set("views", path.join(__dirname, "/src/views"));

mongoose.Promise = global.Promise;
mongoose.connect('mongodb://localhost/projectManagerdb', {
    useMongoClient: true
});


app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());
app.use('/hiscores', hiscoresRouter);
app.use('/users',usersRouter);
app.use(express.static(path.join(__dirname, '/src/views/public')));


app.get('/', function(req, res) {
    res.sendFile(path.join(__dirname + '/src/views/public/hiscores.html'));
    
});



app.listen(PORT, () =>
    console.log(`your server is running on port ${PORT}`)
);

module.exports = app;
