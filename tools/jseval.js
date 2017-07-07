var readline = require('readline');

var rl = readline.createInterface({
    input: process.stdin,
});

rl.on('line', function(line){
  b = JSON.parse(line);
  console.log(JSON.stringify(eval(b.script)));
})
