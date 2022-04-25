/****************************************************************************
** Meta object code from reading C++ file 'CalManager.h'
**
** Created: Mon 13. Nov 13:48:07 2017
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../LightCal/CalManager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CalManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CalManager[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   12,   11,   11, 0x05,
      36,   11,   11,   11, 0x05,
      52,   11,   11,   11, 0x05,
      76,   68,   11,   11, 0x05,
      94,   68,   11,   11, 0x05,
     119,  112,   11,   11, 0x05,
     141,  136,   11,   11, 0x05,
     163,  136,   11,   11, 0x05,
     199,  185,   11,   11, 0x05,
     217,  112,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     235,   11,   11,   11, 0x0a,
     253,   11,   11,   11, 0x0a,
     277,  273,   11,   11, 0x0a,
     292,  136,   11,   11, 0x0a,
     321,  313,   11,   11, 0x0a,
     357,  342,   11,   11, 0x0a,
     378,  313,   11,   11, 0x0a,
     398,   11,   11,   11, 0x0a,
     415,  112,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CalManager[] = {
    "CalManager\0\0i\0SetRelaisChannel(int)\0"
    "GetSmuVoltage()\0GetSmuCurrent()\0channel\0"
    "GetPsVoltage(int)\0GetPsCurrent(int)\0"
    "string\0SendCMD(QString)\0volt\0"
    "SetSmuVoltage(double)\0SetSmuCurrent(double)\0"
    "total,channel\0progress(int,int)\0"
    "SendInfo(QString)\0StopMeasurement()\0"
    "start_cal_session()\0str\0SetPath(char*)\0"
    "SmuVoltageIs(double)\0current\0"
    "SmuCurrentIs(double)\0regulator,load\0"
    "PsVoltageIs(int,int)\0PsCurrentIs(double)\0"
    "ReloadSettings()\0TakeData(QString)\0"
};

void CalManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CalManager *_t = static_cast<CalManager *>(_o);
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
        case 10: _t->StopMeasurement(); break;
        case 11: _t->start_cal_session(); break;
        case 12: _t->SetPath((*reinterpret_cast< char*(*)>(_a[1]))); break;
        case 13: _t->SmuVoltageIs((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 14: _t->SmuCurrentIs((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 15: _t->PsVoltageIs((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 16: _t->PsCurrentIs((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 17: _t->ReloadSettings(); break;
        case 18: _t->TakeData((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CalManager::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CalManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CalManager,
      qt_meta_data_CalManager, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CalManager::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CalManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CalManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CalManager))
        return static_cast<void*>(const_cast< CalManager*>(this));
    return QObject::qt_metacast(_clname);
}

int CalManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    }
    return _id;
}

// SIGNAL 0
void CalManager::SetRelaisChannel(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CalManager::GetSmuVoltage()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void CalManager::GetSmuCurrent()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void CalManager::GetPsVoltage(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void CalManager::GetPsCurrent(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void CalManager::SendCMD(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void CalManager::SetSmuVoltage(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void CalManager::SetSmuCurrent(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void CalManager::progress(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void CalManager::SendInfo(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_END_MOC_NAMESPACE
