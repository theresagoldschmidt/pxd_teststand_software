/****************************************************************************
** Meta object code from reading C++ file 'OvpTest.h'
**
** Created: Mon 12. Feb 14:27:49 2018
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../LightCal/OvpTest.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OvpTest.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_OvpTester[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      11,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   11,   10,   10, 0x05,
      35,   10,   10,   10, 0x05,
      51,   10,   10,   10, 0x05,
      75,   67,   10,   10, 0x05,
      93,   67,   10,   10, 0x05,
     118,  111,   10,   10, 0x05,
     140,  135,   10,   10, 0x05,
     162,  135,   10,   10, 0x05,
     198,  184,   10,   10, 0x05,
     216,  111,   10,   10, 0x05,
     234,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
     251,  247,   10,   10, 0x0a,
     266,   10,   10,   10, 0x0a,
     283,   10,   10,   10, 0x0a,
     301,  135,   10,   10, 0x0a,
     330,  322,   10,   10, 0x0a,
     366,  351,   10,   10, 0x0a,
     387,  322,   10,   10, 0x0a,
     407,   11,   10,   10, 0x0a,
     433,   10,   10,   10, 0x0a,
     448,   10,   10,   10, 0x0a,
     462,   11,   10,   10, 0x0a,
     482,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_OvpTester[] = {
    "OvpTester\0\0i\0SetRelaisChannel(int)\0"
    "GetSmuVoltage()\0GetSmuCurrent()\0channel\0"
    "GetPsVoltage(int)\0GetPsCurrent(int)\0"
    "string\0SendCMD(QString)\0volt\0"
    "SetSmuVoltage(double)\0SetSmuCurrent(double)\0"
    "total,channel\0progress(int,int)\0"
    "SendInfo(QString)\0GetNodeGui()\0str\0"
    "SetPath(char*)\0ReloadSettings()\0"
    "StopMeasurement()\0SmuVoltageIs(double)\0"
    "current\0SmuCurrentIs(double)\0"
    "regulator,load\0PsVoltageIs(int,int)\0"
    "PsCurrentIs(double)\0retrive_channel_info(int)\0"
    "StartOvpTest()\0StopOvpTest()\0"
    "ReceiveNodeGui(int)\0PrepareGateOnTest()\0"
};

void OvpTester::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        OvpTester *_t = static_cast<OvpTester *>(_o);
        switch (_id) {
        case 0: _t->SetRelaisChannel((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->GetSmuVoltage(); break;
        case 2: _t->GetSmuCurrent(); break;
        case 3: _t->GetPsVoltage((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->GetPsCurrent((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->SendCMD((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->SetSmuVoltage((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: _t->SetSmuCurrent((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: _t->progress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 9: _t->SendInfo((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->GetNodeGui(); break;
        case 11: _t->SetPath((*reinterpret_cast< char*(*)>(_a[1]))); break;
        case 12: _t->ReloadSettings(); break;
        case 13: _t->StopMeasurement(); break;
        case 14: _t->SmuVoltageIs((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 15: _t->SmuCurrentIs((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 16: _t->PsVoltageIs((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 17: _t->PsCurrentIs((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 18: _t->retrive_channel_info((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->StartOvpTest(); break;
        case 20: _t->StopOvpTest(); break;
        case 21: _t->ReceiveNodeGui((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 22: _t->PrepareGateOnTest(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData OvpTester::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject OvpTester::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_OvpTester,
      qt_meta_data_OvpTester, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &OvpTester::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *OvpTester::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *OvpTester::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_OvpTester))
        return static_cast<void*>(const_cast< OvpTester*>(this));
    return QObject::qt_metacast(_clname);
}

int OvpTester::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    }
    return _id;
}

// SIGNAL 0
void OvpTester::SetRelaisChannel(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void OvpTester::GetSmuVoltage()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void OvpTester::GetSmuCurrent()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void OvpTester::GetPsVoltage(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void OvpTester::GetPsCurrent(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void OvpTester::SendCMD(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void OvpTester::SetSmuVoltage(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void OvpTester::SetSmuCurrent(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void OvpTester::progress(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void OvpTester::SendInfo(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void OvpTester::GetNodeGui()
{
    QMetaObject::activate(this, &staticMetaObject, 10, 0);
}
QT_END_MOC_NAMESPACE
