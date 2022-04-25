/****************************************************************************
** Meta object code from reading C++ file 'PsRemoteControl.h'
**
** Created: Tue 30. Jan 10:56:34 2018
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../LightCal/PsRemoteControl.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PsRemoteControl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PSMainWin[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      24,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   11,   10,   10, 0x05,
      45,   40,   10,   10, 0x05,
      62,   40,   10,   10, 0x05,
      80,   40,   10,   10, 0x05,
      97,   40,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
     114,   10,   10,   10, 0x0a,
     130,   10,   10,   10, 0x0a,
     141,   10,   10,   10, 0x0a,
     159,   10,   10,   10, 0x0a,
     180,   10,   10,   10, 0x0a,
     193,   10,   10,   10, 0x0a,
     205,   10,   10,   10, 0x0a,
     216,   10,   10,   10, 0x0a,
     237,  223,   10,   10, 0x0a,
     256,   10,   10,   10, 0x0a,
     269,   10,   10,   10, 0x0a,
     285,   10,   10,   10, 0x0a,
     301,   10,   10,   10, 0x0a,
     321,   10,   10,   10, 0x0a,
     341,   10,   10,   10, 0x0a,
     354,   10,   10,   10, 0x0a,
     367,   10,   10,   10, 0x0a,
     379,   10,   10,   10, 0x0a,
     392,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PSMainWin[] = {
    "PSMainWin\0\0string\0receivedData(QString)\0"
    "node\0sigStopNode(int)\0sigStartNode(int)\0"
    "SendOVPNode(int)\0SendNodeGui(int)\0"
    "sessionOpened()\0sendLine()\0sendData(QString)\0"
    "connectionAccepted()\0readSocket()\0"
    "StartNode()\0StopNode()\0test()\0"
    "total,channel\0UpdateBar(int,int)\0"
    "SetChannel()\0SetSMUVoltage()\0"
    "SetSMUCurrent()\0SetSmuCompCurrent()\0"
    "SetSmuCompVoltage()\0SmuSourceI()\0"
    "SmuSourceU()\0SmuEnable()\0SmuDisable()\0"
    "ReadNodeOVP()\0"
};

void PSMainWin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PSMainWin *_t = static_cast<PSMainWin *>(_o);
        switch (_id) {
        case 0: _t->receivedData((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->sigStopNode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->sigStartNode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->SendOVPNode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->SendNodeGui((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->sessionOpened(); break;
        case 6: _t->sendLine(); break;
        case 7: _t->sendData((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->connectionAccepted(); break;
        case 9: _t->readSocket(); break;
        case 10: _t->StartNode(); break;
        case 11: _t->StopNode(); break;
        case 12: _t->test(); break;
        case 13: _t->UpdateBar((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 14: _t->SetChannel(); break;
        case 15: _t->SetSMUVoltage(); break;
        case 16: _t->SetSMUCurrent(); break;
        case 17: _t->SetSmuCompCurrent(); break;
        case 18: _t->SetSmuCompVoltage(); break;
        case 19: _t->SmuSourceI(); break;
        case 20: _t->SmuSourceU(); break;
        case 21: _t->SmuEnable(); break;
        case 22: _t->SmuDisable(); break;
        case 23: _t->ReadNodeOVP(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData PSMainWin::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PSMainWin::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_PSMainWin,
      qt_meta_data_PSMainWin, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PSMainWin::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PSMainWin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PSMainWin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PSMainWin))
        return static_cast<void*>(const_cast< PSMainWin*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int PSMainWin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    }
    return _id;
}

// SIGNAL 0
void PSMainWin::receivedData(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PSMainWin::sigStopNode(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PSMainWin::sigStartNode(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void PSMainWin::SendOVPNode(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void PSMainWin::SendNodeGui(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
