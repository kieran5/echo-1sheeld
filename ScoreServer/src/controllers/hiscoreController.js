//Much of the API was developed following Emmanuel Henri's guide on Lynda
import mongoose from 'mongoose';
import hiscoreSchema from '../models/hiscoreModel';

const hiscore = mongoose.model('hiscore', hiscoreSchema);

export const addNewScore = (req, res) => {
    let newScore = new hiscore(req.body);

    newScore.save((err, score) => {
        if (err) {
            res.send(err);
        }
        res.json(score)
    });
};

export const getScores = (req, res) => {
    hiscore.find({}, (err, score) => {
        if (err) {
            res.send(err);
        }
        res.json(score)
    });
};

export const getScores2 = (req, res) => {
    hiscore.find({ $text : { $search : req.params.text } }) 
    .exec(function(err, score){
        if (err) {
            res.send(err);
        }
        res.json(score);
    });
};

export const updateScores = (req, res) => {
    hiscore.findOneAndUpdate({ _id: req.params.projectId}, req.body, { new: true }, (err, score) => {
        if (err) {
            res.send(err);
        }
        res.json(score);
    })
}

export const deleteScore = (req, res) => {
    hiscore.remove({ _id: req.params.projectId }, (err, score) => {
        if (err) {
            res.send(err);
        }
        res.json({ message: 'Successfully deleted score'})
    })

    
}