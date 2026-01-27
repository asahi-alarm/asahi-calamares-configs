/****************************************************************************
** Meta object code from reading C++ file 'NetworkSetupViewStep.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "NetworkSetupViewStep.h"
#include <QtCore/qmetatype.h>
#include <QtCore/qplugin.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworkSetupViewStep.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN20NetworkSetupViewStepE_t {};
} // unnamed namespace

template <> constexpr inline auto NetworkSetupViewStep::qt_create_metaobjectdata<qt_meta_tag_ZN20NetworkSetupViewStepE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "NetworkSetupViewStep",
        "onConnectionStateChanged",
        "",
        "connected"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onConnectionStateChanged'
        QtMocHelpers::SlotData<void(bool)>(1, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<NetworkSetupViewStep, qt_meta_tag_ZN20NetworkSetupViewStepE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject NetworkSetupViewStep::staticMetaObject = { {
    QMetaObject::SuperData::link<Calamares::ViewStep::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20NetworkSetupViewStepE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20NetworkSetupViewStepE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN20NetworkSetupViewStepE_t>.metaTypes,
    nullptr
} };

void NetworkSetupViewStep::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<NetworkSetupViewStep *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onConnectionStateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *NetworkSetupViewStep::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NetworkSetupViewStep::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20NetworkSetupViewStepE_t>.strings))
        return static_cast<void*>(this);
    return Calamares::ViewStep::qt_metacast(_clname);
}

int NetworkSetupViewStep::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Calamares::ViewStep::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}
namespace {
struct qt_meta_tag_ZN27NetworkSetupViewStepFactoryE_t {};
} // unnamed namespace

template <> constexpr inline auto NetworkSetupViewStepFactory::qt_create_metaobjectdata<qt_meta_tag_ZN27NetworkSetupViewStepFactoryE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "NetworkSetupViewStepFactory"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<NetworkSetupViewStepFactory, qt_meta_tag_ZN27NetworkSetupViewStepFactoryE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject NetworkSetupViewStepFactory::staticMetaObject = { {
    QMetaObject::SuperData::link<CalamaresPluginFactory::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN27NetworkSetupViewStepFactoryE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN27NetworkSetupViewStepFactoryE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN27NetworkSetupViewStepFactoryE_t>.metaTypes,
    nullptr
} };

void NetworkSetupViewStepFactory::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<NetworkSetupViewStepFactory *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *NetworkSetupViewStepFactory::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NetworkSetupViewStepFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN27NetworkSetupViewStepFactoryE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "io.calamares.PluginFactory"))
        return static_cast< CalamaresPluginFactory*>(this);
    return CalamaresPluginFactory::qt_metacast(_clname);
}

int NetworkSetupViewStepFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CalamaresPluginFactory::qt_metacall(_c, _id, _a);
    return _id;
}

#ifdef QT_MOC_EXPORT_PLUGIN_V2
static constexpr unsigned char qt_pluginMetaDataV2_NetworkSetupViewStepFactory[] = {
    0xbf, 
    // "IID"
    0x02,  0x78,  0x1a,  'i',  'o',  '.',  'c',  'a', 
    'l',  'a',  'm',  'a',  'r',  'e',  's',  '.', 
    'P',  'l',  'u',  'g',  'i',  'n',  'F',  'a', 
    'c',  't',  'o',  'r',  'y', 
    // "className"
    0x03,  0x78,  0x1b,  'N',  'e',  't',  'w',  'o', 
    'r',  'k',  'S',  'e',  't',  'u',  'p',  'V', 
    'i',  'e',  'w',  'S',  't',  'e',  'p',  'F', 
    'a',  'c',  't',  'o',  'r',  'y', 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN_V2(NetworkSetupViewStepFactory, NetworkSetupViewStepFactory, qt_pluginMetaDataV2_NetworkSetupViewStepFactory)
#else
QT_PLUGIN_METADATA_SECTION
Q_CONSTINIT static constexpr unsigned char qt_pluginMetaData_NetworkSetupViewStepFactory[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', '!',
    // metadata version, Qt version, architectural requirements
    0, QT_VERSION_MAJOR, QT_VERSION_MINOR, qPluginArchRequirements(),
    0xbf, 
    // "IID"
    0x02,  0x78,  0x1a,  'i',  'o',  '.',  'c',  'a', 
    'l',  'a',  'm',  'a',  'r',  'e',  's',  '.', 
    'P',  'l',  'u',  'g',  'i',  'n',  'F',  'a', 
    'c',  't',  'o',  'r',  'y', 
    // "className"
    0x03,  0x78,  0x1b,  'N',  'e',  't',  'w',  'o', 
    'r',  'k',  'S',  'e',  't',  'u',  'p',  'V', 
    'i',  'e',  'w',  'S',  't',  'e',  'p',  'F', 
    'a',  'c',  't',  'o',  'r',  'y', 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN(NetworkSetupViewStepFactory, NetworkSetupViewStepFactory)
#endif  // QT_MOC_EXPORT_PLUGIN_V2

QT_WARNING_POP
