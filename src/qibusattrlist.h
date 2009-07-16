#ifndef __Q_IBUS_ATTRLIST_H_
#define __Q_IBUS_ATTRLIST_H_

#include "qibusattribute.h"
#include <QVector>

namespace IBus {

class AttrList : public Serializable {
    Q_OBJECT;
public:
    AttrList () {}
    AttrList (int size): m_attrs(size) {};
    virtual ~AttrList ();
    AttributePointer get(uint index);
    void append(const AttributePointer &attr);

    virtual bool serialize (QDBusArgument &argument) const;
    virtual bool deserialize (const QDBusArgument &argument);
private:
    QVector<AttributePointer> m_attrs;

    IBUS_SERIALIZABLE
};

} /* IBUS */


#endif
