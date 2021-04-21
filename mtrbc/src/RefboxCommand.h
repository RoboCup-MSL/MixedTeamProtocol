#ifndef REFBOX_COMMAND_H_
#define REFBOX_COMMAND_H_

#include <string>
#include <QtCore/QJsonObject>

namespace rbc
{
  class RefboxCommand
  {
  public:
    RefboxCommand();
    RefboxCommand(std::string command, std::string team);
    ~RefboxCommand();

    bool isValid();
    std::string command() const;
    std::string target() const;

    static RefboxCommand fromString(const std::string &str);
    static RefboxCommand fromJson(const QJsonObject &json);

  private:
    bool _isValid;
    std::string _command;
    std::string _target;
  };

}
#endif
