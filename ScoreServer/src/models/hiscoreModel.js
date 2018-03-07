import mongoose from 'mongoose';

const Schema = mongoose.Schema;

var hiscoreSchema = new Schema({
    nickname:     {type: String, required: 'No person name provided'},
    score:        {type: Number, required: 'No score specified.'}
});

hiscoreSchema.index({ nickname: 'text', score: 'text'});

export default hiscoreSchema;