'use strict';

if (!require('./is-implemented')()) {
    Object.defineProperty(global, 'Map', {
        value: require('./polyfill'),
        configurable: true,
        enumerable: false,
        writable: true
    });
}
