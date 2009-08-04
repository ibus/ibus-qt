#include "qibusproperty.h"
#include "qibusproplist.h"

namespace IBus {

IBUS_DECLARE_SERIALIZABLE(PropList, IBusPropList);

PropList::~PropList ()
{   
    int i = 0;
    for ( ; i < m_props.size(); ++i )
    {
        if ( m_props[i] )
            delete m_props[i];
    }
}  

bool
PropList::serialize (QDBusArgument & argument) const
{
    if (!Serializable::serialize (argument))
        return false;

    argument.beginArray(QDBusArgument::VariantType);
    for ( int i = 0; i < m_props.size(); ++i )
        argument << m_props[i];
    argument.endArray();

    return true;
}

bool
PropList::deserialize (const QDBusArgument & argument)
{
    if (!Serializable::deserialize (argument))
        return false;

    argument.beginArray();
    while ( !argument.atEnd() )
    {
        PropertyPointer prop;
        argument >> prop;
        m_props.append(prop);
    }
    argument.endArray();

    return true;
}

bool
PropList::updateProperty (const Property & propUpdate)
{
    for ( int i = 0; i < m_props.size(); ++i )
    {
        m_props[i]->update(propUpdate);
    }

    return true;
}
};
