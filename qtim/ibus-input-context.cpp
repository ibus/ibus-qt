/* vim:set et ts=4 sts=4 : */
/*
 * ibus - The Input Bus
 *
 * Copyright (c) 2007-2008 Huang Peng <shawn.p.huang@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 */
#include <QtDebug>
#include <QInputMethodEvent>
#include <QTextCharFormat>
#include <QApplication>
#include <QLineEdit>
#include <qibus.h>
#include "ibus-input-context.h"

#ifdef Q_WS_X11
# include <QX11Info>
# include <X11/Xlib.h>
# include <X11/keysym.h>
# include <X11/Xutil.h>
# ifdef HAVE_X11_XKBLIB_H
#  define HAVE_XKB
#  include <X11/XKBlib.h>
# endif
#endif

#include <stdlib.h>
#include <string.h>
#include <unicode/unorm.h>

#include "ibus-compose-data.h"

typedef struct _IBusComposeTableCompact IBusComposeTableCompact;
struct _IBusComposeTableCompact {
    const quint32 *data;
    int max_seq_len;
    int n_index_size;
    int n_index_stride;
};

static const IBusComposeTableCompact ibus_compose_table_compact = {
    ibus_compose_seqs_compact,
    5,
    23,
    6
};

static const uint ibus_compose_ignore[] = {
    Key_Shift_L,
    Key_Shift_R,
    Key_Control_L,
    Key_Control_R,
    Key_Caps_Lock,
    Key_Shift_Lock,
    Key_Meta_L,
    Key_Meta_R,
    Key_Alt_L,
    Key_Alt_R,
    Key_Super_L,
    Key_Super_R,
    Key_Hyper_L,
    Key_Hyper_R,
    Key_Mode_switch,
    Key_ISO_Level3_Shift,
    Key_VoidSymbol
};

typedef QInputMethodEvent::Attribute QAttribute;

IBusInputContext::IBusInputContext (const BusPointer &bus)
    : m_bus (bus),
      m_context (NULL),
      m_preedit (NULL),
      m_preedit_visible (false),
      m_preedit_cursor_pos (0),
      m_has_focus (false),
      m_password_mode (false),
      m_caps (CapPreeditText | CapFocus),
      m_n_compose (0)
{
    Q_ASSERT (!m_bus.isNull ());

    memset (m_compose_buffer, 0, sizeof (m_compose_buffer));

    createInputContext ();

    connect (m_bus, SIGNAL (connected (void)),
             this, SLOT (slotConnected (void)));
    connect (m_bus, SIGNAL (disconnected (void)),
             this, SLOT (slotDisconnected (void)));

}

IBusInputContext::~IBusInputContext (void)
{
    // client->releaseInputContext (this);
    deleteInputContext ();
    m_preedit = NULL;
    m_context = NULL;
    m_bus = NULL;
}


bool
IBusInputContext::filterEvent (const QEvent *event)
{
    return QInputContext::filterEvent (event);
}

QFont
IBusInputContext::font (void) const
{
    return QInputContext::font ();
}

QString
IBusInputContext::identifierName (void)
{
    return QString ("ibus");
}

QString
IBusInputContext::language (void)
{
    return QString ("");
}

void
IBusInputContext::mouseHandler (int x, QMouseEvent *event)
{
    QInputContext::mouseHandler (x, event);
}

void
IBusInputContext::reset (void)
{
}

void
IBusInputContext::update (void)
{
    QWidget *widget = focusWidget ();

    if (widget == NULL || m_context.isNull ()) {
        return;
    }

    QRect rect = widget->inputMethodQuery(Qt::ImMicroFocus).toRect ();

    QPoint topleft = widget->mapToGlobal(QPoint(0,0));
    rect.translate (topleft);

    m_context->setCursorLocation (rect.x (), rect.y (), rect.width (), rect.height ());

#if 0
    QVariant value;
    qDebug () << "== update == ";
    value = widget->inputMethodQuery(Qt::ImMicroFocus);
    qDebug () << "Qt::ImMicroFocus " << value;
    value = widget->inputMethodQuery(Qt::ImFont);
    qDebug () << "Qt::ImFont " <<value;
    value = widget->inputMethodQuery(Qt::ImCursorPosition);
    qDebug () << "Qt::ImCursorPosition " << value;
    value = widget->inputMethodQuery(Qt::ImSurroundingText);
    qDebug () << "Qt::ImSurroundingText " << value;
    value = widget->inputMethodQuery(Qt::ImCurrentSelection);
    qDebug () << "Qt::ImCurrentSelection " << value;
#endif
}

bool
IBusInputContext::isComposing (void) const
{
    return m_preedit_visible && !m_preedit;
}

void
IBusInputContext::setFocusWidget (QWidget *widget)
{
    QInputContext::setFocusWidget (widget);

    m_has_focus = (widget != NULL);

    if (m_context.isNull ())
        return;

    m_password_mode = false;

    if (m_has_focus && widget->inherits ("QLineEdit")) {
        QLineEdit::EchoMode mode =
                qobject_cast<QLineEdit *>(widget)->echoMode();
        if (mode == QLineEdit::Password || mode == QLineEdit::NoEcho) {
            m_password_mode = true;
        }
    }

    if (m_has_focus) {
        m_context->focusIn ();
    }
    else {
        m_context->focusOut ();
    }
    update ();
}

void
IBusInputContext::widgetDestroyed (QWidget *widget)
{
    QInputContext::widgetDestroyed (widget);

    if (m_has_focus) {
        setFocusWidget (NULL);
    }

    update ();
}

#ifdef Q_WS_X11

static inline bool
translate_x_key_event (XEvent *xevent, uint *keyval, uint *keycode, uint *state)
{
    Q_ASSERT (xevent);
    Q_ASSERT (keyval);
    Q_ASSERT (keycode);
    Q_ASSERT (state);

    if (xevent->type != KeyPress && xevent->type != KeyRelease)
        return false;

    *keycode = xevent->xkey.keycode;
    *state = xevent->xkey.state;
    if (xevent->type == KeyRelease)
        *state |= IBus::ReleaseMask;

    char key_str[64];
    if (XLookupString (&xevent->xkey, key_str, sizeof (key_str), (KeySym *)keyval, 0) <= 0) {
        *keyval = (quint32) XLookupKeysym (&xevent->xkey, 0);
    }

    return true;

}

bool
IBusInputContext::x11FilterEvent (QWidget *keywidget, XEvent *xevent)
{
    uint keyval = 0;
    uint keycode = 0;
    uint state = 0;

    if (m_password_mode) {
        return false;
    }

    if (!m_has_focus) {
        m_has_focus = true;
        if (!m_context.isNull ())
            m_context->focusIn ();
    }

    translate_x_key_event (xevent, &keyval, &keycode, &state);
    keycode -= 8;

    if (!m_context.isNull ()) {
        if (m_context->processKeyEvent (keyval, keycode, state)) {
            m_compose_buffer[0] = 0;
            m_n_compose = 0;
            return true;
        }
    }

    if (processCompose (keyval, state)) {
        // qDebug () << "processComose -> true";
        return true;
    }

    return false;
}

bool
IBusInputContext::processCompose (uint keyval, uint state)
{
    int i;

    if (state & IBus::ReleaseMask)
        return false;

    for (i = 0; ibus_compose_ignore[i] != Key_VoidSymbol; i++) {
        if (keyval == ibus_compose_ignore[i])
            return false;
    }

    m_compose_buffer[m_n_compose ++] = keyval;
    m_compose_buffer[m_n_compose] = 0;

    if (checkCompactTable (&ibus_compose_table_compact)) {
        // qDebug () << "checkCompactTable ->true";
        return true;
    }

    if (checkAlgorithmically ()) {
        // qDebug () << "checkAlgorithmically ->true";
        return true;
    }

    if (m_n_compose > 1) {
        QApplication::beep ();
        m_compose_buffer[0] = 0;
        m_n_compose = 0;
        return true;
    }
    else {
        m_compose_buffer[0] = 0;
        m_n_compose = 0;
        return false;
    }
}

static int
compare_seq_index (const void *key, const void *value) {
    const uint *keysyms = (const uint *)key;
    const quint32 *seq = (const quint32 *)value;

    if (keysyms[0] < seq[0])
        return -1;
    else if (keysyms[0] > seq[0])
        return 1;
    return 0;
}

static int
compare_seq (const void *key, const void *value) {
    int i = 0;
    const uint *keysyms = (const uint *)key;
    const quint32 *seq = (const quint32 *)value;

    while (keysyms[i]) {
        if (keysyms[i] < seq[i])
            return -1;
        else if (keysyms[i] > seq[i])
            return 1;
        i++;
    }

    return 0;
}


bool
IBusInputContext::checkCompactTable (const IBusComposeTableCompact *table)
{
    int row_stride;
    const quint32 *seq_index;
    const quint32 *seq;
    int i;

    /* Will never match, if the sequence in the compose buffer is longer
     * than the sequences in the table.  Further, compare_seq (key, val)
     * will overrun val if key is longer than val. */
    if (m_n_compose > table->max_seq_len)
        return false;

    seq_index = (const quint32 *)bsearch (m_compose_buffer,
                                          table->data, table->n_index_size,
                                          sizeof (quint32) * table->n_index_stride,
                                          compare_seq_index);

    if (!seq_index) {
        return false;
    }

    if (seq_index && m_n_compose == 1) {
        return true;
    }

    seq = NULL;
    for (i = m_n_compose-1; i < table->max_seq_len; i++) {
        row_stride = i + 1;

        if (seq_index[i+1] - seq_index[i] > 0) {
            seq = (const quint32 *) bsearch (m_compose_buffer + 1,
                                             table->data + seq_index[i], (seq_index[i+1] - seq_index[i]) / row_stride,
                                             sizeof (quint32) *  row_stride,
                                             compare_seq);
            if (seq) {
                if (i == m_n_compose - 1)
                    break;
                else {
                    return true;
                }
            }
        }
    }

    if (!seq) {
        return false;
    }
    else
    {
        uint value;
        value = seq[row_stride - 1];
        slotCommitText (new Text (QChar (value)));
        m_compose_buffer[0] = 0;
        m_n_compose = 0;
        return true;
    }
    return false;
}

#define IS_DEAD_KEY(k) \
    ((k) >= Key_dead_grave && (k) <= (Key_dead_dasia+1))
quint32 ibus_keyval_to_unicode (uint keyval);

bool
IBusInputContext::checkAlgorithmically ()
{
    int i;
    UChar combination_buffer[IBUS_MAX_COMPOSE_LEN];

    if (m_n_compose >= IBUS_MAX_COMPOSE_LEN)
        return false;

    for (i = 0; i < m_n_compose && IS_DEAD_KEY (m_compose_buffer[i]); i++);
    if (i == m_n_compose)
        return true;

    if (i > 0 && i == m_n_compose - 1) {
        combination_buffer[0] = ibus_keyval_to_unicode (m_compose_buffer[i]);
        combination_buffer[m_n_compose] = 0;
        i--;
        while (i >= 0) {
            switch (m_compose_buffer[i]) {
            #define CASE(keysym, unicode) \
                case Key_dead_##keysym: combination_buffer[i + 1] = unicode; break
            CASE (grave, 0x0300);
            CASE (acute, 0x0301);
            CASE (circumflex, 0x0302);
            CASE (tilde, 0x0303);   /* Also used with perispomeni, 0x342. */
            CASE (macron, 0x0304);
            CASE (breve, 0x0306);
            CASE (abovedot, 0x0307);
            CASE (diaeresis, 0x0308);
            CASE (hook, 0x0309);
            CASE (abovering, 0x030A);
            CASE (doubleacute, 0x030B);
            CASE (caron, 0x030C);
            CASE (abovecomma, 0x0313);         /* Equivalent to psili */
            CASE (abovereversedcomma, 0x0314); /* Equivalent to dasia */
            CASE (horn, 0x031B);    /* Legacy use for psili, 0x313 (or 0x343). */
            CASE (belowdot, 0x0323);
            CASE (cedilla, 0x0327);
            CASE (ogonek, 0x0328);  /* Legacy use for dasia, 0x314.*/
            CASE (iota, 0x0345);
            CASE (voiced_sound, 0x3099);    /* Per Markus Kuhn keysyms.txt file. */
            CASE (semivoiced_sound, 0x309A);    /* Per Markus Kuhn keysyms.txt file. */
            /* The following cases are to be removed once xkeyboard-config,
             * xorg are fully updated.
             **/
            /* Workaround for typo in 1.4.x xserver-xorg */
            case 0xfe66: combination_buffer[i + 1] = 0x314; break;
            /* CASE (dasia, 0x314); */
            /* CASE (perispomeni, 0x342); */
            /* CASE (psili, 0x343); */
            #undef CASE
            default:
                combination_buffer[i + 1] = ibus_keyval_to_unicode (m_compose_buffer[i]);
            }
            i--;
        }

        /* If the buffer normalizes to a single character,
         * then modify the order of combination_buffer accordingly, if necessary,
         * and return TRUE.
         **/
#if 0
      if (check_normalize_nfc (combination_buffer, m_n_compose))
        {
          gunichar value;
          combination_utf8 = g_ucs4_to_utf8 (combination_buffer, -1, NULL, NULL, NULL);
          nfc = g_utf8_normalize (combination_utf8, -1, G_NORMALIZE_NFC);

          value = g_utf8_get_char (nfc);
          gtk_im_context_simple_commit_char (GTK_IM_CONTEXT (context_simple), value);
          context_simple->compose_buffer[0] = 0;

          g_free (combination_utf8);
          g_free (nfc);

          return TRUE;
        }
#endif
        UErrorCode state = U_ZERO_ERROR;
        UChar result[IBUS_MAX_COMPOSE_LEN + 1];
        i = unorm_normalize (combination_buffer, m_n_compose, UNORM_NFC, 0, result, IBUS_MAX_COMPOSE_LEN + 1, &state);

        // qDebug () << "combination_buffer = " << QString::fromUtf16(combination_buffer) << "m_n_compose" << m_n_compose;
        // qDebug () << "result = " << QString::fromUtf16(result) << "i = " << i << state;

        if (i == 1) {
            slotCommitText (new Text (QChar (result[0])));
            m_compose_buffer[0] = 0;
            m_n_compose = 0;
            return true;
        }
    }
    return false;
}
#endif

void
IBusInputContext::createInputContext (void)
{
    if (!m_context.isNull ()) {
        deleteInputContext ();
    }

    if (!m_bus->isConnected ()) {
        qDebug () << "IBusInputContext::createInputContext:" << "no connection to ibus-daemon";
        return;
    }

    m_context = InputContext::create (m_bus, "Qt");

    if (m_context.isNull ()) {
        qWarning () << "IBusInputContext::createInputContext:" << "create input context failed";
        return;
    }

    /* set capabilities */
    m_context->setCapabilities (m_caps);

    connect (m_context, SIGNAL (commitText (const TextPointer &)),
            this, SLOT (slotCommitText (const TextPointer &)));
    connect (m_context, SIGNAL (updatePreeditText (const TextPointer &, uint, bool)),
            this, SLOT (slotUpdatePreeditText (const TextPointer &, uint, bool)));
    connect (m_context, SIGNAL (showPreeditText (void)),
            this, SLOT (slotShowPreeditText (void)));
    connect (m_context, SIGNAL (hidePreeditText (void)),
            this, SLOT (slotHidePreeditText (void)));

    if (m_has_focus) {
        m_context->focusIn ();
    }
}

void
IBusInputContext::deleteInputContext (void)
{
    /* release context */
    m_context = NULL;
}

void
IBusInputContext::slotCommitText (const TextPointer &text)
{
    if (text.isNull ()) {
        qWarning () << "IBusInputContext::commitText:" << "text == null";
        return;
    }

    QInputMethodEvent event;
    event.setCommitString (text->text ());
    sendEvent (event);
    update ();
}

static bool
sortSegments (const AttributePointer &attr_a, const AttributePointer &attr_b)
{
    return attr_a->start () < attr_b->start ();
}

/*
 * KateViewInternal::inputMethodEvent() in kdelibs doesn't support the
 * duplicated format ranges.
 * This function separates each AttributePointer with the minimum size.
 * E.g.
 * attrs[i = 0]: [0            8]
 * attrs[j = 1]:           [6          10]
 * attrs[j = 2]:      [4       8]
 * If i == 0, the default min_end == 8 == attr_i->start () + attr_i->length ()
 * If j == 1, the min_end becomes 6 == attr_j->start ()
 * but it's not the minimum min_end yet.
 * If j == 2, the min_end becomes 4 == attr_j->start ()
 * Now min_end gets the minimum min_end.
 * After checks the min_end between j = 0 to attrs.size(),
 * attr_i gets the minimum min_end.
 * Let's split attr_i's range [0 8] with [0 4] and [4 8]
 * and then
 * attrs[i = 0]: [0   4]
 * attrs[i = 1]:      [4       8]
 * attrs[j = 2]:           [6          10]
 * attrs[j = 3]:      [4       8]
 * increment i = i + 1 == 1 and apply the same split.
 * after split all attrs with the minimum rages, sort the attrs with
 * the attr->start() so that same range formats can be consolidated into
 * one format in IBusInputContext::displayPreeditText().
 */
static inline void
sortAttrs (QList <AttributePointer> &attrs)
{
    for (int i = 0; i < attrs.size (); i++) {
        AttributePointer attr_i = attrs[i];
        uint min_end = attr_i->start () + attr_i->length ();
        for (int j = 0; j < attrs.size (); j++) {
            if (i == j) {
                continue;
            }
            AttributePointer attr_j = attrs[j];
            if (attr_i->start () < attr_j->start () &&
                attr_j->start () < min_end) {
                min_end = attr_j->start ();
            }
            if (attr_i->start () < attr_j->start () + attr_j->length () &&
                attr_j->start () + attr_j->length () < min_end) {
                min_end = attr_j->start () + attr_j->length ();
            }
        }
        if (min_end == attr_i->start () + attr_i->length ()) {
            continue;
        }
        attrs.removeAt (i);
        int n = i;
        AttributePointer attr1 = new Attribute (attr_i->type (),
                                                attr_i->value (),
                                                attr_i->start (),
                                                min_end);
        attrs.insert (n++, attr1);
        AttributePointer attr2 = new Attribute (attr_i->type (),
                                                attr_i->value (),
                                                min_end,
                                                attr_i->end ());
        attrs.insert (n++, attr2);
    }

    qSort (attrs.begin (), attrs.end (), sortSegments);
}

void
IBusInputContext::displayPreeditText (const TextPointer &text, uint cursor_pos, bool visible)
{
    QList <AttributePointer> attrs_sortable;
    QList <QAttribute> qattrs;
    QString string;

    if (visible) {
        // append cursor pos
        qattrs.append (QAttribute (QInputMethodEvent::Cursor, cursor_pos, true, 0));

        AttrListPointer attrs = text->attrs ();
        for (uint i = 0; i < attrs->size (); i++) {
            AttributePointer attr = attrs->get (i);
            attrs_sortable.append (attr);
        }
        sortAttrs (attrs_sortable);

        for (int i = 0; i < attrs_sortable.size (); i++) {
            QTextCharFormat format;
            AttributePointer attr = attrs_sortable[i];
            int size = qattrs.size ();
            // The first index 0 is used by QInputMethodEvent::Cursor above.
            const int first_index = 1;

            if (size > first_index) {
                QAttribute qattr = qattrs[size - 1];
                if(qattr.start == (int) attr->start () &&
                   qattr.length == (int) attr->length ()) {
                    format = qvariant_cast<QTextFormat> (qattr.value).toCharFormat ();
                }
            }

            switch (attr->type ()) {
            case Attribute::TypeUnderline:
                switch (attr->value ()) {
                case Attribute::UnderlineNone:
                    format.setUnderlineStyle (QTextCharFormat::NoUnderline);
                    break;
                case Attribute::UnderlineError:
                    format.setUnderlineStyle (QTextCharFormat::WaveUnderline);
                    break;
                case Attribute::UnderlineSingle:
                case Attribute::UnderlineLow:
                case Attribute::UnderlineDouble:
                default:
                    format.setUnderlineStyle (QTextCharFormat::SingleUnderline);
                    break;
                }
                break;
            case Attribute::TypeForeground:
                format.setForeground (QBrush (QColor (attr->value ())));
                break;
            case Attribute::TypeBackground:
                format.setBackground (QBrush (QColor (attr->value ())));
                break;
            default:
                qWarning () << "IBusInputContext::displayPreeditText:"
                            << "unknow Attribute type" << attr->type ();
                continue;
            }

            if (size > first_index) {
                QAttribute qattr = qattrs[size - 1];
                if(qattr.start == (int) attr->start () &&
                   qattr.length == (int) attr->length()) {
                    qattrs.removeAt (size - 1);
                }
            }

            qattrs.append (QAttribute (QInputMethodEvent::TextFormat,
                                       attr->start (), attr->length (), QVariant (format)));
        }
        QInputMethodEvent event (text->text (), qattrs);
        sendEvent (event);
    }
    else {
        qattrs.append (QAttribute (QInputMethodEvent::Cursor, 0, true, 0));
        QInputMethodEvent event ("", qattrs);
        sendEvent (event);
    }
    update ();
}

void
IBusInputContext::slotUpdatePreeditText (const TextPointer &text, uint cursor_pos, bool visible)
{
    // set visible to false, if text is empty
    visible = visible && !text->text ().isEmpty ();

    // set cursor at end, if pos is greater than the text length
    if (cursor_pos > (uint)text->text ().length ())
        cursor_pos = text->text ().length ();

    bool update = (m_preedit_visible != visible) || visible;

    m_preedit = text;
    m_preedit_visible = visible;
    m_preedit_cursor_pos = cursor_pos;

    if (update) {
        displayPreeditText (m_preedit, m_preedit_cursor_pos, visible);
    }
}

void
IBusInputContext::slotShowPreeditText (void)
{
    if (m_preedit_visible)
        return;
    m_preedit_visible = true;
    displayPreeditText (m_preedit, m_preedit_cursor_pos, m_preedit_visible);
}

void
IBusInputContext::slotHidePreeditText (void)
{
    if (!m_preedit_visible)
        return;
    m_preedit_visible = false;
    displayPreeditText (m_preedit, m_preedit_cursor_pos, m_preedit_visible);
}

void
IBusInputContext::slotConnected (void)
{
    displayPreeditText (m_preedit, m_preedit_cursor_pos, false);
    createInputContext ();
}

void
IBusInputContext::slotDisconnected (void)
{
    displayPreeditText (m_preedit, m_preedit_cursor_pos, false);
    deleteInputContext ();
}
