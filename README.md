# es6-native-map
==============

Sometimes you need to store so much data in memory that V8 can get a bit clogged up. This Node.js module provides an interface to a native hashmap data structure that exists outside of V8's memory constraints, with the ES6 Map api. This is the same code as in chad3814/node-hashtable, but under the better name.

To install, simply:

    npm install es6-native-map

To use:

    var Map = require('es6-native-map');

    var map = new Map();
    map.set('key', {value: 'value'});
    map.set('something', 'else');

    console.log('There are', map.size, 'item(s) in the map');

    iterator = map.entries();
    while (!iterator.done) {
        console.log(iterator.key, '=', iterator.value);
        iterator.next();
    }

See the official [ES6 Map documentation](http://people.mozilla.org/~jorendorff/es6-draft.html#sec-map-objects)

This package is made possible because of Grokker, one of the best places to work. If you are a JS developer looking for a new gig, send me an email at &#x5b;'chad', String.fromCharCode(64), 'grokker', String.fromCharCode(0x2e), 'com'&#x5d;.join('').
