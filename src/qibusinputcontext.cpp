#include <QEventLoop>
#include "qibusinputcontext.h"
#include "qibusinputcontextproxy.h"

namespace IBus {

InputContext::InputContext (const BusPointer &bus, const QString &path)
 : m_bus (bus)
{
    m_context = new IBusInputContextProxy ("org.freedesktop.IBus",
                                           path,
                                           m_bus->getConnection ());

    /* commit text */
    QObject::connect (m_context, SIGNAL (CommitText (const QDBusVariant &)),
                      this, SLOT (slotCommitText (const QDBusVariant &)));

    /* preedit text */
    QObject::connect (m_context, SIGNAL (UpdatePreeditText (const QDBusVariant &, uint, bool)),
                      this, SLOT (slotUpdatePreeditText (const QDBusVariant &, uint, bool)));
    QObject::connect (m_context, SIGNAL (ShowPreeditText (void)),
                      this, SLOT (slotShowPreeditText (void)));
    QObject::connect (m_context, SIGNAL (HidePreeditText (void)),
                      this, SLOT (slotHidePreeditText (void)));

    /* auxiliary text */
    QObject::connect (m_context, SIGNAL (UpdateAuxiliaryText (const QDBusVariant &, bool)),
                      this, SLOT (slotUpdateAuxiliaryText (const QDBusVariant &, bool)));
    QObject::connect (m_context, SIGNAL (ShowAuxiliaryText (void)),
                      this, SLOT (slotShowAuxiliaryText (void)));
    QObject::connect (m_context, SIGNAL (HideAuxiliaryText (void)),
                      this, SLOT (slotHideAuxiliaryText (void)));

    /*lookup table */
    QObject::connect (m_context, SIGNAL (UpdateLookupTable (const QDBusVariant &, bool)),
                      this, SLOT (slotUpdateLookupTable (const QDBusVariant &, bool)));
    QObject::connect (m_context, SIGNAL (ShowLookupTable (void)),
                      this, SLOT (slotShowLookupTable (void)));
    QObject::connect (m_context, SIGNAL (HideLookupTable (void)),
                      this, SLOT (slotHideLookupTable (void)));
    QObject::connect (m_context, SIGNAL (CursorUpLookupTable (void)),
                      this, SLOT (slotCursorUpLookupTable (void)));
    QObject::connect (m_context, SIGNAL (CursorDownLookupTable (void)),
                      this, SLOT (slotCursorDownLookupTable (void)));
    QObject::connect (m_context, SIGNAL (PageUpLookupTable (void)),
                      this, SLOT (slotPageUpLookupTable (void)));
    QObject::connect (m_context, SIGNAL (PageDownLookupTable (void)),
                      this, SLOT (slotPageDownLookupTable (void)));

    /* property */
    QObject::connect (m_context, SIGNAL (RegisterProperties (const QDBusVariant &)),
                      this, SLOT (slotRegisterProperties (const QDBusVariant &)));
    QObject::connect (m_context, SIGNAL (UpdateProperty (const QDBusVariant &)),
                      this, SLOT (slotUpdateProperty (const QDBusVariant &)));

    /* others */
    QObject::connect (m_context, SIGNAL (DeleteSurroundingText (int, uint)),
                      this, SLOT (slotDeleteSurroundingText (int, uint)));
    QObject::connect (m_context, SIGNAL (ForwardKeyEvent (uint, uint, uint)),
                      this, SLOT (slotForwardKeyEvent (uint, uint, uint)));
    QObject::connect (m_context, SIGNAL (Enabled (void)),
                      this, SLOT (slotEnabled (void)));
    QObject::connect (m_context, SIGNAL (Disabled (void)),
                      this, SLOT (slotDisabled (void)));

}

InputContext::~InputContext (void)
{
    destroy ();
}

InputContextPointer
InputContext::create (const BusPointer &bus, const QString &name)
{
    Q_ASSERT (!bus.isNull ());

    if (!bus->isConnected ()) {
        qWarning () << "InputContext::create:" << "Bus does not connect!";
        return NULL;
    }

    QString path = bus->createInputContext (name);

    return new InputContext (bus, path);
}


void
InputContext::reset (void)
{
    Q_ASSERT (m_context);
    m_context->Reset ();
}

void
InputContext::destroy (void)
{
    Q_ASSERT (m_context);
    m_context->Destroy ();
    delete m_context;
    m_context = NULL;
}

void
InputContext::enable (void)
{
    Q_ASSERT (m_context);
    m_context->Enable ();
}

void
InputContext::disable (void)
{
    Q_ASSERT (m_context);
    m_context->Disable ();
}

void
InputContext::focusIn (void)
{
    Q_ASSERT (m_context);
    m_context->FocusIn ();
}

void
InputContext::focusOut (void)
{
    Q_ASSERT (m_context);
    m_context->FocusOut ();
}

void
InputContext::getEngine (void)
{
    Q_ASSERT (m_context);
    m_context->GetEngine ();
}

bool
InputContext::isEnabled (void)
{
    Q_ASSERT (m_context);
    return m_context->IsEnabled ();
}

bool
InputContext::processKeyEvent (uint keyval, uint keycode, uint state)
{
    Q_ASSERT (m_context);

    QDBusPendingReply<bool> reply = m_context->ProcessKeyEvent (keyval, keycode, state);

    QEventLoop loop;
    QDBusPendingCallWatcher watcher (reply);

    loop.connect (&watcher, SIGNAL(finished(QDBusPendingCallWatcher *)), SLOT(quit()));
    loop.exec (QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents);

    if (reply.isError ()) {
        qWarning () << "InputContext::processKeyEvent:" << reply.error ();
        return false;
    }

    return reply;
}

void
InputContext::setCapabilities (uint caps)
{
    Q_ASSERT (m_context);
    m_context->SetCapabilities (caps);
}

void
InputContext::setCursorLocation (int x, int y, int w, int h)
{
    Q_ASSERT (m_context);
    m_context->SetCursorLocation (x, y, w, h);
}

void
InputContext::setEngine (const QString &name)
{
    Q_ASSERT (m_context);
    m_context->SetEngine (name);
}

/* slots */
void
InputContext::slotCommitText (const QDBusVariant &text)
{
    commitText (qDBusVariantToSerializable<Text> (text));
}

/* preedit text */
void
InputContext::slotUpdatePreeditText (const QDBusVariant &text, uint cursor_pos, bool visible)
{
    updatePreeditText (qDBusVariantToSerializable<Text> (text), cursor_pos, visible);
}

void
InputContext::slotShowPreeditText ()
{
    showPreeditText ();
}

void
InputContext::slotHidePreeditText ()
{
    hidePreeditText ();
}

/* auxiliary text */
void
InputContext::slotUpdateAuxiliaryText (const QDBusVariant &text, bool visible)
{
    updateAuxiliaryText (qDBusVariantToSerializable<Text> (text), visible);
}

void
InputContext::slotShowAuxiliaryText ()
{
    showAuxiliaryText ();
}

void
InputContext::slotHideAuxiliaryText ()
{
    hideAuxiliaryText ();
}

/* lookup table */
void
InputContext::slotUpdateLookupTable (const QDBusVariant &table, bool visible)
{
    updateLookupTable (qDBusVariantToSerializable<LookupTable> (table), visible);
}

void
InputContext::slotShowLookupTable ()
{
    showLookupTable ();
}

void
InputContext::slotHideLookupTable ()
{
    hideLookupTable ();
}

void
InputContext::slotPageUpLookupTable ()
{
    pageUpLookupTable ();
}

void
InputContext::slotPageDownLookupTable ()
{
    pageDownLookupTable ();
}

void
InputContext::slotCursorUpLookupTable ()
{
    cursorUpLookupTable ();
}

void
InputContext::slotCursorDownLookupTable ()
{
    cursorDownLookupTable ();
}

void
InputContext::slotEnabled ()
{
    enabled ();
}

void
InputContext::slotDisabled ()
{
    disabled ();
}

void
InputContext::slotForwardKeyEvent (uint keyval, uint keycode, uint state)
{
    forwardKeyEvent (keyval, keycode, state);
}

void
InputContext::slotDeleteSurroundingText (int offset, uint nchars)
{
    deleteSurroundingText (offset, nchars);
}

void
InputContext::slotRegisterProperties (const QDBusVariant &props)
{
    registerProperties (qDBusVariantToSerializable<PropList> (props));
}

void
InputContext::slotUpdateProperty (const QDBusVariant &prop)
{
    updateProperty (qDBusVariantToSerializable<Property> (prop));
}

};
