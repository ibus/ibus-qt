#ifndef __Q_IBUS_INPUT_CONTEXT_H_
#define __Q_IBUS_INPUT_CONTEXT_H_

#include "qibusbus.h"
#include "qibustext.h"
#include "qibusproperty.h"
#include "qibusproplist.h"
#include "qibuslookuptable.h"

class IBusInputContextProxy;

namespace IBus {

class Bus;
typedef Pointer<Bus> BusPointer;

class InputContext;
typedef Pointer<InputContext> InputContextPointer;

class InputContext : public Object
{
    Q_OBJECT;

public:
    InputContext (const BusPointer &bus, const QString &path);
    ~InputContext (void);

    void reset (void);
    void destroy (void);
    void enable (void);
    void disable (void);
    void focusIn (void);
    void focusOut (void);
    void getEngine (void);
    bool isEnabled (void);
    bool processKeyEvent (uint keyval, uint keycode, uint state);
    void setCapabilities (uint caps);
    void setCursorLocation (int x, int y, int w, int h);
    void setEngine (const QString &name);

public:
    static InputContextPointer create (const BusPointer &bus, const QString &name);

Q_SIGNALS:
    void commitText (const TextPointer &text);

    void updatePreeditText (const TextPointer &text, uint cursor_pos, bool visible);
    void showPreeditText (void);
    void hidePreeditText (void);

    void updateAuxiliaryText (const TextPointer &text, bool visible);
    void showAuxiliaryText (void);
    void hideAuxiliaryText (void);

    void updateLookupTable (const LookupTablePointer &table, bool visible);
    void showLookupTable (void);
    void hideLookupTable (void);
    void cursorUpLookupTable (void);
    void cursorDownLookupTable (void);
    void pageUpLookupTable (void);
    void pageDownLookupTable (void);

    void registerProperties (const PropListPointer &props);
    void updateProperty (const PropertyPointer &property);

    void enabled (void);
    void disabled (void);

    void forwardKeyEvent (uint keyval, uint keycode, uint state);
    void deleteSurroundingText (int offset, uint nchars);

private Q_SLOTS:
    void slotCommitText (const QDBusVariant &text);

    void slotUpdatePreeditText(const QDBusVariant &text, uint cursor_pos, bool visible);
    void slotHidePreeditText();
    void slotShowPreeditText();

    void slotUpdateAuxiliaryText(const QDBusVariant &text, bool visible);
    void slotShowAuxiliaryText();
    void slotHideAuxiliaryText();

    void slotUpdateLookupTable(const QDBusVariant &table, bool visible);
    void slotShowLookupTable();
    void slotHideLookupTable();
    void slotCursorUpLookupTable();
    void slotCursorDownLookupTable();
    void slotPageUpLookupTable();
    void slotPageDownLookupTable();

    void slotRegisterProperties(const QDBusVariant &props);
    void slotUpdateProperty(const QDBusVariant &prop);

    void slotEnabled();
    void slotDisabled();

    void slotForwardKeyEvent(uint keyval, uint keycode, uint state);
    void slotDeleteSurroundingText(int offset, uint nchars);

/*
signals:
    void disconnected (void);
    void connected (void);

private slots:
    void slotAddressChanged (const QString &path);
    void slotIBusDisconnected (void);
*/
private:
    BusPointer m_bus;
    IBusInputContextProxy *m_context;
};

};

#endif
