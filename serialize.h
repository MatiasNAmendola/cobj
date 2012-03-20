#ifndef SERIALIZE_H
#define SERIALIZE_H
/**
 * COObject serialization/unserialization.
 *
 * All objects should be able to be serialized/unserialized, but it's not guaranteed to be portable across versions.
 * This serialization/unserialization is maily for code dump/undump.
 */

COObject *COObject_serialize(COObject *co);
COObject *COObject_unserialize(COObject *s);
COObject *COObject_serializeToFile(COObject *co, FILE *fp);
COObject *COObject_unserializeFromFile(FILE *fp);

#endif
