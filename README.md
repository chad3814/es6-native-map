# es6-native-map
==============

This Node.js module provides an interface to a native hashmap data structure with the ES6 Map api. This is significantly faster than the built-in Map, but loses the order of key/value inserts.

As of 2.0.0, es6-native-map requires node.js 0.12 or later. If you are running node.js 0.10, stick with the 1.x.x line.

As of 4.0.0, es6-native-map actually implements @@iterator correctly, so you can use all the iterable functions and operators.

To install, simply:

    npm install es6-native-map

To use:

    var Map = require('es6-native-map');

    var map = new Map();
    map.set('key', {value: 'value'});
    map.set('something', 'else');

    console.log('There are', map.size, 'item(s) in the map');

    iterator = map.entries();
    item = iterator.next();
    while (!item.done) {
        console.log(item.value[0], '=', item.value[1]);
        item = iterator.next();
    }

See the official [ES6 Map documentation](http://people.mozilla.org/~jorendorff/es6-draft.html#sec-map-objects)

This package is made possible because of Grokker, one of the best places to work. If you are a JS developer looking for a new gig, send me an email at &#x5b;'chad', String.fromCharCode(64), 'grokker', String.fromCharCode(0x2e), 'com'&#x5d;.join('').
