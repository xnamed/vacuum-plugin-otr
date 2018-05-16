include(../plugins.inc) 

#Project Configuration
TARGET              = otrplugin
QT                  = core xml gui

LIBS += -lotr -lgcrypt -lgpg-error

greaterThan(QT_MAJOR_VERSION, 4) {
	QT += concurrent
}

CONFIG += util
include(otrplugin.pri)
