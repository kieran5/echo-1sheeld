import express from 'express';
import * as c from '../controllers/hiscoreController';

var projectsRouter = express.Router();

projectsRouter.get('/', c.getScores)
             .get('/:text', c.getScores2)
             .post('/', c.addNewScore)
             .put('/:projectId', c.updateScores)
             .delete('/:projectId', c.deleteScore);


export default projectsRouter;
