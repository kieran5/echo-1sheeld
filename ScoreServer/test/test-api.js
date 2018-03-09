//largely done following the tutorial on Michael Herman's blog

var chai = require('chai');
var chaiHttp = require('chai-http');
var app = require('../app');
var should = chai.should();

var id;

chai.use(chaiHttp);


describe('projects', function() {
  it('should list ALL projects on /projects GET', function(done) {
    chai.request(app)
      .get('/projects')
      .end(function(err, res){
        res.should.have.status(200);
        res.should.be.json;
        res.body.should.be.a('array');
        done();
      });
  });
  it('should add a SINGLE project on /projects POST',function(done){
    chai.request(app)
    .post('/projects')
    .send({'Person': 'Person Personson', 'Resource': 'Roomy McRoomface', 'Location': 'Building McBuildingface', 'Timing': '12:00'})
    .end(function(err, res){
      res.should.have.status(200);
      res.should.be.json;
      res.body.should.be.a('object');
      res.body.should.have.property('Person');
      res.body.Person.should.equal('Person Personson');
      id = res.body._id;
      done();
    });
  });
  it('should list a SINGLE project on /projects/<id> GET',function(done){
    chai.request(app)
    .get('/projects/'+id)
    .end(function(err, res){
      res.should.have.status(200);
      res.should.be.json;
      res.body.should.be.a('object');
      res.body.should.have.property('Person');
      res.body.Person.should.equal('Person Personson');
      done();
      });
  });
  it('should update a SINGLE project on /projects/<id> PUT',function(done){
    chai.request(app)
    .put('/projects/'+id)
    .send({'Person': 'Person McPersonson'})
    .end(function(err, res){
      res.should.have.status(200);
      res.should.be.json;
      res.body.should.be.a('object');
      res.body.should.have.property('Person');
      res.body.Person.should.equal('Person McPersonson');
      done();
    });
  });
  it('should delete a SINGLE project on /projects/<id> DELETE',function(done){
    chai.request(app)
    .delete('/projects/'+id)
    .end(function(err, res){
      res.should.have.status(200);
      res.should.be.json;
      res.body.should.be.a('object');
      res.body.should.have.property('message');
      done();
    });
  });
});