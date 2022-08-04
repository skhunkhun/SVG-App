'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

let svgLib = ffi.Library('./libsvgparser', {

  'fileToJSON' : ['string', ['string'] ],
  'getTitle' : ['string', ['string'] ],
  'getDesc' : ['string', ['string'] ],
  'getSVGAttrList' : ['string', ['string'] ],
  'getRectList' : ['string', ['string'] ],
  'getCircList' : ['string', ['string'] ],
  'getPathList' : ['string', ['string'] ],
  'getGroupList' : ['string', ['string'] ],
  'getOtherAttr' : ['string', ['string', 'int', 'int'] ],
  'renameSVG' : ['string', ['string', 'string', 'string'] ],
  'createSVGtoJSON' : ['string', ['string', 'string'] ],
  'addShape' : ['string', ['string', 'string', 'int'] ],
  'editAttr': ['string', ['string', 'string', 'string', 'int', 'int'] ],
  'scaleShapes' : ['string', ['string', 'int', 'float'] ],
});

app.get('/serverFiles', function(req, res) {

  let uploads = fs.readdirSync("uploads");
  let SVGArray = [];
  let errorMsg = [];

  uploads.forEach((file) => {

    let pathofFile = "uploads/" + file;
    let data = svgLib.fileToJSON(pathofFile);

    if(data !== "{}"){

      let parse = JSON.parse(data);
      parse.img = file;
      parse.filename = file;
      let sizeKB = fs.statSync(pathofFile).size;
      parse.fileSize = Math.round(sizeKB/1024);
      SVGArray.push(parse);

    } else{

      fs.unlinkSync("uploads/" + file);
      errorMsg.push("ERROR: " + file + " is an invalid SVG file.\n It has been removed from the server.");

    }

  });

  res.send({

    SVGData : SVGArray,
    errorMsgs : errorMsg
  });

});

app.get('/SVGViewPanel', function(req, res) {

  let currentFile = "uploads/" + req.query.filename;

  let title = svgLib.getTitle(currentFile);
  let desc = svgLib.getDesc(currentFile);
  let attrList = svgLib.getSVGAttrList(currentFile);
  let rectList = svgLib.getRectList(currentFile);
  let circList = svgLib.getCircList(currentFile);
  let pathList = svgLib.getPathList(currentFile);
  let groupList = svgLib.getGroupList(currentFile);

  res.send({

    titleData : title,
    descData : desc,
    attrData : JSON.parse(attrList),
    rectData : JSON.parse(rectList),
    circData : JSON.parse(circList),
    pathData : JSON.parse(pathList),
    groupData : JSON.parse(groupList)
    
  });

});

app.get('/otherAttributes', function(req, res) {

  let currentFile = "uploads/" + req.query.file;
  let index = req.query.index;
  let type = req.query.type;

  let attrList = svgLib.getOtherAttr(currentFile, index, type);

  res.send({
    attrData : JSON.parse(attrList)
  });

});

app.get('/createSVG', function(req, res) {

  let currentFile = req.query.file
  let title = req.query.title;
  let desc = req.query.description;

  let msg;

  let currentFiles = fs.readdirSync("uploads");
  let unique = 0;
  currentFiles.forEach((file) => {

    if (file === currentFile) {
      unique = 1;
      msg = "ERROR: " + currentFile + " already exists.\nPlease change the name.";
    }

  });

  if(unique == 0){

    let svgtoJSON = JSON.stringify({"title": title, "descr": desc});
    console.log(svgtoJSON);
    msg = svgLib.createSVGtoJSON("uploads/"+currentFile, svgtoJSON);
  }

  res.send({
    message : msg
  });

});

app.get('/rename', function(req, res) {

  let currentFile = "uploads/" + req.query.file;
  let type = req.query.chosen;
  let name = req.query.name;

  let msg = svgLib.renameSVG(currentFile, name, type);

  res.send({
    msg : msg
  });

});

app.get('/addShape', function(req, res) {

  let currentFile = "uploads/" + req.query.filename;
  let type = req.query.type;
  let string = req.query.jsonStr;

  let msg = svgLib.addShape(currentFile, string, type);

  res.send({
    message : msg
  });

});

app.get('/editAttr', function(req, res) {

  let currentFile = "uploads/" + req.query.file;
  let index = req.query.index;
  let type = req.query.type;
  let name = req.query.name;
  let value = req.query.value;

  let message = svgLib.editAttr(currentFile, name, value, type, index);

  res.send({
    msg: message,
  });

});

app.get('/scaleSVG', function(req, res) {

  let currentFile = "uploads/" + req.query.filename;
  let shape = req.query.type;
  let scale = req.query.scale;

  let msg = svgLib.scaleShapes(currentFile, shape, scale);

  res.send({
    message: msg,
  });

});

//Sample endpoint
app.get('/endpoint1', function(req , res){
  let retStr = req.query.data1 + " " + req.query.data2;
 
  res.send(
    {
      somethingElse: retStr
    }
  );
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);