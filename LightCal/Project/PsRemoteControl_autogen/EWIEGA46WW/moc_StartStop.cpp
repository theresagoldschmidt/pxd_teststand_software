/****************************************************************************
** Meta object code from reading C++ file 'StartStop.h'
**
** Created: Mon 29. Jan 10:57:17 2018
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../LightCal/StartStop.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StartStop.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_StartStop[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   11,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      38,   10,   10,   10, 0x0a,
      59,   54,   10,   10, 0x0a,
      75,   54,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_StartStop[] = {
    "StartStop\0\0string\0SeqSendCMD(QString)\0"
    "SeqReceiveCMD()\0node\0SeqStartUp(int)\0"
    "SeqStopPS(int)\0"
};

void StartStop::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        StartStop *_t = static_cast<StartStop *>(_o);
        switch (_id) {
        case 0: _t->SeqSendCMD((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->SeqReceiveCMD(); break;
        case 2: _t->SeqStartUp((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->SeqStopPS((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData StartStop::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject StartStop::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_StartStop,
      qt_meta_data_StartStop, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &StartStop::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *StartStop::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *StartStop::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_StartStop))
        return static_cast<void*>(const_cast< StartStop*>(this));
    return QObject::qt_metacast(_clname);
}

int StartStop::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void StartStop::SeqSendCMD(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
