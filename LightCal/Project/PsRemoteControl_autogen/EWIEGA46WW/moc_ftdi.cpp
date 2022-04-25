/****************************************************************************
** Meta object code from reading C++ file 'ftdi.h'
**
** Created: Mon 13. Nov 13:48:08 2017
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../LightCal/ftdi.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ftdi.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FTDI[] = {

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
      13,    6,    5,    5, 0x05,

 // slots: signature, parameters, type, tag, flags
      33,   31,    5,    5, 0x0a,
      50,    5,    5,    5, 0x0a,
      68,    5,    5,    5, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_FTDI[] = {
    "FTDI\0\0String\0SendInfo(QString)\0i\0"
    "SET_Channel(int)\0FTDI_OpenDevice()\0"
    "reset()\0"
};

void FTDI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FTDI *_t = static_cast<FTDI *>(_o);
        switch (_id) {
        case 0: _t->SendInfo((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->SET_Channel((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->FTDI_OpenDevice(); break;
        case 3: _t->reset(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FTDI::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FTDI::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_FTDI,
      qt_meta_data_FTDI, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FTDI::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FTDI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FTDI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FTDI))
        return static_cast<void*>(const_cast< FTDI*>(this));
    return QObject::qt_metacast(_clname);
}

int FTDI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void FTDI::SendInfo(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
