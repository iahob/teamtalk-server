// Copyright (C) 2012 by wubenqi
//
// By: wubenqi<wubenqi@gmail.com>
//
// 单件设计模式实现
// 非线程安全
//

#ifndef BASE_SINGLETON_H_
#define BASE_SINGLETON_H_

template<typename T>
class Singleton  {
public:
    static T* Instance() {
         static T instance;
         return &instance;
    };
    virtual ~Singleton(){};
    Singleton(const Singleton&)=delete;
    Singleton& operator =(const Singleton &) = delete;
protected:
    Singleton(){}; 

};


#endif // BASE_SINGLETON_H_
