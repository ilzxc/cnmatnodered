( function(){
    var o = require('./build/Release/o_parse');

    for (var i = 0; i < 100000; ++i) {
	var string = "/foo : " + i + ", /bar : " + (i * -2);
	var test = o.parse(string);
	console.log(test);
    }

    var test_bundle = o.parse("/foo : 10, /bar : 20");
    console.log(test_bundle);

}() );
