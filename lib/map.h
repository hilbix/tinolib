/* NOT YET
 *
 * Simple key/value map
 *
 * MAP key/value are int64_t
 *
 * MAP m = { 0 };
 * mapInc(m, key);		// increment key, returns count
 * mapDec(m, key);		// decrement key, returns count
 * mapAdd(m, key, val);		// 1=added, 0=key exists: val not overwritten
 * mapSet(m, key, val);		// create a value, returns old value or 0
 * mapGet(m, key);		// return value, 0 if key not found
 * mapRet(m, key, &val);	// 1=found, 0=notfound: retrieve val
 * mapSwap(m, key, &val);	// 1=found, 0=notfound: swap val
 * mapHas(m, key);		// 1=found, 0=notfound
 * mapDel(m, key);		// 1=deleted, 0=notfound
 * mapNext(m, key);		// return next (higher) key or 0, wraps on MAX_INT64
 * mapPrev(m, key);		// return next (lower)  key or 0, wraps on MIN_INT64
 * mapMin(m, key);		// minimum positive key, -1 if none
 * mapMax(m, key);		// maximum positive key, -1 if none
 * mapLow(m, key);		// lowest key (integer wise), 0 if none
 * mapNeg(m, key);		// highest negative key, 0 if none
 * mapHigh(m, key);		// highest key (integer wise), 0 if none
 */

