import mongoose from 'mongoose';
import { userSchema } from '../models/userModel';

const user = mongoose.model('user', userSchema);

export const addNewUser = (req, res) => {
    let newuser = new user(req.body);

    newuser.save((err, user) => {
        if (err) {
            res.send(err);
        }
        res.json(user)
    });
};

export const getUsers = (req, res) => {
    user.findOne(req.body, (err, user) => {
        if (err) {
            res.send(err);
        }
        res.json(user)
    })
}

export const deleteUser = (req, res) => {
    user.remove({ _id: req.params.userId }, (err, user) => {
        if (err) {
            res.send(err);
        }
        res.json({ message: 'Successfully deleted user'})
    })

    
}