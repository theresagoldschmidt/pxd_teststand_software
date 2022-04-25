/****************************************************************************
** Meta object code from reading C++ file 'SMU.h'
**
** Created: Mon 20. Nov 12:17:50 2017
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../LightCal/SMU.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SMU.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SMU[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      26,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: signature, parameters, type, tag, flags
       5,    4,    4,    4, 0x05,
      22,    4,    4,    4, 0x05,
      44,   39,    4,    4, 0x05,
      76,   68,    4,    4, 0x05,
     107,  100,    4,    4, 0x05,
     130,  125,    4,    4, 0x05,

 // slots: signature, parameters, type, tag, flags
     149,    4,    4,    4, 0x0a,
     164,    4,    4,    4, 0x0a,
     185,  180,    4,    4, 0x0a,
     207,  180,    4,    4, 0x0a,
     226,    4,    4,    4, 0x0a,
     239,    4,    4,    4, 0x0a,
     259,    4,  252,    4, 0x0a,
     278,    4,    4,    4, 0x0a,
     293,    4,    4,    4, 0x0a,
     309,    4,    4,    4, 0x0a,
     325,    4,    4,    4, 0x0a,
     344,    4,    4,    4, 0x0a,
     363,   39,    4,    4, 0x0a,
     392,   68,    4,    4, 0x0a,
     421,    4,    4,    4, 0x0a,
     445,    4,    4,    4, 0x0a,
     467,    4,    4,    4, 0x0a,
     482,    4,    4,    4, 0x0a,
     501,    4,    4,    4, 0x0a,
     521,   39,    4,    4, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SMU[] = {
    "SMU\0\0VoltageApplied()\0CurrentApplied()\0"
    "volt\0MeasuredVoltage(double)\0current\0"
    "MeasuredCurrent(double)\0string\0"
    "SendInfo(QString)\0mode\0OutputStatus(long)\0"
    "open_session()\0reset_session()\0curr\0"
    "SourceCurrent(double)\0SetVoltage(double)\0"
    "GetCurrent()\0GetVoltage()\0double\0"
    "GetVoltageDouble()\0EnableOutput()\0"
    "DisableOutput()\0GetOutputStat()\0"
    "SetSourceCurrent()\0SetSourceVoltage()\0"
    "SetVoltageCompliance(double)\0"
    "SetCurrentCompliance(double)\0"
    "SetMeasureRangeManual()\0SetMeasureRangeAuto()\0"
    "SetApperture()\0SetRemoteSenseOn()\0"
    "SetRemoteSenseOff()\0SetMeasureRange(double)\0"
};

void SMU::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SMU *_t = static_cast<SMU *>(_o);
        switch (_id) {
        case 0: _t->VoltageApplied(); break;
        case 1: _t->CurrentApplied(); break;
        case 2: _t->MeasuredVoltage((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->MeasuredCurrent((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->SendInfo((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->OutputStatus((*reinterpret_cast< long(*)>(_a[1]))); break;
        case 6: _t->open_session(); break;
        case 7: _t->reset_session(); break;
        case 8: _t->SourceCurrent((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->SetVoltage((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 10: _t->GetCurrent(); break;
        case 11: _t->GetVoltage(); break;
        case 12: { double _r = _t->GetVoltageDouble();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 13: _t->EnableOutput(); break;
        case 14: _t->DisableOutput(); break;
        case 15: _t->GetOutputStat(); break;
        case 16: _t->SetSourceCurrent(); break;
        case 17: _t->SetSourceVoltage(); break;
        case 18: _t->SetVoltageCompliance((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 19: _t->SetCurrentCompliance((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 20: _t->SetMeasureRangeManual(); break;
        case 21: _t->SetMeasureRangeAuto(); break;
        case 22: _t->SetApperture(); break;
        case 23: _t->SetRemoteSenseOn(); break;
        case 24: _t->SetRemoteSenseOff(); break;
        case 25: _t->SetMeasureRange((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SMU::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SMU::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SMU,
      qt_meta_data_SMU, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SMU::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SMU::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SMU::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SMU))
        return static_cast<void*>(const_cast< SMU*>(this));
    return QObject::qt_metacast(_clname);
}

int SMU::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 26)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 26;
    }
    return _id;
}

// SIGNAL 0
void SMU::VoltageApplied()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void SMU::CurrentApplied()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void SMU::MeasuredVoltage(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SMU::MeasuredCurrent(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void SMU::SendInfo(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void SMU::OutputStatus(long _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
