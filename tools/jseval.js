var readline = require('readline');

var rl = readline.createInterface({
    input: process.stdin,
});

Date.prototype.toJSON = function() {
  return {"__espress__": "Date", "__value__": this-0};
}

rl.on('line', function(line){
  b = JSON.parse(line);
  console.log(JSON.stringify(eval(b.script), function(k, x) {
    var t = typeof(x);
    if (t == 'number' && isNaN(x)) return {"__espress__": "NaN"};
    if (t == 'number' && x == 1/0) return {"__espress__": "Inf"};
    if (t == 'number' && x == -1/0) return {"__espress__": "-Inf"};
    if (t == 'undefined' && x === undefined) return {"__espress__": "undef"};
    return x;
  }));
})
