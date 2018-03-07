import mongoose from 'mongoose';

const Schema = mongoose.Schema;

export const userSchema = new Schema({
    Username:      {type: String, required: 'No username provided'},
    Password:      {type: String, required: 'No password provided'}
});
