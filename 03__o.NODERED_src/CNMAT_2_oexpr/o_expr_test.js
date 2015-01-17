var o = require('./build/Release/odot');
var expr = new o.expr("/anomaly = list(1, 2, 3, 4, 5, 6, \"bass\")");
expr.set("/foo = 1, /bar = 2, /result = /foo + /bar, /temp = /foo, /foo = /bar, /bar = /temp");
var result = expr.eval();
console.log(result);

/*
var udp = require('dgram').createSocket('udp4');
udp.bind(55556);

udp.on('message', function(message, rinfo){
    expr.set("/foo = 1, /bar = 2, /result = /foo + /bar, /temp = /foo, /foo = /bar, /bar = /temp");
    var result = expr.eval(message);
    console.log(result);
    return udp.send(result, 0, result.length, 55555, 'localhost');
});
*/
