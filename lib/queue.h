/* NOT YET
 *
 * Queue, Priority Queue, etc.
 *
 * QUEUE q = { 0 };
 * queueCount(q);				// length of queue
 * queueHead(q);				// peek first entry, NULL if empty
 * queueTail(q);				// peek last entry, NULL if empty
 * queuePush(q);				// pushes new first entry and returns it
 * queuePull(q);				// pull first entry, NULL if empty
 * queuePut(q)					// put new last entry and return it
 * queuePop(q);					// pop last entry, NULL if empty
 * queueNext(q);				// get next entry
 * queuePrev(q);				// get prev entry
 * queueSet(q, ptr, sizeof *ptr);		// overwrites data on the queue
 * queueGet(q, ptr, len);			// retrieves data from queue, nondestructive
 * queueData(q);				// return data pointer
 * queueBytes(q);				// return length of data
 * queueRead(q, ptr, len);			// == queueDel(queueGet(queuePull(q), ptr, len))
 * queueWrite(q, ptr, len);			// == queueSet(queuePut(q), ptr, len)
 * queueDel(q);					// remove queue or queue entry
 * queuePrio(q, p);				// change queue priority if p!=0, returns prio
 * queueInc(q);					// queuePrio(q, 1)
 * queueDec(q);					// queuePrio(q, -1)
 * queueMin(q);					// get lowest Prio, NULL if empty
 * queueMax(q);					// get highest Prio, NULL if empty
 * queueFind(q, prio);				// find entry with given prio
 * queueSwap(q, p);				// swap two elements, can be on different QUEUES
 * queueCopy(q, p);				// copies q to p (p and q independent)
 * queueLink(q, p);				// links q to p, overwrites p (p depends on q)
 * queueLink2(q, p);				// queueLink(q, p); queueLink(p, q)
 * queueRel(q, p);				// links without overwrite
 * queueRel2(q, p);				// queueRel(q, p); queueRel(p, q)
 * queueIter(q);				// get snapshot (entries are shared)
 * queueLinks(q);				// get snapshot of links of q
 * queueRels(q);				// get snapshot of rels of q
 *
 * Queue entries are queues themself.  This might be a bit confusing,
 * so be sure not to mix the pointers.
 */

