/****************************************************************************
** Meta object code from reading C++ file 'DePackagesViewStep.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "DePackagesViewStep.h"
#include <QtCore/qmetatype.h>
#include <QtCore/qplugin.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DePackagesViewStep.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN18DePackagesViewStepE_t {};
} // unnamed namespace

template <> constexpr inline auto DePackagesViewStep::qt_create_metaobjectdata<qt_meta_tag_ZN18DePackagesViewStepE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DePackagesViewStep"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DePackagesViewStep, qt_meta_tag_ZN18DePackagesViewStepE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DePackagesViewStep::staticMetaObject = { {
    QMetaObject::SuperData::link<Calamares::ViewStep::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18DePackagesViewStepE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18DePackagesViewStepE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN18DePackagesViewStepE_t>.metaTypes,
    nullptr
} };

void DePackagesViewStep::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DePackagesViewStep *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *DePackagesViewStep::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DePackagesViewStep::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18DePackagesViewStepE_t>.strings))
        return static_cast<void*>(this);
    return Calamares::ViewStep::qt_metacast(_clname);
}

int DePackagesViewStep::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Calamares::ViewStep::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN25DePackagesViewStepFactoryE_t {};
} // unnamed namespace

template <> constexpr inline auto DePackagesViewStepFactory::qt_create_metaobjectdata<qt_meta_tag_ZN25DePackagesViewStepFactoryE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DePackagesViewStepFactory"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DePackagesViewStepFactory, qt_meta_tag_ZN25DePackagesViewStepFactoryE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DePackagesViewStepFactory::staticMetaObject = { {
    QMetaObject::SuperData::link<CalamaresPluginFactory::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25DePackagesViewStepFactoryE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25DePackagesViewStepFactoryE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN25DePackagesViewStepFactoryE_t>.metaTypes,
    nullptr
} };

void DePackagesViewStepFactory::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DePackagesViewStepFactory *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *DePackagesViewStepFactory::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DePackagesViewStepFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25DePackagesViewStepFactoryE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "io.calamares.PluginFactory"))
        return static_cast< CalamaresPluginFactory*>(this);
    return CalamaresPluginFactory::qt_metacast(_clname);
}

int DePackagesViewStepFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CalamaresPluginFactory::qt_metacall(_c, _id, _a);
    return _id;
}

#ifdef QT_MOC_EXPORT_PLUGIN_V2
static constexpr unsigned char qt_pluginMetaDataV2_DePackagesViewStepFactory[] = {
    0xbf, 
    // "IID"
    0x02,  0x78,  0x1a,  'i',  'o',  '.',  'c',  'a', 
    'l',  'a',  'm',  'a',  'r',  'e',  's',  '.', 
    'P',  'l',  'u',  'g',  'i',  'n',  'F',  'a', 
    'c',  't',  'o',  'r',  'y', 
    // "className"
    0x03,  0x78,  0x19,  'D',  'e',  'P',  'a',  'c', 
    'k',  'a',  'g',  'e',  's',  'V',  'i',  'e', 
    'w',  'S',  't',  'e',  'p',  'F',  'a',  'c', 
    't',  'o',  'r',  'y', 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN_V2(DePackagesViewStepFactory, DePackagesViewStepFactory, qt_pluginMetaDataV2_DePackagesViewStepFactory)
#else
QT_PLUGIN_METADATA_SECTION
Q_CONSTINIT static constexpr unsigned char qt_pluginMetaData_DePackagesViewStepFactory[] = {
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
    0x03,  0x78,  0x19,  'D',  'e',  'P',  'a',  'c', 
    'k',  'a',  'g',  'e',  's',  'V',  'i',  'e', 
    'w',  'S',  't',  'e',  'p',  'F',  'a',  'c', 
    't',  'o',  'r',  'y', 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN(DePackagesViewStepFactory, DePackagesViewStepFactory)
#endif  // QT_MOC_EXPORT_PLUGIN_V2

QT_WARNING_POP
