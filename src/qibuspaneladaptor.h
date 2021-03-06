/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -a qibuspaneladaptor -c IBusPanelAdaptor -v org.freedesktop.IBus.Panel.xml org.freedesktop.IBus.Panel
 *
 * qdbusxml2cpp is Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef QIBUSPANELADAPTOR_H_1268725630
#define QIBUSPANELADAPTOR_H_1268725630

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;

/*
 * Adaptor class for interface org.freedesktop.IBus.Panel
 */
class IBusPanelAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.IBus.Panel")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.freedesktop.IBus.Panel\">\n"
"    <method name=\"UpdateLookupTable\">\n"
"      <arg direction=\"in\" type=\"v\" name=\"lookup_table\"/>\n"
"      <arg direction=\"in\" type=\"b\" name=\"visible\"/>\n"
"    </method>\n"
"    <method name=\"StartSetup\"/>\n"
"    <method name=\"SetCursorLocation\">\n"
"      <arg direction=\"in\" type=\"i\" name=\"x\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"y\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"w\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"h\"/>\n"
"    </method>\n"
"    <method name=\"UpdateAuxiliaryText\">\n"
"      <arg direction=\"in\" type=\"v\" name=\"text\"/>\n"
"      <arg direction=\"in\" type=\"b\" name=\"visible\"/>\n"
"    </method>\n"
"    <method name=\"FocusOut\">\n"
"      <arg direction=\"in\" type=\"o\" name=\"ic\"/>\n"
"    </method>\n"
"    <signal name=\"CandidateClicked\">\n"
"      <arg type=\"u\" name=\"index\"/>\n"
"      <arg type=\"u\" name=\"button\"/>\n"
"      <arg type=\"u\" name=\"state\"/>\n"
"    </signal>\n"
"    <method name=\"HideAuxiliaryText\"/>\n"
"    <method name=\"Destroy\"/>\n"
"    <method name=\"PageDownLookupTable\"/>\n"
"    <method name=\"StateChanged\"/>\n"
"    <method name=\"HideLookupTable\"/>\n"
"    <signal name=\"PageUp\"/>\n"
"    <method name=\"ShowPreeditText\"/>\n"
"    <signal name=\"CursorUp\"/>\n"
"    <method name=\"Reset\"/>\n"
"    <method name=\"UpdateProperty\">\n"
"      <arg direction=\"in\" type=\"v\" name=\"prop\"/>\n"
"    </method>\n"
"    <method name=\"HidePreeditText\"/>\n"
"    <method name=\"CursorUpLookupTable\"/>\n"
"    <method name=\"ShowLookupTable\"/>\n"
"    <method name=\"UpdatePreeditText\">\n"
"      <arg direction=\"in\" type=\"v\" name=\"text\"/>\n"
"      <arg direction=\"in\" type=\"u\" name=\"cursor_pos\"/>\n"
"      <arg direction=\"in\" type=\"b\" name=\"visible\"/>\n"
"    </method>\n"
"    <method name=\"RegisterProperties\">\n"
"      <arg direction=\"in\" type=\"v\" name=\"props\"/>\n"
"    </method>\n"
"    <signal name=\"CursorDown\"/>\n"
"    <method name=\"CursorDownLookupTable\"/>\n"
"    <signal name=\"PropertyActivate\">\n"
"      <arg type=\"s\" name=\"prop_name\"/>\n"
"      <arg type=\"i\" name=\"prop_state\"/>\n"
"    </signal>\n"
"    <method name=\"ShowAuxiliaryText\"/>\n"
"    <signal name=\"PageDown\"/>\n"
"    <method name=\"HideLanguageBar\"/>\n"
"    <signal name=\"PropertyHide\">\n"
"      <arg type=\"s\" name=\"prop_name\"/>\n"
"    </signal>\n"
"    <method name=\"FocusIn\">\n"
"      <arg direction=\"in\" type=\"o\" name=\"ic\"/>\n"
"    </method>\n"
"    <signal name=\"PropertyShow\">\n"
"      <arg type=\"s\" name=\"prop_name\"/>\n"
"    </signal>\n"
"    <method name=\"PageUpLookupTable\"/>\n"
"    <method name=\"ShowLanguageBar\"/>\n"
"  </interface>\n"
        "")
public:
    IBusPanelAdaptor(QObject *parent);
    virtual ~IBusPanelAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void CursorDownLookupTable();
    void CursorUpLookupTable();
    void Destroy();
    void FocusIn(const QDBusObjectPath &ic);
    void FocusOut(const QDBusObjectPath &ic);
    void HideAuxiliaryText();
    void HideLanguageBar();
    void HideLookupTable();
    void HidePreeditText();
    void PageDownLookupTable();
    void PageUpLookupTable();
    void RegisterProperties(const QDBusVariant &props);
    void Reset();
    void SetCursorLocation(int x, int y, int w, int h);
    void ShowAuxiliaryText();
    void ShowLanguageBar();
    void ShowLookupTable();
    void ShowPreeditText();
    void StartSetup();
    void StateChanged();
    void UpdateAuxiliaryText(const QDBusVariant &text, bool visible);
    void UpdateLookupTable(const QDBusVariant &lookup_table, bool visible);
    void UpdatePreeditText(const QDBusVariant &text, uint cursor_pos, bool visible);
    void UpdateProperty(const QDBusVariant &prop);
Q_SIGNALS: // SIGNALS
    void CandidateClicked(uint index, uint button, uint state);
    void CursorDown();
    void CursorUp();
    void PageDown();
    void PageUp();
    void PropertyActivate(const QString &prop_name, int prop_state);
    void PropertyHide(const QString &prop_name);
    void PropertyShow(const QString &prop_name);
};

#endif
