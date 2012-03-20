#ifndef SERIALIZE_H
#define SERIALIZE_H
/**
 * COObject serialization/unserialization.
 *
 * All objects should be able to be serialized/unserialized, but it's not guaranteed to be portable across versions.
 * This module is only for code object cache.
 */

COObject *COObject_serialize(COObject *co);
COObject *COObject_unserialize(COObject *s);

#endif
