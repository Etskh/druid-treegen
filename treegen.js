'use strict';

var tree = require("./build/Release/tree");

module.exports.generate = function( config, cb ) {
  cb( tree.generate(config));
};
