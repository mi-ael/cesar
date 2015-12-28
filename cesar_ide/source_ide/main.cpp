#include <iostream>

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlEngine>

#include "CodeTextEdit.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	CodeTextEdit edit;
	edit.show();

//	qmlRegisterType<CodeTextEdit>("com.cesar.ide", 1, 0, "CodeTextEdit");
//	QQmlApplicationEngine engine;
//
//	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	return app.exec();
}
