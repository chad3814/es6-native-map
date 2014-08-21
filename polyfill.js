'use strict';

var Hashtable = require('hashtable');

var MapPoly = function () {
    if (!(this instanceof MapPoly)) {
        return new MapPoly();
    }
    Object.defineProperty(this, '_ht', {
        value: new Hashtable(),
        enumerable: false,
        writable: false,
        configurable: true
    });
};

Object.defineProperties(MapPoly.prototype, {
    clear: {
        value: function () {
            this._ht.clear();
        },
        configurable: true,
        enumerable: false,
        writable: true
    },
    delete: {
        value: function (key) {
            return this._ht.remove(key);
        },
        configurable: true,
        enumerable: false,
        writable: true
    },
    forEach: {
        value: function (cb, context) {
            var self = this;
            return this._ht.forEach(function (key, value) {
                return cb.call(context, value, key, self);
            });
        },
        configurable: true,
        enumerable: false,
        writable: true
    },
    get: {
        value: function (key) {
            return this._ht.get(key);
        },
        configurable: true,
        enumerable: false,
        writable: true
    },
    has: {
        value: function (key) {
            return this.get(key) !== undefined;
        },
        configurable: true,
        enumerable: false,
        writable: true
    },
    set: {
        value: function (key, value) {
            return this._ht.put(key, value);
        },
        configurable: true,
        enumerable: false,
        writable: true
    },
    size: {
        get: function () {
            return this._ht.size();
        },
        configurable: true,
        enumerable: false
    }
});

module.exports = MapPoly;
