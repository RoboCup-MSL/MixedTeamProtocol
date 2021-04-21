#include "RefboxCommand.h"
#include <string.h>
#include <QtCore/QJsonDocument>

using namespace std;

namespace rbc
{
	RefboxCommand::RefboxCommand()
		: _isValid(false), _command(""), _target("")
	{
	}

	RefboxCommand::RefboxCommand(std::string command, std::string target)
		: _isValid(true), _command(command), _target(target)
	{
	}

	RefboxCommand::~RefboxCommand()
	{
	}

    bool RefboxCommand::isValid()
	{
		return _isValid;
	}

	std::string RefboxCommand::command() const
	{
		return _command;
	}

	std::string RefboxCommand::target() const
	{
		return _target;
	}

	RefboxCommand RefboxCommand::fromString(const std::string &str)
	{
		QJsonParseError error;
		const QJsonDocument d = QJsonDocument::fromJson(QString::fromStdString(str).toUtf8(), &error);

		if (d.isNull())
		{
			QMessageLogger().warning("[RefboxProtocol2020_Command] fromString() failure: header invalid JSON, %s", qUtf8Printable(error.errorString()));
			return RefboxCommand();
		}

		if (!d.isObject())
		{
			QMessageLogger().warning("[RefboxProtocol2020_Command] fromString() failure: header JSON document not an object");
			return RefboxCommand();
		}
		const QJsonObject obj = d.object();

		return fromJson(obj);
	}

	RefboxCommand RefboxCommand::fromJson(const QJsonObject &obj)
	{
		const QJsonValue command = obj.value("command");
		if (!command.isString())
		{
			QMessageLogger().warning("[RefboxProtocol2020_Command] fromJson() failure: header missing/invalid 'command' field");
			return RefboxCommand();
		}
		QMessageLogger().info("[RefboxProtocol2020_Command] fromJson() notice: received refbox command '%s'", qUtf8Printable(command.toString()));

		const QJsonValue targetTeam = obj.value("targetTeam");
		if (!targetTeam.isString())
		{
			QMessageLogger().warning("[RefboxProtocol2020_Command] fromJson() failure: header missing/invalid 'targetteam' field");
			return RefboxCommand();
		}

		return RefboxCommand(command.toString().toStdString(), targetTeam.toString().toStdString());
	}
}