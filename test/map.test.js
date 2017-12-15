'use strict';

const test = require('tape');

// run tests on builtin Map as well as native C++ implementation
[['builtin', Map], ['native', require('../index.js')]].forEach(([mapType, Map]) => {

  test(`test ${mapType} constructor`, (assert) => {
    assert.doesNotThrow(() => {new Map();}, 'can construct an empty new Map()');
    // see array constructor tests below
    assert.throws(() => {new Map([1,2,3,4]);}, TypeError, 'cannot construct a Map from a flat array');
    assert.throws(() => {new Map({ 1:2, 3:4 });}, TypeError, 'cannot construct a Map from an object');
    assert.end();
  });

  test(`test ${mapType} set method`, (assert) => {
    const myMap = new Map();
    const startSize = myMap.size;

    assert.doesNotThrow(() => {myMap.set('a string', 'value');}, 'can set string key to a value');
    assert.equal(myMap.size, startSize + 1, 'setting key-value increases map size by 1');

    assert.doesNotThrow(() => {myMap.set({}, 'value');}, 'can set object key to a value');
    assert.doesNotThrow(() => {myMap.set(()=>{}, 'value');}, 'can set function key to a value');

    assert.doesNotThrow(() => {myMap.set(2,3,4);}, 'can call set with more than 2 non-null arguments');
    assert.ok(myMap.has(2) && myMap.get(2) === 3, 'set ignores arguments after 2nd');

    assert.end();
  });

  test(`test ${mapType} get method`, (assert) => {
    const m = new Map([[1, 2], [3, 4], [{}, 6]]);
    assert.equal(m.get(1), 2, 'get returns the value associated with an existing key');
    assert.equal(m.get(5), undefined, 'get returns undefined for a nonexistent key');
    assert.equal(m.get({}), undefined, 'get returns undefined for nonidentical keys');
    assert.end();
  });

  test(`test ${mapType} has method`, (assert) => {
    const empty = new Map();
    assert.notOk(empty.has('anything') || empty.has(1) || empty.has(''), 'empty map has no keys');
    const obj = { 'some': 'object' };
    const fun = () => { console.log('some function'); };
    const m = new Map([[1, 2], [3, 4], [()=>{}, 6], [{}, 8], [obj, 10], [fun, 12]]);
    assert.ok(m.has(1) && m.has(3), 'has returns true for existing keys');
    assert.notOk(m.has(2) || m.has(4), 'has returns false for existing values');
    assert.notOk(m.has(5) || m.has('missing'), 'has returns false for nonexistent keys');
    assert.notOk(m.has({}) || m.has(() => {}), 'has returns false for nonidentical object/function keys');
    assert.ok(m.has(obj) && m.has(fun), 'has returns true for identical object/function keys');
    assert.end();
  });

  test(`test ${mapType} delete method`, (assert) => {
    const m = new Map([[1,2],[3,4]]);
    const startSize = m.size;
    assert.ok(m.delete(1), 'deleting an existing key returns true');
    assert.notOk(m.has(1), 'after deleting map no longer has key');
    assert.equal(m.size, startSize - 1, 'deleting reduces size by 1');
    assert.notOk(m.delete(5), 'deleting a nonexistent key returns false');
    assert.equal(m.size, startSize - 1, 'deleting nonexistent key does not change size');
    assert.end();
  });

  test(`test ${mapType} clear method`, (assert) => {
    const m = new Map([[1,2],[3,4]]);
    assert.equal(m.size, 2, 'before clearing map has size > 0');
    assert.doesNotThrow(() => {m.clear();}, 'nonempty map can be cleared');
    assert.equal(m.size, 0, 'after clearing map has size == 0');
    const empty = new Map();
    assert.doesNotThrow(() => {empty.clear();}, 'empty map can be cleared');
    assert.equal(empty.size, 0, 'after clearing map has size == 0');
    assert.end();
  });


  test(`test ${mapType} iteration with for..of`, (assert) => {
    const myMap = new Map();
    myMap.set(0, 'zero');
    myMap.set(1, 'one');

    const objKeys = [];
    const objValues = [];
    assert.doesNotThrow(() => {
      for (let [key, value] of myMap) {
        objKeys.push(key);
        objValues.push(value);
      }
    }, 'can iterate over map object with for..of');
    assert.deepEquals(objKeys.sort(), [0,1].sort(), 'object iterator includes all keys');
    assert.skip('object keys iterate in insertion order');
    assert.deepEquals(objValues.sort(), ['zero', 'one'].sort(), 'object iterator includes all values');
    assert.skip('object values iterate in insertion order');

    const keys = [];
    assert.doesNotThrow(() => {
      for (let key of myMap.keys()) {
        keys.push(key);
      }
    }, 'can iterate over map.keys() with for..of');
    assert.deepEquals(keys.sort(), [0,1].sort(), 'key iterator includes all keys');
    assert.skip('keys iterate in insertion order');

    const values = [];
    assert.doesNotThrow(() => {
      for (let value of myMap.values()) {
        values.push(value);
      }
    }, 'can iterate over map.values() with for..of');
    assert.deepEquals(values.sort(), ['zero', 'one'].sort(), 'value iterator includes all values');
    assert.skip('values iterate in insertion order');

    const entryKeys = [];
    const entryValues = [];
    assert.doesNotThrow(() => {
      for (let [key, value] of myMap.entries()) {
        entryKeys.push(key);
        entryValues.push(value);
      }
    }, 'can iterate over map.entries() with for..of');
    assert.deepEquals(entryKeys.sort(), [0,1].sort(), 'entries iterator includes all keys');
    assert.deepEquals(entryValues.sort(), ['zero', 'one'].sort(), 'entries iterator includes all values');
    assert.skip('entry keys iterate in insertion order');
    assert.skip('entry values iterate in insertion order');

    assert.end();
  });


  test(`test ${mapType} iteration with .forEach()`, (assert) => {
    const myMap = new Map();
    myMap.set(0, 'zero');
    myMap.set(1, 'one');

    const keys = [];
    const values = [];
    assert.doesNotThrow(() => {
      myMap.forEach((value, key) => {
        keys.push(key);
        values.push(value);
      });
    }, 'can iterate over map object with .forEach()');
    assert.deepEquals(keys.sort(), [0,1].sort(), 'object iterator includes all keys');
    assert.deepEquals(values.sort(), ['zero', 'one'].sort(), 'object iterator includes all values');
    assert.skip('object keys iterate in insertion order');
    assert.skip('object values iterate in insertion order');

    assert.end();
  });


  test(`test ${mapType} relation with Array objects`, (assert) => {
    const kvArray = [['key1', 'value1'], ['key2', 'value2']];
    let myMap;

    assert.doesNotThrow(() => {myMap = new Map(kvArray);}, 'can create map from key-value pair array');

    let mapArray;
    assert.doesNotThrow(() => {mapArray = Array.from(myMap);}, 'can create Array from map object');
    assert.deepEquals(mapArray.sort(), kvArray.sort(), 'array from map object has all key-value pairs');

    let entryArray;
    assert.doesNotThrow(() => {entryArray = Array.from(myMap.entries());}, 'can create Array from map.entries()');
    assert.deepEquals(entryArray.sort(), kvArray.sort(), 'array from map.entries() has all key-value pairs');

    let keyArray;
    assert.doesNotThrow(() => {keyArray = Array.from(myMap.keys());}, 'can create Array from map.keys()');
    assert.deepEquals(keyArray.sort(), ['key1', 'key2'].sort(), 'array from map.keys() has all keys');
    assert.skip('array from map.keys() has keys in order');

    let valueArray;
    assert.doesNotThrow(() => {valueArray = Array.from(myMap.values());}, 'can create Array from map.values()');
    assert.deepEquals(valueArray.sort(), ['value1', 'value2'].sort(), 'array from map.values() has all values');
    assert.skip('array from map.values() has values in order');

    assert.end();
  });

});
