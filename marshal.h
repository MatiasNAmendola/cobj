#ifndef MARSHAL_H
#define MARSHAL_H
/**
 * All objects should be able to support marshalling/unmarshalling, but it's not
 * guaranteed to be portable across versions.
 * It's maily for code/dump/load.
 *
 * @link http://en.wikipedia.org/wiki/Marshalling_(computer_science)
 */

COObject *marshal(COObject *co);
COObject *unmarshal(COObject *s);
int marshal_tofile(COObject *co, FILE *fp);
COObject *unmarshal_fromfile(FILE *fp);

#endif
