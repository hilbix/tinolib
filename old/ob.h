/* $Header$
 *
 * NOT YET IMPLEMENTED
 *
 * Generic polymorphic polyinheritant Objects.
 *
 * Objects are a little bit like in pike or python.
 * You can replace their (base) "classes" in the runtime.
 *
 * There is nothing like C++ need or anything to understand or test.
 * Objects have types, however there are no special types.
 * All objects can be used interchangeably for anything.
 *
 * Objects are polymorphic, such that any object can hold anything.
 * If you assign an int to it, it is an int object.
 * You can change it to contain a string later on then it
 * becomes a string object.
 *
 * Object are polyinheritant.  In fact there is nothing like a
 * heritage, it's more like borrowing.  If an object misses some
 * functionality, it can "borrow" it from a base object.
 * This base object is another object, like any object.
 *
 * If you need something like a "class" for an object, this is
 * just another object which is thought to describe the class.
 *
 * It's even more easy than that.  You have several "levels" of
 * structuring, that's all:
 * Level 0 is the contents of the object
 * Level 1 are the object functions
 * Level 2 is the inheritance or base class feature.
 *         This is where base functions are looked for.
 * Level 3 are the class types.
 *         This is, which class is an object.  Multi-class is supported.
 * Level 4 is reserved
 * Level 5 is used to implement containers.
 *         Any object can be in more than one container.
 * Level 6 and 7 are reserved
 * Level 8 to 15 are free for your use.
 *
 * Note about freeing object:
 *
 * Objects can be garbage collected.
 * For this there are some "base objects" which are marked "in use".
 * All objects which are marked "in use" and all dependent objects
 * are not freed by the garbage collector.
 * However, garbage colletion takes time and is an iterative process.
 *
 * To make it more easy, you can free objects directly.
 * Then they are primarily marked "not in use".
 * However there are possibilities where objects cannot be destroyed
 * immediately.  For example, an object controlling a process might
 * want to HUP the process first and only destroy it if it does
 * not react within 30 minutes.  You will not want to wait so long.
 * Or the object can only be freed if some depending object are
 * freed before, but some of these might be still in use.
 *
 * So to free the memory immediately, it is best to free it in the
 * opposit order you created the objects, and have all references
 * to other objects resolved (broken) first.  If you do this,
 * there is no problem, and the memory is freed immediately.
 *
 * You also can choose if you want to free once or recoursive.
 * Recoursive means, all directly depending objects (and so on)
 * are destroyed as well.  The recoursion, however, stops at
 * objects which cannot be freed immediately (like on circular
 * rings R->A->B->C->D->A).  If you destroy R, then R can be freed,
 * but A cannot be freed as D points to A, and the recoursion stops.
 *
 * In this case you can solve this by destroying A, B, C or D before
 * or after R.  However this case is easy, usually it's more complex.
 *
 * Complex dependicies can only be handled by the garbage collector.
 *
 * $Log$
 * Revision 1.1  2004-05-19 05:00:04  tino
 * idea added
 *
 */

#ifndef tino_INC_ob_h
#define tino_INC_ob_h

typedef struct tino_ob *TINO_OB;
static int		tino_ob_chunksize	= 4096;

/* Call this first with the number of expected objects.
 * This will set how many object structures will be preallocated.
 * This way pointers to objects can be faster detected to be valid.
 *
 * If you choose wisely you can greatly improve the running speed.
 * If you choose badly, you either degrade running speed or waste memory.
 */
static void
tino_ob_init(int chunk)
{
  tino_ob_chunksize	= chunk;
  000;
}

/* Is the pointer pointing to a valid object
 *
 * Always no, objects do not exist for now.
 */
static void
tino_ob_valid(TINO_OB ob)
{
  000;
  return 0;
}

/* Destroy an object
 *
 * If you just want to destroy this object, use 0 as recoursion level.
 * If you want to recourse, give the depth >0 there.
 * If you want no limit, use -1 as recourse.
 */
static void
tino_ob_destroy(TINO_OB ob, int recourse)
{
  FATAL(!tino_ob_valid(ob));
  000;
}

#endif
