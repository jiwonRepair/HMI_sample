#ifndef LOGGINGDECORATOR_H
#define LOGGINGDECORATOR_H

#include <QString>
#include <functional>  // 람다 지원을 위해 추가

class LoggingDecorator {
public:
    explicit LoggingDecorator(const QString &name);

    void execute(std::function<void()> func);
    void log(const QString &message);

private:
    QString componentName;
};

#endif // LOGGINGDECORATOR_H
