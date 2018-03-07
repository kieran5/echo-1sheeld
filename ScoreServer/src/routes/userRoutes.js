import express from 'express';
import {
    addNewUser,
    getUsers,
    deleteUser
} from '../controllers/userController';

var usersRouter = express.Router();

usersRouter.post('/:Username', getUsers)
           .post('/', addNewUser)
           .delete('/:userId', deleteUser);


export default usersRouter;